#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <input.h>

#define BUF_SIZE 256

int input_get_num_records() {
    FILE *input_file;
    int lineno = 0;
    char buffer[BUF_SIZE];

    input_file = fopen("../input.txt", "r");
    if (input_file == NULL) {
        perror("Error opening file");
        return -1;
    }

    while(fgets(buffer, sizeof(buffer), input_file) != NULL) {
        lineno++;
    }

    fclose(input_file);
    return lineno;
}

input_record* input_read() {
    FILE *input_file, *linecounter;
    
    int lineno = input_get_num_records();

    if (lineno <= 0) {
        fprintf(stderr, "Error at input_read(): no valid records found.\n");
        return NULL;
    }

    char buffer[BUF_SIZE];
    input_record* records = (input_record*)malloc(lineno * sizeof(input_record));

    if(records == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    
    input_file = fopen("../input.txt", "r");

    memset(buffer, 0, sizeof(buffer));
    int i = 0, j;
    while(i < lineno) {
        fgets(buffer, sizeof(buffer), input_file);
        char *p = buffer;
        j = 0;

        while(j < NUM_ATTRIBUTES) {
            if(*p == 'y' || *p == 'n') {
                records[i].attributes[j] = *p == 'y' ? YES : NO;
                p += 2; // Move past the attribute and the comma
            } else {
                fprintf(stderr, "Invalid attribute value: \'%c\'\n", *p);
                fclose(input_file);
                return NULL;
            }
            j++;
        }

        if (strncmp(p, "democrat", 8) == 0) {
            records[i].label = DEMOCRAT;
        } else if (strncmp(p, "republican", 10) == 0) {
            records[i].label = REPUBLICAN;
        } else {
            fprintf(stderr, "Invalid class label: %s\n", p);
            fclose(input_file);
            return NULL;
        }
        i++;
    }

    fclose(input_file);
    return records;
}

void input_print_records(input_record* records, int num_records) {
    if (records == NULL) {
        printf("No records to print.\n");
        return;
    }

    input_record* p = records;

    for(int i = 0; i < num_records; i++) {
        printf("Senator #%d: \n", i + 1);
        for(int j = 0; j < NUM_ATTRIBUTES; j++) {
            printf("\tAttribute #%d: %s\n", j + 1, p->attributes[j] == YES ? "yes" : "no");
        }
        printf("\tSTANCE: %s\n\n", p->label == DEMOCRAT ? "democrat" : "republican");
        p++;
    }
}
