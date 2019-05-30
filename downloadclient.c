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
                printf("Choice must be L, D, A, N, or Q\n");
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
    printf("(L) List files\n");
    printf("(D) Download a file\n");
    printf("(A) All files\n");
    printf("(N) Numbered download\n");
    printf("(Q) Quit\n");
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
    // Declare variable
    char filename[200];
    
    // Ask the user to enter file name to download
    printf("Please enter the file name you want to download: ");

    // Get the file name
    scanf("%s", filename);

    // Download the file
    download_file(s, filename);
}

/* 
 * Close the connection to the server.
 */
void quit(FILE *s)
{
    fclose(s);
}

/*
 * Gets the size of the file
 */
int size(FILE *s, char *filename)
{
    // Declare buffer
    char request[200];
    char buffer[200];
    char tmp[200];
    char *ptr;

    // Format request
    sprintf(request, "SIZE %s\n", filename);

    // Send request to server
    fprintf(s, "%s", request);

    // Read the first line of respond
    fgets(buffer, sizeof(buffer), s);

    // Let's see if the file was valid
    if (strncmp(buffer, "+OK", 3) == 0) {
        // Copy string right after +OK to find size of file
        strcpy(tmp, buffer+4);

        // Convert the string to integer
        int size = strtol(tmp, &ptr, 10);
        
        // Return size
        return size;
    } else { // File was not valid and didn't exists
        printf("File did not exists!\n");
        return -1;
    }
}

/*
 * Download a single file
 */
void download_file(FILE *s, char *filename)
{
    // Declare variables and buffers
    char request[300];
    char buffer[200];
    int filesize;
    char buffer_for_reading_file[1025];
    int left;

    // Let's check if file exists
    if(file_exists(filename) == 0) {
        printf("File exists, do you want to override it (y/n)? ");
        char input[100];
        scanf("%s", input);
        if(strcmp(input, "y") != 0 && strcmp(input, "Y") != 0) {
            exit(0);
        }
    }

    // Let's get file size
    filesize = size(s, filename);
    left = filesize;

    // Let's check the file size
    // If it is -1 that means file didn't exist
    if(filesize == -1) {
        printf("File did not exists!\n");
        return;
    }

    // Send request to server with the file name
    sprintf(request, "GET %s\n", filename);
    fprintf(s, "%s", request);

    // Flush the request
    fflush(s);

    // Read the first line
    fgets(buffer, sizeof(buffer), s);

    // Let's double check and see if the file was valid
    if (strcmp(buffer, "+OK\n") == 0) {
        // Open write file
        FILE * fWrite = fopen(filename, "wb");

        // Read file
        while ( left > 0 ) {
            // Read 1024 bytes of chunks of data until less than 1024 is left
            // Then we go to else part and read the rest
            if(left >= 1024) {
                // Read 1024 bytes
                fread(buffer_for_reading_file, 1024, 1, s);

                // Write 1024 bytes to file
                fwrite(buffer_for_reading_file, 1024, 1, fWrite);
                fflush(fWrite);

                // Decrease 1024 bytes from left
                left -= 1024;

                // Print progress bar
                print_progress_bar(left, filesize);
            } else { // If less than 1024 bytes left
                // Read "left" size
                fread(buffer_for_reading_file, left, 1, s);

                // Write the rest to file
                fwrite(buffer_for_reading_file, left, 1, fWrite);
                fflush(fWrite);

                // Decrease the last little of bit for no reason!
                left -= left;

                // Print progress bar
                print_progress_bar(left, filesize);
            }
        }
        fclose(fWrite);
    }
    printf("\n");
}

/*
 * This function will download all the files
 */
void download_all(FILE *s)
{
    // Create a two dimensional array for name of all files
    char name_of_all_files[100][100];

    // Get the list of files
    int number_of_files = list_all(s, name_of_all_files);

    for(int i=0; i<number_of_files; i++) {
        printf("%s\n", name_of_all_files[i]);
        download_file(s, name_of_all_files[i]);
    }
}

/*
 * This function will get the list of all files
 * And will return the number of files read
 */
int list_all(FILE *s, char filenames[100][100])
{
    // Create a buffer to read respond
    char buffer[200];
    // Number of items read
    int counter = 0;

    // Send command LIST to server
    fprintf(s, "LIST\n");

    // Flush fprintf
    fflush(s);

    // Read first line
    fgets(buffer, sizeof(buffer), s);

    // Let's double check and see if the request was valid
    if (strcmp(buffer, "+OK\n") == 0) {
        // Read back the received data and print until we find ".\n"
        while ( fgets(buffer, sizeof(buffer), s) ) {
            if (strcmp(buffer, ".\n") == 0)
                break;
            
            // Find the first space, that's where file name starts
            char *ptr = strstr(buffer, " ");

            // Remove the last character which is new line
            int last_character = strlen(ptr);
            ptr[last_character-1] = 0;

            // Copy the file name to the list
            strcpy(filenames[counter], ptr+1);

            // Increment the counter
            counter ++;
        }
    } else { // Something went wrong
        printf("Something went wrong!\n");
        printf("Exiting");
        exit(-1);
    }
    return counter;
}

/*
 * Print the progress bar
 */
void print_progress_bar(int left, int total)
{
    // How much percent is downloaded
    float percent = (float)(total-left)/(float)total;

    // We will print [#####   ] like this
    // Number of # is passed
    // and number of spaces is left
    // Terminal is 80 characters long
    // [ and ] is used so 78 characters left for progress bar
    int passed = (int)(percent * 78);
    int remainder = 78 - passed;

    char output[100] = {0};

    strcat(output, "\r[");
    for(int i=0; i<passed; i++)
        strcat(output, "|");
    for(int i=0; i<remainder; i++)
        strcat(output, " ");
    strcat(output, "]");
    printf("%s", output);
    fflush(stdout);
}

/*
 * Checks whether file exists or not
 */
int file_exists(char *filename)
{
    if (access(filename, F_OK) == 0) {
        return 0;
    } else {
        return 1;
    }   
}

void print_all_files_with_number(FILE *s)
{
    // Create a two dimensional array for name of all files
    char name_of_all_files[100][100];

    // buffer for user input
    char input[100];
    char *tmp;

    // Get the list of files
    int number_of_files = list_all(s, name_of_all_files);

    for(int i=0; i<number_of_files; i++) {
        printf("%i - %s\n", i+1, name_of_all_files[i]);
    }

    // Print info
    printf("Which file you want to download (enter number)? ");

    // Get user input
    scanf("%s", input);

    // Convert number to integer
    int number = strtol(input, &tmp, 10);

    // Print a message
    printf("Downloading %s\n", name_of_all_files[number-1]);

    // Download the file
    download_file(s, name_of_all_files[number-1]);
}