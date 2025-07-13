#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include <openssl/sha.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

/*
command line arguments:
file_name, port_number

argv[0] = file_name
argv[1] = port_number
*/

/*
fork() desc
creates a separate process (child process) - used for multi process programming
required to handle multiple users at once
the child processes has an id of 0 which is returned by the fork() function
*/

/*
this program listens for users connecting to the server and accepts them
and forks the program to create a child process that will hndle the user
the parent process goes back to the listening for new users
*/

typedef struct
{
    char username[65];
    char password[65];
    char date_of_birth[11];
    char favourite_animal[21];
    char account_no[14];
    float balance;

} user_info;

typedef struct
{
    char username[65];
    char password[65];

} admin_info;

void main_menu(int newsockfd);
bool check_unique_username(char username[]);
void generate_acc_no(char account_no[]);
bool check_unique_account_no(char account_no[]);

void user_signup(char command[]);
void user_login(int sockfd, char command[]);
bool check_password(char username[], char password[]);
bool forgot_password(char username[], char date_of_birth[], char favourite_animal[]);
bool change_password(char username[], char new_password[]);






void log_data(char entry[]);

void error(const char *message);


int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        fprintf(stderr, "Port number is not provided. Program terminated.\n");
        exit(1);
    }

    printf("Server running.\n");

    int sockfd, newsockfd, port_number, n;
    char buffer[255];
    struct sockaddr_in server_address, client_address;
    socklen_t client_len;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        error("Error opening socket.");
    }

    bzero((char *) &server_address, sizeof(server_address));      //sets all bytes in the server_address block to be zero

    port_number = atoi(argv[1]);

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port_number);

    if(bind(sockfd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
    {
        error("Binding failed");
    }

    listen(sockfd, 5);
    client_len = sizeof(client_address);


    while (1) 
    {
        newsockfd = accept(sockfd, (struct sockaddr *) &client_address, &client_len);
        if (newsockfd < 0) 
        {
            error("Error on accept.");
        }

        pid_t pid = fork();
        if (pid < 0) 
        {
            error("Fork failed.");
        }

        if (pid == 0) 
        {
            close(sockfd); 

            printf("New client connected.\n");

            FILE *f = fopen("user_database.bin", "ab");   
            fclose(f);
            f = fopen("admin_database.bin", "ab");
            fclose(f);

            main_menu(newsockfd); 
            close(newsockfd);
            exit(0);
        } 
        else 
        {
            close(newsockfd); 
        }
    }

    return 0;
}

/*
command format
[0]   - main command
[20]  - username
[85]  - password
[150] - dateofbirth
[161] - fav ani
[182] - acc no
*/

void main_menu(int sockfd)
{
    printf("Server running.\n");
    while(1)
    {
        char command[256] = {0};
        int n = read(sockfd, &command, 256);
        printf("%s\n", command);

        if(n < 0)
            error("Error on reading.");

        command[strcspn(command, "\n")] = '\0';

        if(strncmp(command, "USERNAME-CHECK", 14) == 0)
        {
            printf("checking username\n");
            bool isUnique = check_unique_username(&command[20]);
            write(sockfd, &isUnique, sizeof(bool));
        }
        else if(strncmp(command, "REQ-ACC-NO", 10) == 0)
        {
            printf("Generating acc no\n");
            char account_no[14];
            generate_acc_no(account_no);
            write(sockfd, &account_no, sizeof(account_no));
        }
        else if(strncmp(command, "SIGNUP", 6) == 0)
        {
            user_signup(command);
            bool response = true;  
            write(sockfd, &response, sizeof(response));
        }
        else if(strncmp(command, "PASS-CHECK", 10) == 0)
        {
            char username[65] = {0};
            char password[65] = {0};
            strcpy(username, &command[20]);
            strcpy(password, &command[85]);
            bool response = check_password(username, password); 
            write(sockfd, &response, sizeof(bool));   
            
            if(response == true)
            {
                printf("Login success.\n");
                char entry[255];
                sprintf(entry, "LOGIN       | username: %s", username);
                log_data(entry);
            } 
        }
        else if(strncmp(command, "FORGOT-PASS", 11) == 0)
        {
            char username[65] = {0};
            char date_of_birth[11];
            char favourite_animal[21];

            strcpy(username, &command[20]);
            strcpy(date_of_birth, &command[85]);
            strcpy(favourite_animal, &command[150]);

            bool response = forgot_password(username, date_of_birth, favourite_animal);
            write(sockfd, &response, sizeof(bool));

        }
        else if(strncmp(command, "CHANGE-PASS", 11) == 0)
        {
            char username[65];
            char new_password[65];
            strcpy(username, &command[20]);
            strcpy(new_password, &command[85]);
            bool response = change_password(username, new_password);
            write(sockfd, &response, sizeof(bool));
        }
            
                  


    }
}


void user_signup(char command[])
{
    printf("SIGNUP\n");
    
    user_info user;
    
    strcpy(user.username, &command[20]);
    strcpy(user.password, &command[85]);
    strcpy(user.date_of_birth, &command[150]);
    strcpy(user.favourite_animal, &command[161]);
    strcpy(user.account_no, &command[182]);
    user.balance = 0.0;

    FILE *f;
    f = fopen("user_database.bin", "ab");

    if(f == NULL)
    {
        error("File opening failed.\n");
    }

    fwrite(&user, 1, sizeof(user_info), f);
    fclose(f);

    char entry[255];
    sprintf(entry, "SIGNUP      | username: %s ", user.username);
    log_data(entry);
}
bool check_unique_username(char username[])
{
    FILE *f;

    f = fopen("user_database.bin", "rb");

    if(f == NULL)
        error("File opening failed.\n");

    user_info user;

    while(fread(&user, sizeof(user_info), 1, f) == 1)
    {
        if(strcmp(user.username, username) == 0)
        {
            fclose(f);
            return false;
        }
            
    }

    fclose(f);
    return true;
}
void generate_acc_no(char account_no[])
{
    LABEL01:
    bzero(account_no, 14);
    srand(time(NULL));
    
    for(int i = 0; i < 13; i++)
    {
        int random_no = rand() % 10;
        account_no[i] = random_no + '0';
    }
    account_no[13] = '\0';

    if(check_unique_account_no(account_no) == false)
        goto LABEL01;

}
bool check_unique_account_no(char account_no[])
{
    FILE *f;

    f = fopen("user_database.bin", "rb");

    if(f == NULL)
        error("File opening failed.\n");

    user_info user;

    while(fread(&user, sizeof(user_info), 1, f) == 1)
    {
        if(strcmp(user.account_no, account_no) == 0)
            return false;
    }
    return true;
}
bool check_password(char username[], char password[])
{
    FILE *f;

    f = fopen("user_database.bin", "rb");

    if(f == NULL)
        error("File opening failed.\n");

    user_info user;

    while(fread(&user, sizeof(user_info), 1, f) == 1)
    {
        if(strcmp(user.username, username) == 0 && strcmp(user.password, password) == 0)
        {
            fclose(f);
            return true;
        }       
    }

    fclose(f);
    return false;
}
bool forgot_password(char username[], char date_of_birth[], char favourite_animal[])
{
    bool response = false;

    FILE *f;
    f = fopen("user_database.bin", "rb");
    if(f == NULL)
        error("File opening failed.\n");

    user_info user;

    while(fread(&user, sizeof(user_info), 1, f) == 1)
    {
        if(strcmp(user.username, username) == 0 && strcmp(user.date_of_birth, date_of_birth) == 0 && strcmp(user.favourite_animal, favourite_animal) == 0)
        {
            response = true;
            printf("MATCH FOUND.\n");
            break;
        }
    }

    fclose(f);

    return response;
    
}
bool change_password(char username[], char new_password[])
{
    
    FILE *f;
    f = fopen("user_database.bin", "r+b");
    if(f == NULL)
        error("File opening failed.\n");

    user_info user;
    bool response = false;

    while(fread(&user, sizeof(user_info), 1, f) == 1)
    {
        if(strcmp(user.username, username) == 0)
        {
            response = true;
            strcpy(user.password, new_password);
            fseek(f, -sizeof(user_info), SEEK_CUR);  
            fwrite(&user, sizeof(user_info), 1, f);

            char entry[255];
            sprintf(entry, "PASSWORD-CH | username: %s", user.username);
            log_data(entry);

            break;
        }
    }

    fclose(f);

    return response;
}





void log_data(char entry[])
{
    time_t now;
    struct tm *current;
    time(&now); 
    current = localtime(&now);

    int year = current->tm_year + 1900;
    int month = current->tm_mon + 1;
    int day = current->tm_mday;
    int hour = current->tm_hour;
    int minute = current->tm_min;
    int second = current->tm_sec;

    FILE *f = fopen("logbook.txt", "a");

    if(f == NULL)
        error("Logbook opening failed");

    fprintf(f, "[%02d-%02d-%04d %02d:%02d:%02d] %s\n", day, month, year, hour, minute, second, entry);

    fclose(f);
}


void error(const char *message)
{
    perror(message);
    exit(1);
}












