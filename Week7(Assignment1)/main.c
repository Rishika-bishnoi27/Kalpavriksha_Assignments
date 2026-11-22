#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include "Players_data.h"

#define BAT 1
#define BOWL 2
#define ALL 3
#define MAX_NAME 50
#define MAX_TEAM_PLAYERS 50
#define MAX_ID 1000
#define MAX_PLAYER_ID 9999
#define LINE_SIZE 256

typedef struct PlayerInfo {
    int id;
    char name[MAX_NAME + 1];
    const char *teamName;
    int role;
    int runs;
    float avg;
    float sr;
    int wkts;
    float eco;
    float perf;
    int order;
    struct PlayerInfo *next;
    struct PlayerInfo *teamRoleNext;
    struct PlayerInfo *allRoleNext;
} PlayerInfo;

typedef struct {
    int id;
    char name[MAX_NAME + 1];
    int total;
    int batCount;
    double srSum;
    PlayerInfo *head;
    PlayerInfo *tail;
    PlayerInfo *roleHead[4];
} Team;

static Team *teamList;
static PlayerInfo *allRoleHead[4];
static int nextOrder;

static const char *roleName[] = {"", "Batsman", "Bowler", "All-Rounder"};
static const char *rolePlural[] = {"", "Batsmen", "Bowlers", "All-rounders"};

static void freeAll(void) {
    if (!teamList) return;
    for (int position = 0; position < teamCount; position++) {
        PlayerInfo *current = teamList[position].head;
        while (current) {
            PlayerInfo *following = current->next;
            free(current);
            current = following;
        }
    }
    free(teamList);
    teamList = NULL;
}

static char *trim(char *text) {
    while (isspace((unsigned char)*text)) text++;
    int length = (int)strlen(text);
    while (length > 0 && isspace((unsigned char)text[length - 1])) text[--length] = '\0';
    return text;
}

static int readLine(char *buffer, int size) {
    if (!fgets(buffer, size, stdin)) {
        printf("\nInput ended. Exiting.\n");
        freeAll();
        exit(0);
    }
    int length = (int)strlen(buffer);
    if (length > 0 && buffer[length - 1] == '\n') {
        buffer[--length] = '\0';
        return 1;
    }
    int letter, extra = 0;
    while ((letter = getchar()) != '\n' && letter != EOF) extra = 1;
    return extra ? 0 : 1;
}

static int readInt(const char *message, int low, int high) {
    char buffer[LINE_SIZE];
    while (1) {
        printf("%s", message);
        int valid = readLine(buffer, sizeof(buffer));
        char *text = trim(buffer);
        char *leftover;
        errno = 0;
        long value = strtol(text, &leftover, 10);
        if (valid && *text != '\0' && *leftover == '\0' && errno == 0 && value >= low && value <= high)
            return (int)value;
        printf("Invalid input. Enter a whole number from %d to %d.\n", low, high);
    }
}

static float readFloat(const char *message, float low, float high) {
    char buffer[LINE_SIZE];
    while (1) {
        printf("%s", message);
        int valid = readLine(buffer, sizeof(buffer));
        char *text = trim(buffer);
        char *leftover;
        errno = 0;
        double value = strtod(text, &leftover);
        if (valid && *text != '\0' && *leftover == '\0' && errno == 0 && isfinite(value) && value >= low && value <= high)
            return (float)value;
        printf("Invalid input. Enter a number from %.1f to %.1f.\n", low, high);
    }
}

static void readName(char *target) {
    char buffer[LINE_SIZE];
    while (1) {
        printf("Name: ");
        int valid = readLine(buffer, sizeof(buffer));
        char *text = trim(buffer);
        int length = (int)strlen(text);
        if (valid && length >= 1 && length <= MAX_NAME) {
            strcpy(target, text);
            return;
        }
        printf("Invalid name. Length must be 1 to %d characters.\n", MAX_NAME);
    }
}

