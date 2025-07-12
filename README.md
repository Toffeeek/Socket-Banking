# Socket-Banking
A banking application in C, simulating real world online banking (with few exceptions) through simulated database handling, client management and security.

The project primarily consists of a client side and a server side c program. The programs are compiled via Bash and the server is launched first using a suitable port number as its command line arugment (note that the flag -lcrypto needs to be added to compile the client program since it consists of OpenSSL's SHA256 library). The client is then launched by providing the IP Address of the server and the port number used by the server as its command line arguments. The server program uses multiprocessing via fork() to handle multiple clients simultaneously.

There are two modes of access: Client Mode and Administrator Mode

# Client Mode
Clients are first required to create an account by setting up usernames, passwords, dates of birth and their favourite animals (used as security question in case they forget their passwords). The usernames and passwords are hashed via SHA256 BEFORE being sent to the server to assure that client usernames and passwords are not leaked in case of a server break-in. The server receives these credentials and stores them in a binary file as structures which acts as a client database.Then clients can use their credentials to login to their accounts, alternatively they can reset their passwords in case they forget it by entering their dates of birth and favourite animals. Once logged in, they can deposit, withdraw or transfer funds to other accounts. They will also be able to change any credential and view their balances.

# Administrator Mode
Admins first need to set a username and password, upon which a request will be sent to the server. If the server approves of the request, an admin account will be created and admins can use their accounts to request access to client information and view activity through a logbook which records every client activity. Admin credentials are stored in a separate database to prevent mix-up. 

# Additional Program
The repository also consists of a database reader and writer program to ease with debugging and testing the application.
