#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_USERS 10
#define TEXT 40

typedef struct Content {
    int id;
    char title[TEXT];
    char genre[TEXT];
    int duration;
    struct Content *next;
} Content;

typedef struct {
    int id;
    char name[TEXT];
    int plan;
} User;

typedef struct QueueNode {
    int userId;
    int contentId;
    struct QueueNode *next;
} QueueNode;

typedef struct StackNode {
    char user[TEXT];
    char title[TEXT];
    struct StackNode *next;
} StackNode;

User users[MAX_USERS];
int userCount = 0;
Content *catalog = NULL;
QueueNode *front = NULL, *rear = NULL;
StackNode *top = NULL;
int nextContentId = 1;

const char *planName(int plan) {
    if (plan == 1) return "Basic";
    if (plan == 2) return "Standard";
    return "Premium";
}

int planFee(int plan) {
    if (plan == 1) return 149;
    if (plan == 2) return 299;
    return 499;
}

void loadContent(void) {
    FILE *fp = fopen("ott_content.txt", "r");
    int id, duration;
    char title[TEXT], genre[TEXT];
    if (fp == NULL) return;
    while (fscanf(fp, "%d %39s %39s %d", &id, title, genre, &duration) == 4) {
        Content *n = (Content *)malloc(sizeof(Content));
        if (n == NULL) break;
        n->id = id;
        strcpy(n->title, title);
        strcpy(n->genre, genre);
        n->duration = duration;
        n->next = NULL;
        if (catalog == NULL)
            catalog = n;
        else {
            Content *cur = catalog;
            while (cur->next != NULL) cur = cur->next;
            cur->next = n;
        }
        if (id >= nextContentId) nextContentId = id + 1;
    }
    fclose(fp);
}

void saveContent(void) {
    FILE *fp = fopen("ott_content.txt", "w");
    Content *cur = catalog;
    if (fp == NULL) {
        printf("Error: could not save content file.\n");
        return;
    }
    while (cur != NULL) {
        fprintf(fp, "%d %s %s %d\n", cur->id, cur->title, cur->genre, cur->duration);
        cur = cur->next;
    }
    fclose(fp);
}

void loadUsers(void) {
    FILE *fp = fopen("ott_users.txt", "r");
    int id, plan;
    char name[TEXT];
    if (fp == NULL) return;
    while (userCount < MAX_USERS && fscanf(fp, "%d %39s %d", &id, name, &plan) == 3) {
        users[userCount].id = id;
        strcpy(users[userCount].name, name);
        users[userCount].plan = plan;
        userCount++;
    }
    fclose(fp);
}

void saveUsers(void) {
    FILE *fp = fopen("ott_users.txt", "w");
    if (fp == NULL) {
        printf("Error: could not save user file.\n");
        return;
    }
    for (int i = 0; i < userCount; i++)
        fprintf(fp, "%d %s %d\n", users[i].id, users[i].name, users[i].plan);
    fclose(fp);
}

void appendHistory(const char *action, const char *user, const char *title, int amount) {
    FILE *fp = fopen("ott_history.txt", "a");
    if (fp == NULL) {
        printf("Error: could not write history file.\n");
        return;
    }
    fprintf(fp, "%s | User: %s | Title: %s | Amount: Rs.%d\n", action, user, title, amount);
    fclose(fp);
}

Content *findContent(int id) {
    Content *cur = catalog;
    while (cur != NULL) {
        if (cur->id == id) return cur;
        cur = cur->next;
    }
    return NULL;
}

int findUser(int id) {
    for (int i = 0; i < userCount; i++)
        if (users[i].id == id) return i;
    return -1;
}

