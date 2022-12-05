/*
    Name:       Byron Dowling
    Course:     5143 Advanced Operating Systems
    Date:       11/19/2022
    Assignment: Linux/Unix Seek
        Keyword Byte position
            - intro 	20
            - pass 		40
            - fail 		60
            - next 		80
*/

#include<sys/types.h>
#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>


int main(int argc, char* argv[])
{
    // Variables to hold our values
	char intro[] = "intro", pass[] = "pass", fail[] = "fail", justGiveUp[] = "next";
    char choice[20];

    int Fin;
    Fin = open("input.txt", O_RDONLY);

    while(1) {
        printf("\nWaiting for input...Type STOP to end the program\n");
        scanf("%s", choice);

        char buffer[20];

        // If "intro" is typed
        if (strcmp(choice, intro) == 0) {
            lseek (Fin, 20, SEEK_CUR);          // Seek to position 20
            read(Fin, buffer, 20);
            printf("\nResponse: %s", buffer);
            lseek(Fin, 0, SEEK_SET);            // Reset seek to beginning
        }

        // If "pass" is entered
        else if (strcmp(choice, pass) == 0) {
            lseek (Fin, 40, SEEK_CUR);          // Seek to position 40
            read(Fin, buffer, 20);
            printf("\nResponse: %s", buffer);
            lseek(Fin, 0, SEEK_SET);            // Reset seek to beginning
        }

        // If "fail" is entered
        else if (strcmp(choice, fail) == 0) {
            lseek (Fin, 60, SEEK_CUR);          // Seek to position 60
            read(Fin, buffer, 20);
            printf("\nResponse: %s", buffer);
            lseek(Fin, 0, SEEK_SET);            // Reset seek to beginning
        }

        // If "next" is typed
        else if (strcmp(choice, justGiveUp) == 0) {
            lseek (Fin, 80, SEEK_CUR);          // Seek to position 80
            read(Fin, buffer, 20);
            printf("\nResponse: %s", buffer);
            lseek(Fin, 0, SEEK_SET);            // Reset seek to beginning
        }

        // If stop or wrong input is typed, the loop will break
        else
            break;
    }
	
	return 0;
}
