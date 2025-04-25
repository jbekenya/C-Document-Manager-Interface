/*Josephine Ekenya
117210668
jekenya*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sysexits.h>
#include <ctype.h>
#include "document.h"


int get_command(Document *doc, char command[]);
int add_paragraph_command(Document *doc, int paragraph_number);
int reset_document_command(Document *doc);
int print_document_command(Document *doc);
int add_paragraph_after_command(Document *doc, int paragraph_number);
int add_line_after_command(Document *doc, int paragraph_number, int line_number,
		   const char *new_line);
int get_number_lines_paragraph_command(Document *doc, int paragraph_number, int *number_of_lines);
int append_line_command(Document *doc, int paragraph_number, const char *new_line);
int remove_line_command(Document *doc, int paragraph_number, int line_number);
int load_document_command(Document *doc, char data[][MAX_STR_SIZE + 1], int data_lines);
int replace_text_command(Document *doc, const char *target, const char *replacement);
int highlight_text_command(Document *doc, const char *target);
int remove_text_command(Document *doc, const char *target);
int load_file_command(Document *doc, const char *filename);
int save_document_command(Document *doc, const char *filename);
int get_text(char character, char *text, char *source, char command[]);

int main(int argc, char *argv[]){
    int max_line = 1024;
    FILE *input;
    char line[MAX_STR_SIZE + 1];
    Document doc;
    const char *doc_name = "main_document";
    
     /*Upon starting execution your program should initialize a single document with the name ”main document”,
    and perform operations on that document as instructed by the commands the program reads.*/  

   if (init_document(&doc, doc_name) == FAILURE) {
      printf("Initialization failed\n");
   } 

    /*If there is no file specified when the program is started, the program should read its data from standard input.
    The program will display a prompt (represented by >) after which commands will be entered.*/
    if (argc == 1) {
        input = stdin;   /* Standard input */
        printf("> ");

        fgets(line, max_line, input); /* Reads a line */
        while (strcmp(line, "quit\n") && strcmp(line, "exit\n")) {  /* Checking for when to stop */
           /* When a comment or blank line is provided
           a new prompt will be generated.*/
            char *tempLine = line; 
            while (isspace((unsigned char)*tempLine)) { /*skip trailing white spaces*/
                tempLine++;
            }
            if (*tempLine != '#' || *tempLine != '\0') {
                get_command(&doc, line);
                
            }
            printf("> ");
            fgets(line, max_line, input); /* Reads next line */
        }
    } else if (argc == 2) { 
      /* If a file is named, the program reads its data from that file*/
      input = fopen(argv[1], "r");
      /*In case of an error opening the file,program  print (to standard error) the message ”FILENAME
        cannot be opened.”*/
        if (input == NULL){
            fprintf(stderr, "%s cannot be opend.\n", argv[1]);
            return EX_OSERR;
        } else{
            fgets(line, MAX_STR_SIZE + 1, input); /* Reads a line */
            while (strcmp(line, "quit\n") && strcmp(line, "exit\n") && !feof(input)){  /* Checking for end of file */
                char *tempLine = line;
                while (isspace((unsigned char)*tempLine)){ /*skip trailing white spaces*/
                    tempLine++;
                } 
                if (*tempLine != '#' && *tempLine != '\0') { /*check if line is a comment or an empty line*/
                    /*if it is not a comment or an empty line, get the 
                    command associated with it*/
                    get_command(&doc, line);
                }
                
                fgets(line, max_line, input); /* Reads next line */
            }
        } 
  } else {
    /*if there are more than 1 arg the program prints the 
    following usage message to standard error, 
    and exits with exit code EX_USAGE*/
      fprintf(stderr, "Usage: a.out\n");
      fprintf(stderr, "Usage: a.out <filename>\n");
      return EX_USAGE;     
   }

   
   /* Closing the stream */
   fclose(input);
   
   return EXIT_SUCCESS;
}

