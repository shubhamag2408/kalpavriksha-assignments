
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include <errno.h>
#include <ctype.h>

#define FIRST_FIT 1
#define BEST_FIT 2
#define WORST_FIT 3

struct Block
{
    size_t start;
    size_t size;
    int isFree;
    unsigned long long id;
};

struct MemoryManager
{
    char *memoryPool;

    struct Block *blocks;

    size_t poolSize;
    size_t blockCount;
    size_t capacity;

    int strategy;
    unsigned long long nextId;
};


int readNumber(unsigned long long *number);

void initializeMemory(struct MemoryManager *m);
void freeMemory(struct MemoryManager *m);

int increaseCapacity(struct MemoryManager *m);
void insertBlock(struct MemoryManager *m, int index, struct Block b);
void removeBlock(struct MemoryManager *m, int index);

int findFreeBlock(struct MemoryManager *m, size_t size);
int findAllocation(struct MemoryManager *m, unsigned long long id);

void allocateMemory(struct MemoryManager *m);
void releaseMemory(struct MemoryManager *m);
void resizeAllocation(struct MemoryManager *m);

void mergeFreeBlocks(struct MemoryManager *m);

void displayMemoryMap(struct MemoryManager *m);
void displayStatistics(struct MemoryManager *m);
void detectFragmentation(struct MemoryManager *m, size_t requested);

void changeStrategy(struct MemoryManager *m);


int readNumber(unsigned long long *number)

{
    char input[100];
    char *end;
    char *start;
    unsigned long long value;
    int ch;

    if (fgets(input, sizeof(input), stdin) == NULL)
        return 0;

   
    if (strchr(input, '\n') == NULL && !feof(stdin))
    {
        while ((ch = getchar()) != '\n' && ch != EOF)
        {
           
        }

        printf("Invalid input. Enter a number.\n");
        return -1;
    }

    start = input;

    while (isspace((unsigned char)*start))
        start++;

   
    if (*start == '-')
    {
        printf("Invalid input. Enter a positive number.\n");
        return -1;
    }

    errno = 0;
    value = strtoull(start, &end, 10);

    if (start == end || errno == ERANGE)
    {
        printf("Invalid input. Enter a valid number.\n");
        return -1;
    }

    
    while (isspace((unsigned char)*end))
        end++;

    if (*end != '\0')
    {
        printf("Invalid input. Enter numbers only.\n");
        return -1;
    }

    *number = value;
    return 1;
}

