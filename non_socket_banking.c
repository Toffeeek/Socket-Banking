#include <stdio.h>
#include <string.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include <openssl/sha.h>

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

void clear_screen(); 

void get_username(char username[], const char prompt[]);
void get_password(char password[], const char prompt[]);
void get_date_of_birth(char date_of_birth[], const char prompt[]);
void get_favourite_animal(char favourite_animal[], const char prompt[]);
void get_account_no(char account_no[], const char prompt[]);
void sha256(char input[]);

void user_signup();
void user_input_signup(char username[], char password[], char date_of_birth[], char favourite_animal[]);
bool check_valid_date(int birth_date, int birth_month, int birth_year);
void generate_acc_no(char account_no[]);
bool check_unique_username(char username[]);
bool check_unique_account_no(char account_no[]);
user_info packet_userinfo_signup(char username[], char password[], char date_of_birth[], char favourite_animal[], char account_no[]);

void user_login();
void user_input_login(char username[], char password[]);
bool check_password(char username[], char password[]);
void forgot_password(char username[]);
void set_new_password(char username[]);

void homepage(char unhashed_username[], char unhashed_password[]);
user_info get_user_info(char username[]);
user_info withdraw(user_info user, char unhashed_username[], char unhashed_password[]);
user_info deposit(user_info user, char unhashed_username[], char unhashed_password[]);
user_info transfer(user_info user, char unhashed_username[], char unhashed_password[]);
void display_account_info(user_info user, const char unhashed_username[], const char unhashed_password[]);

user_info change_account_details(user_info user, char unhashed_username[], char unhashed_password[]);
user_info change_username(user_info user, char unhashed_username[], char unhashed_password[]);
user_info change_password(user_info user, char unhashed_username[], char unhashed_password[]);
user_info change_date_of_birth(user_info user, char unhashed_username[], char unhashed_password[]);
user_info change_favourite_animal(user_info user, char unhashed_username[], char unhashed_password[]);

void get_admin_username(char username[], const char prompt[]);
void admin_registration();
void admin_login();
void admin_input_login(char username[], char password[]);
bool check_unique_admin_username(char username[]);
bool check_admin_password(char username[], char assword[]);
user_info get_user_by_account_no(char username[], char password[]);

void admin_homepage(char username[], char password[]);
void read_users();
void log_data(char entry[]);
void request_user_datbase();
void request_logbook();

void error(const char message[]);


int main()
{   

    FILE *f = fopen("user_database.bin", "ab");   // creating the user database file if it doesnt exist
    fclose(f);

    f = fopen("admin_database.bin", "ab");       // creating the admin database file if it doesnt exist
    fclose(f);

    printf("Welcome to Jashim Bank.\n");

    while(1)
    {
        int choice1, choice2, choice3;

        printf("1. Login as existing user.\n"
               "2. Signup as new user.\n"
               "3. Register as an admin.\n"
               "4. Login as admin.\n"
               "Please select your desired option (1-3): ");
        scanf("%d", &choice1);
        getchar();

        switch(choice1)
        {
            case  1:    clear_screen(); 
                        user_login();
                        break;

            case  2:    clear_screen(); 
                        user_signup();
                        break;

            case  3:    clear_screen(); 
                        admin_registration();
                        break;

            case  4:    clear_screen(); 
                        admin_login();
                        break;

            default:    clear_screen(); 
                        printf("Invalid choice. Try again.\n");
        }
    }

    return 0;
}

void clear_screen()
{
    system("clear");
}