/*This will parse a line that represents a
command and run the command */
int get_command(Document *doc, char command[]){
    int entries_read;
    int paragraph_num;
    char run_command[MAX_STR_SIZE + 1], addit[MAX_STR_SIZE + 1];
    int run = FAILURE, line_number;
	char line[MAX_STR_SIZE + 1], new_line[MAX_STR_SIZE + 1];


    /*check what the command is*/
    if(strstr(command, "add_paragraph_after") != NULL){
        /*parse the line*/
            entries_read = sscanf(command, "%s%d%s", run_command, &paragraph_num, addit);

            /*if no number was provided, or number provided does not represent
                a number, or the number provided is less than 0, or additional information
                is provided, "invaled command" message will be generated*/
            if (entries_read == 2 && paragraph_num >= 0 ){
                if(add_paragraph_command(doc, paragraph_num) != SUCCESS){
                        printf("add_paragraph_after failed\n");
                }
                
            } else{
                printf("Invalid Command\n");
            }
    } else if(strstr(command, "print_document") != NULL){
        /*parse the line*/
            entries_read = sscanf(command, "%s%s", run_command, addit);

            
            if (entries_read > 1){
                printf("Invalid Command\n");
            } else if(print_document_command(doc) != SUCCESS){
                /*If the command cannot be successfully executed the message ”add paragraph 
                after failed” will be generated.*/
                printf("print_document failed\n");
            }else{
                run = SUCCESS;
            }
    } else if(strstr(command, "add_line_after") != NULL){
        /*parse the line
         add_line_after 1 0 *first line of the document*/
        char *remain_line, *newline_pos;
        entries_read = sscanf(command, "%s%d%d%s", run_command, &paragraph_num, &line_number, line);

            /*get all the values after * using a pointer*/
            if (strchr(command, '*') != NULL) {
                remain_line = strchr(command, '*') + 1;
                newline_pos = strchr(remain_line, '\n'); /*Find the newline character, if any*/
                if (newline_pos != NULL) {
                    *newline_pos = '\0'; /* terminate the string at the newline character*/
                }
                strcpy(new_line, remain_line); /*Copy the string without the newline*/
            } 
            /*check that it is a valid command*/
            if (entries_read < 4 || strchr(command, '*') == NULL ||
             paragraph_num <= 0 || line_number < 0  || strchr(command, '*') == NULL ){
                printf("Invalid Command\n");
            } else if(add_line_after_command(doc, paragraph_num, line_number, new_line) != SUCCESS){
                /*If the command cannot be successfully executed the message ”add paragraph 
                after failed” will be generated.*/
                printf("add_line_after failed\n");
            }else{
                run = SUCCESS;
            }
    } else if(strstr(command, "append_line") != NULL){
        /*append_line 1 *first line, first paragraph*/
        char *remain_line, *newline_pos;
        entries_read = sscanf(command, "%s%d%s", run_command, &paragraph_num, line);
         /*get all the values after * */
        /*get all the values after * using a pointer*/
        if (strchr(command, '*') != NULL) {
            remain_line = strchr(command, '*') + 1;
            newline_pos = strchr(remain_line, '\n'); /*Find the newline character, if any*/
            if (newline_pos != NULL) {
                *newline_pos = '\0'; /* Temporarily terminate the string at the newline character*/
            }
            strcpy(new_line, remain_line); /*Copy the string without the newline*/
        }

        if (entries_read < 3 || paragraph_num <= 0 || strchr(command, '*') == NULL){
                printf("Invalid Command\n");
            } else if(append_line_command(doc, paragraph_num, new_line) != SUCCESS){
                /*If the command cannot be successfully executed the message ”add paragraph 
                after failed” will be generated.*/
                printf("append_line failed\n");
            }else{
                run = SUCCESS;
            }

    } else if (strstr(command, "remove_line") != NULL){
        /*remove_line PARAGRAPH_NUMBER LINE_NUMBER*/
        entries_read = sscanf(command, "%s%d%d%s", run_command, &paragraph_num, &line_number, addit);
        if (entries_read < 3 || paragraph_num <= 0 || line_number <= 0 || entries_read > 3){
            printf("Invalid Command\n");
        }else if (remove_line_command(doc, paragraph_num, line_number) != SUCCESS){
            printf("remove_line failed\n");
        }else{
            run = SUCCESS;
        }

    } else if (strstr(command, "load_file") != NULL){
        char filename[1 + 1];
        entries_read = sscanf(command, "%s%s%s", run_command, filename, addit);

        if (entries_read == 2){
            if (load_file_command(doc, filename) != SUCCESS){
                printf("load_file failed\n");
            } else{
                run = SUCCESS;
            }
        } else{
            printf("Invalid Command\n"); 
        }

    } else if (strstr(command, "save_document") != NULL){
        char filename[MAX_STR_SIZE + 1];
        entries_read = sscanf(command, "%s%s%s", run_command, filename, addit);

        if (entries_read < 2 || entries_read > 2 ){
            printf("Invalid Command\n");
        } else if (save_document_command(doc, filename) != SUCCESS){
            printf("save_document failed\n");
        } else{
            run = SUCCESS;
        }

    } else if (strstr(command, "reset_document") != NULL){
        entries_read = sscanf(command, "%s%s", run_command, addit);
        if (entries_read == 1){
            reset_document_command(doc);
            run = SUCCESS;
        } else{
            printf("Invalid Command\n");
        }
    } else if (strstr(command, "highlight_text") != NULL){
        char target[MAX_STR_SIZE + 1], text[MAX_STR_SIZE + 1];       
        entries_read = sscanf(command,"%s%s", run_command, target);
        if (entries_read < 2){
            printf("Invalid Command\n");
        } else{
            get_text('\"', text, target, command);
            highlight_text_command(doc, text);
            run = SUCCESS;
        }

    } else if (strstr(command, "replace_text") != NULL){
        char target[MAX_STR_SIZE + 1], replacement[MAX_STR_SIZE + 1],
        target_text[MAX_STR_SIZE + 1], replace_text[MAX_STR_SIZE + 1];
        /*used to indicate the end and begining of a string*/
        entries_read = sscanf(command, "%s%s%s", run_command, target, replacement);
        
        if (entries_read == 3){
            /*target will never be an empty string but replacement can be an empty string
            so check if replacement is an empty string*/
    
            /* first check whether replacement is an empty string. 
            If not get the text portion of replacement */
            if (strcmp(replacement, "") != 0){
                get_text('\"', replace_text, replacement, command);
            } 

            /*target will not be an empty string, so get the text portion*/
            if (target != NULL){
                get_text('\"', target_text, target, command);
            }
            /*pass in to replace text*/
            if (replace_text_command(doc, target_text, replace_text) != SUCCESS){
                printf("replace_text failed\n");
            } else{
                run = SUCCESS;
            }
        }  else {
            printf("Invalid Command\n");

        }
    } else if (strstr(command, "remove_text") != NULL){
        char target[MAX_STR_SIZE + 1], text[MAX_STR_SIZE + 1];
        entries_read = sscanf(command, "%s%s", run_command, target);
        
        if(entries_read == 2){
            get_text('\"', text, target, command);
            run = remove_text_command(doc, text);
        } else{
            printf("Invalid Command\n");
        }
    } else if (strstr(command, "quit") != NULL || strstr(command, "exit") != NULL){
        char exit_or_quit[MAX_STR_SIZE + 1];
        entries_read = sscanf(command, "%s%s%s", run_command, exit_or_quit, addit);
        
        if(entries_read == 1){
            return run;
        } else{
            printf("Invalid Command\n");
        }
    }
    return run;
}

