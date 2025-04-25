/*Josephine Ekenya
117210668
jekenya*/

#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sysexits.h>
#include <ctype.h>
#include "document.h"



int save_document(Document *doc, const char *filename);
int load_file(Document *doc, const char *filename);
int load_file_helper(Document *dummy, const char *filename, FILE *input);
int load_helper(Document *dummy, char data[][MAX_STR_SIZE + 1], int data_lines);
int highlight_in_line(char *new_line, char *line, const char *target);
int replace_in_line(char *new_line, char *line, const char *target, const char *replacement);
/* Initializes the paragraph with a name*/
int init_document(Document *doc, const char *name){
    if (doc == NULL || name == NULL || strlen(name) > MAX_STR_SIZE){
        return FAILURE;
    }

    /* an empty document is one with 0 paragraphs*/
    doc->number_of_paragraphs = 0;
    strcpy(doc->name, name);

    return SUCCESS;
}

/* sets the number of paragraphs to 0*/
int reset_document(Document *doc){
    if (doc == NULL){
        return FAILURE;
    }
    doc->number_of_paragraphs = 0;

    return SUCCESS;
}

/* Prints all lines in a document, paragraph by paragraph*/
int print_document(Document *doc){
    int i, j;
    Paragraph *paras; /*Create a paragrapgh pointer that points 
    to the paragraphs in the document's paraghraph array*/
    
    if (doc == NULL){
        return FAILURE;
    }

    printf("Document name: \"%s\"\n", doc->name);
    printf("Number of Paragraphs: %d\n", doc->number_of_paragraphs);
    /*access each paragrapgh's number of lines field*/
    paras = doc->paragraphs;
    /*go through the paragraphs and print lines in each paragraph*/
    for (i = 0; i < doc->number_of_paragraphs; i++){
        for (j = 0; j < paras[i].number_of_lines; j++){
            /*for line in each paragraph lines array, print out the line*/
            printf("%s\n", paras[i].lines[j]);
        }
        printf("\n");
    }
    
    return SUCCESS;
}

/*The function will return FAILURE if doc is NULL, the document 
has the maximum number of paragraphs allowed (MAX PARAGRAPHS) or 
if the paragraph number is larger than the number of paragraphs available;
*/
int add_paragraph_after(Document *doc, int paragraph_number){
    Paragraph paragraph = {0};
    int i;
    /*num of paragraphs available = max_num - paragraph_number >= 0*/
    if (doc != NULL && doc->number_of_paragraphs < MAX_PARAGRAPHS
    && paragraph_number <= doc->number_of_paragraphs){
        /*shift all paragraph one down*/
        for (i = doc->number_of_paragraphs; i > paragraph_number; i--) {
            doc->paragraphs[i] = doc->paragraphs[i - 1];
        }
        if (paragraph_number == 0){
            /*if the paragraph number is zero, add paragraph to the
            beginning of the document*/
            doc->paragraphs[0] = paragraph;
        }else{
            doc->paragraphs[paragraph_number] = paragraph;
        }
        doc->number_of_paragraphs += 1;

        return SUCCESS;
    }
    return FAILURE;
}

/*This function adds a new line after the line with the specified line number.*/
int add_line_after(Document *doc, int paragraph_number, int line_number,
		   const char *new_line){
    int number_of_lines;
    int i;
    /*The function will return FAILURE if doc is NULL, the paragraph number exceeds 
    the number of paragraphs available, the paragraph already has the maximum number 
    of lines allowed, the line number is larger than the available number of lines or
    if new line is NULL; otherwise, the function will return SUCCESS.*/

    if (doc == NULL || new_line == NULL || 
    paragraph_number > doc->number_of_paragraphs || 
    doc->paragraphs[paragraph_number - 1].number_of_lines == MAX_PARAGRAPH_LINES 
    || line_number > doc->paragraphs[paragraph_number - 1].number_of_lines){
        return FAILURE;
    }
    number_of_lines = doc->paragraphs[paragraph_number - 1].number_of_lines;
   
    if (line_number == 0){
        strcpy(doc->paragraphs[paragraph_number - 1].lines[0], new_line);
       } else{
        /* shift the lines to make space for a new line.*/
        for(i = number_of_lines; i >= line_number; i -= 1){

            strcpy(doc->paragraphs[paragraph_number - 1].lines[i], 
            doc->paragraphs[paragraph_number - 1].lines[i - 1]);
        }
        strcpy(doc->paragraphs[paragraph_number - 1].lines[line_number], new_line); 
    }
    /*increment the number of lines*/
    doc->paragraphs[paragraph_number - 1].number_of_lines += 1;
    return SUCCESS;

}

