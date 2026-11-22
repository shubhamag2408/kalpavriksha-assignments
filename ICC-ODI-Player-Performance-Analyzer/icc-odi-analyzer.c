#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Rename the header's counters while including it to avoid name conflicts. */
#define playerCount headerPlayerCount
#define teamCount headerTeamCount
#include "Players_data.h"
#undef playerCount
#undef teamCount

/* Independent player structure required by the assignment. */
typedef struct
{
    int playerId;
    char name[51];
    char teamName[30];
    char role[20];
    int totalRuns;
    float battingAverage;
    float strikeRate;
    int wickets;
    float economyRate;
    float performanceIndex;
} ODIPlayer;

typedef struct
{
    int teamId;
    char name[30];
    int totalPlayers;
    float averageBattingStrikeRate;
} Team;

#define MAX_PLAYERS 500
#define MAX_TEAMS 10

ODIPlayer playerList[MAX_PLAYERS];
int playerCount = 0;
Team teamList[MAX_TEAMS];
int teamCount = 10;

typedef struct PlayerNode
{
    ODIPlayer player;
    struct PlayerNode *next;
} PlayerNode;


/* Sorted linked list per team and role: Batsman, Bowler, All-rounder. */
PlayerNode *rolePlayerLists[MAX_TEAMS][3] = {NULL};

typedef struct
{
    PlayerNode *node;
    int teamIndex;
} HeapNode;

HeapNode maxHeap[MAX_TEAMS];
int heapSize = 0;

float calculatePerformanceIndex(ODIPlayer p);
void initializeData(void);
int searchTeamById(int teamId);
int getRoleIndex(const char role[]);
void insertPlayerSorted(int teamIndex, ODIPlayer player);
void addPlayer(void);
void displayPlayersOfTeam(void);
void displayTeamsByStrikeRate(void);
void displayTopKPlayers(void);
void displayPlayersOfRoleAllTeams(void);
void heapInsert(HeapNode item);
HeapNode heapRemove(void);
void updateTeamAverageStrikeRate(int teamIndex);
void displayMenu(void);

/* Performance index formula based on role. */
float calculatePerformanceIndex(ODIPlayer p)
{
    if (strcmp(p.role, "Batsman") == 0)
        return (p.battingAverage * p.strikeRate) / 100.0f;
    if (strcmp(p.role, "Bowler") == 0)
        return (p.wickets * 2) + (100.0f - p.economyRate);
    if (strcmp(p.role, "All-rounder") == 0)
        return ((p.battingAverage * p.strikeRate) / 100.0f) + (p.wickets * 2);
    return 0.0f;
}


int getRoleIndex(const char role[])
{
    if (strcmp(role, "Batsman") == 0) return 0;
    if (strcmp(role, "Bowler") == 0) return 1;
    if (strcmp(role, "All-rounder") == 0) return 2;
    return -1;
}


/* Insert a player into the team's role list in descending Performance Index. */
void insertPlayerSorted(int teamIndex, ODIPlayer player)
{
    int roleIndex = getRoleIndex(player.role);
    if (roleIndex < 0 || teamIndex < 0 || teamIndex >= teamCount) return;

    PlayerNode *newNode = (PlayerNode *)malloc(sizeof(PlayerNode));
    if (newNode == NULL)
    {
        printf("Memory allocation failed. Player ranking list was not updated.\n");
        return;
    }
    newNode->player = player;
    newNode->next = NULL;

    PlayerNode **head = &rolePlayerLists[teamIndex][roleIndex];
    if (*head == NULL || player.performanceIndex > (*head)->player.performanceIndex)
    {
        newNode->next = *head;
        *head = newNode;
    }
    else
    {
        PlayerNode *current = *head;
        while (current->next != NULL &&
               current->next->player.performanceIndex >= player.performanceIndex)
            current = current->next;
        newNode->next = current->next;
        current->next = newNode;
    }
}


/* Recalculate batting SR using batsmen and all-rounders only. */
void updateTeamAverageStrikeRate(int teamIndex)
{
    float totalStrikeRate = 0.0f;
    int battingPlayerCount = 0;

    for (int i = 0; i < playerCount; i++)
    {
        if (strcmp(playerList[i].teamName, teamList[teamIndex].name) == 0 &&
            (strcmp(playerList[i].role, "Batsman") == 0 ||
             strcmp(playerList[i].role, "All-rounder") == 0))
        {
            totalStrikeRate += playerList[i].strikeRate;
            battingPlayerCount++;
        }
    }

    teamList[teamIndex].averageBattingStrikeRate = battingPlayerCount > 0
        ? totalStrikeRate / battingPlayerCount : 0.0f;
}