/* this method is used to get and copy the text portion
of a string from a command line in the format "computer scinece"*/
int get_text(char character, char *text, char *source, char command[]){
    /*methods: 
        - get the position of the opening string char
        - find the ending string char
        - copy the text portion to the text string.
    */
   char *remain_str, *new_str_pos;
   if (strchr(source, character) != NULL){
        remain_str = strchr(source, character) + 1;
        new_str_pos = strchr(remain_str, character);
        if (new_str_pos != NULL){ /* a sencond char has been found*/
            *new_str_pos = '\0'; /*terminate at the second character*/
            strcpy(text, remain_str);
        } else{
            if (strstr(command, source) != NULL){
                remain_str = strstr(command, source) + 1;
                new_str_pos = strchr(remain_str, '\n') - 1;
                if (new_str_pos != NULL){
                    *new_str_pos = '\0';
                }
                strcpy(text, remain_str);
            }   
        }
        return SUCCESS;
   }
   return FAILURE;
}

int add_paragraph_command(Document *doc, int paragraph_number){
    return add_paragraph_after(doc, paragraph_number);
}

int add_line_after_command(Document *doc, int paragraph_number, int line_number,
		   const char *new_line){
            return add_line_after(doc, paragraph_number, line_number, new_line);
}

int append_line_command(Document *doc, int paragraph_number, const char *new_line){
    return append_line(doc, paragraph_number, new_line);
}

int print_document_command(Document *doc){
    return print_document(doc);
}

int reset_document_command(Document *doc){
    return reset_document(doc);
}

int remove_line_command(Document *doc, int paragraph_number, int line_number){
    return remove_line(doc, paragraph_number, line_number);
}

int replace_text_command(Document *doc, const char *target, const char *replacement){
    return replace_text(doc, target, replacement);
}
int highlight_text_command(Document *doc, const char *target){
    return highlight_text(doc, target);
}

int remove_text_command(Document *doc, const char *target){
    return remove_text(doc, target);
}
int load_file_command(Document *doc, const char *filename){
    return load_file(doc, filename);
}
int save_document_command(Document *doc, const char *filename){

    return save_document(doc, filename); 
}