void get_username(char username[], const char prompt[])
{
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
void get_favourite_animal(char favourite_animal[], const char prompt[])
{
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
void get_account_no(char account_no[], const char prompt[])
{
    bool valid_account_no = false;

    while(valid_account_no == false)
    {
        char test_account_no[20];
        printf("%s", prompt);
        bzero(test_account_no, 20);
        fgets(test_account_no, 20, stdin);
        if (test_account_no[strlen(test_account_no) - 1] != '\n') 
        {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
        }
        test_account_no[strcspn(test_account_no, "\n")] = '\0';
        
        if(strcmp(test_account_no, "0") == 0)
        {
            strcpy(account_no, test_account_no);
            return;
        }

        int size = strlen(test_account_no);

        
        if(size =! 13)
        {
            clear_screen(); 
            printf("Account number must be 13 characters long.\n");
            continue;
        }
        
        bool fault_found = false;
        for(int i = 0; i < size; i++)
        {
            if(!isdigit(test_account_no[i]))
            {
                clear_screen(); 
                fault_found = true;
                printf("Account number must contain digits only.\n");
                break;
            } 
        }

        if(fault_found == true)
            continue;
        else
        {
            strcpy(account_no, test_account_no);
            valid_account_no = true;
        }      
    }
}

void user_input_signup(char username[], char password[], char date_of_birth[], char favourite_animal[])
{
    LABEL01:
    get_username(username, "Set username (0 to go back): ");
    if(strcmp(username, "0") == 0)
    {
        clear_screen();
        main();
    }


    //SERVER CODE HERE
    if(check_unique_username(username) == false)
    {
        clear_screen(); 
        printf("Username already taken.\n");
        goto LABEL01;
    }
    printf("Username validated: %s\n", username);
    sha256(username);

    LABEL02:
    get_date_of_birth(date_of_birth, "Set date of birth in the form DD-MM-YYYY (0 to go back): ");
    if(strcmp(date_of_birth, "0") == 0)
    {
        //clear_screen();
        goto LABEL01;
    }

    printf("Date of birth validated: %s\n", date_of_birth);

    LABEL03:
    get_password(password, "Set password (0 to go back): ");
    if(strcmp(password, "0") == 0)
    {
        //clear_screen();
        goto LABEL02;
    }

    printf("Password validated: %s\n", password);
    sha256(password);

    get_favourite_animal(favourite_animal, "Set favourite animal (0 to go back): ");
    if(strcmp(favourite_animal, "0") == 0)
    {
        //clear_screen();
        goto LABEL03;
    }

    printf("Favourite animal validated: %s\n", favourite_animal);
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
user_info packet_userinfo_signup(char username[], char password[], char date_of_birth[], char favourite_animal[], char account_no[])
{
    user_info user;

    strcpy(user.username, username); 
    strcpy(user.password, password); 
    strcpy(user.date_of_birth, date_of_birth); 
    strcpy(user.favourite_animal, favourite_animal); 
    strcpy(user.account_no, account_no);
    user.balance = 0;
    
    return user;
}
void user_signup()
{
    char username[65] = {0}, password[65] = {0}, date_of_birth[11], favourite_animal[21], account_no[14];
    float balance;

    user_input_signup(username, password, date_of_birth, favourite_animal);
    generate_acc_no(account_no);
    clear_screen();
    printf("Account number: %s.\nSignup successful. Returning to main menu.\n\n", account_no);

    user_info user = packet_userinfo_signup(username, password, date_of_birth, favourite_animal, account_no);

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

void user_login()
{
    char unhashed_username[21] = {0}, unhashed_password[21] = {0};
    user_input_login(unhashed_username, unhashed_password);
    clear_screen();
    
    char entry[255];
    char username[65] = {0};
    strcpy(username, unhashed_username);
    sha256(username);
    sprintf(entry, "LOGIN       | username: %s", username);
    log_data(entry);

    homepage(unhashed_username, unhashed_password);
}
void user_input_login(char unhashed_username[], char unhashed_password[])
{
    LABEL01:
    char username[65] = {0};
    get_username(username, "Enter username (0 to go back): ");
    if(strcmp(username, "0") == 0)
    {
        clear_screen();
        main();
    }

    strcpy(unhashed_username, username);
    sha256(username);
    if(check_unique_username(username) == true)
    {
        printf("Username does not exist.\n");
        goto LABEL01;
    }
    
    LABEL02:
    int choice;
    printf("1. Enter password.\n"
           "2. Forgot/Change password\n"
           "Please select an option (1-2): ");
    scanf("%d", &choice);
    getchar();

    LABEL03:
    char password[65] = {0};
    switch (choice)
    {
        case 1: get_password(password, "Enter password (0 to go back): ");
                if(strcmp(password, "0") == 0)
                {
                    //clear_screen();
                    goto LABEL01;
                }
                strcpy(unhashed_password, password);
                sha256(password);
                if(check_password(username, password) == false)
                {
                    printf("Password incorrect. Try again\n");
                    goto LABEL03;
                }
                else
                    break;

        case 2: forgot_password(username);
                break;

        default:clear_screen();
                printf("Invalid option\n");
                goto LABEL02;
    }
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
void forgot_password(char username[])
{
    LABEL01:
    char date_of_birth[21] = {0}, favourite_animal[21] ={0};
    get_date_of_birth(date_of_birth, "Enter your date of birth in the form DD-MM-YYYY (0 to go back): ");
    if(strcmp(date_of_birth, "0") == 0)
    {
        clear_screen();
        user_login();
    }
    get_favourite_animal(favourite_animal, "Enter your favourite animal (0 to go back): ");
    if(strcmp(favourite_animal, "0") == 0)
    {
        //clear_screen();
        goto LABEL01;
    }

    FILE *f;
    f = fopen("user_database.bin", "rb");
    if(f == NULL)
        error("File opening failed.\n");

    user_info user;

    while(fread(&user, sizeof(user_info), 1, f) == 1)
    {
        if(strcmp(user.username, username) == 0 && strcmp(user.date_of_birth, date_of_birth) == 0 && strcmp(user.favourite_animal, favourite_animal) == 0)
        {
            fclose(f);
            set_new_password(username);
        }
    }

    clear_screen(); 
    printf("Date of birth or favourite animal does not match. Returning to main menu");
    fclose(f);
    main();
}
void set_new_password(char username[])
{
    char new_password[65] = {0};
    get_password(new_password, "Enter new password (0 to go back): ");
    if(strcmp(new_password, "0") == 0)
    {
        clear_screen();
        user_login();
    }
    sha256(new_password);

    FILE *f;
    f = fopen("user_database.bin", "r+b");
    if(f == NULL)
        error("File opening failed.\n");


    user_info user;
    while(fread(&user, sizeof(user_info), 1, f) == 1)
    {
        if(strcmp(user.username, username) == 0)
        {
            strcpy(user.password, new_password);
            fseek(f, -sizeof(user_info), SEEK_CUR);  
            fwrite(&user, sizeof(user_info), 1, f);
            clear_screen(); 
            printf("Password updated. Returning to login screen.\n\n");
            fclose(f);

            char entry[255];
            sprintf(entry, "PASSWORD-CH | username: %s", user.username);
            log_data(entry);

            user_login();
        }
    }

    fclose(f);
}

void homepage(char unhashed_username[], char unhashed_password[])
{
    char username[65] = {0};
    strcpy(username, unhashed_username);
    sha256(username);

    user_info user = get_user_info(username);
    printf("Welcome %s\n\n", unhashed_username);
    int choice;

    while(1)
    {
        printf("1. Withdraw money\n"
               "2. Deposit money\n"
               "3. Transfer to another account\n"
               "4. Check Balance\n"
               "5. View Account Details\n"
               "6. Change Account Details\n"
               "7. Logout\n"
               "Please select an operation (1-7): ");
        scanf("%d", &choice);
        getchar();

        switch (choice)
        {
            case  1:    user = withdraw(user, unhashed_username, unhashed_password);
                        break;
            case  2:    user = deposit(user, unhashed_username, unhashed_password);
                        break;

            case  3:    user = transfer(user, unhashed_username, unhashed_password);
                        break;

            case  4:    clear_screen();
                        printf("Balance: Tk%06.2f\n\n", user.balance);
                        break;

            case  5:    clear_screen();
                        display_account_info(user, unhashed_username, unhashed_password);
                        break;

            case  6:    clear_screen();
                        user = change_account_details(user, unhashed_username, unhashed_password);
                        break;

            case  7:    clear_screen();
                        char entry[255];
                        sprintf(entry, "LOGOUT      | username: %s", user.username);
                        log_data(entry);    
                        main();
                        break;

            default:    printf("Invalid choice.\n");
        }
    }

}
user_info withdraw(user_info user, char unhashed_username[], char unhashed_password[])
{
    clear_screen(); 
    float withdraw_amount;
    while(1)
    {
        printf("Current balance: %06.2f\n", user.balance);
        printf("Enter amount to withdraw (0 to go back): ");
        scanf("%f", &withdraw_amount);
        getchar();

        if(withdraw_amount == 0)
        {
            clear_screen();
            homepage(unhashed_username, unhashed_password);
        }

        if(user.balance >= withdraw_amount && withdraw_amount > 0)
        {
            user.balance -= withdraw_amount;
            break;
        }
        else
        {
            clear_screen();
            printf("Invalid amount. Try again.\n");
        }         
    }

    FILE *f;
    f = fopen("user_database.bin", "r+b");
    if(f == NULL)
        error("File opening failed.\n");

    user_info users;
    while(fread(&users, sizeof(user_info), 1, f) == 1)
    {
        if(strcmp(users.username, user.username) == 0)
        {
            fseek(f, -sizeof(user_info), SEEK_CUR);  
            fwrite(&user, sizeof(user_info), 1, f);
            clear_screen();
            printf("Withdrawal successful. New balance: Tk%06.2f.\n"
                   "Visit a nearby ATM to collect your cash.\n"
                   "Returning to home screen.\n\n", user.balance);
            fclose(f);         
            break;
        }
    }

    char entry[255];
    sprintf(entry, "WITHDRAWAL  | username: %s | amount: tk%06.2f", user.username, withdraw_amount);
    log_data(entry);

    return user;
}
user_info deposit(user_info user, char unhashed_username[], char unhashed_password[])
{
    float deposit_amount;
    while(1)
    {
        clear_screen();
        printf("Enter amount to deposit (0 to go back): ");
        scanf("%f", &deposit_amount);
        getchar();

        if(deposit_amount == 0)
        {
            clear_screen();
            homepage(unhashed_username, unhashed_password);
        }

        if(deposit_amount > 0)
        {
            user.balance += deposit_amount;
            break;
        }
        else
        {
            clear_screen();
            printf("Invalid amount. Try again.\n");
        }     
    }

    FILE *f;
    f = fopen("user_database.bin", "r+b");
    if(f == NULL)
        error("File opening failed.\n");

    user_info users;
    while(fread(&users, sizeof(user_info), 1, f) == 1)
    {
        if(strcmp(users.username, user.username) == 0)
        {
            fseek(f, -sizeof(user_info), SEEK_CUR);  
            fwrite(&user, sizeof(user_info), 1, f);
            clear_screen();
            printf("Deposition successful. New balance: Tk%06.2f.\n"
                   "Visit a nearby ATM to deposit your cash.\n"
                   "Returning to home screen.\n\n", user.balance);
            fclose(f);         
            break;
        }
    }

    char entry[255];
    sprintf(entry, "DEPOSITION  | username: %s | amount: tk%06.2f", user.username, deposit_amount);
    log_data(entry);

    return user;
}
user_info transfer(user_info user, char unhashed_username[], char unhashed_password[])
{
    float transfer_amount;
    char transferee_account_no[14] = {0};

    while(1)
    {
        LABEL01:
        clear_screen();
        printf("Current balance: Tk%06.2f\n", user.balance);
        printf("Enter the amount to be be transferred: ");
        scanf("%f", &transfer_amount);
        getchar();

        if(transfer_amount == 0)
        {
            clear_screen();
            homepage(unhashed_username, unhashed_password);
        }

        if(user.balance >= transfer_amount && transfer_amount > 0)
        {
            user.balance -= transfer_amount;
            break;
        }
        else
        {
            clear_screen();
            printf("Invalid amount. Try again.\n");
        }
            
    }

    while(1)
    {
        get_account_no(transferee_account_no, "Enter account number of the transferee: ");
        if(strcmp(transferee_account_no, "0") == 0)
        {
            goto LABEL01;
        }
        if(check_unique_account_no(transferee_account_no) == true)
        {
            clear_screen();
            printf("Account does not exist.Try again.\n");
        }
                   
        else if(strcmp(user.account_no, transferee_account_no) == 0)
        {
            clear_screen();
            printf("Cannot transfer money to own account. Try a different account.\n");
        }
            
        else
            break;
    }

    FILE *f;
    f = fopen("user_database.bin", "r+b");
    if(f == NULL)
        error("File opening failed.\n");

    user_info users, transferee;
    int n = 0;
    while(fread(&users, sizeof(user_info), 1, f) == 1 || n < 2)
    {
        if(strcmp(users.username, user.username) == 0)
        {
            fseek(f, -sizeof(user_info), SEEK_CUR);  
            fwrite(&user, sizeof(user_info), 1, f);    
            n++;   
        }
        else if(strcmp(users.account_no, transferee_account_no) == 0)
        {
            transferee = users;
            transferee.balance += transfer_amount;
            fseek(f, -sizeof(user_info), SEEK_CUR);  
            fwrite(&transferee, sizeof(user_info), 1, f);       
            n++;
        }
    }
    clear_screen();
    printf("Transfer successful. New balance: Tk%06.2f.\nReturning to home screen.\n", user.balance);     
    
    char entry[255];
    sprintf(entry, "TRANSFER    | username: %s | amount: tk%06.2f | transferee account no: %s", user.username, transfer_amount, transferee_account_no);
    log_data(entry);

    return user;
    
}
void display_account_info(user_info user, const char unhashed_username[], const char unhashed_password[])
{
    if(unhashed_username[0] == '\0' && unhashed_password[0] == '\0')
        printf("Username        : %s\n"
            "Password        : %s\n"
            "Date of birth   : %s\n"
            "Balance         : Tk%06.2f\n"
            "Account Number  : %s\n"
            "Favourite Animal: %s\n\n", 
            user.username, user.password, user.date_of_birth, user.balance, user.account_no, user.favourite_animal);
    else
        printf("Username        : %s\n"
            "Password        : %s\n"
            "Date of birth   : %s\n"
            "Balance         : Tk%06.2f\n"
            "Account Number  : %s\n"
            "Favourite Animal: %s\n\n", 
            unhashed_username, unhashed_password, user.date_of_birth, user.balance, user.account_no, user.favourite_animal);
}

user_info change_account_details(user_info user, char unhashed_username[], char unhashed_password[])
{
    while(1)
    {
        display_account_info(user, unhashed_username, unhashed_password);

        int choice;
        printf("1. Change username\n"
               "2. Change password\n"
               "3. Change date of birth\n"
               "4. Change favourite animal\n"
               "5. Quit to home screen.\n"
               "Please select an operation (1-6): ");
               scanf("%d", &choice);
               getchar();

        switch (choice)
        {
            case 1:     clear_screen();
                        user = change_username(user,unhashed_username, unhashed_password);
                        break;

            case 2:     clear_screen();
                        user = change_password(user, unhashed_username, unhashed_password);
                        break;

            case 3:     clear_screen();
                        user = change_date_of_birth(user, unhashed_username, unhashed_password);
                        break;

            case 4:     clear_screen();
                        user = change_favourite_animal(user, unhashed_username, unhashed_password);
                        break;

            case 5:     clear_screen();   
                        return user;

            default:    clear_screen();
                        printf("Invalid choice. Try again(1-5).\n");
        }
        
    }
    
}
user_info change_username(user_info user, char unhashed_username[], char unhashed_password[])
{
    char old_username[65] = {0};
    strcpy(old_username, user.username);
    char new_username[65] = {0};
    while(1)
    {
        get_username(new_username, "Enter new usernname (0 to go back): ");

        if(strcmp(new_username, "0") == 0)
        {
            clear_screen();
            change_account_details(user, unhashed_username, unhashed_password);
        }
        sha256(new_username);
        if(check_unique_username(new_username) == false)
        {
            printf("Username already taken.\n");
        }
        else
            break;
    }

    FILE *f;
    f = fopen("user_database.bin", "r+b");
    if(f == NULL)
        error("File opening failed.\n");

    user_info users;
    while(fread(&users, sizeof(user_info), 1, f) == 1)
    {
        if(strcmp(users.username, user.username) == 0)
        {
            fseek(f, -sizeof(user_info), SEEK_CUR); 
            strcpy(users.username, new_username); 
            strcpy(user.username, new_username); 
            fwrite(&users, sizeof(user_info), 1, f);
            clear_screen();
            printf("Username updated. Returning to home screen");
            fclose(f);         
            break;
        }
    }

    char entry[255];
    sprintf(entry, "USERNAME-CH | old username: %s | new username: %s", old_username, new_username);
    log_data(entry);

    return user;
}
user_info change_password(user_info user, char unhashed_username[], char unhashed_password[])
{
    char new_password[65] = {0};
    get_password(new_password, "Set new password (0 to go back): ");

    if(strcmp(new_password, "0") == 0)
    {
        clear_screen();
        change_account_details(user, unhashed_username, unhashed_password);
    }

    sha256(new_password);

    FILE *f;
    f = fopen("user_database.bin", "r+b");
    if(f == NULL)
        error("File opening failed.\n");

    user_info users;
    while(fread(&users, sizeof(user_info), 1, f) == 1)
    {
        if(strcmp(users.username, user.username) == 0)
        {
            strcpy(users.password, new_password);
            strcpy(user.password, new_password);
            fseek(f, -sizeof(user_info), SEEK_CUR);  
            fwrite(&users, sizeof(user_info), 1, f);
            clear_screen(); 
            printf("Password updated. Returning to home screen.\n");
            fclose(f);
            break;
        }
    }

    char entry[255];
    sprintf(entry, "PASSWORD-CH | username: %s", user.username);
    log_data(entry);

    return user;
}
user_info change_date_of_birth(user_info user, char unhashed_username[], char unhashed_password[])
{
    char new_date_of_birth[21];
    get_date_of_birth(new_date_of_birth, "Enter new date of birth in the form DD-MM-YYYY(0 to go back): ");
    if(strcmp(new_date_of_birth, "0") == 0)
    {
        clear_screen();
        change_account_details(user, unhashed_username, unhashed_password);
    }

    FILE *f;
    f = fopen("user_database.bin", "r+b");
    if(f == NULL)
        error("File opening failed.\n");

    user_info users;
    while(fread(&users, sizeof(user_info), 1, f) == 1)
    {
        if(strcmp(users.username, user.username) == 0)
        {
            strcpy(users.date_of_birth, new_date_of_birth);
            strcpy(user.date_of_birth, new_date_of_birth);
            fseek(f, -sizeof(user_info), SEEK_CUR);  
            fwrite(&users, sizeof(user_info), 1, f);
            clear_screen(); 
            printf("Date of birth updated. Returning to home screen.\n");
            fclose(f);
            break;
        }
    }

    char entry[255];
    sprintf(entry, "DOB-CH      | username: %s", user.username);
    log_data(entry);

    return user;
}
user_info change_favourite_animal(user_info user, char unhashed_username[], char unhashed_password[])
{
    char new_favourite_animal[21];
    get_favourite_animal(new_favourite_animal, "Enter your new favourite animal (0 to go back): ");
    if(strcmp(new_favourite_animal, "0") == 0)
    {
        clear_screen();
        change_account_details(user, unhashed_username, unhashed_password);
    }

    FILE *f;
    f = fopen("user_database.bin", "r+b");
    if(f == NULL)
        error("File opening failed.\n");

    user_info users;
    while(fread(&users, sizeof(user_info), 1, f) == 1)
    {
        if(strcmp(users.username, user.username) == 0)
        {
            strcpy(users.favourite_animal, new_favourite_animal);
            strcpy(user.favourite_animal, new_favourite_animal);
            fseek(f, -sizeof(user_info), SEEK_CUR);  
            fwrite(&users, sizeof(user_info), 1, f);
            clear_screen(); 
            printf("Favourite animal updated. Returning to home screen.\n");
            fclose(f);
            break;
        }
    }

    char entry[255];
    sprintf(entry, "ANIMAL-CH   | username: %s", user.username);
    log_data(entry);

    return user;
}
user_info get_user_info(char username[])
{
    FILE *f;

    f = fopen("user_database.bin", "rb");

    if(f == NULL)
        error("File opening failed.\n");

    user_info user = {0};

    while(fread(&user, sizeof(user_info), 1, f) == 1)
    {
        if(strcmp(user.username, username) == 0)
        {
            fclose(f);
            return user;
        }
    }
    fclose(f);
    error("User not found.\n");
    return user;
}
void sha256(char input[]) 
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    
    // ensures that all the bytes after the 20th byte are nulls
    bzero(input + 21, 44);

    // hashes the entire 65-byte buffer 
    SHA256((unsigned char *)input, 65, hash);

    // overwrites input with the hex string representation 
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(&input[i * 2], "%02x", hash[i]);
    }

    input[64] = '\0'; 
}

void admin_registration()
{
    LABEL01:
    char username[65] = {0}, password[65] = {0};

    while(1)
    {
        get_admin_username(username, "Set admin username (0 to go back): admin_");
        if(strcmp(username, "0") == 0)
        {
            clear_screen();
            main();
        }
        sha256(username);

        if(check_unique_admin_username(username) == false)
        {
            printf("Username is taken. Try a different username.\n");
        }
        else
            break;

    }
    

    get_password(password, "Set admin password (0 to go back): ");
    if(strcmp(password, "0") == 0)
    {
        //clear_screen();
        goto LABEL01;
    }
    sha256(password);

    admin_info admin;

    strcpy(admin.username, username);
    strcpy(admin.password, password);

    FILE *f;
    f = fopen("admin_database.bin", "ab");

    if(f == NULL)
        error("File opening failed.\n");

    fwrite(&admin, 1, sizeof(admin_info), f);
    fclose(f);    

    clear_screen();
    printf("Registration successful. Retuning to main menu.\n\n");
}
void get_admin_username(char username[], const char prompt[])
{
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

        if(strcpy(test_username, "0") == 0)
            return;

        int size = strlen(test_username);

        if(isdigit(test_username[0]))
        {
            clear_screen(); 
            printf("Username cannot start with a digit.\n");
            continue;
        }
        else if(size > 14)
        {
            clear_screen(); 
            printf("Username cannot be over 14 characters.\n");
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
            sprintf(username, "admin_%s", test_username);
            valid_username = true;
        }      
    }
}
void admin_login()
{
    char username[65] = {0}, password[65] = {0};
    admin_input_login(username, password);
    clear_screen();
    admin_homepage(username, password);
}
void admin_input_login(char username[], char password[])
{
    LABEL01:
    while(1)
    {
        get_admin_username(username, "Enter admin username (0 to go back): admin_");
        if(strcmp(username, "0") == 0)
        {
            clear_screen();
            main();
        }
        sha256(username);

        if(check_unique_admin_username(username) == false)
        {
            break;
        }
        else
            printf("Username does not exist.\n");

    }
    
    LABEL02:
    get_password(password, "Enter admin password (0 to go back): ");
    if(strcmp(password, "0") == 0)
    {
        //clear_screen();
        goto LABEL01;
    }
    sha256(password);
    if(check_admin_password(username, password) == false)
    {
        printf("Password incorrect. Try again\n");
        goto LABEL02;
    }
}
bool check_unique_admin_username(char username[])
{
    FILE *f;

    f = fopen("admin_database.bin", "rb");

    if(f == NULL)
        error("File opening failed.\n");

    admin_info admin;

    while(fread(&admin, sizeof(admin_info), 1, f) == 1)
    {
        if(strcmp(admin.username, username) == 0)
        {
            fclose(f);
            return false;
        }
            
    }

    fclose(f);
    return true;
}
bool check_admin_password(char username[], char password[])
{
    FILE *f;

    f = fopen("admin_database.bin", "rb");

    if(f == NULL)
        error("File opening failed.\n");

    admin_info admin;

    while(fread(&admin, sizeof(admin_info), 1, f) == 1)
    {
        if(strcmp(admin.username, username) == 0 && strcmp(admin.password, password) == 0)
        {
            fclose(f);
            return true;
        }       
    }

    fclose(f);
    return false;
}
void admin_homepage(char username[], char password[])
{
    printf("Admin Homepage\n");
    int choice;

    while(1)
    {
        printf("1. View all user profiles\n"
               "2. View specific user profile\n"
               "3. Request user database\n"
               "4. Request logbook\n"
               "Please select an operation (1-6): ");
        scanf("%d", &choice);
        getchar();

        switch (choice)
        {
            case  1:    read_users();
                        break;

            case  2:    {
                            user_info user = get_user_by_account_no(username, password);
                            display_account_info(user, "\0", "\0");
                            break;
                        }                  

            case  3:    request_user_datbase();
                        printf("Not available for this version,\n");
                        break;

            case  4:    request_logbook();
                        printf("Not available for this version.\n");
                        break;

            default:    printf("Invalid choice.\n");
        }
    }
}
void read_users()
{
    FILE *f;

    f = fopen("user_database.bin", "rb");

    if(f == NULL)
        error("File opening failed.\n");

    user_info user;

    while(fread(&user, sizeof(user_info), 1, f) == 1)
    {
        printf("Username        : %s\n"
               "Password        : %s\n"
               "Date of birth   : %s\n"
               "Balance         : Tk%06.2f\n"
               "Account Number  : %s\n"
               "Favourite Animal: %s\n\n", 
           user.username, user.password, user.date_of_birth, user.balance, user.account_no, user.favourite_animal);
    }

    fclose(f);
}  
user_info get_user_by_account_no(char username[], char password[])
{
    char account_no[14];
    get_account_no(account_no, "Enter account number of target user (0 to go back): ");
    if(strcmp(account_no, "0") == 0)
    {
        clear_screen();
        admin_homepage(username, password);
    }

    FILE *f;

    f = fopen("user_database.bin", "rb");

    if(f == NULL)
        error("File opening failed.\n");

    user_info user = {0};

    while(fread(&user, sizeof(user_info), 1, f) == 1)
    {
        if(strcmp(user.account_no, account_no) == 0)
            return user;

    }
    fclose(f);
    error("User not found");
    return user;

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
void request_user_datbase()
{

}
void request_logbook()
{

}

void error(const char message[])
{
    perror(message);
    exit(1);
}