/* Initialize the teams and predefined players from Players_data.h. */
void initializeData(void)
{
    (void)headerPlayerCount;
    (void)headerTeamCount;
    int totalDataPlayers = sizeof(players) / sizeof(players[0]);

    for (int i = 0; i < teamCount; i++)
    {
        teamList[i].teamId = i + 1;
        strcpy(teamList[i].name, teams[i]);
        teamList[i].totalPlayers = 0;
        teamList[i].averageBattingStrikeRate = 0.0f;
    }

    for (int i = 0; i < totalDataPlayers && playerCount < MAX_PLAYERS; i++)
    {
        ODIPlayer p;
        p.playerId = players[i].id;
        strcpy(p.name, players[i].name);
        strcpy(p.teamName, players[i].team);
        strcpy(p.role, players[i].role);
        p.totalRuns = players[i].totalRuns;
        p.battingAverage = players[i].battingAverage;
        p.strikeRate = players[i].strikeRate;
        p.wickets = players[i].wickets;
        p.economyRate = players[i].economyRate;
        p.performanceIndex = calculatePerformanceIndex(p);

        playerList[playerCount++] = p;

        int teamIndex = -1;
        for (int j = 0; j < teamCount; j++)
        {
            if (strcmp(teamList[j].name, p.teamName) == 0)
            {
                teamIndex = j;
                break;
            }
        }
        if (teamIndex != -1)
        {
            teamList[teamIndex].totalPlayers++;
            insertPlayerSorted(teamIndex, p);
        }
    }

    for (int i = 0; i < teamCount; i++)
        updateTeamAverageStrikeRate(i);
}


