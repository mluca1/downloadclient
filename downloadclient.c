/*
    Matt Luca 
    Date: 5/21/2019 @ 11:26pm
    Subject: C Programming
    Lab 12: File Download Client.c
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#define ADDRESS "65.19.178.177"
#define PORT 1234

FILE * connect_to_server();
void menu();
char get_choice();
void list_files(FILE *s);
void download(FILE *s);
void quit(FILE *s);
int size(FILE *s, char *filename);
void download_file(FILE *s, char *filename);
void download_all(FILE *s);
int list_all(FILE *s, char filenames[100][100]);
void print_progress_bar(int left, int total);
int file_exists(char *filename);
void print_all_files_with_number(FILE *s);

int main()
{
    // Connect
    FILE * s = connect_to_server();
    
    while(1) {
        // Menu
        menu();
        
        // Get choice
        char choice = get_choice();
        
        // Handle choice
        switch(choice)
        {
            case 'l':
            case 'L':
                list_files(s);
                break;
            
            case 'd':
            case 'D':
                download(s);
                break;
                
            case 'q':
            case 'Q':
                quit(s);
                exit(0);
                break;
            
            case 'a':
            case 'A':
                download_all(s);
                break;

            case 'n':
            case 'N':
                print_all_files_with_number(s);
                break;

            default:
                printf("Choice must be d, l, or q\n");
        }
    }
}

/*
 * Connect to server. Returns a FILE pointer that the
 * rest of the program will use to send/receive data.
 */
FILE * connect_to_server()
{
    // Create buffer and necessary variables
    char buffer[200];
    struct sockaddr_in address;
    int sock = 0;
    FILE * fp = NULL;
    struct sockaddr_in serv_addr;

    // Creating socket file descriptor 
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("Socket creation error!\n"); 
        return NULL;
    }

    // Set socket to zero
    memset(&serv_addr, '0', sizeof(serv_addr));

    // Configure socket
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 address from text to binary form 
    if(inet_pton(AF_INET, ADDRESS, &serv_addr.sin_addr)<=0)  
    { 
        printf("Address not supported!\n"); 
        return NULL; 
    }

    // Connect socket
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("Connection Failed!\n"); 
        return NULL; 
    }

    // Convert socket stream to file stream
    fp = fdopen(sock, "r+");

    // Read incoming message
    fgets(buffer, sizeof(buffer), fp);
    printf("%s", buffer);

    // Return file descriptor
    return fp;
}

/*
 * Display menu of choices.
 */
void menu()
{
    printf("\n\n");
    printf("List files\n");
    printf("Download a file\n");
    printf("All files\n");
    printf("Numbered download\n");
    printf("Quit\n");
    printf("\n");
}

/*
 * Get the menu choice from the user. Allows the user to
 * enter up to 100 characters, but only the first character
 * is returned.
 */
char get_choice()
{
    printf("Your choice: ");
    char buf[100];
    fgets(buf, 100, stdin);
    return buf[0];
}

/*
 * Display a file list to the user.
 */
void list_files(FILE *s)
{
    // Create a buffer to read respond
    char buffer[200];

    // Send command LIST to server
    fprintf(s, "LIST\n");

    // Flush fprintf
    fflush(s);

    // Read back the received data and print until we find ".\n"
    while ( fgets(buffer, sizeof(buffer), s) ) {
        if (strcmp(buffer, ".\n") == 0)
            break;
        printf("%s", buffer);
    }
}

/*
 * Download a file.
 * Prompt the user to enter a filename.
 * Download it from the server and save it to a file with the
 * same name.
 */
void download(FILE *s)
{
    
}

/* 
 * Close the connection to the server.
 */
void quit(FILE *s)
{
    fclose(s);
}

