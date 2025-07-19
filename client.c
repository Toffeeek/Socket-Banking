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
#include <netdb.h>

/*
command line arguments:
file_name, server_ipaddress, port_number

argv[0] = file_name
argv[1] = server_ipaddress
argv[2] = port_number
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

void get_username(char username[], const char prompt[]);
void get_password(char password[], const char prompt[]);
void get_date_of_birth(char date_of_birth[], const char prompt[]);
bool check_valid_date(int birth_date, int birth_month, int birth_year);
void get_favourite_animal(char favourite_animal[], const char prompt[]);
void package_command(char command[], const char seg1[], const char seg2[], const char seg3[], const char seg4[], const char seg5[], const char seg6[], const char seg7[]); 
void sha256(char input[], int mode, char salt[]);
void salter(char input[], char salt[]);

void main_menu(int sockfd);
void user_signup(int sockfd);
void user_input_signup(int sockfd, char username[], char password[], char salt[], char date_of_birth[], char favourite_animal[]);

user_info packet_userinfo_signup(char username[], char password[], char date_of_birth[], char favourite_animal[], char account_no[]);
void user_login(int sockfd);
void user_input_login(int sockfd, char username[]);
void forgot_password(int sockfd, char username[]);
void change_password(int sockfd, char username[]);



void homepage(int sockfd, char username[]);
user_info get_user_info(char username[]);
user_info withdraw(user_info user);
user_info deposit(user_info user);
user_info transfer(user_info user);
user_info withdraw(user_info user);
user_info check_balance(user_info user);
user_info change_account_details(user_info user);
void request_statement(user_info user);




void admin_registration(int sockfd);
void admin_login(int sockfd);


void error(const char *message);
void clear_screen(); 

int main(int argc, char *argv[])
{
    int sockfd, port_number, n;
    struct sockaddr_in server_address;
    struct hostent *server;        //stores information about the given host, ie, host name and ipv4 address

    if(argc < 3)
    {
        fprintf(stderr, "Usage %s hostname port\n", argv[0]);
        exit(1);
    }

    port_number = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        error("Error opening socket.");
    }

    server = gethostbyname(argv[1]);    //IP address of the server
    if(server == NULL)
    {
        fprintf(stderr, "Error! No such host.");
        exit(1);
    }

    bzero((char *) &server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    bcopy((char *) server->h_addr, (char *)&server_address.sin_addr.s_addr, server->h_length);
    server_address.sin_port = htons(port_number);

    if(connect(sockfd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
    {
        error("Connection failed.");
    }


    main_menu(sockfd);


    return 0;
}

void get_username(char username[], const char prompt[])
{
    bzero(username, 65);
    bool valid_username = false;
    while(valid_username == false)
    {
        
        char test_username[25];
        printf("%s", prompt);
        bzero(test_username, 25);
        fgets(test_username, 25, stdin);
        if (test_username[strlen(test_username) - 1] != '\n') 
        {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
        }
        test_username[strcspn(test_username, "\n")] = '\0'; 

        if(strcmp(test_username, "0") == 0)
        {
            strcpy(username, test_username);
            return;
        }
            

        int size = strlen(test_username);

        if(isdigit(test_username[0]))
        {
            clear_screen(); 
            printf("Username cannot start with a digit.\n");
            continue;
        }
        else if(size > 20)
        {
            clear_screen(); 
            printf("Username cannot be over 20 characters.\n");
            continue;
        }
        
        bool fault_found = false;
        for(int i = 0; i < size; i++)
        {
            if(!isdigit(test_username[i]) && !isalpha(test_username[i]))
            {
                clear_screen(); 
                fault_found = true;
                printf("Username cannot contain special characters or spaces.\n");
                break;
            } 
            else if(test_username[i] >= 'A' && test_username[i] <= 'Z') 
            {
                clear_screen(); 
                fault_found = true;
                printf("Username cannot contain uppercase characters.\n");
                break;
            }       
        }

        if(fault_found == true)
            continue;
        else
        {
            strcpy(username, test_username);
            valid_username = true;
        }      
    }
}
void get_password(char password[], const char prompt[])
{
    bzero(password, 65);
    bool valid_password = false;

    while(valid_password == false)
    {
        char test_password[25];
        printf("%s", prompt);
        bzero(test_password, 25);
        fgets(test_password, 25, stdin);
        if (test_password[strlen(test_password) - 1] != '\n') 
        {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
        }
        test_password[strcspn(test_password, "\n")] = '\0'; 

        if(strcmp(test_password, "0") == 0)
        {
            strcpy(password, test_password);
            return;
        }

        int size = strlen(test_password);

        if(size > 20)
        {
            clear_screen(); 
            printf("Password cannot be over 20 characters.\n");
            continue;
        }
        if(size < 7)
        {
            clear_screen(); 
            printf("Password needs to be atleast 8 characters long.\n");
            continue;
        }

        bool fault_found = false;
        for(int i = 0; i < size; i++)
        {
            if(test_password[0] == ' ' || test_password[size - 1] == ' ')
            {
                clear_screen(); 
                fault_found = true;
                printf("Password cannot contain spaces in the beginning or end.\n");
                break;
            }

            if(test_password[i] == '\t' || test_password[i] == '\n' || test_password[i] == '\0' || test_password[i] == '\b')
            {
                clear_screen(); 
                fault_found = true;
                printf("Password cannot contain control characters.\n");
                break;
            }    
        }

        if(fault_found == true)
            continue;
        else
        {
            strcpy(password, test_password);
            valid_password = true;
        }          
    }
}
void get_date_of_birth(char date_of_birth[],  const char prompt[])
{
    bzero(date_of_birth, 11);
    bool valid_dob = false;

    while(valid_dob == false)
    {
        char test_dob[15];
        printf("%s", prompt);
        bzero(test_dob, 15);
        fgets(test_dob, 15, stdin);
        if (test_dob[strlen(test_dob) - 1] != '\n') 
        {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
        }
        test_dob[strcspn(test_dob, "\n")] = '\0'; 

        if(strcmp(test_dob, "0") == 0)
        {
            strcpy(date_of_birth, test_dob);
            return;
        }

        int size = strlen(test_dob);

        if(size != 10)
        {
            clear_screen(); 
            printf("Invalid format.\n");
            continue;
        }

        bool fault_found = false;
        for(int i = 0; i < 10; i++)
        {
            if(test_dob[2] != '-' || test_dob[5] != '-')
            {
                clear_screen(); 
                fault_found = true;
                printf("Invalid format.\n");
                break;
            }

            if(test_dob[i] - '\0' == '\t' || test_dob[i] - '\0' == '\n' || test_dob[i] - '\0' == '\0' || test_dob[i] - '\0' == '\b')
            {
                clear_screen(); 
                fault_found = true;
                printf("Date of birth cannot contain control characters.\n");
                break;
            }            
        }

        int birth_date = 0, birth_month = 0, birth_year = 0;
        int age;

        int multiplier = 10;
        for(int i = 0; i <= 1; i++)
        {
            birth_date += (test_dob[i] - '0') * multiplier;
            multiplier /= 10;
        }

        multiplier = 10;
        for(int i = 3; i <= 4; i++)
        {
            birth_month += (test_dob[i] - '0') * multiplier;
            multiplier /= 10;
        }
        
        multiplier = 1000;
        for(int i = 6; i <= 9; i++)
        {
            birth_year += (test_dob[i] - '0') * multiplier;
            multiplier /= 10;
        }

        if(check_valid_date(birth_date, birth_month, birth_year) == false)
        {
            clear_screen(); 
            fault_found = true;
            printf("Invalid date/month.\n");
            continue;
        }

        time_t now;
        struct tm *current;
        time(&now); 
        current = localtime(&now);

        int current_year = current->tm_year + 1900;
        int current_month = current->tm_mon + 1;
        int current_day =current->tm_mday;


        if(current_month < birth_month)
        {
            age = current_year - birth_year - 1;
        }
        else if(current_month == birth_month && current_day <= birth_date)
        {
            age = current_year - birth_year - 1;
        }
        else
        {
            age = current_year - birth_year;
        }
        
        if(age < 0)
        {
            clear_screen(); 
            printf("Invalid date.\n");
            fault_found = true;
            continue;
        }
        else if(age < 18)
        {
            clear_screen(); 
            printf("You are not old enough to create an account.\n");
            fault_found = true;
            continue;
        }

        if(fault_found == true)
            continue;
        else
        {
            strcpy(date_of_birth, test_dob);
            valid_dob = true;
        }           
    }
}
bool check_valid_date(int birth_date, int birth_month, int birth_year)
{
    int max_date;

    bool leap_year = false;

    if((birth_year % 400) == 0)
        leap_year = true;
    if((birth_year % 4) == 0 && (birth_year % 100) != 0)
        leap_year = true;

    switch (birth_month)
    {
        case  4: 
        case  6:
        case  9:
        case 11: max_date = 30;
                 break;

        case  2: max_date = 28;
                 break;

        default: max_date = 31;
    }

    if(leap_year && birth_month == 2)
        max_date = 29;

    if(birth_date <= 0 || birth_date > max_date)
        return false;

    if(birth_month <= 0 || birth_month > 12)
        return false;

    return true;  
}
void get_favourite_animal(char favourite_animal[], const char prompt[])
{
    bzero(favourite_animal, 21);
    bool valid_animal = false;

    while(valid_animal == false)
    {
        char test_animal[25];
        printf("%s", prompt);
        bzero(test_animal, 25);
        fgets(test_animal, 25, stdin);
        if (test_animal[strlen(test_animal) - 1] != '\n') 
        {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
        }
        test_animal[strcspn(test_animal, "\n")] = '\0'; 

        if(strcmp(test_animal, "0") == 0)
        {
            strcpy(favourite_animal, test_animal);
            return;
        }

        int size = strlen(test_animal);
        
        bool fault_found = false;
        for(int i = 0; i < size; i++)
        {
            if(!isalpha(test_animal[i]) && test_animal[i] != '-')
            {
                clear_screen(); 
                fault_found = true;
                printf("Animal name cannot contain non-alphabetic characters or non-dashes.\n");
                break;
            }  
        }

        if(test_animal[0] == '-' || test_animal[size - 1] == '-')
        {
            clear_screen(); 
            printf("Animal name cannot contain dashes at the beginning or end.\n");
            fault_found = true;
        }

        if(fault_found == true)
            continue;
        else
        {
            strcpy(favourite_animal, test_animal);
            valid_animal = true;
        }    
    }
}
void package_command(char command[], const char seg1[], const char seg2[], const char seg3[], const char seg4[], const char seg5[], const char seg6[], const char seg7[])
{
    bzero(command, 256);
    strcpy(&command[0], seg1);         //max 18 - main cmd
    strcpy(&command[20], seg2);        //max 64 - username
    strcpy(&command[85], seg3);        //max 64 - pass
    strcpy(&command[150], seg4);       //max 10 - dob
    strcpy(&command[161], seg5);       //max 20 - fav ani
    strcpy(&command[182], seg6);       //max 13 - acc no
    strcpy(&command[196], seg7);       //       - salt
}

void sha256(char input[], int mode, char salt[]) 
{
    // mode 0 for usernames and 1 for passwords

    unsigned char hash[SHA256_DIGEST_LENGTH];
    
    // ensures that all the bytes after the 20th byte are nulls
    bzero(input + 21, 44);

    if(mode == 1)
        salter(input, salt);
    else if(mode == 2)
        strcat(input, salt);

    // hashes the entire 65-byte buffer 
    SHA256((unsigned char *)input, 65, hash);

    // overwrites input with the hex string representation 
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) 
    {
        sprintf(&input[i * 2], "%02x", hash[i]);
    }

    input[64] = '\0'; 
}
void salter(char input[], char salt[])
{
    char salt_letters[] = "QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm";
    int size = strlen(salt_letters);
    srand(time(NULL));

    for(int i = 0; i < 16; i++)
    {
        int letter_index = rand() % size;
        salt[i] = salt_letters[letter_index];
    }

    strcat(input, salt);
}
void main_menu(int sockfd)
{
    printf("Welcome to Jashim Bank.\n");

    while(1)
    {
        int choice;
        char command[21];

        printf("1. Login as existing user.\n"
               "2. Signup as new user.\n"
               "3. Register as an admin.\n"
               "4. Login as admin.\n"
               "Please select your desired option (1-3): ");
        scanf("%d", &choice);
        getchar();

        switch(choice)
        {
            case  1:    clear_screen(); 
                        user_login(sockfd);
                        break;

            case  2:    clear_screen(); 
                        user_signup(sockfd);
                        break;

            case  3:    clear_screen(); 
                        break;

            case  4:    clear_screen(); 
                        break;

            default:    clear_screen(); 
                        printf("Invalid choice. Try again.\n");
        }


    }
}

void user_signup(int sockfd)
{
    char command[256];
    char username[65] = {0}, password[65] = {0}, salt[17] = {0}, date_of_birth[11], favourite_animal[21], account_no[14];
    bool response;


    user_input_signup(sockfd, username, password, salt, date_of_birth, favourite_animal);

    if(strcmp(username, "0") == 0)
        return;

    package_command(command, "REQ-ACC-NO", "", "", "", "", "", "");

    int n = write(sockfd, command, sizeof(command));
    if(n < 0)
            error("Error on writing.");

    n = read(sockfd, &account_no, sizeof(account_no));
    if(n < 0)
            error("Error on writing.");

    printf("Generated acc no\n");

    //clear_screen();

    package_command(command, "SIGNUP", username, password, date_of_birth, favourite_animal, account_no, salt);

    write(sockfd, command, sizeof(command));

    read(sockfd, &response, sizeof(response));
    if(response == true)
    {
        printf("Account creation successful. Your account number is %s\n\n", account_no);
    }
    else
    {
        printf("Account creation failed.\n");
    }
}
void user_input_signup(int sockfd, char username[], char password[], char salt[], char date_of_birth[], char favourite_animal[])
{

    LABEL01:
    get_date_of_birth(date_of_birth, "Enter your date of birth [DD-MM-YYYY] (0 to go back): ");
    if(strcmp(date_of_birth, "0") == 0)
    {
        clear_screen();
        return;
    }

    LABEL02:
    bzero(username, 65);
    char command[256] = {0};
    bool response = false;

    while(response == false)
    {
        get_username(username, "Set username (0 to go back): ");
        if(strcmp(username, "0") == 0)
        {
            goto LABEL01;
        }
        sha256(username, 0, username);
        package_command(command, "USERNAME-CHECK", username, "", "", "", "", "");
        printf("%s\n", command);
        printf("%s\n", &command[20]);
        write(sockfd, &command, sizeof(command));
        read(sockfd, &response, sizeof(bool));
        if(response == false)
        {
            printf("Username taken already, try a different username.\n");
        }
    }

    printf("Username validated.\n");

    

    LABEL03:
    get_password(password, "Set password (0 to go back): ");
    if(strcmp(password, "0") == 0)
    {
        clear_screen();
        goto LABEL02;
    }
    sha256(password, 1, salt);

    

    get_favourite_animal(favourite_animal, "What is your favourite animal? (0 to go back): ");
    if(strcmp(favourite_animal, "0") == 0)
    {
        clear_screen();
        goto LABEL03;
    }

    printf("Salt: %s\n", salt);
}



void user_login(int sockfd)
{
    char username[65] = {0};
    
    user_input_login(sockfd, username);

    if(strcmp(username, "0") == 0)
        return;

    printf("Login initiated.\n");

    homepage(sockfd, username);

}
void user_input_login(int sockfd, char username[])
{
    LABEL01:
    
    char command[256];
    bool response;
    get_username(username, "Enter username (0 to go back): ");
    if(strcmp(username, "0") == 0)
    {
        clear_screen();
        return;
    }
    
    sha256(username, 0, username);

    package_command(command, "USERNAME-CHECK", username, "", "", "", "", "");
    printf("%s\n", command);
    printf("%s\n", &command[20]);
    write(sockfd, &command, sizeof(command));
    read(sockfd, &response, sizeof(bool));
    if(response == true)
    {
        printf("Username does not exist.\n");
        goto LABEL01;
    }

    printf("Username validated.\n");

    LABEL02:
    
    int choice;
    printf("1. Enter password.\n"
           "2. Forgot/Change password\n"
           "Please select an option (0 to go back): ");
    scanf("%d", &choice);
    getchar();


    LABEL03:

    char password[65] = {0};
    switch (choice)
    {
        case 1: {
                    get_password(password, "Enter password (0 to go back): ");
                    if(strcmp(password, "0") == 0)
                    {
                        clear_screen();
                        goto LABEL02;
                    }
                
                    package_command(command, "PASS-CHECK", username, "", "", "", "", "");
                    write(sockfd, &command, sizeof(command));
                    char salt[17];
                    read(sockfd, &salt, sizeof(salt));
                    sha256(password, 2, salt);
                    write(sockfd, &password, sizeof(password));
                    read(sockfd, &response, sizeof(bool));
                    if(response == false)
                    {
                        clear_screen();
                        printf("Password incorrect. Try again\n");
                        goto LABEL03;
                    }
                    else
                        break;
                }

        case 2: forgot_password(sockfd, username);
                goto LABEL02;
                break;

        case 0: goto LABEL01;

        default:clear_screen();
                printf("Invalid option\n");
                goto LABEL02;
    }
}
void forgot_password(int sockfd, char username[])
{
    LABEL01:
    char command[256];
    bool response;
    char date_of_birth[21] = {0}, favourite_animal[21] ={0};
    get_date_of_birth(date_of_birth, "Enter your date of birth [DD-MM-YYYY] (0 to go back): ");
    if(strcmp(date_of_birth, "0") == 0)
    {
        clear_screen();
        return;
    }

    
    get_favourite_animal(favourite_animal, "Enter your favourite animal (0 to go back): ");
    if(strcmp(favourite_animal, "0") == 0)
    {
        clear_screen();
        goto LABEL01;
    }
    
    package_command(command, "FORGOT-PASS", username, date_of_birth, favourite_animal, "", "", "");

    write(sockfd, &command, sizeof(command));
    read(sockfd, &response, sizeof(bool));

    if(response == false)
    {
        clear_screen(); 
        printf("Date of birth and/or favourite animal does not match. Try again.\n");
        goto LABEL01;  
    }
        
    change_password(sockfd, username);     
    
}
void change_password(int sockfd, char username[])
{
    bool response;
    char command[256];
    char new_password[65] = {0};
    char salt[17] = {0};
    get_password(new_password, "Enter new password (0 to go back): ");
    if(strcmp(new_password, "0") == 0)
    {
        return;
    }

    sha256(new_password, 1, salt);

    package_command(command, "CHANGE-PASS", username, new_password, "", "", "", salt);
    write(sockfd, &command, sizeof(command));

    read(sockfd, &response, sizeof(response));
    clear_screen();

    if(response == true)
    {
        printf("Password updated. Returning to login screen.\n\n");
    }
    else
    {   
        printf("Password update failed. Client not found. Returning to login screen.\n\n");
    }
}



void homepage(int sockfd, char username[])
{
    user_info user = get_user_info(username);
    printf("Welcome user\n\n");
    int choice;

    while(1)
    {
        printf("1. Withdraw money\n"
               "2. Deposit money\n"
               "3. Transfer to another account\n"
               "4. Check Balance\n"
               "5. Change Account Details\n"
               "6. Request Bank Statement\n"
               "7. Logout\n"
               "Please select an operation (1-7): ");
        scanf("%d", &choice);
        getchar();

        switch (choice)
        {
            case  1:    user = withdraw(user);
                        break;
            case  2:    user = deposit(user);
                        break;

            case  3:    user = transfer(user);
                        break;

            case  4:    clear_screen();
                        check_balance(user);
                        break;

            case  5:    clear_screen();
                        user = change_account_details(user);
                        break;

            case  6:    request_statement(user);

            case  7:    clear_screen();
                        break;

            default:    printf("Invalid choice.\n");
        }
    }
}
user_info get_user_info(char useraname[])
{

}
user_info withdraw(user_info user)
{

}
user_info deposit(user_info user)
{

}
user_info transfer(user_info user)
{

}
user_info check_balance(user_info user)
{

}
user_info change_account_details(user_info user)
{

}
void request_statement(user_info user)
{

}




void admin_registration(int sockfd)
{

}
void admin_login(int sockfd)
{

}




void error(const char *message)
{
    perror(message);
    exit(1);
}
void clear_screen()
{
    system("clear");
}















