#include <stdio.h>
#include <string.h>

#include "gradebook.h"

#define MAX_CMD_LEN 128

/*
 * This is in general *very* similar to the list_main file seen in lab
 * One big difference is the notion of switching between gradebooks in one
 * run of the program.
 * You have to create or load a gradebook from a file before you can do things
 * like add, lookup, or write.
 * The code below has to check if gradebook is NULL before the operations
 * occur. Also, the user has to explicitly clear the current gradebook before
 * they can create or load in a new one.
 */
int main(int argc, char **argv) {
    gradebook_t *book = NULL;
    if(argc > 1) {
        if(strstr(argv[1], ".txt")) {
            book = read_gradebook_from_text(argv[1]);
            if(book == NULL) {
                printf("Failed to read gradebook from text file\n");
            } 
            else
                printf("Gradebook loaded from text file\n");
        }
        else if(strstr(argv[1], ".bin")) {
            book = read_gradebook_from_binary(argv[1]);
            if(book == NULL) {
                printf("Failed to read gradebook from binary file\n");
            } 
            else
                printf("Gradebook loaded from binary file\n");
        }   
        else
            printf("Error: Unknown gradebook file extension\n");
    }

    printf("CSCI 2021 Gradebook System\n");
    printf("Commands:\n");
    printf("  create <name>:          creates a new class with specified name\n");
    printf("  class:                  shows the name of the class\n");
    printf("  add <name> <score>:     adds a new score\n");
    printf("  lookup <name>:          searches for a score by student name\n");
    printf("  clear:                  resets current gradebook\n");
    printf("  print:                  shows all scores, sorted by student name\n");
    printf("  write_text:             saves all scores to text file\n");
    printf("  read_text <file_name>:  loads scores from text file\n");
    printf("  write_bin:              saves all scores to binary file\n");
    printf("  read_bin <file_name>:   loads scores from binary file\n");
    printf("  exit:                   exits the program\n");

    char cmd[MAX_CMD_LEN];
    while (1) {
        printf("gradebook> ");
        if (scanf("%s", cmd) == EOF) {
            printf("\n");
            break;
        }

        if (strcmp("exit", cmd) == 0) {
            break;
        }

        else if (strcmp("create", cmd) == 0) {
            scanf("%s", cmd); // Read in class name 
            if (book != NULL) {
                printf("Error: You already have a gradebook.\n");
                printf("You can remove it with the \'clear\' command\n");
            } else {
                book = create_gradebook(cmd);
                if (book == NULL) {
                    printf("Gradebook creation failed\n");
                }
            }
        }


        // TODO Add cases for the other commands
        // Read in the command and (possibly) additional arguments with scanf()
        else if(strcmp("class", cmd) == 0) {
            // class command 
            if(book == NULL) {
                printf("Error: You must create or load a gradebook first\n");
            }
            else {
                char name[MAX_NAME_LEN];
                strcpy(name,get_gradebook_name(book));
                printf("%s\n", name);
            }
        }

        else if(strcmp("add", cmd) == 0) {
            // add command
            char name[MAX_NAME_LEN];
            int score;
            scanf("%s %d", name, &score);
            if(book == NULL) {
                printf("Error: You must create or load a gradebook first\n");
            }
            else {
                if(add_score(book, name, score) == -1) {
                    printf("Error: You must enter a score in the valid range (0 <= score)\n");
                }
                
            }
        }

        else if(strcmp("lookup", cmd) == 0) {
            // lookup command
            char name[64];
            scanf("%s", name);
            if(book == NULL) {
                printf("Error: You must create or load a gradebook first\n");
            }
            else {
                if(find_score(book, name) == -1) {
                    printf("No score for '%s' found\n", name);
                }
                else {
                    int i = find_score(book, name);
                    printf("%s: %d\n", name, i);
                }
            }
        }

        else if(strcmp("clear", cmd) == 0) {
            // clear command
            if(book == NULL) {
                printf("Error: No gradebook to clear\n");
            }
            else {
                free_gradebook(book);
                book = NULL;
            }
        }

        else if(strcmp("print", cmd) == 0) {
            // print command
            if(book == NULL) {
                printf("Error: You must create or load a gradebook first\n");
            }
            else {
                print_gradebook(book);
            }
        }

        else if(strcmp("write_text", cmd) == 0) {
            // write_text command
            if(book == NULL) {
                printf("Error: You must create or load a gradebook first\n");
            }
            else if(write_gradebook_to_text(book) == -1) {
                printf("Failed to write gradebook to text file\n");
            }
            else {
                write_gradebook_to_text(book);
                printf("Gradebook successfully written to %s\n", strcat(book->class_name, ".txt"));
            }
        }

        else if(strcmp("read_text", cmd) == 0) {
            // read_text command
            char name[MAX_NAME_LEN];
            scanf("%s", name);
            if(book != NULL) {
                printf("Error: You must clear current gradebook first\n");
            }
            else {
                book = read_gradebook_from_text(name);
                if(book == NULL) {
                    printf("Failed to read gradebook from text file\n");
                }
                else
                    printf("Gradebook loaded from text file\n");
            }
            
        }

        else if(strcmp("write_bin", cmd) == 0) {
            // write_bin command
            if(book == NULL) {
                printf("Error: You must create or load a gradebook first\n");
            }
            else if(write_gradebook_to_binary(book) == -1) {
                printf("Failed to write gradebook to binary file\n");
            }
            else {
                write_gradebook_to_binary(book);
                printf("Gradebook successfully written to %s\n", strcat(book->class_name, ".bin"));
            }
        }

        else if(strcmp("read_bin", cmd) == 0) {
            // read_bin command
            char name[MAX_NAME_LEN];
            scanf("%s", name);
            if(book != NULL) {
                printf("Error: You must clear current gradebook first\n");
            }
            else {
                book = read_gradebook_from_binary(name);
                if(book == NULL) {
                    printf("Failed to read gradebook from binary file\n");
                }
                else
                    printf("Gradebook loaded from binary file\n");
            }
        }

        else {
            printf("Unknown command %s\n", cmd);
        }
    }
    if (book != NULL) {
        free_gradebook(book);
    }
    return 0;
}