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
    for (int i = 0; i < teamCount; i++) {
        PlayerInfo *cur = teamList[i].head;
        while (cur) {
            PlayerInfo *nxt = cur->next;
            free(cur);
            cur = nxt;
        }
    }
    free(teamList);
    teamList = NULL;
}

static char *trim(char *s) {
    while (isspace((unsigned char)*s)) s++;
    int len = (int)strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1])) s[--len] = '\0';
    return s;
}

static int readLine(char *buf, int size) {
    if (!fgets(buf, size, stdin)) {
        printf("\nInput ended. Exiting.\n");
        freeAll();
        exit(0);
    }
    int len = (int)strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') {
        buf[--len] = '\0';
        return 1;
    }
    int c, extra = 0;
    while ((c = getchar()) != '\n' && c != EOF) extra = 1;
    return extra ? 0 : 1;
}

static int readInt(const char *msg, int low, int high) {
    char buf[LINE_SIZE];
    while (1) {
        printf("%s", msg);
        int ok = readLine(buf, sizeof(buf));
        char *s = trim(buf);
        char *end;
        errno = 0;
        long val = strtol(s, &end, 10);
        if (ok && *s != '\0' && *end == '\0' && errno == 0 && val >= low && val <= high)
            return (int)val;
        printf("Invalid input. Enter a whole number from %d to %d.\n", low, high);
    }
}

static float readFloat(const char *msg, float low, float high) {
    char buf[LINE_SIZE];
    while (1) {
        printf("%s", msg);
        int ok = readLine(buf, sizeof(buf));
        char *s = trim(buf);
        char *end;
        errno = 0;
        double val = strtod(s, &end);
        if (ok && *s != '\0' && *end == '\0' && errno == 0 && isfinite(val) && val >= low && val <= high)
            return (float)val;
        printf("Invalid input. Enter a number from %.1f to %.1f.\n", low, high);
    }
}

