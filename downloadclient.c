/*
    Matt Luca 
    Date: 5/16/2019 @ 11:47am
    Subject: C Programming
    Lab 12: File Download Client.c
*/

#include <stdio.h>
#include <stdlib.h>

FILE * connect();
void menu();
char get_choice();
void list_files(FILE *s);
void download(FILE *s);
void quit(FILE *s);

int main()
{
    // Connect
    FILE * s = connect();
    
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
            
        default:
            printf("Choice must be d, l, or q\n");
    }
}

/*
 * Connect to server. Returns a FILE pointer that the
 * rest of the program will use to send/receive data.
 */
FILE * connect()
{
    return NULL;
}

/*
 * Display menu of choices.
 */
void menu()
{
    printf("List files\n");
    printf("Download a file\n");
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