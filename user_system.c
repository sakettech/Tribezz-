#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define DB_FILE "users.dat"
#define MAX_STR 64
#define MAX_PATH 256

// User Data Structure
typedef struct {
    int user_id;
    char first_name[MAX_STR];
    char last_name[MAX_STR];
    char email[MAX_STR];
    char username[MAX_STR];
    char password[MAX_STR];
    char profile_pic_path[MAX_PATH];
    char dept[MAX_STR];
    int eco_points;
    int swaps_count;
    int following_count;
} User;

// Function Prototypes
void registerUser();
void loginAndShowProfile();
void searchProfileByUsername();
void displayProfileCard(const User *u);
void updateProfilePic(char *username);
int isUsernameTaken(const char *username);
int isEmailTaken(const char *email);
void cleanInput(char *str);
void clearScreen();

int main() {
    int choice;
    while (1) {
        printf("\n==================================================\n");
        printf("       TRIBEZZ - USER AUTH & PROFILE SYSTEM       \n");
        printf("==================================================\n");
        printf(" [1] Create New Account (Register)\n");
        printf(" [2] Login & View My Profile\n");
        printf(" [3] Search User Profile by Username\n");
        printf(" [4] Update Profile Photo\n");
        printf(" [5] Exit\n");
        printf("--------------------------------------------------\n");
        printf(" Enter your choice (1-5): ");

        if (scanf("%d", &choice) != 1) {
            printf("\n[!] Invalid input! Exiting.\n");
            break;
        }
        getchar(); // consume leftover newline

        switch (choice) {
            case 1:
                registerUser();
                break;
            case 2:
                loginAndShowProfile();
                break;
            case 3:
                searchProfileByUsername();
                break;
            case 4: {
                char uname[MAX_STR];
                printf("\nEnter your username to update photo: ");
                fgets(uname, sizeof(uname), stdin);
                cleanInput(uname);
                updateProfilePic(uname);
                break;
            }
            case 5:
                printf("\nExiting System. Thank you for using EcoFeed! 🌱\n");
                exit(0);
            default:
                printf("\n[!] Invalid option! Please select between 1-5.\n");
        }
    }
    return 0;
}

// ----------------------------------------------------
// Utility: Remove trailing newlines and spaces
// ----------------------------------------------------
void cleanInput(char *str) {
    size_t len = strlen(str);
    if (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r')) {
        str[len - 1] = '\0';
    }
}

// ----------------------------------------------------
// Check if Username already exists in DB
// ----------------------------------------------------
int isUsernameTaken(const char *username) {
    FILE *fp = fopen(DB_FILE, "rb");
    if (!fp) return 0; // File does not exist yet

    User temp;
    while (fread(&temp, sizeof(User), 1, fp)) {
        if (strcasecmp(temp.username, username) == 0) {
            fclose(fp);
            return 1; // Already taken
        }
    }
    fclose(fp);
    return 0;
}

// ----------------------------------------------------
// Check if Email already exists in DB
// ----------------------------------------------------
int isEmailTaken(const char *email) {
    FILE *fp = fopen(DB_FILE, "rb");
    if (!fp) return 0;

    User temp;
    while (fread(&temp, sizeof(User), 1, fp)) {
        if (strcasecmp(temp.email, email) == 0) {
            fclose(fp);
            return 1; // Already registered
        }
    }
    fclose(fp);
    return 0;
}

// ----------------------------------------------------
// 1. REGISTRATION MODULE
// ----------------------------------------------------
void registerUser() {
    FILE *fp = fopen(DB_FILE, "ab+");
    if (!fp) {
        printf("\n[ERROR] Unable to open database file.\n");
        return;
    }

    User newUser;
    memset(&newUser, 0, sizeof(User));

    // Calculate dynamic auto-increment ID
    fseek(fp, 0, SEEK_END);
    long fileSize = ftell(fp);
    newUser.user_id = (int)(fileSize / sizeof(User)) + 101;

    printf("\n==================================================\n");
    printf("              NEW USER REGISTRATION               \n");
    printf("==================================================\n");

    // First Name
    printf("Enter First Name: ");
    fgets(newUser.first_name, sizeof(newUser.first_name), stdin);
    cleanInput(newUser.first_name);

    // Last Name
    printf("Enter Last Name: ");
    fgets(newUser.last_name, sizeof(newUser.last_name), stdin);
    cleanInput(newUser.last_name);

    // Email
    while (1) {
        printf("Enter Email Address: ");
        fgets(newUser.email, sizeof(newUser.email), stdin);
        cleanInput(newUser.email);

        if (strlen(newUser.email) < 5 || strchr(newUser.email, '@') == NULL) {
            printf("[!] Invalid email format! Please try again.\n");
            continue;
        }
        if (isEmailTaken(newUser.email)) {
            printf("[!] This email is already registered! Use another email.\n");
            continue;
        }
        break;
    }

    // Username
    while (1) {
        printf("Enter Unique Username (e.g. sameer_eco): ");
        fgets(newUser.username, sizeof(newUser.username), stdin);
        cleanInput(newUser.username);

        if (strlen(newUser.username) < 3) {
            printf("[!] Username must be at least 3 characters long.\n");
            continue;
        }
        if (isUsernameTaken(newUser.username)) {
            printf("[!] Username '@%s' is already taken! Choose a different one.\n", newUser.username);
            continue;
        }
        break;
    }

    // Password
    printf("Enter Password: ");
    fgets(newUser.password, sizeof(newUser.password), stdin);
    cleanInput(newUser.password);

    // Department / Course
    printf("Enter Department/Branch (e.g. CSE 3rd Year): ");
    fgets(newUser.dept, sizeof(newUser.dept), stdin);
    cleanInput(newUser.dept);

    // Profile Photo Path / Image URL
    printf("Enter Profile Photo File Path or URL:\n(e.g., /images/avatar.png or https://site.com/pic.jpg): ");
    fgets(newUser.profile_pic_path, sizeof(newUser.profile_pic_path), stdin);
    cleanInput(newUser.profile_pic_path);

    if (strlen(newUser.profile_pic_path) == 0) {
        strcpy(newUser.profile_pic_path, "default_avatar.png");
    }

    // Default initial eco-stats
    newUser.eco_points = 50;       // 50 Welcome bonus points
    newUser.swaps_count = 0;
    newUser.following_count = 0;

    // Save to binary file
    fwrite(&newUser, sizeof(User), 1, fp);
    fclose(fp);

    printf("\n--------------------------------------------------\n");
    printf(" [SUCCESS] Account created successfully for @%s!\n", newUser.username);
    printf(" [BONUS] You received 50 Welcome EcoPoints! 🌱\n");
    printf("--------------------------------------------------\n");
}