void addContent(void) {
    char title[TEXT], genre[TEXT];
    int duration;
    Content *n, *cur;
    printf("Enter title (no spaces): ");
    scanf("%39s", title);
    printf("Enter genre (no spaces): ");
    scanf("%39s", genre);
    printf("Enter duration in minutes: ");
    scanf("%d", &duration);
    if (duration <= 0) {
        printf("Invalid duration.\n");
        return;
    }
    cur = catalog;
    while (cur != NULL) {
        if (strcmp(cur->title, title) == 0) {
            printf("Content already exists in the catalog.\n");
            return;
        }
        cur = cur->next;
    }
    n = (Content *)malloc(sizeof(Content));
    if (n == NULL) return;
    n->id = nextContentId++;
    strcpy(n->title, title);
    strcpy(n->genre, genre);
    n->duration = duration;
    n->next = NULL;
    if (catalog == NULL)
        catalog = n;
    else {
        cur = catalog;
        while (cur->next != NULL) cur = cur->next;
        cur->next = n;
    }
    saveContent();
    printf("Content added successfully with ID %d.\n", n->id);
}

void displayCatalog(void) {
    Content *cur = catalog;
    if (cur == NULL) {
        printf("Catalog is empty.\n");
        return;
    }
    printf("\n--- OTT CONTENT CATALOG ---\n");
    printf("%-4s %-18s %-12s %s\n", "ID", "Title", "Genre", "Duration");
    while (cur != NULL) {
        printf("%-4d %-18s %-12s %d min\n", cur->id, cur->title, cur->genre, cur->duration);
        cur = cur->next;
    }
}

void searchContent(void) {
    char title[TEXT];
    Content *cur = catalog;
    printf("Enter title to search: ");
    scanf("%39s", title);
    while (cur != NULL) {
        if (strcmp(cur->title, title) == 0) {
            printf("Found: ID %d | %s | %s | %d min\n", cur->id, cur->title, cur->genre, cur->duration);
            return;
        }
        cur = cur->next;
    }
    printf("Content not found.\n");
}

void deleteContent(void) {
    int id;
    Content *cur = catalog, *prev = NULL;
    printf("Enter content ID to delete: ");
    scanf("%d", &id);
    while (cur != NULL) {
        if (cur->id == id) {
            if (prev == NULL) catalog = cur->next;
            else prev->next = cur->next;
            printf("Content '%s' deleted.\n", cur->title);
            free(cur);
            saveContent();
            return;
        }
        prev = cur;
        cur = cur->next;
    }
    printf("Content ID not found.\n");
}

void registerUser(void) {
    char name[TEXT];
    int plan;
    if (userCount == MAX_USERS) {
        printf("Subscriber limit reached.\n");
        return;
    }
    printf("Enter subscriber name (no spaces): ");
    scanf("%39s", name);
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].name, name) == 0) {
            printf("Subscriber already registered.\n");
            return;
        }
    }
    printf("Select plan (1-Basic, 2-Standard, 3-Premium): ");
    scanf("%d", &plan);
    if (plan < 1 || plan > 3) {
        printf("Invalid plan.\n");
        return;
    }
    users[userCount].id = userCount + 1;
    strcpy(users[userCount].name, name);
    users[userCount].plan = plan;
    userCount++;
    appendHistory("SUBSCRIBE", name, planName(plan), planFee(plan));
    saveUsers();
    printf("Subscriber registered with ID %d. %s plan fee: Rs.%d\n",
           userCount, planName(plan), planFee(plan));
}

void displayUsers(void) {
    if (userCount == 0) {
        printf("No subscribers registered.\n");
        return;
    }
    printf("\n--- SUBSCRIBER LIST ---\n");
    printf("%-4s %-18s %-10s %s\n", "ID", "Name", "Plan", "Fee");
    for (int i = 0; i < userCount; i++)
        printf("%-4d %-18s %-10s Rs.%d\n", users[i].id, users[i].name,
               planName(users[i].plan), planFee(users[i].plan));
}

void push(const char *user, const char *title) {
    StackNode *n = (StackNode *)malloc(sizeof(StackNode));
    if (n == NULL) return;
    strcpy(n->user, user);
    strcpy(n->title, title);
    n->next = top;
    top = n;
}