static void readName(char *out) {
    char buf[LINE_SIZE];
    while (1) {
        printf("Name: ");
        int ok = readLine(buf, sizeof(buf));
        char *s = trim(buf);
        int len = (int)strlen(s);
        if (ok && len >= 1 && len <= MAX_NAME) {
            strcpy(out, s);
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
    for (int i = 0; i < teamCount; i++)
        for (PlayerInfo *p = teamList[i].head; p; p = p->next)
            if (p->id == id) return 1;
    return 0;
}

static float findPerf(int role, float avg, float sr, int wkts, float eco) {
    if (role == BAT) return avg * sr / 100;
    if (role == BOWL) return wkts * 2 + (100 - eco);
    return avg * sr / 100 + wkts * 2;
}

static void attachToTeam(Team *t, PlayerInfo *p) {
    if (t->tail) t->tail->next = p;
    else t->head = p;
    t->tail = p;
    t->total++;
    if (p->role != BOWL) {
        t->batCount++;
        t->srSum += p->sr;
    }
}

static void insertSorted(Team *t, PlayerInfo *p) {
    PlayerInfo **cur = &t->roleHead[p->role];
    while (*cur && (*cur)->perf >= p->perf) cur = &(*cur)->teamRoleNext;
    p->teamRoleNext = *cur;
    *cur = p;

    cur = &allRoleHead[p->role];
    while (*cur && (*cur)->perf >= p->perf) cur = &(*cur)->allRoleNext;
    p->allRoleNext = *cur;
    *cur = p;
}

static void addPlayer(Team *t, PlayerInfo *p) {
    attachToTeam(t, p);
    insertSorted(t, p);
}

static PlayerInfo *makePlayer(int id, const char *name, const char *teamName, int role,
                              int runs, float avg, float sr, int wkts, float eco) {
    PlayerInfo *p = (PlayerInfo *)calloc(1, sizeof(PlayerInfo));
    if (!p) {
        printf("Memory allocation failed.\n");
        freeAll();
        exit(1);
    }
    p->id = id;
    snprintf(p->name, sizeof(p->name), "%s", name);
    p->teamName = teamName;
    p->role = role;
    p->runs = runs;
    p->avg = avg;
    p->sr = sr;
    p->wkts = wkts;
    p->eco = eco;
    p->perf = findPerf(role, avg, sr, wkts, eco);
    p->order = nextOrder++;
    return p;
}

static int roleFromText(const char *text) {
    if (strcmp(text, "Batsman") == 0) return BAT;
    if (strcmp(text, "Bowler") == 0) return BOWL;
    if (strcmp(text, "All-rounder") == 0) return ALL;
    return 0;
}

static int comparePerf(const void *a, const void *b) {
    const PlayerInfo *x = *(PlayerInfo *const *)a;
    const PlayerInfo *y = *(PlayerInfo *const *)b;
    if (x->perf > y->perf) return -1;
    if (x->perf < y->perf) return 1;
    if (x->order < y->order) return -1;
    if (x->order > y->order) return 1;
    return 0;
}

static Team *findTeamByName(const char *name) {
    for (int i = 0; i < teamCount; i++)
        if (strcmp(teamList[i].name, name) == 0) return &teamList[i];
    return NULL;
}

static void loadData(void) {
    teamList = (Team *)calloc(teamCount, sizeof(Team));
    if (!teamList) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    for (int i = 0; i < teamCount; i++) {
        teamList[i].id = i + 1;
        snprintf(teamList[i].name, sizeof(teamList[i].name), "%s", teams[i]);
    }
    PlayerInfo **loaded = (PlayerInfo **)malloc(playerCount * sizeof(PlayerInfo *));
    if (!loaded) {
        printf("Memory allocation failed.\n");
        freeAll();
        exit(1);
    }
    int loadedCount = 0, skipped = 0;
    for (int i = 0; i < playerCount; i++) {
        Team *t = NULL;
        for (int j = 0; j < teamCount; j++)
            if (strcmp(teamList[j].name, players[i].team) == 0) t = &teamList[j];
        int role = roleFromText(players[i].role);
        if (!t || !role || t->total >= MAX_TEAM_PLAYERS || idExists(players[i].id)) {
            skipped++;
            continue;
        }
        PlayerInfo *p = makePlayer(players[i].id, players[i].name, t->name, role,
                                   players[i].totalRuns, players[i].battingAverage,
                                   players[i].strikeRate, players[i].wickets, players[i].economyRate);
        attachToTeam(t, p);
        loaded[loadedCount++] = p;
    }
    qsort(loaded, loadedCount, sizeof(PlayerInfo *), comparePerf);
    for (int i = loadedCount - 1; i >= 0; i--) {
        PlayerInfo *p = loaded[i];
        Team *t = findTeamByName(p->teamName);
        p->teamRoleNext = t->roleHead[p->role];
        t->roleHead[p->role] = p;
        p->allRoleNext = allRoleHead[p->role];
        allRoleHead[p->role] = p;
    }
    free(loaded);
    if (skipped > 0)
        printf("Warning: %d predefined player record(s) were skipped.\n", skipped);
}

static void printLine(int width) {
    for (int i = 0; i < width; i++) putchar('=');
    putchar('\n');
}

static void printHeader(int showTeam) {
    printLine(showTeam ? 105 : 90);
    printf("%-6s %-22s ", "ID", "Name");
    if (showTeam) printf("%-14s ", "Team");
    printf("%-12s %-8s %-6s %-6s %-5s %-5s %s\n", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf.Index");
    printLine(showTeam ? 105 : 90);
}

static void printPlayer(const PlayerInfo *p, int showTeam) {
    printf("%-6d %-22s ", p->id, p->name);
    if (showTeam) printf("%-14s ", p->teamName);
    printf("%-12s %-8d %-6.1f %-6.1f %-5d %-5.1f %.2f\n",
           roleName[p->role], p->runs, p->avg, p->sr, p->wkts, p->eco, p->perf);
}

static double teamAvgSr(const Team *t) {
    return t->batCount ? t->srSum / t->batCount : 0.0;
}

static void addPlayerMenu(void) {
    int teamId = readInt("Enter Team ID to add player: ", 1, teamCount);
    Team *t = findTeam(teamId);
    if (!t) {
        printf("Team ID %d not found.\n", teamId);
        return;
    }
    if (t->total >= MAX_TEAM_PLAYERS) {
        printf("Team %s already has %d players. Cannot add more.\n", t->name, MAX_TEAM_PLAYERS);
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

    addPlayer(t, makePlayer(id, name, t->name, role, runs, avg, sr, wkts, eco));
    printf("Player added successfully to Team %s!\n", t->name);
}

static void showTeamPlayers(void) {
    int teamId = readInt("Enter Team ID: ", 1, teamCount);
    Team *t = findTeam(teamId);
    if (!t) {
        printf("Team ID %d not found.\n", teamId);
        return;
    }
    printf("Players of Team %s:\n", t->name);
    if (t->total == 0) {
        printf("No players in this team.\n");
        return;
    }
    printHeader(0);
    for (PlayerInfo *p = t->head; p; p = p->next) printPlayer(p, 0);
    printLine(90);
    printf("Total Players: %d\n", t->total);
    printf("Average Batting Strike Rate: %.2f\n", teamAvgSr(t));
}

static void showTeamsBySr(void) {
    Team **order = (Team **)malloc(teamCount * sizeof(Team *));
    if (!order) {
        printf("Memory allocation failed.\n");
        return;
    }
    for (int i = 0; i < teamCount; i++) order[i] = &teamList[i];
    for (int i = 1; i < teamCount; i++) {
        Team *key = order[i];
        int j = i - 1;
        while (j >= 0 && teamAvgSr(order[j]) < teamAvgSr(key)) {
            order[j + 1] = order[j];
            j--;
        }
        order[j + 1] = key;
    }
    printf("Teams Sorted by Average Batting Strike Rate\n");
    printLine(55);
    printf("%-5s %-16s %-12s %s\n", "ID", "Team Name", "Avg Bat SR", "Total Players");
    printLine(55);
    for (int i = 0; i < teamCount; i++)
        printf("%-5d %-16s %-12.2f %d\n", order[i]->id, order[i]->name, teamAvgSr(order[i]), order[i]->total);
    printLine(55);
    free(order);
}

static void showTopK(void) {
    int teamId = readInt("Enter Team ID: ", 1, teamCount);
    Team *t = findTeam(teamId);
    if (!t) {
        printf("Team ID %d not found.\n", teamId);
        return;
    }
    int role = readInt("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ", 1, 3);
    int k = readInt("Enter number of players: ", 1, MAX_TEAM_PLAYERS);
    if (!t->roleHead[role]) {
        printf("Team %s has no %s.\n", t->name, rolePlural[role]);
        return;
    }
    int shown = 0;
    for (PlayerInfo *p = t->roleHead[role]; p && shown < k; p = p->teamRoleNext) shown++;
    if (shown < k)
        printf("Team %s has only %d %s. Showing all of them.\n", t->name, shown, rolePlural[role]);
    printf("Top %d %s of Team %s:\n", shown, rolePlural[role], t->name);
    printHeader(0);
    shown = 0;
    for (PlayerInfo *p = t->roleHead[role]; p && shown < k; p = p->teamRoleNext, shown++)
        printPlayer(p, 0);
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
    for (PlayerInfo *p = allRoleHead[role]; p; p = p->allRoleNext) printPlayer(p, 1);
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