/* Binary search by Team ID; teamList is kept in ascending ID order. */
int searchTeamById(int teamId)
{
    int low = 0, high = teamCount - 1;
    while (low <= high)
    {
        int mid = low + (high - low) / 2;
        if (teamList[mid].teamId == teamId) return mid;
        if (teamList[mid].teamId < teamId) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}


/* Add a player to the selected team. */
void addPlayer(void)
{
    int teamId, teamIndex, roleChoice;
    if (playerCount >= MAX_PLAYERS)
    {
        printf("Player storage is full.\n");
        return;
    }

    printf("Enter Team ID to add player: ");
    if (scanf("%d", &teamId) != 1) { printf("Invalid input.\n"); return; }
    teamIndex = searchTeamById(teamId);
    if (teamIndex == -1) { printf("Invalid Team ID.\n"); return; }
    if (teamList[teamIndex].totalPlayers >= 50)
    { printf("This team has reached the maximum of 50 players.\n"); return; }

    ODIPlayer newPlayer;
    printf("\nEnter Player Details:\nPlayer ID: ");
    if (scanf("%d", &newPlayer.playerId) != 1) { printf("Invalid Player ID.\n"); return; }
    if (newPlayer.playerId < 1 || newPlayer.playerId > 1000)
    { printf("Player ID must be between 1 and 1000.\n"); return; }
    for (int i = 0; i < playerCount; i++)
    {
        if (playerList[i].playerId == newPlayer.playerId)
        { printf("Player ID already exists.\n"); return; }
    }

    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) { }
    printf("Name: ");
    if (fgets(newPlayer.name, sizeof(newPlayer.name), stdin) == NULL) { printf("Invalid name.\n"); return; }
    newPlayer.name[strcspn(newPlayer.name, "\n")] = '\0';
    if (strlen(newPlayer.name) < 1 || strlen(newPlayer.name) > 50)
    { printf("Name must contain 1 to 50 characters.\n"); return; }

    printf("Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    if (scanf("%d", &roleChoice) != 1) { printf("Invalid role.\n"); return; }
    if (roleChoice == 1) strcpy(newPlayer.role, "Batsman");
    else if (roleChoice == 2) strcpy(newPlayer.role, "Bowler");
    else if (roleChoice == 3) strcpy(newPlayer.role, "All-rounder");
    else { printf("Invalid role.\n"); return; }

    printf("Total Runs: "); scanf("%d", &newPlayer.totalRuns);
    printf("Batting Average: "); scanf("%f", &newPlayer.battingAverage);
    printf("Strike Rate: "); scanf("%f", &newPlayer.strikeRate);
    printf("Wickets: "); scanf("%d", &newPlayer.wickets);
    printf("Economy Rate: "); scanf("%f", &newPlayer.economyRate);

    strcpy(newPlayer.teamName, teamList[teamIndex].name);
    newPlayer.performanceIndex = calculatePerformanceIndex(newPlayer);

    playerList[playerCount++] = newPlayer;
    teamList[teamIndex].totalPlayers++;
    insertPlayerSorted(teamIndex, newPlayer);
    updateTeamAverageStrikeRate(teamIndex);

    printf("\nPlayer added successfully to Team %s!\n", teamList[teamIndex].name);
}


/* Display players belonging to a specific team and team totals. */
void displayPlayersOfTeam(void)
{
    int teamId;
    printf("Enter Team ID: ");
    if (scanf("%d", &teamId) != 1) { printf("Invalid input.\n"); return; }
    int teamIndex = searchTeamById(teamId);
    if (teamIndex == -1) { printf("Invalid Team ID.\n"); return; }

    printf("\nPlayers of Team %s:\n\n", teamList[teamIndex].name);
    printf("-------------------------------------------------------------------------------------------------------------\n");
    printf("%-6s %-25s %-15s %-10s %-10s %-10s %-8s %-8s %-12s\n",
           "ID", "Name", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf.Index");
    printf("-------------------------------------------------------------------------------------------------------------\n");
git status    for (int i = 0; i < playerCount; i++)
    {
        if (strcmp(playerList[i].teamName, teamList[teamIndex].name) == 0)
        {
            printf("%-6d %-25s %-15s %-10d %-10.1f %-10.1f %-8d %-8.1f %-12.2f\n",
                   playerList[i].playerId, playerList[i].name, playerList[i].role,
                   playerList[i].totalRuns, playerList[i].battingAverage,
                   playerList[i].strikeRate, playerList[i].wickets,
                   playerList[i].economyRate, playerList[i].performanceIndex);
        }
    }
    printf("-------------------------------------------------------------------------------------------------------------\n");
    printf("Total Players: %d\n", teamList[teamIndex].totalPlayers);
    printf("Average Batting Strike Rate: %.2f\n", teamList[teamIndex].averageBattingStrikeRate);
}


/* Sort a copy of teams by average batting strike rate descending. */
void displayTeamsByStrikeRate(void)
{
    Team sortedTeams[MAX_TEAMS], temp;
    for (int i = 0; i < teamCount; i++) sortedTeams[i] = teamList[i];

    for (int i = 0; i < teamCount - 1; i++)
    {
        for (int j = 0; j < teamCount - i - 1; j++)
        {
            if (sortedTeams[j].averageBattingStrikeRate < sortedTeams[j + 1].averageBattingStrikeRate)
            {
                temp = sortedTeams[j];
                sortedTeams[j] = sortedTeams[j + 1];
                sortedTeams[j + 1] = temp;
            }
        }
    }

    printf("\nTeams Sorted by Average Batting Strike Rate (Descending):\n\n");
    printf("----------------------------------------------------------\n");
    printf("%-10s %-20s %-15s %-15s\n", "Team ID", "Team Name", "Players", "Avg Batting SR");
    printf("----------------------------------------------------------\n");
    for (int i = 0; i < teamCount; i++)
    {
        printf("%-10d %-20s %-15d %-15.2f\n",
               sortedTeams[i].teamId, sortedTeams[i].name,
               sortedTeams[i].totalPlayers, sortedTeams[i].averageBattingStrikeRate);
    }
    printf("----------------------------------------------------------\n");
}


/* Display the top K players from a team's selected role list. */
void displayTopKPlayers(void)
{
    int teamId, roleChoice, k;
    printf("Enter Team ID: ");
    if (scanf("%d", &teamId) != 1) { printf("Invalid input.\n"); return; }
    int teamIndex = searchTeamById(teamId);
    if (teamIndex == -1) { printf("Invalid Team ID.\n"); return; }

    printf("\nSelect Role:\n1. Batsman\n2. Bowler\n3. All-rounder\nEnter choice: ");
    if (scanf("%d", &roleChoice) != 1 || roleChoice < 1 || roleChoice > 3)
    { printf("Invalid role choice.\n"); return; }
    printf("Enter K: ");
    if (scanf("%d", &k) != 1 || k <= 0) { printf("K must be greater than 0.\n"); return; }

    PlayerNode *current = rolePlayerLists[teamIndex][roleChoice - 1];
    printf("\nTop %d Players of Team %s:\n\n", k, teamList[teamIndex].name);
    printf("---------------------------------------------------------------\n");
    printf("%-6s %-25s %-15s %-12s\n", "ID", "Name", "Role", "Perf.Index");
    printf("---------------------------------------------------------------\n");
    int displayed = 0;
    while (current != NULL && displayed < k)
    {
        printf("%-6d %-25s %-15s %-12.2f\n",
               current->player.playerId, current->player.name,
               current->player.role, current->player.performanceIndex);
        current = current->next;
        displayed++;
    }
    if (displayed == 0) printf("No players found for this role.\n");
    printf("---------------------------------------------------------------\n");
}


/* Max-heap insertion keyed by Performance Index. */
void heapInsert(HeapNode item)
{
    int i = heapSize++;
    while (i > 0)
    {
        int parent = (i - 1) / 2;
        if (maxHeap[parent].node->player.performanceIndex >= item.node->player.performanceIndex) break;
        maxHeap[i] = maxHeap[parent];
        i = parent;
    }
    maxHeap[i] = item;
}


/* Remove and return the maximum Performance Index heap item. */
HeapNode heapRemove(void)
{
    HeapNode root = maxHeap[0];
    HeapNode last = maxHeap[heapSize - 1];
    heapSize--;
    if (heapSize > 0)
    {
        int i = 0;
        while (2 * i + 1 < heapSize)
        {
            int child = 2 * i + 1;
            if (child + 1 < heapSize &&
                maxHeap[child + 1].node->player.performanceIndex > maxHeap[child].node->player.performanceIndex)
                child++;
            if (last.node->player.performanceIndex >= maxHeap[child].node->player.performanceIndex) break;
            maxHeap[i] = maxHeap[child];
            i = child;
        }
        maxHeap[i] = last;
    }
    return root;
}


/* Merge all team lists for the selected role with a max heap. */
void displayPlayersOfRoleAllTeams(void)
{
    int roleChoice;
    printf("\nSelect Role:\n1. Batsman\n2. Bowler\n3. All-rounder\nEnter choice: ");
    if (scanf("%d", &roleChoice) != 1 || roleChoice < 1 || roleChoice > 3)
    { printf("Invalid role choice.\n"); return; }

    int roleIndex = roleChoice - 1;
    heapSize = 0;
    for (int i = 0; i < teamCount; i++)
    {
        PlayerNode *head = rolePlayerLists[i][roleIndex];
        if (head != NULL)
        {
            HeapNode item = {head, i};
            heapInsert(item);
        }
    }

    printf("\nPlayers of selected role sorted by Performance Index:\n\n");
    printf("--------------------------------------------------------------------------------\n");
    printf("%-6s %-25s %-15s %-15s %-12s\n", "ID", "Name", "Team", "Role", "Perf.Index");
    printf("--------------------------------------------------------------------------------\n");
    while (heapSize > 0)
    {
        HeapNode highest = heapRemove();
        ODIPlayer p = highest.node->player;
        printf("%-6d %-25s %-15s %-15s %-12.2f\n",
               p.playerId, p.name, p.teamName, p.role, p.performanceIndex);
        if (highest.node->next != NULL)
        {
            HeapNode nextItem = {highest.node->next, highest.teamIndex};
            heapInsert(nextItem);
        }
    }
    printf("--------------------------------------------------------------------------------\n");
}


void displayMenu(void)
{
    printf("\n========== ICC ODI PLAYER PERFORMANCE ANALYZER ==========\n");
    printf("1. Add Player to Team\n");
    printf("2. Display Players of a Specific Team\n");
    printf("3. Display Teams by Average Batting Strike Rate\n");
    printf("4. Display Top K Players of a Specific Team by Role\n");
    printf("5. Display All Players of Specific Role Across All Teams by Performance Index\n");
    printf("6. Exit\n");
    printf("======================================================\n");
    printf("Enter your choice: ");
}


int main(void)
{
    int choice;
    initializeData();
    do
    {
        displayMenu();
        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input.\n");
            return 0;
        }
        switch (choice)
        {
            case 1: addPlayer(); break;
            case 2: displayPlayersOfTeam(); break;
            case 3: displayTeamsByStrikeRate(); break;
            case 4: displayTopKPlayers(); break;
            case 5: displayPlayersOfRoleAllTeams(); break;
            case 6: printf("Exiting program.\n"); break;
            default: printf("Invalid choice. Try again.\n");
        }
    } while (choice != 6);
    return 0;
}