int get_number_lines_paragraph(Document *doc, int paragraph_number, int *number_of_lines){
    if (doc == NULL || number_of_lines == NULL || (MAX_PARAGRAPHS - paragraph_number) < 0){
        return FAILURE;
    }
    /*Returns the number of lines in a paragraph using the number of lines out parameter.*/
    *number_of_lines = doc->paragraphs[paragraph_number - 1].number_of_lines;
    
    return SUCCESS;
}

/*Appends a line to the specified paragraph. */
int append_line(Document *doc, int paragraph_number, const char *new_line){
    int index_to_append;
    /*The conditions that make the add line after fail:The function will return FAILURE
    if doc is NULL, the document has the maximum number of paragraphs allowed (MAX PARAGRAPHS)
    or if the paragraph number is larger than the number of paragraphs available; otherwise, the function
    will return SUCCESS.*/
    if (doc != NULL && (MAX_PARAGRAPHS - paragraph_number) > 0 && doc->number_of_paragraphs < 
    MAX_PARAGRAPHS && new_line != NULL ){
        /*get the first empty space in the array and append the line in that space*/
        /*using number of lines we can add at the end of the array and then increment*/
        index_to_append = doc->paragraphs[paragraph_number - 1].number_of_lines;
        strcpy(doc->paragraphs[paragraph_number - 1].lines[index_to_append], new_line);

        /*increment the number of lines for that paragraph*/
        doc->paragraphs[paragraph_number - 1].number_of_lines += 1;

        return SUCCESS;
    } 
    return FAILURE;
}

/*Removes the specified line from the paragraph. The function will return FAILURE 
if doc is NULL, the paragraph number exceeds the number of paragraphs available 
or line number is larger than the number of lines in the paragraph; otherwise 
the function will return SUCCESS.*/
int remove_line(Document *doc, int paragraph_number, int line_number){
    int number_of_lines;
    if (doc == NULL || (MAX_PARAGRAPHS - paragraph_number) < 0 || 
    line_number > doc->paragraphs[paragraph_number - 1].number_of_lines){
        return FAILURE;
    } 

    /* if the line number is the last line, make the last line  empty and decrement the number of lines*/
    number_of_lines = doc->paragraphs[paragraph_number - 1].number_of_lines;

    if (line_number == number_of_lines){
        /* then we know we are removing the last line*/
        /*make the last line equal an empty string and then decriment number of lines by 1*/
        doc->paragraphs[paragraph_number - 1].lines[number_of_lines - 1][0] = '\0';
        doc->paragraphs[paragraph_number - 1].number_of_lines -= 1;
    }else{
        /*if the line number is not the last line move all the lines below it up by one and make the last line be 
        empty*/
        while (line_number < number_of_lines ){
            /*make the string at that line equal to the string at the line belwo it*/
            strcpy(doc->paragraphs[paragraph_number - 1].lines[line_number - 1], 
            doc->paragraphs[paragraph_number - 1].lines[line_number]);
            line_number += 1;
        }  
        /* now make the last line equal to an empty string to mark the end*/
        doc->paragraphs[paragraph_number - 1].lines[number_of_lines - 1][0] = '\0';
        doc->paragraphs[paragraph_number - 1].number_of_lines -= 1;
    }
    return SUCCESS;
}

/*The function will add the first data lines number of lines from the data array to the document. An empty
string in the array will create a new paragraph.It will accomplish this by doing the following. 
create a paragraph for document
add the first number of lines from the data array to the document
if there is an empty string in the array, create a new paragraph and add the remaining lines
to create a new paragrapgh, reuse the add_paragraph_after function */
int load_document(Document *doc, char data[][MAX_STR_SIZE + 1], int data_lines){
    int paragraph_number;
    int i, j;
    Document dummy;
    const char *doc_name = "Dummy Document ";
    if (doc == NULL || data == NULL || data_lines == 0){
        return FAILURE;
    }
    paragraph_number = doc->number_of_paragraphs;
    init_document(&dummy, doc_name);


    /*if this function is called when there are already paragraphs in the document,
    push all the paragraphs number_of_paragraphs amount down*/
   if (load_helper(&dummy, data, data_lines) == SUCCESS){
        int dummy_paragraphs = dummy.number_of_paragraphs; 
        /*check if there are already paragraphs in doc*/
        if (paragraph_number > 0){
            /*shift all the paragraphs down to make space for the dummy paragraphs*/
            for (i = doc->number_of_paragraphs - 1; i >= 0; i--) {
                int newPosition = i + dummy_paragraphs;
                if (newPosition < MAX_PARAGRAPHS) {
                    /*Shift the paragraph down by n spaces*/
                    doc->paragraphs[newPosition] = doc->paragraphs[i];
                }
            }
        }
        /* Now copy all the paragraphs from dummy's paragraph array to doc's paragraph's array*/
        for (j = 0; j < dummy.number_of_paragraphs; j++){
            doc->paragraphs[j] = dummy.paragraphs[j];
            doc->number_of_paragraphs += 1;
        }

   }
   return SUCCESS;
}