// ----------------------------------------------------
// 2. LOGIN & VIEW PROFILE
// ----------------------------------------------------
void loginAndShowProfile() {
    FILE *fp = fopen(DB_FILE, "rb");
    if (!fp) {
        printf("\n[!] No users found in database. Please register first.\n");
        return;
    }

    char uname[MAX_STR], pass[MAX_STR];
    printf("\n--- USER LOGIN ---\n");
    printf("Enter Username: ");
    fgets(uname, sizeof(uname), stdin);
    cleanInput(uname);

    printf("Enter Password: ");
    fgets(pass, sizeof(pass), stdin);
    cleanInput(pass);

    User u;
    int authenticated = 0;

    while (fread(&u, sizeof(User), 1, fp)) {
        if (strcmp(u.username, uname) == 0 && strcmp(u.password, pass) == 0) {
            authenticated = 1;
            break;
        }
    }
    fclose(fp);

    if (authenticated) {
        printf("\n[ACCESS GRANTED] Welcome back, %s!\n", u.first_name);
        displayProfileCard(&u);
    } else {
        printf("\n[ERROR] Invalid Username or Password! Access Denied.\n");
    }
}

// ----------------------------------------------------
// 3. SEARCH PROFILE BY USERNAME (Explore / Search)
// ----------------------------------------------------
void searchProfileByUsername() {
    FILE *fp = fopen(DB_FILE, "rb");
    if (!fp) {
        printf("\n[!] Database is empty.\n");
        return;
    }

    char searchUname[MAX_STR];
    printf("\nEnter Username to Search: ");
    fgets(searchUname, sizeof(searchUname), stdin);
    cleanInput(searchUname);

    User u;
    int found = 0;

    while (fread(&u, sizeof(User), 1, fp)) {
        if (strcasecmp(u.username, searchUname) == 0) {
            found = 1;
            displayProfileCard(&u);
            break;
        }
    }
    fclose(fp);

    if (!found) {
        printf("\n[!] User with username '@%s' not found.\n", searchUname);
    }
}

// ----------------------------------------------------
// 4. DISPLAY INSTAGRAM-STYLE PROFILE CARD
// ----------------------------------------------------
void displayProfileCard(const User *u) {
    printf("\n+-------------------------------------------------------------+\n");
    printf("|                    ECOFEED USER PROFILE                     |\n");
    printf("+-------------------------------------------------------------+\n");
    printf("|  User ID      : #%d\n", u->user_id);
    printf("|  Username     : @%s [Verified \u2713]\n", u->username);
    printf("|  Full Name    : %s %s\n", u->first_name, u->last_name);
    printf("|  Email        : %s\n", u->email);
    printf("|  Department   : %s\n", u->dept);
    printf("|  Photo Path   : %s\n", u->profile_pic_path);
    printf("+-------------------------------------------------------------+\n");
    printf("|                     CAMPUS METRICS                          |\n");
    printf("+-------------------------------------------------------------+\n");
    printf("|   Swaps: %-6d  |  Following: %-6d  |  EcoPoints: %-6d   |\n", 
           u->swaps_count, u->following_count, u->eco_points);
    printf("+-------------------------------------------------------------+\n");
    printf("|  Photo Preview: [Loaded from: %s]\n", u->profile_pic_path);
    printf("+-------------------------------------------------------------+\n");
}

// ----------------------------------------------------
// 5. UPDATE PROFILE PICTURE
// ----------------------------------------------------
void updateProfilePic(char *username) {
    FILE *fp = fopen(DB_FILE, "rb+");
    if (!fp) {
        printf("\n[!] Database file not found.\n");
        return;
    }

    User u;
    int found = 0;

    while (fread(&u, sizeof(User), 1, fp)) {
        if (strcmp(u.username, username) == 0) {
            found = 1;
            char newPath[MAX_PATH];
            printf("\nCurrent Photo Path: %s\n", u.profile_pic_path);
            printf("Enter New Photo Path / Image URL: ");
            fgets(newPath, sizeof(newPath), stdin);
            cleanInput(newPath);

            if (strlen(newPath) > 0) {
                strcpy(u.profile_pic_path, newPath);
                
                // Move file pointer backward to overwrite current struct record
                fseek(fp, -((long)sizeof(User)), SEEK_CUR);
                fwrite(&u, sizeof(User), 1, fp);
                printf("\n[SUCCESS] Profile photo path updated successfully for @%s!\n", username);
            }
            break;
        }
    }
    fclose(fp);

    if (!found) {
        printf("\n[!] User '@%s' not found.\n", username);
    }
}