void initializeMemory(struct MemoryManager *m)
{
    unsigned long long size;

    printf("Enter memory pool size: ");

    while (1)
    {
        int result = readNumber(&size);

        if (result == 0)
            exit(EXIT_FAILURE);

        if (result == -1)
            continue;

        if (size >= 1 && size <= 1000000)
            break;

        printf("Enter size between 1 and 1000000: ");
    }

    m->poolSize = (size_t)size;

    m->memoryPool = malloc(m->poolSize);

    if (m->memoryPool == NULL)
    {
        printf("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    memset(m->memoryPool, 0, m->poolSize);

    m->capacity = 1;
    m->blockCount = 1;

    m->blocks = malloc(sizeof(struct Block));

    if (m->blocks == NULL)
    {
        printf("Metadata allocation failed.\n");
        free(m->memoryPool);
        exit(EXIT_FAILURE);
    }

    m->blocks[0].start = 0;
    m->blocks[0].size = m->poolSize;
    m->blocks[0].isFree = 1;
    m->blocks[0].id = 0;

    m->nextId = 1;

    changeStrategy(m);

    printf("\nMemory pool initialized successfully.\n");
}

void changeStrategy(struct MemoryManager *m)
{
    unsigned long long choice;

    printf("\nAllocation Strategy\n");
    printf("1. First Fit\n");
    printf("2. Best Fit\n");
    printf("3. Worst Fit\n");

    while (1)
    {
        printf("Enter choice: ");

        int result = readNumber(&choice);

        if (result == 0)
            exit(EXIT_FAILURE);

        if (result == 1 && choice >= 1 && choice <= 3)
            break;

        printf("Invalid choice.\n");
    }

    m->strategy = (int)choice;

    printf("Strategy selected successfully.\n");
}

int increaseCapacity(struct MemoryManager *m)
{
    if (m->capacity > SIZE_MAX / 2)
        return 0;

    size_t newCapacity = m->capacity * 2;

    if (newCapacity > SIZE_MAX / sizeof(struct Block))
        return 0;

    struct Block *temp = realloc(
        m->blocks,
        newCapacity * sizeof(struct Block)
    );

    if (temp == NULL)
        return 0;

    m->blocks = temp;
    m->capacity = newCapacity;

    return 1;
}

void insertBlock(struct MemoryManager *m, int index, struct Block b)
{

    memmove(
        &m->blocks[index + 1],
        &m->blocks[index],
        (m->blockCount - index) * sizeof(struct Block)
    );

    m->blocks[index] = b;
    m->blockCount++;
}

void removeBlock(struct MemoryManager *m, int index)
{
    memmove(
        &m->blocks[index],
        &m->blocks[index + 1],
        (m->blockCount - index - 1) * sizeof(struct Block)
    );

    m->blockCount--;
}

int findFreeBlock(struct MemoryManager *m, size_t requested)
{
    int selected = -1;

    for (int i = 0; i < (int)m->blockCount; i++)
    {
        if (m->blocks[i].isFree == 0)
            continue;

        if (m->blocks[i].size < requested)
            continue;

        if (m->strategy == FIRST_FIT)
            return i;

        if (m->strategy == BEST_FIT)
        {
            if (selected == -1 ||
                m->blocks[i].size < m->blocks[selected].size)
            {
                selected = i;
            }
        }
        else if (m->strategy == WORST_FIT)
        {
            if (selected == -1 ||
                m->blocks[i].size > m->blocks[selected].size)
            {
                selected = i;
            }
        }
    }

    return selected;
}

int findAllocation(struct MemoryManager *m, unsigned long long id)
{
    for (int i = 0; i < (int)m->blockCount; i++)
    {
        if (m->blocks[i].isFree == 0 &&
            m->blocks[i].id == id)
        {
            return i;
        }
    }

    return -1;
}

void allocateMemory(struct MemoryManager *m)
{
    unsigned long long requested;

    printf("Requested size: ");

    if (readNumber(&requested) != 1)
        return;

    if (requested == 0 || requested > m->poolSize)
    {
        printf("Invalid allocation size.\n");
        return;
    }

    if (m->nextId == ULLONG_MAX)
    {
        printf("Allocation ID limit reached.\n");
        return;
    }

    int index = findFreeBlock(m, (size_t)requested);

    if (index == -1)
    {
        printf("Allocation failed.\n");
        detectFragmentation(m, (size_t)requested);
        return;
    }

    size_t oldSize = m->blocks[index].size;
    size_t start = m->blocks[index].start;

    if (oldSize > requested)
    {
        if (m->blockCount == m->capacity)
        {
            if (!increaseCapacity(m))
            {
                printf("Metadata allocation failed.\n");
                return;
            }
        }
    }
    m->blocks[index].size = (size_t)requested;
    m->blocks[index].isFree = 0;
    m->blocks[index].id = m->nextId;

    memset(m->memoryPool + start, 0, (size_t)requested);

    if (oldSize > requested)
    {
        struct Block freeBlock;

        freeBlock.start = start + (size_t)requested;
        freeBlock.size = oldSize - (size_t)requested;
        freeBlock.isFree = 1;
        freeBlock.id = 0;

        insertBlock(m, index + 1, freeBlock);
    }

    printf("\nAllocation successful.\n");
    printf("ID: %llu\n", m->nextId);
    printf("Start: %zu\n", start);
    printf("Size: %llu bytes\n", requested);

    m->nextId++;
}

void mergeFreeBlocks(struct MemoryManager *m)
{
    int i = 0;

    while (i < (int)m->blockCount - 1)
    {
        if (m->blocks[i].isFree &&
            m->blocks[i + 1].isFree)
        {
           
            m->blocks[i].size += m->blocks[i + 1].size;

            removeBlock(m, i + 1);
        }
        else
        {
            i++;
        }
    }
}

void releaseMemory(struct MemoryManager *m)
{
    unsigned long long id;

    printf("Enter allocation ID: ");

    if (readNumber(&id) != 1)
        return;

    if (id == 0)
    {
        printf("Invalid allocation ID.\n");
        return;
    }

    int index = findAllocation(m, id);

    if (index == -1)
    {
        printf("Allocation not found or already released.\n");
        return;
    }
    m->blocks[index].isFree = 1;
    m->blocks[index].id = 0;

    mergeFreeBlocks(m);

    printf("Memory released successfully.\n");
}

void resizeAllocation(struct MemoryManager *m)
{
    unsigned long long id;
    unsigned long long requested;

    printf("Enter allocation ID: ");

    if (readNumber(&id) != 1)
        return;

    int index = findAllocation(m, id);

    if (index == -1)
    {
        printf("Invalid allocation ID.\n");
        return;
    }

    printf("Enter new size: ");

    if (readNumber(&requested) != 1)
        return;

    if (requested == 0 || requested > m->poolSize)
    {
        printf("Invalid size.\n");
        return;
    }

    size_t oldSize = m->blocks[index].size;
    size_t newSize = (size_t)requested;
    size_t oldStart = m->blocks[index].start;

    if (newSize == oldSize)
    {
        printf("Size is already the same.\n");
        return;
    }

    if (newSize < oldSize)
    {
        if (m->blockCount == m->capacity)
        {
            if (!increaseCapacity(m))
            {
                printf("Metadata allocation failed.\n");
                return;
            }
        }

        struct Block freeBlock;

        freeBlock.start = oldStart + newSize;
        freeBlock.size = oldSize - newSize;
        freeBlock.isFree = 1;
        freeBlock.id = 0;

        m->blocks[index].size = newSize;

        insertBlock(m, index + 1, freeBlock);

        mergeFreeBlocks(m);

        printf("Allocation decreased successfully.\n");
        return;
    }

    size_t extra = newSize - oldSize;

    if (index + 1 < (int)m->blockCount)
    {
        struct Block *next = &m->blocks[index + 1];

        if (next->isFree &&
            next->size >= extra)
        {
            m->blocks[index].size = newSize;

            if (next->size == extra)
            {
                removeBlock(m, index + 1);
            }
            else
            {
                next->start += extra;
                next->size -= extra;
            }

            printf("Allocation increased in place.\n");
            return;
        }
    }
    int newIndex = findFreeBlock(m, newSize);

    if (newIndex == -1)
    {
        printf("Resize failed: no suitable free block.\n");
        detectFragmentation(m, newSize);
        printf("Original allocation remains unchanged.\n");
        return;
    }

    size_t newStart = m->blocks[newIndex].start;
    size_t availableSize = m->blocks[newIndex].size;

    if (availableSize > newSize &&
        m->blockCount == m->capacity)
    {
        if (!increaseCapacity(m))
        {
            printf("Metadata allocation failed.\n");
            printf("Original allocation remains unchanged.\n");
            return;
        }
    }

    memmove(
        m->memoryPool + newStart,
        m->memoryPool + oldStart,
        oldSize
    );

    m->blocks[newIndex].size = newSize;
    m->blocks[newIndex].isFree = 0;
    m->blocks[newIndex].id = id;

    if (availableSize > newSize)
    {
        struct Block freeBlock;

        freeBlock.start = newStart + newSize;
        freeBlock.size = availableSize - newSize;
        freeBlock.isFree = 1;
        freeBlock.id = 0;

        insertBlock(m, newIndex + 1, freeBlock);
    }

    int oldIndex = -1;

    for (int i = 0; i < (int)m->blockCount; i++)
    {
        if (m->blocks[i].id == id &&
            m->blocks[i].start == oldStart)
        {
            oldIndex = i;
            break;
        }
    }

    if (oldIndex != -1)
    {
        m->blocks[oldIndex].isFree = 1;
        m->blocks[oldIndex].id = 0;
    }

    mergeFreeBlocks(m);

    printf("Allocation relocated and resized successfully.\n");
    printf("New starting offset: %zu\n", newStart);
    printf("New size: %zu bytes\n", newSize);
}

void displayMemoryMap(struct MemoryManager *m)
{
    printf("\n========== MEMORY MAP ==========\n");

    printf("Total memory: %zu bytes\n\n", m->poolSize);

    for (int i = 0; i < (int)m->blockCount; i++)
    {
        if (m->blocks[i].isFree)
        {
            printf("[Free: %zu bytes, Start: %zu]\n",
                   m->blocks[i].size,
                   m->blocks[i].start);
        }
        else
        {
            printf("[Allocated #%llu: %zu bytes, Start: %zu]\n",
                   m->blocks[i].id,
                   m->blocks[i].size,
                   m->blocks[i].start);
        }
    }

    printf("================================\n");
}

void displayStatistics(struct MemoryManager *m)
{
    size_t allocated = 0;
    size_t freeBytes = 0;

    size_t allocatedBlocks = 0;
    size_t freeBlocks = 0;

    size_t largestFree = 0;

    for (int i = 0; i < (int)m->blockCount; i++)
    {
        if (m->blocks[i].isFree)
        {
            freeBytes += m->blocks[i].size;
            freeBlocks++;

            if (m->blocks[i].size > largestFree)
            {
                largestFree = m->blocks[i].size;
            }
        }
        else
        {
            allocated += m->blocks[i].size;
            allocatedBlocks++;
        }
    }

    printf("\n========== MEMORY STATISTICS ==========\n");

    printf("Total pool size: %zu bytes\n", m->poolSize);
    printf("Allocated bytes: %zu bytes\n", allocated);
    printf("Free bytes: %zu bytes\n", freeBytes);

    printf("Allocated blocks: %zu\n", allocatedBlocks);
    printf("Free blocks: %zu\n", freeBlocks);

    printf("Largest free block: %zu bytes\n", largestFree);

    printf("=======================================\n");
}

void detectFragmentation(struct MemoryManager *m, size_t requested)
{
    size_t totalFree = 0;
    size_t largestFree = 0;

    for (int i = 0; i < (int)m->blockCount; i++)
    {
        if (m->blocks[i].isFree)
        {
            totalFree += m->blocks[i].size;

            if (m->blocks[i].size > largestFree)
            {
                largestFree = m->blocks[i].size;
            }
        }
    }

    if (totalFree >= requested && largestFree < requested)
    {
        printf("\nExternal fragmentation detected.\n");
        printf("Requested size: %zu bytes\n", requested);
        printf("Total free memory: %zu bytes\n", totalFree);
        printf("Largest free block: %zu bytes\n", largestFree);
        printf("Free memory is divided into smaller blocks.\n");
    }
    else
    {
        printf("Insufficient free memory.\n");
        printf("Total free memory: %zu bytes\n", totalFree);
        printf("Largest free block: %zu bytes\n", largestFree);
    }
}

void freeMemory(struct MemoryManager *m)
{
    free(m->blocks);
    free(m->memoryPool);

    m->blocks = NULL;
    m->memoryPool = NULL;
}

int main()
{
    struct MemoryManager manager = {0};

    unsigned long long choice;

    printf("\n===== SMART MEMORY MANAGER =====\n");

    initializeMemory(&manager);

    while (1)
    {
        printf("\n========== MENU ==========\n");
        printf("1. Allocate memory\n");
        printf("2. Release memory\n");
        printf("3. Resize allocation\n");
        printf("4. Display memory map\n");
        printf("5. Display memory statistics\n");
        printf("6. Change allocation strategy\n");
        printf("7. Exit\n");

        printf("Enter choice: ");

        int result = readNumber(&choice);

        if (result == 0)
            break;

        if (result == -1)
            continue;

        switch (choice)
        {
            case 1:
                allocateMemory(&manager);
                break;

            case 2:
                releaseMemory(&manager);
                break;

            case 3:
                resizeAllocation(&manager);
                break;

            case 4:
                displayMemoryMap(&manager);
                break;

            case 5:
                displayStatistics(&manager);
                break;

            case 6:
                changeStrategy(&manager);
                break;

            case 7:
                freeMemory(&manager);
                printf("Memory released. Exiting program.\n");
                return 0;

            default:
                printf("Invalid choice.\n");
        }
    }

    freeMemory(&manager);

    return 0;
}
