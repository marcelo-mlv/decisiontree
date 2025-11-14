#include <stdio.h>
#include <stdlib.h>

#include <tree.h>
#include <input.h>
#include <ID3.h>

int main() {
    input_record* records = input_read();
    if (records == NULL) {
        printf("Error at main: could not read input records.\n");
        return 1;
    }

    int num_records = input_get_num_records();
    if (num_records <= 0) {
        printf("Error at main: invalid number of records.\n");
        free(records);
        return 1;
    }

    input_record* training_set = NULL;
    input_record* testing_set = NULL;

    ID3_build_datasets(records, num_records, &training_set, &testing_set);

    int train_size = (int)(num_records * TRAINING_SET_RATIO);
    int test_size = num_records - train_size;

    free(records);

    input_print_records(training_set, train_size);
    input_print_records(testing_set, test_size);

    free(training_set);
    free(testing_set);
    return 0;
}
