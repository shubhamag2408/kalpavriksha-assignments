
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 512
#define MAX_BLOCKS 5000
#define MAX_NAME 50

struct FreeBlock
{
    int index;

    struct FreeBlock *next;
    struct FreeBlock *prev;
};


struct FileNode
{
    char name[MAX_NAME + 1];

    int isDirectory;

    struct FileNode *parent;
    struct FileNode *children;

    size_t contentSize;

    int *blockPointers;
    size_t blockCount;

    struct FileNode *next;
};

struct VirtualDisk
{
    unsigned char (*disk)[BLOCK_SIZE];

    int totalBlocks;
    int usedBlocks;
    int freeBlocks;

    struct FreeBlock *freeHead;
    struct FreeBlock *freeTail;
};

struct FileSystem
{
    struct VirtualDisk vdisk;

    struct FileNode *root;
    struct FileNode *cwd;
};


/* Add a free block at the end of the list */
void addFreeBlock(struct VirtualDisk *vd, int index)
{
    struct FreeBlock *newBlock;

    newBlock = malloc(sizeof(struct FreeBlock));

    if (newBlock == NULL)
    {
        printf("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    newBlock->index = index;
    newBlock->next = NULL;
    newBlock->prev = vd->freeTail;

    if (vd->freeHead == NULL)
    {
        vd->freeHead = newBlock;
        vd->freeTail = newBlock;
    }
    else
    {
        vd->freeTail->next = newBlock;
        vd->freeTail = newBlock;
    }

    vd->freeBlocks++;
}

/* Initialize the virtual disk */
void initializeDisk(struct VirtualDisk *vd, int numBlocks)
{
    vd->totalBlocks = numBlocks;
    vd->usedBlocks = 0;
    vd->freeBlocks = 0;

    vd->freeHead = NULL;
    vd->freeTail = NULL;

    /* Allocate the two-dimensional disk */
    vd->disk = calloc(numBlocks, sizeof(*vd->disk));

    if (vd->disk == NULL)
    {
        printf("Virtual disk allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    /* Initially, all blocks are free */
    for (int i = 0; i < numBlocks; i++)
    {
        addFreeBlock(vd, i);
    }

    printf("Virtual disk initialized successfully.\n");
}

/* Create the root directory */
struct FileNode *createRoot()
{
    struct FileNode *root;

    root = calloc(1, sizeof(struct FileNode));

    if (root == NULL)
    {
        printf("Root directory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    strcpy(root->name, "/");

    root->isDirectory = 1;
    root->parent = NULL;
    root->children = NULL;
    root->next = NULL;

    printf("Root directory created successfully.\n");

    return root;
}
int isValidName(char name[]);

/* Check whether a file or directory name is valid */
int isValidName(char name[])
{
    if (name[0] == '\0')
        return 0;

    if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
        return 0;

    if (strcmp(name, "/") == 0)
        return 0;

    if (strchr(name, '/') != NULL)
        return 0;

    return 1;
}

struct FileNode *createNode(char name[], int isDirectory);

void insertChild(struct FileNode *parent, struct FileNode *newNode);

struct FileNode *findEntry(struct FileNode *parent, char name[]);

void mkdirCommand(struct FileSystem *fs);

void lsCommand(struct FileSystem *fs);

void cleanupDirectory(struct FileNode *directory);

/* Create a file or directory node */
struct FileNode *createNode(char name[], int isDirectory)
{
    struct FileNode *newNode;

    newNode = calloc(1, sizeof(struct FileNode));

    if (newNode == NULL)
    {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    strcpy(newNode->name, name);

    newNode->isDirectory = isDirectory;
    newNode->parent = NULL;
    newNode->children = NULL;

    newNode->contentSize = 0;
    newNode->blockPointers = NULL;
    newNode->blockCount = 0;

    newNode->next = NULL;

    return newNode;
}
/* Insert a child into the parent's circular linked list */
void insertChild(struct FileNode *parent, struct FileNode *newNode)
{
    struct FileNode *temp;

    newNode->parent = parent;

    /* Case 1: List is empty */
    if (parent->children == NULL)
    {
        parent->children = newNode;
        newNode->next = newNode;
        return;
    }

    /* Case 2: List already contains nodes */
    temp = parent->children;

    while (temp->next != parent->children)
    {
        temp = temp->next;
    }

    /* Insert new node at the end */
    temp->next = newNode;
    newNode->next = parent->children;
}
/* Find an entry inside a directory */
struct FileNode *findEntry(struct FileNode *parent, char name[])
{
    struct FileNode *temp;

    if (parent->children == NULL)
        return NULL;

    temp = parent->children;

    do
    {
        if (strcmp(temp->name, name) == 0)
        {
            return temp;
        }

        temp = temp->next;

    } while (temp != parent->children);

    return NULL;
}
/* Create a new directory */
void mkdirCommand(struct FileSystem *fs)
{
    char name[MAX_NAME + 1];

    printf("Enter directory name: ");
    scanf("%50s", name);

    if (!isValidName(name))
{
    printf("Invalid name. Try another name.\n");
    return;
}

    /* Check whether the name already exists */
    if (findEntry(fs->cwd, name) != NULL)
    {
        printf("An entry with this name already exists.\n");
        return;
    }

    /* Create a directory node */
    struct FileNode *newDirectory = createNode(name, 1);

    if (newDirectory == NULL)
        return;

    /* Insert into current directory */
    insertChild(fs->cwd, newDirectory);

    printf("Directory '%s' created successfully.\n", name);
}
/* Display all entries in the current directory */
void lsCommand(struct FileSystem *fs)
{
    struct FileNode *temp;

    if (fs->cwd->children == NULL)
    {
        printf("(empty)\n");
        return;
    }

    temp = fs->cwd->children;

    do
    {
        printf("%s", temp->name);

        if (temp->isDirectory)
            printf("/");

        printf("\n");

        temp = temp->next;

    } while (temp != fs->cwd->children);
}
/* Free all child entries recursively */
/* Free all child entries recursively */
void cleanupDirectory(struct FileNode *directory)
{
    struct FileNode *head;
    struct FileNode *current;
    struct FileNode *next;
    struct FileNode *last;

    if (directory->children == NULL)
        return;

    head = directory->children;

    /* Find the last node in the circular list */
    last = head;

    while (last->next != head)
    {
        last = last->next;
    }

    /* Break the circular link */
    last->next = NULL;

    current = head;

    /* Free every child node */
    while (current != NULL)
    {
        next = current->next;

        if (current->isDirectory)
            cleanupDirectory(current);

        free(current->blockPointers);
        free(current);

        current = next;
    }

    directory->children = NULL;
}

void cdCommand(struct FileSystem *fs);
void pwdCommand(struct FileSystem *fs);

/* Change current working directory */
void cdCommand(struct FileSystem *fs)
{
    char name[MAX_NAME + 1];

    printf("Enter directory name: ");
    scanf("%50s", name);

    /* Move to root directory */
    if (strcmp(name, "/") == 0)
    {
        fs->cwd = fs->root;
        printf("Moved to /\n");
        return;
    }

    /* Move to parent directory */
    if (strcmp(name, "..") == 0)
    {
        if (fs->cwd->parent != NULL)
        {
            fs->cwd = fs->cwd->parent;
        }

        printf("Moved to parent directory.\n");
        return;
    }

    /* Find the requested directory */
    struct FileNode *entry = findEntry(fs->cwd, name);

    if (entry == NULL || !entry->isDirectory)
    {
        printf("Directory not found.\n");
        return;
    }

    fs->cwd = entry;

    printf("Directory changed successfully.\n");
}

/* Display current working directory */
void pwdCommand(struct FileSystem *fs)
{
    struct FileNode *temp = fs->cwd;

    char path[1000] = "";
    char part[55];

    while (temp != NULL && temp != fs->root)
    {
        snprintf(part, sizeof(part), "/%s", temp->name);

        /* Add current directory name at the beginning */
        memmove(path + strlen(part), path, strlen(path) + 1);
        memcpy(path, part, strlen(part));

        temp = temp->parent;
    }

    if (strlen(path) == 0)
        printf("/\n");
    else
        printf("%s\n", path);
}

void createCommand(struct FileSystem *fs);

/* Create a new file in the current directory */
void createCommand(struct FileSystem *fs)
{
    char name[MAX_NAME + 1];

    printf("Enter file name: ");
    scanf("%50s", name);

    if (!isValidName(name))
{
    printf("Invalid name. Try another name.\n");
    return;
}

    /* Check whether the name already exists */
    if (findEntry(fs->cwd, name) != NULL)
    {
        printf("An entry with this name already exists.\n");
        return;
    }

    /* Create a file node */
    struct FileNode *newFile = createNode(name, 0);

    if (newFile == NULL)
        return;

    /* Insert the file into the circular linked list */
    insertChild(fs->cwd, newFile);

    printf("File '%s' created successfully.\n", name);
}

int allocateBlock(struct VirtualDisk *vd);

void freeBlock(struct VirtualDisk *vd, int index);

void writeCommand(struct FileSystem *fs);

/* Allocate a block from the head of the free list */
int allocateBlock(struct VirtualDisk *vd)
{
    struct FreeBlock *temp;
    int index;

    if (vd->freeHead == NULL)
        return -1;

    temp = vd->freeHead;
    index = temp->index;

    vd->freeHead = temp->next;

    if (vd->freeHead == NULL)
    {
        vd->freeTail = NULL;
    }
    else
    {
        vd->freeHead->prev = NULL;
    }

    free(temp);

    vd->freeBlocks--;
    vd->usedBlocks++;

    return index;
}
/* Return a block to the tail of the free list */
void freeBlock(struct VirtualDisk *vd, int index)
{
    addFreeBlock(vd, index);

    vd->usedBlocks--;
}
/* Write or replace file content */
void writeCommand(struct FileSystem *fs)
{
    char name[MAX_NAME + 1];

    printf("Enter file name: ");
    scanf("%50s", name);

    struct FileNode *file = findEntry(fs->cwd, name);

    if (file == NULL || file->isDirectory)
    {
        printf("File not found.\n");
        return;
    }

    /* Allocate a buffer large enough for the virtual disk */
    size_t maxSize = (size_t)fs->vdisk.totalBlocks * BLOCK_SIZE;

    char *content = malloc(maxSize + 1);

    if (content == NULL)
    {
        printf("Memory allocation failed.\n");
        return;
    }

    /* Consume the newline after the filename */
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF)
    {
    }

    printf("Enter file content: ");

    if (fgets(content, (int)(maxSize + 1), stdin) == NULL)
    {
        free(content);
        return;
    }

    /* Remove trailing newline */
    content[strcspn(content, "\n")] = '\0';

    size_t newSize = strlen(content);

    /* Calculate required blocks */
    size_t requiredBlocks =
        (newSize + BLOCK_SIZE - 1) / BLOCK_SIZE;

    /* Check available blocks, including the file's old blocks */
    if (requiredBlocks > (size_t)fs->vdisk.freeBlocks
                         + file->blockCount)
    {
        printf("Not enough free disk space.\n");
        free(content);
        return;
    }

    /* Allocate a new block pointer array */
    int *newPointers = NULL;

    if (requiredBlocks > 0)
    {
        newPointers = malloc(requiredBlocks * sizeof(int));

        if (newPointers == NULL)
        {
            printf("Memory allocation failed.\n");
            free(content);
            return;
        }
    }

    /* Release the old blocks */
    for (size_t i = 0; i < file->blockCount; i++)
    {
        int oldIndex = file->blockPointers[i];

        memset(fs->vdisk.disk[oldIndex], 0, BLOCK_SIZE);
        freeBlock(&fs->vdisk, oldIndex);
    }

    free(file->blockPointers);

    file->blockPointers = newPointers;
    file->blockCount = 0;

    /* Allocate new blocks from the free-list head */
    for (size_t i = 0; i < requiredBlocks; i++)
    {
        int index = allocateBlock(&fs->vdisk);

        if (index == -1)
        {
            printf("Block allocation failed.\n");
            free(content);
            return;
        }

        file->blockPointers[i] = index;
        file->blockCount++;
    }

    /* Write content across the allocated blocks */
    size_t copied = 0;

    for (size_t i = 0; i < requiredBlocks; i++)
    {
        size_t remaining = newSize - copied;
        size_t bytesToCopy = remaining;

        if (bytesToCopy > BLOCK_SIZE)
            bytesToCopy = BLOCK_SIZE;

        memcpy(
            fs->vdisk.disk[file->blockPointers[i]],
            content + copied,
            bytesToCopy
        );

        copied += bytesToCopy;
    }

    file->contentSize = newSize;

    printf("Data written successfully (size=%zu bytes).\n", newSize);
    printf("Blocks allocated: %zu\n", file->blockCount);

    free(content);
}

void readCommand(struct FileSystem *fs);


/* Read content from a file */
void readCommand(struct FileSystem *fs)
{
    char name[MAX_NAME + 1];

    printf("Enter file name: ");
    scanf("%50s", name);

    struct FileNode *file = findEntry(fs->cwd, name);

    if (file == NULL || file->isDirectory)
    {
        printf("File not found.\n");
        return;
    }

    if (file->contentSize == 0)
    {
        printf("File is empty.\n");
        return;
    }

    printf("\nFile content:\n");

    size_t remaining = file->contentSize;

    for (size_t i = 0; i < file->blockCount; i++)
    {
        size_t bytesToRead = remaining;

        if (bytesToRead > BLOCK_SIZE)
            bytesToRead = BLOCK_SIZE;

        fwrite(
            fs->vdisk.disk[file->blockPointers[i]],
            1,
            bytesToRead,
            stdout
        );

        remaining -= bytesToRead;
    }

    printf("\n");
}

void dfCommand(struct FileSystem *fs);

/* Display virtual disk usage */
void dfCommand(struct FileSystem *fs)
{
    struct VirtualDisk *vd = &fs->vdisk;

    double usage = 0.0;

    if (vd->totalBlocks > 0)
    {
        usage = ((double)vd->usedBlocks / vd->totalBlocks) * 100;
    }

    printf("\n========== DISK USAGE ==========\n");

    printf("Total Blocks: %d\n", vd->totalBlocks);
    printf("Used Blocks: %d\n", vd->usedBlocks);
    printf("Free Blocks: %d\n", vd->freeBlocks);
    printf("Disk Usage: %.2f%%\n", usage);

    printf("===============================\n");
}

void deleteCommand(struct FileSystem *fs);

/* Delete a file from the current directory */
void deleteCommand(struct FileSystem *fs)
{
    char name[MAX_NAME + 1];

    printf("Enter file name: ");
    scanf("%50s", name);

    struct FileNode *head = fs->cwd->children;

    if (head == NULL)
    {
        printf("Directory is empty.\n");
        return;
    }

    struct FileNode *current = head;
    struct FileNode *previous = NULL;

    /* Search for the file */
    do
    {
        if (strcmp(current->name, name) == 0)
            break;

        previous = current;
        current = current->next;

    } while (current != head);

    if (strcmp(current->name, name) != 0)
    {
        printf("File not found.\n");
        return;
    }

    if (current->isDirectory)
    {
        printf("Cannot delete a directory using delete.\n");
        return;
    }

    /* Return all allocated blocks to the free list */
    for (size_t i = 0; i < current->blockCount; i++)
    {
        int index = current->blockPointers[i];

        memset(fs->vdisk.disk[index], 0, BLOCK_SIZE);

        freeBlock(&fs->vdisk, index);
    }

    /* Remove node from circular linked list */
    if (current->next == current)
    {
        fs->cwd->children = NULL;
    }
    else
    {
        if (previous == NULL)
        {
            struct FileNode *last = head;

            while (last->next != head)
                last = last->next;

            fs->cwd->children = current->next;
            last->next = current->next;
        }
        else
        {
            previous->next = current->next;
        }
    }

    free(current->blockPointers);
    free(current);

    printf("File deleted successfully.\n");
}

void rmdirCommand(struct FileSystem *fs);

/* Remove an empty directory */
void rmdirCommand(struct FileSystem *fs)
{
    char name[MAX_NAME + 1];

    printf("Enter directory name: ");
    scanf("%50s", name);

    struct FileNode *parent = fs->cwd;
    struct FileNode *head = parent->children;

    if (head == NULL)
    {
        printf("Directory not found.\n");
        return;
    }

    struct FileNode *current = head;
    struct FileNode *previous = NULL;

    /* Search for the directory */
    do
    {
        if (strcmp(current->name, name) == 0)
            break;

        previous = current;
        current = current->next;

    } while (current != head);

    if (strcmp(current->name, name) != 0)
    {
        printf("Directory not found.\n");
        return;
    }

    if (!current->isDirectory)
    {
        printf("This is a file, not a directory.\n");
        return;
    }

    /* Do not remove a non-empty directory */
    if (current->children != NULL)
    {
        printf("Directory is not empty.\n");
        return;
    }

    /* Remove node from circular linked list */
    if (current->next == current)
    {
        parent->children = NULL;
    }
    else
    {
        if (previous == NULL)
        {
            struct FileNode *last = head;

            while (last->next != head)
                last = last->next;

            parent->children = current->next;
            last->next = current->next;
        }
        else
        {
            previous->next = current->next;
        }
    }

    free(current);

    printf("Directory removed successfully.\n");
}

void displayPrompt(struct FileSystem *fs)
{
    char path[1000] = "";
    char part[55];

    struct FileNode *temp = fs->cwd;

    while (temp != NULL && temp != fs->root)
    {
        snprintf(part, sizeof(part), "/%s", temp->name);

        memmove(path + strlen(part), path, strlen(path) + 1);
        memcpy(path, part, strlen(part));

        temp = temp->parent;
    }

    if (strlen(path) == 0)
        printf("/ > ");
    else
        printf("%s > ", path);
}



int main()
{
    struct FileSystem fs = {0};

    int numBlocks;
    char command[20];

    printf("===== VIRTUAL FILE SYSTEM =====\n");

    printf("Enter number of disk blocks (1-5000): ");

    /* Validate numeric input */
    if (scanf("%d", &numBlocks) != 1)
    {
        printf("Invalid input.\n");
        return 1;
    }

    /* Validate disk size */
    if (numBlocks < 1 || numBlocks > MAX_BLOCKS)
    {
        printf("Invalid number of blocks.\n");
        return 1;
    }

    /* Initialize virtual disk */
    initializeDisk(&fs.vdisk, numBlocks);

    /* Create root directory */
    fs.root = createRoot();
    fs.cwd = fs.root;

    printf("\nVirtual File System is ready!\n");

    /* Main command loop */
    while (1)
    {
        printf("\n");
        displayPrompt(&fs);

        if (scanf("%19s", command) != 1)
            break;

        if (strcmp(command, "mkdir") == 0)
        {
            mkdirCommand(&fs);
        }
        else if (strcmp(command, "ls") == 0)
        {
            lsCommand(&fs);
        }
        else if (strcmp(command, "cd") == 0)
        {
            cdCommand(&fs);
        }
        else if (strcmp(command, "pwd") == 0)
        {
            pwdCommand(&fs);
        }
        else if (strcmp(command, "create") == 0)
        {
            createCommand(&fs);
        }
        else if (strcmp(command, "write") == 0)
        {
            writeCommand(&fs);
        }
        else if (strcmp(command, "read") == 0)
        {
            readCommand(&fs);
        }
        else if (strcmp(command, "df") == 0)
        {
            dfCommand(&fs);
        }
        else if (strcmp(command, "delete") == 0)
        {
            deleteCommand(&fs);
        }
        else if (strcmp(command, "rmdir") == 0)
        {
            rmdirCommand(&fs);
        }
        else if (strcmp(command, "exit") == 0)
        {
            break;
        }
        else
        {
            printf("Unknown command.\n");
        }
    }

    cleanupDirectory(fs.root);
    free(fs.root);

    struct FreeBlock *temp;
    while (fs.vdisk.freeHead != NULL)
    {
        temp = fs.vdisk.freeHead;
        fs.vdisk.freeHead = temp->next;
        free(temp);
    }
    free(fs.vdisk.disk);

    printf("\nMemory released. Exiting program.\n");

    return 0;
}
