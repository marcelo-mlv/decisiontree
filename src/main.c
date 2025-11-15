#include <stdio.h>
#include <stdlib.h>

#include <tree.h>
#include <input.h>
#include <ID3.h>

int main() {

    // Read all input records
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

    ID3_problem* problem = ID3_generate_problem(records, num_records);

    int train_size = (int)(num_records * TRAINING_SET_RATIO);
    int test_size = num_records - train_size;

    free(records);

    // Begin training (decision tree construction)
    ID3_begin_training(problem, train_size);

    ID3_free_problem(problem);
    return 0;
}
