/*
    Sample input:

    n,y,y,n,y,n,n,n,y,n,y,n,n,y,democrat 
    n,y,y,n,y,n,n,n,n,n,y,y,y,y,democrat 
    n,y,n,y,y,n,n,n,n,y,y,y,n,y,republican

    Each line contains 14 attributes followed by a class label.
    This should be the only accepted format.
*/

#ifndef READ_INPUT_H
#define READ_INPUT_H

#define NUM_ATTRIBUTES 14

typedef enum { NO, YES } attribute_value;

typedef enum { DEMOCRAT, REPUBLICAN } class_label;

typedef struct input_record {
    attribute_value attributes[NUM_ATTRIBUTES];
    class_label     label;
} input_record;

int input_get_num_records();

input_record* input_read();

void input_print_records(input_record* records);

#endif