/* This method will take in a dummy document, add lines to the document's array and return*/
int load_helper(Document *dummy, char data[][MAX_STR_SIZE + 1], int data_lines){
    /*create an array of paragraphs*/
    /*return to caller*/
    /*caller will add to document by shifting any initial paragraphs in the array n times*/
    /*Create a dummy document, add to the dummy document's array*/
    int paragraph_number;
    int i;
    if (dummy == NULL || data == NULL || data_lines == 0){
        return FAILURE;
    }
    paragraph_number = dummy->number_of_paragraphs;
    if (add_paragraph_after(dummy, paragraph_number) == SUCCESS){
        paragraph_number += 1;
        i = 0; 
        while ( i < data_lines){
            if (strcmp(data[i], "") != 0){
                const char *new_line = data[i];
                append_line(dummy, paragraph_number, new_line); 
            } else if (strcmp(data[i], "") == 0){
                add_paragraph_after(dummy, paragraph_number);
                paragraph_number += 1;
            }
            i += 1;
        }
    }
   return SUCCESS;
}


/*The function will replace the text target with the text replacement 
everywhere it appears in the document.
*/
int replace_text(Document *doc, const char *target, const char *replacement){
    int i, j;
    Paragraph *paras; /*Create a paragrapgh pointer that points 
    to the paragraphs in the document's paraghraph array*/
    if (doc == NULL || target == NULL || replacement == NULL){
        return FAILURE;
    }

    /*access each paragrapgh for doc*/
    paras = doc->paragraphs;
    /*go through the paragraphs and compare the lines in each paragraph*/
    for (i = 0; i < doc->number_of_paragraphs; i++){
        for (j = 0; j < paras[i].number_of_lines; j++){
            char new_line[MAX_STR_SIZE + 1] = {'\0'} ;
            replace_in_line(new_line, paras[i].lines[j], target, replacement);
            strcpy(paras[i].lines[j], new_line);                         
        }
    }
    return SUCCESS;
}

/* This method takes an array, goes through the array and sees where the target is
then replaces the. This might be cuasing the warnings.*/
int replace_in_line(char *new_line, char *line, const char *target, const char *replacement){
   const char *temp = line;
   while (*temp != '\0'){
    /* check if the target exist*/
    const char *target_pos = strstr(temp, target);
    if (target_pos == NULL) {
        strcat(new_line, temp); /*copy the rest of the line if not*/
        break;
    }

    strncat(new_line, temp, target_pos - temp); /*copy part of the line before target*/
    strcat(new_line, replacement); /*replace target*/

    /*move past target*/
    temp = target_pos + strlen(target);
    }
    return SUCCESS;
}


/*
The function will highlight the text associated with target everywhere it 
appears in the document bysurrounding the text with the strings HIGHLIGHT 
START STR and HIGHLIGHT END STR (see document.h).
*/
int highlight_text(Document *doc, const char *target){
    int i, j;
    Paragraph *paras; /*Create a paragrapgh pointer that points 
    to the paragraphs in the document's paraghraph array*/
    if (doc == NULL || target == NULL){
        return FAILURE;
    }
    /*access each paragrapgh for doc*/
    paras = doc->paragraphs;
    /*go through the paragraphs and compare the lines in each paragraph*/
    for (i = 0; i < doc->number_of_paragraphs; i++){
        for (j = 0; j < paras[i].number_of_lines; j++){
            char new_line[MAX_STR_SIZE + 1] = {'\0'} ;
            highlight_in_line(new_line, paras[i].lines[j], target);
            strcpy(paras[i].lines[j], new_line);                         
        }
    }
    return SUCCESS;
}

/* This is a replica of the replace in line function just modified a 
to add highlight instead of replace*/
int highlight_in_line(char *new_line, char *line, const char *target){
   const char *temp = line;
   while (*temp != '\0'){
        /* check if the target exist*/
        const char *target_pos = strstr(temp, target);
        if (target_pos == NULL) {
            strcat(new_line, temp); /*copy the rest of the line if not*/
            break;
        }
        
        /*copy part of the line before target*/
        strncat(new_line, temp, target_pos - temp);
        strcat(new_line, HIGHLIGHT_START_STR); /*add start str */
        strcat(new_line, target); /*add target*/
        strcat(new_line, HIGHLIGHT_END_STR); /*add end str*/

        /*move past target*/
        temp = target_pos + strlen(target);
    }

    return SUCCESS;
}

