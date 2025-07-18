#include <stdio.h>
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
    char salt[17];
    char date_of_birth[11];
    char favourite_animal[21];
    char account_no[14];
    float balance;

} user_info;

void get_username(char username[], const char prompt[]);
void get_password(char password[], const char prompt[]);
void get_date_of_birth(char date_of_birth[], const char prompt[]);
bool check_valid_date(int birth_date, int birth_month, int birth_year);
void get_favourite_animal(char favourite_animal[], const char prompt[]);
void sha256(char input[]);
void clear_screen();

int main()
{
    while(1)
    {
        int choice;
        printf("Read - 1, Write - 2: ");
        scanf("%d", &choice);
        getchar();
        if(choice == 1)
        {
            FILE *f;

            f = fopen("user_database.bin", "rb");

            if(f == NULL)
                perror("File opening failed.\n");

            user_info user;

            while(fread(&user, sizeof(user_info), 1, f) == 1)
            {
                printf("Username: %s\n", user.username);
                printf("Password: %s\n", user.password);
                printf("Salt: %s\n", user.salt);
                printf("DOB: %s\n", user.date_of_birth);
                printf("Fav Animal: %s\n", user.favourite_animal);
                printf("Balance: %06.2f\n", user.balance);
                printf("\n");
            }
            fclose(f);
        }
        else if (choice == 2)
        {
            char username[65] = {0};
            char password[65] = {0};
            char date_of_birth[11];
            char favourite_animal[21];
            char account_no[14] = "1234567891011";
            float balance = 0.0;

            FILE *f;

            f = fopen("user_database.bin", "ab");  

            if (f == NULL)
                perror("File opening failed.\n");

            user_info new_user;

            get_username(username, "Enter username.");
            sha256(username);
            get_password(password, "Enter password.");
            sha256(password);
            get_date_of_birth(date_of_birth, "Enter dob.");
            get_favourite_animal(favourite_animal, "Enter fav ani");
            scanf("%f", &balance);
            getchar();


            strcpy(new_user.username, username);
            strcpy(new_user.password, password);
            strcpy(new_user.date_of_birth, date_of_birth);
            strcpy(new_user.favourite_animal, favourite_animal);
            new_user.balance = balance;


            size_t written = fwrite(&new_user, sizeof(user_info), 1, f);
            if (written != 1)
                perror("Error writing to file.\n");
            
            fclose(f);
            printf("User data saved successfully.\n");
        }
    }
    
    
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
        if(size < 8)
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


void clear_screen()
{
    system("clear");
}