static Team *findTeam(int id) {
    int low = 0, high = teamCount - 1;
    while (low <= high) {
        int mid = (low + high) / 2;
        if (teamList[mid].id == id) return &teamList[mid];
        if (teamList[mid].id < id) low = mid + 1;
        else high = mid - 1;
    }
    return NULL;
}

static int idExists(int id) {
    for (int position = 0; position < teamCount; position++)
        for (PlayerInfo *player = teamList[position].head; player; player = player->next)
            if (player->id == id) return 1;
    return 0;
}

static float findPerf(int role, float avg, float sr, int wkts, float eco) {
    if (role == BAT) return avg * sr / 100;
    if (role == BOWL) return wkts * 2 + (100 - eco);
    return avg * sr / 100 + wkts * 2;
}

static void attachToTeam(Team *team, PlayerInfo *player) {
    if (team->tail) team->tail->next = player;
    else team->head = player;
    team->tail = player;
    team->total++;
    if (player->role != BOWL) {
        team->batCount++;
        team->srSum += player->sr;
    }
}

static void insertSorted(Team *team, PlayerInfo *player) {
    PlayerInfo **current = &team->roleHead[player->role];
    while (*current && (*current)->perf >= player->perf) current = &(*current)->teamRoleNext;
    player->teamRoleNext = *current;
    *current = player;

    current = &allRoleHead[player->role];
    while (*current && (*current)->perf >= player->perf) current = &(*current)->allRoleNext;
    player->allRoleNext = *current;
    *current = player;
}

static void addPlayer(Team *team, PlayerInfo *player) {
    attachToTeam(team, player);
    insertSorted(team, player);
}

static PlayerInfo *makePlayer(int id, const char *name, const char *teamName, int role,
                              int runs, float avg, float sr, int wkts, float eco) {
    PlayerInfo *player = (PlayerInfo *)calloc(1, sizeof(PlayerInfo));
    if (!player) {
        printf("Memory allocation failed.\n");
        freeAll();
        exit(1);
    }
    player->id = id;
    snprintf(player->name, sizeof(player->name), "%s", name);
    player->teamName = teamName;
    player->role = role;
    player->runs = runs;
    player->avg = avg;
    player->sr = sr;
    player->wkts = wkts;
    player->eco = eco;
    player->perf = findPerf(role, avg, sr, wkts, eco);
    player->order = nextOrder++;
    return player;
}

static int roleFromText(const char *text) {
    if (strcmp(text, "Batsman") == 0) return BAT;
    if (strcmp(text, "Bowler") == 0) return BOWL;
    if (strcmp(text, "All-rounder") == 0) return ALL;
    return 0;
}

static int comparePerf(const void *left, const void *right) {
    const PlayerInfo *first = *(PlayerInfo *const *)left;
    const PlayerInfo *second = *(PlayerInfo *const *)right;
    if (first->perf > second->perf) return -1;
    if (first->perf < second->perf) return 1;
    if (first->order < second->order) return -1;
    if (first->order > second->order) return 1;
    return 0;
}

static Team *findTeamByName(const char *name) {
    for (int position = 0; position < teamCount; position++)
        if (strcmp(teamList[position].name, name) == 0) return &teamList[position];
    return NULL;
}

