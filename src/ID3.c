#include <ID3.h>
#include <stdio.h>

ID3_problem* ID3_create_problem(tree_node* root, input_record* training_set, input_record* testing_set, int training_set_ratio) {
    ID3_problem* problem = (ID3_problem*)malloc(sizeof(ID3_problem));
    
    if(problem == NULL) {
        printf("Error at ID3_create_problem: memory allocation failed.\n");
        return NULL;
    }
    
    problem->training_set_ratio = training_set_ratio;
    problem->training_set = training_set;
    problem->testing_set  = testing_set;
    problem->root = root;
    
    return problem;
}

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

ID3_problem* ID3_generate_problem(input_record* records, int num_records) {
    int train_size = (int)(num_records * TRAINING_SET_RATIO);
    int test_size = num_records - train_size;

    input_record* training_set = (input_record*)malloc(train_size * sizeof(input_record));
    input_record* testing_set  = (input_record*)malloc(test_size * sizeof(input_record));

    if(training_set == NULL || testing_set == NULL) {
        printf("Error at ID3_generate_problem: memory allocation failed.\n");
        exit(1);
    }

    // pointer array allocation
    input_record** indices = (input_record**)malloc(num_records * sizeof(input_record*));
    if(indices == NULL) {
        printf("Error at ID3_generate_problem: memory allocation failed.\n");
        exit(1);
    }

    // initialize pointer array
    for(int i = 0; i < num_records; i++)
        indices[i] = &records[i];

    // shuffle pointers from array
    shuffle(indices, num_records);

    // training set attribution
    for(int i = 0; i < train_size; i++)
        training_set[i] = *indices[i];

    // testing set attribution
    for(int i = train_size; i < num_records; i++)
        testing_set[i - train_size] = *indices[i];

    free(indices);

    ID3_problem* problem = ID3_create_problem(NULL, training_set, testing_set, TRAINING_SET_RATIO);
    return problem;
}

void ID3_free_problem(ID3_problem* problem) {
    if(problem == NULL)
        return;

    free(problem->training_set);
    free(problem->testing_set);
    free(problem->root);
    free(problem);
}

int ID3_get_entropy(const input_record* records, int num_records) {

}

void ID3_begin_training(ID3_problem* problem, int train_size) {
    if(problem == NULL) {
        printf("Error at ID3_begin_training: NULL problem pointer.\n");
        return;
    }

    // root starts with all samples
    int* root_indices = (int*)malloc(train_size * sizeof(int));
    if(root_indices == NULL) {
        printf("Error at ID3_begin_training: memory allocation failed.\n");
        return;
    }
    
    // Initialize indices array with all training samples
    for(int i = 0; i < train_size; i++) {
        root_indices[i] = i;
    }

    // TODO: Implement ID3 algorithm to build the decision tree
    // problem->root = tree_create_internal(class_label, root_indices, train_size);
    
    printf("Training started with %d samples...\n", train_size);
}

