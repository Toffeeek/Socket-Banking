#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/time.h>
#include <time.h>
#include <openssl/sha.h>
#include <sys/file.h>
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
    char salt[17];
    char date_of_birth[11];
    char favourite_animal[21];
    char account_no[14];
    float balance;

} user_info;

typedef struct
{
    char username[65];
    char password[65];
    char date_of_birth[11];
    char favourite_animal[21];
    char account_no[14];
    float balance;

} user_info_package;


typedef struct
{
    char username[65];
    char password[65];

} admin_info;

void main_menu(int newsockfd);
int check_unique_username(char username[]);
void generate_acc_no(char account_no[]);
bool check_unique_account_no(char account_no[]);

void user_signup(char command[]);
void user_login(int sockfd, char command[]);
void send_salt(int sockfd, char username[]);
bool check_password(char username[], char password[]);
bool forgot_password(char username[], char date_of_birth[], char favourite_animal[]);
bool change_password(char username[], char new_password[], char salt[]);

user_info_package get_user_info(char username[]);
bool withdraw(char username[], float withdraw_amount);
bool deposit(char username[], float deposit_amount);
void generate_trxid(char trxid[], char username[], const char type);
bool change_username(char old_username[], char new_username[]);
bool change_date_of_birth(char username[], char new_date_of_birth[]);
bool change_favourite_animal(char username[], char new_favourite_animal[]);

void long_to_base62(long input, char output[]);
void generate_time(int *day, int *month, int *year, int *hour, int *minute, int *second, int *millisecond);
void log_transaction(char trxid[], char sender[], const char receiver[], float amount, int status);
void log_activity(char entry[]);
void error(const char message[]);


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
[196] - salt
*/