static void loadData(void) {
    teamList = (Team *)calloc(teamCount, sizeof(Team));
    if (!teamList) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    for (int position = 0; position < teamCount; position++) {
        teamList[position].id = position + 1;
        snprintf(teamList[position].name, sizeof(teamList[position].name), "%s", teams[position]);
    }
    PlayerInfo **loaded = (PlayerInfo **)malloc(playerCount * sizeof(PlayerInfo *));
    if (!loaded) {
        printf("Memory allocation failed.\n");
        freeAll();
        exit(1);
    }
    int loadedCount = 0, skipped = 0;
    for (int position = 0; position < playerCount; position++) {
        Team *team = NULL;
        for (int scan = 0; scan < teamCount; scan++)
            if (strcmp(teamList[scan].name, players[position].team) == 0) team = &teamList[scan];
        int role = roleFromText(players[position].role);
        if (!team || !role || team->total >= MAX_TEAM_PLAYERS || idExists(players[position].id)) {
            skipped++;
            continue;
        }
        PlayerInfo *player = makePlayer(players[position].id, players[position].name, team->name, role,
                                   players[position].totalRuns, players[position].battingAverage,
                                   players[position].strikeRate, players[position].wickets, players[position].economyRate);
        attachToTeam(team, player);
        loaded[loadedCount++] = player;
    }
    qsort(loaded, loadedCount, sizeof(PlayerInfo *), comparePerf);
    for (int position = loadedCount - 1; position >= 0; position--) {
        PlayerInfo *player = loaded[position];
        Team *team = findTeamByName(player->teamName);
        player->teamRoleNext = team->roleHead[player->role];
        team->roleHead[player->role] = player;
        player->allRoleNext = allRoleHead[player->role];
        allRoleHead[player->role] = player;
    }
    free(loaded);
    if (skipped > 0)
        printf("Warning: %d predefined player record(s) were skipped.\n", skipped);
}

static void printLine(int width) {
    for (int position = 0; position < width; position++) putchar('=');
    putchar('\n');
}

