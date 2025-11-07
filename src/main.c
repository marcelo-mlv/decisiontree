#include <stdio.h>
#include <stdlib.h>

#include <tree.h>
#include <input.h>

int main() {
    input_record* records = input_read();
    if (records == NULL) {
        printf("Error at main:input_read(): could not read input records.\n");
        return 1;
    }

    input_print_records(records);

    free(records);
    return 0;
}