void watchContent(void) {
    int uid, cid, u;
    Content *c;
    printf("Enter subscriber ID: ");
    scanf("%d", &uid);
    u = findUser(uid);
    if (u == -1) {
        printf("Subscriber not found.\n");
        return;
    }
    printf("Enter content ID to watch: ");
    scanf("%d", &cid);
    c = findContent(cid);
    if (c == NULL) {
        printf("Content not found.\n");
        return;
    }
    push(users[u].name, c->title);
    appendHistory("WATCH", users[u].name, c->title, 0);
    printf("%s is now watching '%s' (%d min).\n", users[u].name, c->title, c->duration);
}

void enqueue(int uid, int cid) {
    QueueNode *n = (QueueNode *)malloc(sizeof(QueueNode));
    if (n == NULL) return;
    n->userId = uid;
    n->contentId = cid;
    n->next = NULL;
    if (rear == NULL) front = rear = n;
    else {
        rear->next = n;
        rear = n;
    }
}

void addWatchlist(void) {
    int uid, cid;
    printf("Enter subscriber ID: ");
    scanf("%d", &uid);
    if (findUser(uid) == -1) {
        printf("Subscriber not found.\n");
        return;
    }
    printf("Enter content ID to add: ");
    scanf("%d", &cid);
    if (findContent(cid) == NULL) {
        printf("Content not found.\n");
        return;
    }
    enqueue(uid, cid);
    printf("Added to watchlist queue.\n");
}

void displayWatchlist(void) {
    QueueNode *cur = front;
    if (cur == NULL) {
        printf("Watchlist queue is empty.\n");
        return;
    }
    printf("\n--- WATCHLIST QUEUE ---\n");
    while (cur != NULL) {
        int u = findUser(cur->userId);
        Content *c = findContent(cur->contentId);
        if (u != -1 && c != NULL)
            printf("%s -> %s\n", users[u].name, c->title);
        cur = cur->next;
    }
}

void playNext(void) {
    QueueNode *q;
    int u;
    Content *c;
    if (front == NULL) {
        printf("Watchlist queue is empty.\n");
        return;
    }
    q = front;
    front = front->next;
    if (front == NULL) rear = NULL;
    u = findUser(q->userId);
    c = findContent(q->contentId);
    if (u != -1 && c != NULL) {
        push(users[u].name, c->title);
        appendHistory("WATCH", users[u].name, c->title, 0);
        printf("Playing next: %s -> %s (%d min)\n", users[u].name, c->title, c->duration);
    } else {
        printf("Queued content is no longer available.\n");
    }
    free(q);
}

void displayHistory(void) {
    StackNode *cur = top;
    if (cur == NULL) {
        printf("No watch history in this session.\n");
        return;
    }
    printf("\n--- RECENT WATCH HISTORY (latest first) ---\n");
    while (cur != NULL) {
        printf("%s watched %s\n", cur->user, cur->title);
        cur = cur->next;
    }
}

int main(void) {
    int choice;
    setvbuf(stdout, NULL, _IONBF, 0);
    loadContent();
    loadUsers();
    printf("OTT MANAGEMENT SYSTEM\n");
    do {
        printf("\n1. Add content\n2. View catalog\n3. Search content\n");
        printf("4. Delete content\n5. Register subscriber\n6. View subscribers\n");
        printf("7. Watch content\n8. Add to watchlist\n9. View watchlist\n");
        printf("10. Play next from watchlist\n11. View watch history\n12. Exit\n");
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) break;
        switch (choice) {
            case 1: addContent(); break;
            case 2: displayCatalog(); break;
            case 3: searchContent(); break;
            case 4: deleteContent(); break;
            case 5: registerUser(); break;
            case 6: displayUsers(); break;
            case 7: watchContent(); break;
            case 8: addWatchlist(); break;
            case 9: displayWatchlist(); break;
            case 10: playNext(); break;
            case 11: displayHistory(); break;
            case 12: saveContent(); saveUsers();
                     printf("Records saved. Exiting...\n"); break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 12);
    return 0;
}