void main_menu(int sockfd)
{
    printf("Server running.\n");
    while(1)
    {
        char command[256] = {0};
        int n = read(sockfd, command, 256);
        printf("%s\n", command);

        if(n < 0)
            error("Error on reading.");

        command[strcspn(command, "\n")] = '\0';

        if(strncmp(command, "USERNAME-CHECK", 14) == 0)
        {
            printf("checking username\n");
            char username[65];
            strcpy(username, &command[20]);
            printf("Username: %s\n", username);
            bool response = check_unique_username(username);
            
            write(sockfd, &response, sizeof(bool));     
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
           strcpy(username, &command[20]);
           send_salt(sockfd, username);
           char password[65] = {0};
           read(sockfd, &password, sizeof(password));

           bool response = check_password(username, password); 
           write(sockfd, &response, sizeof(bool));   
            
           if(response == true)
           {
                printf("Login success.\n");
                char entry[255];
                sprintf(entry, "LOGIN       | username: %s", username);
                log_activity(entry); 
                
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
            char salt[17];
            strcpy(username, &command[20]);
            strcpy(new_password, &command[85]);
            strcpy(salt, &command[196]);
            bool response = change_password(username, new_password, salt);
            write(sockfd, &response, sizeof(bool));

            if(response == true)
            {
                    printf("Pass change success.\n");
                    char entry[255];
                    sprintf(entry, "PASS-CH     | username: %s", username);
                    log_activity(entry); 
                    
            } 

        }
        else if(strncmp(command, "GET-USER-INFO", 13) == 0)
        {
            char username[65];
            strcpy(username, &command[20]);
            user_info_package user = get_user_info(username);
            write(sockfd, &user, sizeof(user_info_package));
        }
        else if(strncmp(command, "WITHDRAW", 8) == 0)
        {
            char username[65];
            float withdraw_amount;
            strcpy(username, &command[20]);
            read(sockfd, &withdraw_amount, sizeof(float));
            bool response = withdraw(username, withdraw_amount);
            write(sockfd, &response, sizeof(bool));

            if(response == true)
            {
                char trxid[20] = {0};
                generate_trxid(trxid, username, 'A');   
                log_transaction(trxid, username, "", withdraw_amount, 1); 
            }
        }  
        else if(strncmp(command, "DEPOSIT", 8) == 0)
        {
            char username[65];
            float deposit_amount;
            strcpy(username, &command[20]);
            read(sockfd, &deposit_amount, sizeof(float));
            bool response = deposit(username, deposit_amount);
            write(sockfd, &response, sizeof(bool));

            if(response == true)
            {
                char trxid[20] = {0};
                generate_trxid(trxid, username, 'B');   
                log_transaction(trxid, username, "", deposit_amount, 1); 
            }
            if(response == false)
            {
                printf("failed to write data\n");
            }
        } 
        else if(strncmp(command, "LOGOUT", 6) == 0)
        {
            char username[65];
            
            strcpy(username, &command[20]);
            
            char entry[255];
            sprintf(entry, "LOGOUT      | username: %s", username);
            log_activity(entry);         
        } 
        else if(strncmp(command, "CHANGE-USERNAME", 15) == 0)
        {
            char old_username[65];
            char new_username[65];
            strcpy(old_username, &command[20]);
            strcpy(new_username, &command[85]);

            bool response = change_username(old_username, new_username);
            write(sockfd, &response, sizeof(bool));  

            if(response == true)
            {
                printf("username changed successfully\n");
                char entry[255];
                sprintf(entry, "USERNAME-CH | old username: %s | new username: %s", old_username, new_username);
                log_activity(entry); 
                    
            }
            if(response == false)
            {
                printf("failed to write data\n");
            }
        } 
        else if(strncmp(command, "CHANGE-DOB", 10) == 0)
        {
            char username[65];
            char new_date_of_birth[65];
            strcpy(username, &command[20]);
            strcpy(new_date_of_birth, &command[85]);

            bool response = change_date_of_birth(username, new_date_of_birth);
            write(sockfd, &response, sizeof(bool));  

            if(response == true)
            {
                printf("dob changed successfully\n");
                char entry[255];
                sprintf(entry, "DOB-CH      | username: %s | new date of birth: %s", username, new_date_of_birth);
                log_activity(entry); 
                    
            }
            if(response == false)
            {
                printf("failed to write data\n");
            }
        }
        else if(strncmp(command, "CHANGE-FAVANI", 13) == 0)
        {
            char username[65];
            char new_favourite_animal[65];
            strcpy(username, &command[20]);
            strcpy(new_favourite_animal, &command[85]);

            bool response = change_favourite_animal(username, new_favourite_animal);
            write(sockfd, &response, sizeof(bool));  

            if(response == true)
            {
                printf("username changed successfully\n");
                char entry[255];
                sprintf(entry, "FAVANI-CH   | username: %s | new favourite animal: %s", username, new_favourite_animal);
                log_activity(entry); 
                    
            }
            if(response == false)
            {
                printf("failed to write data\n");
            }
        } 


        
            
    
        
       
        else
            printf("Invalid command.\n");    
    
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
    strcpy(user.salt, &command[196]);
    user.balance = 0.0;

    FILE *f;
    f = fopen("user_database.bin", "ab");

    if(f == NULL)
    {
        error("File opening failed.\n");
    }

    int fd = fileno(f);

    if (flock(fd, LOCK_EX) != 0) 
    {
        fclose(f);
        error("flock() failed.\n");
    }

    fwrite(&user, 1, sizeof(user_info), f);

    flock(fd, LOCK_UN);
    fclose(f);

    char entry[255];
    sprintf(entry, "SIGNUP      | username: %s ", user.username);
    log_activity(entry);
}
int check_unique_username(char username[])
{
    FILE *f;

    f = fopen("user_database.bin", "rb");
    if(f == NULL)
        error("File opening failed.\n");

    int fd = fileno(f);

    if (flock(fd, LOCK_SH) != 0) 
    {
        fclose(f);
        error("flock() failed.\n");
    }

    user_info user;
    int response = 1;

    while(fread(&user, sizeof(user_info), 1, f) == 1)
    {
        if(strcmp(user.username, username) == 0)
        {
            response = 0;
        }
            
    }

    fclose(f);
    flock(fd, LOCK_UN);
    return response;
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

    int fd = fileno(f);

    if (flock(fd, LOCK_SH) != 0) 
    {
        fclose(f);
        error("flock() failed.\n");
    }

    

    user_info user;
    bool response = true;
    while(fread(&user, sizeof(user_info), 1, f) == 1)
    {
        if(strcmp(user.account_no, account_no) == 0)
        {
            response = false;
            break;
        }
            
    }

    fclose(f);
    flock(fd, LOCK_UN);
    return response;
}
void send_salt(int sockfd, char username[])
{
    FILE *f;

    f = fopen("user_database.bin", "rb");

    if(f == NULL)
        error("File opening failed.\n");

    int fd = fileno(f);

    if(flock(fd, LOCK_SH) != 0) 
    {
        fclose(f);
        error("flock() failed.\n");
    }

    

    user_info user;
    char salt[17];

    while(fread(&user, sizeof(user_info), 1, f) == 1)
    {
        if(strcmp(user.username, username) == 0)
        {
            strcpy(salt, user.salt);
            break;
        }       
    }

    flock(fd, LOCK_UN);
    fclose(f);

    printf("Salt: %s\n", salt);

    write(sockfd, &salt, sizeof(salt));



}
bool check_password(char username[], char password[])
{
    FILE *f;

    f = fopen("user_database.bin", "rb");

    if(f == NULL)
        error("File opening failed.\n");

    int fd = fileno(f);

    if (flock(fd, LOCK_SH) != 0) 
    {
        fclose(f);
        error("flock() failed.\n");
    }

    printf("username: %s\n"
            "password: %s\n", username, password);

    

    user_info user;
    bool response = false;

    while(fread(&user, sizeof(user_info), 1, f) == 1)
    {
        if(strcmp(user.username, username) == 0 && strcmp(user.password, password) == 0)
        {
            response = true;
        }       
    }

    fclose(f);
    flock(fd, LOCK_UN);
    return response;
}
bool forgot_password(char username[], char date_of_birth[], char favourite_animal[])
{
    bool response = false;

    FILE *f;
    f = fopen("user_database.bin", "rb");
    if(f == NULL)
        error("File opening failed.\n");

    int fd = fileno(f);

    if (flock(fd, LOCK_SH) != 0) 
    {
        fclose(f);
        error("flock() failed.\n");
    }

    

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

    flock(fd, LOCK_UN);
    fclose(f);

    return response;
    
}
bool change_password(char username[], char new_password[], char salt[])
{
    
    FILE *f;
    f = fopen("user_database.bin", "r+b");
    if(f == NULL)
        error("File opening failed.\n");

    int fd = fileno(f);

    if (flock(fd, LOCK_EX) != 0) 
    {
        fclose(f);
        error("flock() failed.\n");
    }

    

    user_info user;
    bool response = false;

    while(fread(&user, sizeof(user_info), 1, f) == 1)
    {
        if(strcmp(user.username, username) == 0)
        {
            response = true;
            strcpy(user.password, new_password);
            strcpy(user.salt, salt);
            fseek(f, -sizeof(user_info), SEEK_CUR);  
            fwrite(&user, sizeof(user_info), 1, f);

            break;
        }
    }


    flock(fd, LOCK_UN);
    fclose(f);

    return response;
}

user_info_package get_user_info(char username[])
{
    printf("username: %s\n", username);
    FILE *f;

    f = fopen("user_database.bin", "rb");
    if(f == NULL)
        error("File opening failed.\n");

    int fd = fileno(f);

    if (flock(fd, LOCK_SH) != 0) 
    {
        fclose(f);
        error("flock() failed.\n");
    }

    user_info user;
    user_info_package target;
    

    while(fread(&user, sizeof(user_info), 1, f) == 1)
    {
        if(strcmp(user.username, username) == 0)
        {
            strcpy(target.username, user.username);
            strcpy(target.password, user.username);
            strcpy(target.date_of_birth, user.date_of_birth);
            strcpy(target.favourite_animal, user.favourite_animal);
            strcpy(target.account_no, user.account_no);
            target.balance = user.balance;
            
            break;
        }
            
    }

    flock(fd, LOCK_UN);
    fclose(f);
    
    return target;
}
bool withdraw(char username[], float withdraw_amount)
{
    FILE *f;
    f = fopen("user_database.bin", "r+b");
    if(f == NULL)
        error("File opening failed.\n");

    int fd = fileno(f);

    if (flock(fd, LOCK_EX) != 0) 
    {
        fclose(f);
        error("flock() failed.\n");
    }

    user_info users;
    bool response = false;

    while(fread(&users, sizeof(user_info), 1, f) == 1)
    {
        if(strcmp(users.username, username) == 0)
        {    
            if(users.balance >= withdraw_amount)
            {
                users.balance -= withdraw_amount;

                fseek(f, -sizeof(user_info), SEEK_CUR);  
                fwrite(&users, sizeof(user_info), 1, f);       
                response = true;   
                break;
            }
            else
                break;
            
        }
    }

    flock(fd, LOCK_UN);
    fclose(f); 
    return response;
}
bool deposit(char username[], float deposit_amount)
{
    printf("username: %s\n", username);
    printf("deposit amount: %f\n", deposit_amount);


    FILE *f;
    f = fopen("user_database.bin", "r+b");
    if(f == NULL)
        error("File opening failed.\n");

    int fd = fileno(f);

    if (flock(fd, LOCK_EX) != 0) 
    {
        fclose(f);
        error("flock() failed.\n");
    }

    user_info users;
    bool response = false;

    while(fread(&users, sizeof(user_info), 1, f) == 1)
    {
        if(strcmp(users.username, username) == 0)
        {
            response = true;
            users.balance += deposit_amount;
            fseek(f, -sizeof(user_info), SEEK_CUR);
            fwrite(&users, sizeof(user_info), 1, f);
            
            break;
        }
    }

    flock(fd, LOCK_UN);
    fclose(f); 
    

    return response;
}
bool transfer()
{
    return true;
}
void generate_trxid(char trxid[], char username[], const char type)
{
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int millisecond;

    long timestamp_b10;
    char timestamp_b62[10];

    char shortened_username[6];
    snprintf(shortened_username, sizeof(shortened_username), "%s", username);
    

    generate_time(&day, &month, &year, &hour, &minute, &second, &millisecond);


    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%02d%02d%04d%02d%02d%02d%03d",day, month, year, hour, minute, second, millisecond);

    timestamp_b10 = strtol(buffer, NULL, 10); 

    long_to_base62(timestamp_b10, timestamp_b62);

    sprintf(trxid, "%c-%s-%s", type, timestamp_b62, shortened_username);
}
bool change_username(char old_username[], char new_username[])
{
    FILE *f;
    f = fopen("user_database.bin", "r+b");
    if(f == NULL)
        error("File opening failed.\n");

    int fd = fileno(f);

    if (flock(fd, LOCK_EX) != 0) 
    {
        fclose(f);
        error("flock() failed.\n");
    }

    

    user_info user;
    bool response = false;

    while(fread(&user, sizeof(user_info), 1, f) == 1)
    {
        if(strcmp(user.username, old_username) == 0)
        {
            strcpy(user.username, new_username);
            fseek(f, -sizeof(user_info), SEEK_CUR);  
            fwrite(&user, sizeof(user_info), 1, f);
            response = true;

            break;
        }
    }


    flock(fd, LOCK_UN);
    fclose(f);
    return response;
}
bool change_date_of_birth(char username[], char new_date_of_birth[])
{
    FILE *f;
    f = fopen("user_database.bin", "r+b");
    if(f == NULL)
        error("File opening failed.\n");

    int fd = fileno(f);

    if (flock(fd, LOCK_EX) != 0) 
    {
        fclose(f);
        error("flock() failed.\n");
    }

    

    user_info user;
    bool response = false;

    while(fread(&user, sizeof(user_info), 1, f) == 1)
    {
        if(strcmp(user.username, username) == 0)
        {
            strcpy(user.date_of_birth, new_date_of_birth);
            fseek(f, -sizeof(user_info), SEEK_CUR);  
            fwrite(&user, sizeof(user_info), 1, f);
            response = true;

            break;
        }
    }


    flock(fd, LOCK_UN);
    fclose(f);
    return response;
}
bool change_favourite_animal(char username[], char new_favourite_animal[])
{
    FILE *f;
    f = fopen("user_database.bin", "r+b");
    if(f == NULL)
        error("File opening failed.\n");

    int fd = fileno(f);

    if (flock(fd, LOCK_EX) != 0) 
    {
        fclose(f);
        error("flock() failed.\n");
    }

    

    user_info user;
    bool response = false;

    while(fread(&user, sizeof(user_info), 1, f) == 1)
    {
        if(strcmp(user.username, username) == 0)
        {
            strcpy(user.favourite_animal, new_favourite_animal);
            fseek(f, -sizeof(user_info), SEEK_CUR);  
            fwrite(&user, sizeof(user_info), 1, f);
            response = true;

            break;
        }
    }


    flock(fd, LOCK_UN);
    fclose(f);
    return response;
}


void long_to_base62(long input, char output[])
{
    const char chars[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    char temp[32];
    int i = 0;

    if (input == 0) 
    {
        strcpy(output, "0");
        return;
    }

    while (input > 0) 
    {
        temp[i++] = chars[input % 62];
        input /= 62;
    }

    temp[i] = '\0';
    
    for (int j = 0; j < i; j++) 
    {
        output[j] = temp[i - j - 1];
    }
    output[i] = '\0';

}
void log_transaction(char trxid[], char sender[], const char receiver[], float amount, int status)
{
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int millisecond;

    generate_time(&day, &month, &year, &hour, &minute, &second, &millisecond);



    FILE *f = fopen("logbook_transactions.log", "a");

    if(f == NULL)
        error("Logbook opening failed");

    int fd = fileno(f);

    if (flock(fd, LOCK_EX) != 0) 
    {
        fclose(f);
        error("flock() failed.\n");
    }

    fprintf(f, "[%02d-%02d-%04d %02d:%02d:%02d:%03d] TRXID: %s | Sender: %65s | Receiver:%65s | Amount: %09.2f | Status: %d\n", day, month, year, hour, minute, second, millisecond, trxid, sender, receiver, amount, status);

    flock(fd, LOCK_UN);
    fclose(f);
}
void generate_time(int *day, int *month, int *year, int *hour, int *minute, int *second, int *millisecond) 
{
    time_t now;
    struct tm *current;
    struct timeval tv;

    gettimeofday(&tv, NULL);
    now = tv.tv_sec;
    current = localtime(&now);

    *year = current->tm_year + 1900;
    *month = current->tm_mon + 1;
    *day = current->tm_mday;
    *hour = current->tm_hour;
    *minute = current->tm_min;
    *second = current->tm_sec;
    *millisecond = tv.tv_usec / 1000;
}
void log_activity(char entry[])
{
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int millisecond;

    generate_time(&day, &month, &year, &hour, &minute, &second, &millisecond);




    FILE *f = fopen("logbook_activity.log", "a");

    if(f == NULL)
        error("Logbook opening failed");

    int fd = fileno(f);

    if (flock(fd, LOCK_EX) != 0) 
    {
        fclose(f);
        error("flock() failed.\n");
    }

    

    fprintf(f, "[%02d-%02d-%04d %02d:%02d:%02d:%03d] %s\n", day, month, year, hour, minute, second, millisecond, entry);

    flock(fd, LOCK_UN);
    fclose(f);
}


void error(const char *message)
{
    perror(message);
    exit(1);
}