static void printHeader(int showTeam) {
    printLine(showTeam ? 105 : 90);
    printf("%-6s %-22s ", "ID", "Name");
    if (showTeam) printf("%-14s ", "Team");
    printf("%-12s %-8s %-6s %-6s %-5s %-5s %s\n", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf.Index");
    printLine(showTeam ? 105 : 90);
}

static void printPlayer(const PlayerInfo *player, int showTeam) {
    printf("%-6d %-22s ", player->id, player->name);
    if (showTeam) printf("%-14s ", player->teamName);
    printf("%-12s %-8d %-6.1f %-6.1f %-5d %-5.1f %.2f\n",
           roleName[player->role], player->runs, player->avg, player->sr, player->wkts, player->eco, player->perf);
}

static double teamAvgSr(const Team *team) {
    return team->batCount ? team->srSum / team->batCount : 0.0;
}

static void addPlayerMenu(void) {
    int teamId = readInt("Enter Team ID to add player: ", 1, teamCount);
    Team *team = findTeam(teamId);
    if (!team) {
        printf("Team ID %d not found.\n", teamId);
        return;
    }
    if (team->total >= MAX_TEAM_PLAYERS) {
        printf("Team %s already has %d players. Cannot add more.\n", team->name, MAX_TEAM_PLAYERS);
        return;
    }
    printf("Enter Player Details:\n");
    int id;
    while (1) {
        id = readInt("Player ID: ", 1, MAX_PLAYER_ID);
        if (!idExists(id)) break;
        printf("Player ID %d already exists. Enter a different ID.\n", id);
    }
    char name[MAX_NAME + 1];
    readName(name);
    int role = readInt("Role (1-Batsman, 2-Bowler, 3-All-rounder): ", 1, 3);
    int runs = readInt("Total Runs: ", 0, 100000);
    float avg = readFloat("Batting Average: ", 0, 1000);
    float sr = readFloat("Strike Rate: ", 0, 1000);
    int wkts = readInt("Wickets: ", 0, 10000);
    float eco = readFloat("Economy Rate: ", 0, 36);

    addPlayer(team, makePlayer(id, name, team->name, role, runs, avg, sr, wkts, eco));
    printf("Player added successfully to Team %s!\n", team->name);
}

static void showTeamPlayers(void) {
    int teamId = readInt("Enter Team ID: ", 1, teamCount);
    Team *team = findTeam(teamId);
    if (!team) {
        printf("Team ID %d not found.\n", teamId);
        return;
    }
    printf("Players of Team %s:\n", team->name);
    if (team->total == 0) {
        printf("No players in this team.\n");
        return;
    }
    printHeader(0);
    for (PlayerInfo *player = team->head; player; player = player->next) printPlayer(player, 0);
    printLine(90);
    printf("Total Players: %d\n", team->total);
    printf("Average Batting Strike Rate: %.2f\n", teamAvgSr(team));
}

static void showTeamsBySr(void) {
    Team **order = (Team **)malloc(teamCount * sizeof(Team *));
    if (!order) {
        printf("Memory allocation failed.\n");
        return;
    }
    for (int position = 0; position < teamCount; position++) order[position] = &teamList[position];
    for (int position = 1; position < teamCount; position++) {
        Team *key = order[position];
        int scan = position - 1;
        while (scan >= 0 && teamAvgSr(order[scan]) < teamAvgSr(key)) {
            order[scan + 1] = order[scan];
            scan--;
        }
        order[scan + 1] = key;
    }
    printf("Teams Sorted by Average Batting Strike Rate\n");
    printLine(55);
    printf("%-5s %-16s %-12s %s\n", "ID", "Team Name", "Avg Bat SR", "Total Players");
    printLine(55);
    for (int position = 0; position < teamCount; position++)
        printf("%-5d %-16s %-12.2f %d\n", order[position]->id, order[position]->name, teamAvgSr(order[position]), order[position]->total);
    printLine(55);
    free(order);
}

static void showTopK(void) {
    int teamId = readInt("Enter Team ID: ", 1, teamCount);
    Team *team = findTeam(teamId);
    if (!team) {
        printf("Team ID %d not found.\n", teamId);
        return;
    }
    int role = readInt("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ", 1, 3);
    int limit = readInt("Enter number of players: ", 1, MAX_TEAM_PLAYERS);
    if (!team->roleHead[role]) {
        printf("Team %s has no %s.\n", team->name, rolePlural[role]);
        return;
    }
    int shown = 0;
    for (PlayerInfo *player = team->roleHead[role]; player && shown < limit; player = player->teamRoleNext) shown++;
    if (shown < limit)
        printf("Team %s has only %d %s. Showing all of them.\n", team->name, shown, rolePlural[role]);
    printf("Top %d %s of Team %s:\n", shown, rolePlural[role], team->name);
    printHeader(0);
    shown = 0;
    for (PlayerInfo *player = team->roleHead[role]; player && shown < limit; player = player->teamRoleNext, shown++)
        printPlayer(player, 0);
    printLine(90);
}

static void showRoleAcrossTeams(void) {
    int role = readInt("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ", 1, 3);
    if (!allRoleHead[role]) {
        printf("No %s found in any team.\n", rolePlural[role]);
        return;
    }
    printf("%s of all teams:\n", rolePlural[role]);
    printHeader(1);
    for (PlayerInfo *player = allRoleHead[role]; player; player = player->allRoleNext) printPlayer(player, 1);
    printLine(105);
}

static void showMenu(void) {
    printLine(80);
    printf(" ICC ODI Player Performance Analyzer\n");
    printLine(80);
    printf("1. Add Player to Team\n");
    printf("2. Display Players of a Specific Team\n");
    printf("3. Display Teams by Average Batting Strike Rate\n");
    printf("4. Display Top K Players of a Specific Team by Role\n");
    printf("5. Display all Players of specific role Across All Teams by performance index\n");
    printf("6. Exit\n");
    printLine(80);
}

int main(void) {
    loadData();
    int choice;
    do {
        showMenu();
        choice = readInt("Enter your choice: ", 1, 6);
        switch (choice) {
            case 1: addPlayerMenu(); break;
            case 2: showTeamPlayers(); break;
            case 3: showTeamsBySr(); break;
            case 4: showTopK(); break;
            case 5: showRoleAcrossTeams(); break;
            default: printf("Exiting program.\n");
        }
        printf("\n");
    } while (choice != 6);
    freeAll();
    return 0;
}