#include <ID3.h>
#include <stdio.h>
#include <math.h>

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

// Calculates the entropy of a set of records
// H(S) = -p_democrat * log2(p_democrat) - p_republican * log2(p_republican)
// Returns a value between 0.0 (pure) and 1.0 (50%-50%, maximum impurity)
float ID3_get_entropy(const input_record* records, int num_records) {
    if(num_records <= 0)
        return 0.0;
    
    int democrat_count = 0;
    int republican_count = 0;
    
    for(int i = 0; i < num_records; i++) {
        if(records[i].label == DEMOCRAT)
            democrat_count++;
        else
            republican_count++;
    }
    
    // If all are in the same class, entropy = 0 (pure)
    if(democrat_count == 0 || republican_count == 0) {
        return 0.0;
    }
    
    // Calculate proportions
    float p_democrat = (float)democrat_count / num_records;
    float p_republican = (float)republican_count / num_records;
    
    // H(S) = -p1*log2(p1) - p2*log2(p2)
    float entropy = -(p_democrat * log2f(p_democrat)) - (p_republican * log2f(p_republican));
    
    return entropy;
}

// Calculates the information gain for a specific attribute
// Gain(S, A) = H(S) - Σ(|S_v|/|S| * H(S_v))
// where S_v are the subsets created by splitting S on attribute A
float ID3_get_information_gain(const input_record* records, int num_records, int attribute_index) {
    if(num_records <= 0 || attribute_index < 0 || attribute_index >= NUM_ATTRIBUTES)
        return 0.0;
    
    // original entropy
    float total_entropy = ID3_get_entropy(records, num_records);
    
    // Divide records by attribute value (YES or NO)
    int yes_count = 0;
    int no_count = 0;
    
    // Count how many have YES and NO for this attribute
    for(int i = 0; i < num_records; i++) {
        if(records[i].attributes[attribute_index] == YES)
            yes_count++;
        else
            no_count++;
    }
    
    // Allocate temporary subsets
    input_record* yes_subset = (input_record*)malloc(yes_count * sizeof(input_record));
    input_record* no_subset = (input_record*)malloc(no_count * sizeof(input_record));
    
    if(yes_subset == NULL || no_subset == NULL) {
        printf("Error at ID3_get_information_gain: memory allocation failed.\n");
        free(yes_subset);
        free(no_subset);
        return 0.0;
    }
    
    // Preencher os subsets
    int yes_idx = 0;
    int no_idx = 0;
    
    for(int i = 0; i < num_records; i++) {
        if(records[i].attributes[attribute_index] == YES) {
            yes_subset[yes_idx] = records[i];
            yes_idx++;
        } else {
            no_subset[no_idx] = records[i];
            no_idx++;
        }
    }
    
    // Calculate weighted entropy of subsets
    float yes_entropy = ID3_get_entropy(yes_subset, yes_count);
    float no_entropy  = ID3_get_entropy(no_subset, no_count);
    
    float weighted_entropy = ((float)yes_count / num_records) * yes_entropy +
                            ((float)no_count / num_records) * no_entropy;
    
    free(yes_subset);
    free(no_subset);
    
    // Information gain = original entropy - weighted entropy
    float information_gain = total_entropy - weighted_entropy;
    
    return information_gain;
}

// Finds the best attribute (with highest information gain)
// Returns the attribute index, or -1 if error
int ID3_find_best_attribute(const input_record* records, int num_records, const int* available_attributes, int num_available) {
    if(num_records <= 0 || available_attributes == NULL || num_available <= 0) {
        return -1;
    }
    
    int best_attribute = -1;
    float best_gain = -1.0;
    
    for(int i = 0; i < num_available; i++) {
        int attr_idx = available_attributes[i];
        float gain = ID3_get_information_gain(records, num_records, attr_idx);
        
        if(gain > best_gain) {
            best_gain = gain;
            best_attribute = attr_idx;
        }
    }
    
    return best_attribute;
}

void ID3_begin_training(ID3_problem* problem, int train_size) {
    if(problem == NULL) {
        printf("Error at ID3_begin_training: NULL problem pointer.\n");
        return;
    }

    // root starts with all samples
    int* root_samples = (int*)malloc(train_size * sizeof(int));
    if(root_samples == NULL) {
        printf("Error at ID3_begin_training: memory allocation failed.\n");
        return;
    }
    
    // Initialize samples array with all training samples
    for(int i = 0; i < train_size; i++) {
        root_samples[i] = i;
    }
    
    // For now, just find the best attribute and create root
    int* available_attrs = (int*)malloc(NUM_ATTRIBUTES * sizeof(int));
    for(int i = 0; i < NUM_ATTRIBUTES; i++) {
        available_attrs[i] = i;
    }
    
    int best_attr = ID3_find_best_attribute(problem->training_set, train_size, available_attrs, NUM_ATTRIBUTES);
    free(available_attrs);
    
    if(best_attr >= 0) {
        problem->root = tree_create_internal(best_attr, root_samples, train_size);
        printf("Training started with %d samples. Root splits on attribute %d\n", train_size, best_attr);
        tree_print(problem->root);
    } else {
        printf("Training failed: could not find best attribute\n");
        free(root_samples);
    }
}