/*
The function will remove the text target everywhere it appears in the 
document by using replace text function to replace target
with an empty string
*/
int remove_text(Document *doc, const char *target){
    if (doc == NULL || target == NULL){
        return FAILURE;
    }
    replace_text(doc, target, "");

    return SUCCESS;
}

/* This method will take in both a document and a filename
and it will then add lines from the file to the document*/
int load_file(Document *doc, const char *filename){
    int paragraph_number, i, j;
    Document dummy;
    FILE *input;
    const char *doc_name = "Dummy Document ";

    if (doc == NULL || filename == NULL){
        return FAILURE;
    }

    if ((input = fopen(filename, "r")) == NULL){
        return FAILURE;
    }
    paragraph_number = doc->number_of_paragraphs;
    init_document(&dummy, doc_name); /*a dummy doc to hold the lines that will be added*/

    /*if this function is called when there are already paragraphs in the document,
    push all the paragraphs number_of_paragraphs amount down*/
   if (load_file_helper(&dummy, filename, input) == SUCCESS){
        int dummy_paragraphs = dummy.number_of_paragraphs; 
        /*check if there are already paragraphs in doc*/
        if (paragraph_number > 0){
            /*shift all the paragraphs down to make space for the dummy paragraphs*/
            for (i = doc->number_of_paragraphs - 1; i >= 0; i--) {
                int newPosition = i + dummy_paragraphs;
                if (newPosition < MAX_PARAGRAPHS) {
                    /*Shift the paragraph down by n spaces*/
                    doc->paragraphs[newPosition] = doc->paragraphs[i];
                }
            }
        }
        /* Now copy all the paragraphs from dummy's paragraph array to doc's paragraph's array*/
        for (j = 0; j < dummy.number_of_paragraphs; j++){
            doc->paragraphs[j] = dummy.paragraphs[j];
            doc->number_of_paragraphs += 1;
        }
   }
   return SUCCESS;
}

/* This method will take in a dummy document, add lines to the document's array and return*/
int load_file_helper(Document *dummy, const char *filename, FILE *input){
    /*create an array of paragraphs*/
    /*return to caller*/
    /*caller will add to document by shifting any initial paragraphs in the array n times*/
    /*Create a dummy document, add to the dummy document's array*/
    int paragraph_number;
    int max_line = 1024;
    char line[MAX_STR_SIZE + 1];

    if (dummy == NULL || filename == NULL){
        return FAILURE;
    }
    paragraph_number = dummy->number_of_paragraphs;
    if (add_paragraph_after(dummy, paragraph_number) == SUCCESS){
        paragraph_number += 1;
           /* Copying data line by line */
        while (fgets(line, max_line, input) != NULL) {
            /* used to check if it is an empty line*/
            char *tempLine = line; 

            /*any blank lines will mark the beginning of
            a new paragraph*/
            while (isspace((unsigned char)*tempLine)) { /*skip trailing white spaces*/
                tempLine++;
            }
            if (*tempLine == '\0') { /* it is a blank line*/
                add_paragraph_after(dummy, paragraph_number); /*add a new paragraph*/
                paragraph_number += 1;   
            }else{
                append_line(dummy, paragraph_number, line);
            }
            
        }
        /* Closing streams */
        fclose(input);
    }
    return SUCCESS;
}


int save_document(Document *doc, const char *filename){
   FILE *output_file;
   int i, j;
   Paragraph *paras; /*Create a paragrapgh pointer that points 
   to the paragraphs in the document's paraghraph array*/

   /* Output file */
   if (doc == NULL || filename == NULL){
        return FAILURE;
   }
   if ((output_file = fopen(filename, "w")) == NULL) {
      return FAILURE;
   }

    /*access each paragrapgh's number of lines field*/
    paras = doc->paragraphs;
    /*go through the paragraphs and print lines in each paragraph*/
    for (i = 0; i < doc->number_of_paragraphs; i++){
        for (j = 0; j < paras[i].number_of_lines; j++){
            /*for line in each paragraph lines array, print out the line*/
            fprintf(output_file, "%s\n", paras[i].lines[j]);
        }
}

 /* Closing stream */
   fclose(output_file);

   return SUCCESS;
}
     