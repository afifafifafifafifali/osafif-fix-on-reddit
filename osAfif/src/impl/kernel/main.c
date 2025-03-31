
#include "print.h"
#include "console_input.h"
#include "mallocate.h"
#include "str.h"
#include "disk.h"
#include "fat32.h"
void kernel_main() {

    print_clear();
    print_set_color(PRINT_COLOR_WHITE, PRINT_COLOR_BLACK);
    initiateFat32();
    print_str("\n\nWelcome to the AfifOS!\n");
    print_str("Type 'help' to learn more. \n");

    string ch = (string)malloc(200);
    string prompt = "root@COMPUTER-Vm3rf2av$ ";

    do {
        print_format("%s", prompt);
        readStr(ch);

        if (strcmp(ch, "cmd") == 0) {
            printf("\nYou're already in a command prompt. The command processor has been refreshed.\n");

        } else if (strcmp(ch, "help") == 0) {
            printf("\nWelcome to AfifOS!\n");
            printf("Here are some commands you can use:\n");
            printf("cmd - Refresh the command processor\n");
            printf("help - Display this help message\n");
            printf("clear - Clear the screen\n");
            printf("echo - Print a message\n");
            printf("devinfo - Display developer's information\n");
            printf("Disktee- Write AFIFOS to disk\n");
            printf("Diskread- Read AFIFOS from disk\n");
            printf("But remember, no file system currently available :D\n");

        } else if (strcmp(ch, "clear") == 0) {
            print_clear();

        } else if (strcmp(ch, "echo") == 0) {
            printf("\nEnter the message you want to print: ");
            readStr(ch);
            printf("%s\n", ch);

        } else if (strcmp(ch, "devinfo") == 0) {
            printf("\nAfifOS is developed by Afif Ali Saadman\n");
            printf("Github: https://github.com/afifafifafifafifali\n");

        } else if(strcmp(ch, "Disktee")==0) {
            printf("\nWriting AFIFOS or AYMANFAZIL to disk...\n");
            unsigned char *data = (unsigned char *)malloc(512);
             if (!data) {
                printf("Memory allocation failed!\n");
                return;
             }

             strcpy((char *)data, "AYMANFAZIL");

             // Write the data to disk
             putDiskBytes(data, 0, 1);
            printf("Data written to disk\n");

            // Free the memory after use
             free(data);
         } else if (strcmp(ch, "Diskread") == 0) {
            printf("\nReading the disk data\n");
            unsigned char *data = (unsigned char *)malloc(512);
            if (!data) {
                printf("Memory allocation failed!\n");
                return;
            }
            getDiskBytes(data, 0, 1);
            printf("Data read from disk: %s\n", data);
            free(data);

         }
        else {
            printf("\nCommand not found. Type 'help' to learn more.\n");
        }    


    } while (1);
}



