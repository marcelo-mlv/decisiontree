#include <ID3.h>
#include <stdio.h>

// Fisher-Yates shuffle algorithm to randomize an array of pointers to records
static void shuffle(input_record** arr, int n) {
    if(n <= 1) return;
    for(int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);

        input_record* tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }
}

void ID3_build_datasets(input_record* records, int num_records, input_record** training_set, input_record** testing_set) {
    int train_size = (int)(num_records * TRAINING_SET_RATIO);
    int test_size = num_records - train_size;

    *training_set = (input_record*)malloc(train_size * sizeof(input_record));
    *testing_set  = (input_record*)malloc(test_size * sizeof(input_record));

    if(*training_set == NULL || *testing_set == NULL) {
        printf("Error at ID3_build_datasets: memory allocation failed.\n");
        exit(1);
    }

    // Create pointer array referencing original records
    input_record** indices = (input_record**)malloc(num_records * sizeof(input_record*));
    if(indices == NULL) {
        printf("Error at ID3_build_datasets: memory allocation failed.\n");
        exit(1);
    }

    for(int i = 0; i < num_records; i++)
        indices[i] = &records[i];

    shuffle(indices, num_records);

    for(int i = 0; i < train_size; i++)
        (*training_set)[i] = *indices[i];

    for(int i = train_size; i < num_records; i++)
        (*testing_set)[i - train_size] = *indices[i];

    free(indices);
}
