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
    
    // Initialize: all samples and all attributes available
    int* root_samples = (int*)malloc(train_size * sizeof(int));
    if(root_samples == NULL) {
        printf("Error at ID3_begin_training: memory allocation failed.\n");
        return;
    }
    
    for(int i = 0; i < train_size; i++) {
        root_samples[i] = i;
    }
    
    int* all_attrs = (int*)malloc(NUM_ATTRIBUTES * sizeof(int));
    if(all_attrs == NULL) {
        printf("Error at ID3_begin_training: memory allocation failed.\n");
        free(root_samples);
        return;
    }
    
    for(int i = 0; i < NUM_ATTRIBUTES; i++)
        all_attrs[i] = i;
    
    // Start recursive training
    ID3_train_rec(&(problem->root), problem->training_set, root_samples, train_size, all_attrs, NUM_ATTRIBUTES);
    
    free(all_attrs);
    // root_samples ownership transferred to tree node
}

// Helper function to get majority class from sample indices
static int get_majority_class(input_record* training_set, int* sample_indices, int num_samples) {
    int democrat_count = 0;
    int republican_count = 0;
    
    for(int i = 0; i < num_samples; i++) {
        int idx = sample_indices[i];
        if(training_set[idx].label == DEMOCRAT)
            democrat_count++;
        else
            republican_count++;
    }
    
    return (democrat_count >= republican_count) ? DEMOCRAT : REPUBLICAN;
}

// Helper function to check if all samples have same class
static int all_same_class(input_record* training_set, int* sample_indices, int num_samples) {
    if(num_samples == 0) return 1;
    
    class_label first_label = training_set[sample_indices[0]].label;
    for(int i = 1; i < num_samples; i++) {
        if(training_set[sample_indices[i]].label != first_label)
            return 0;
    }
    return 1;
}

// Helper to create subset of records based on indices (for information gain calc)
static void get_records_by_indices(input_record* training_set, int* sample_indices, 
                                   int num_samples, input_record* output) {
    for(int i = 0; i < num_samples; i++) {
        output[i] = training_set[sample_indices[i]];
    }
}

void ID3_train_rec(tree_node** node_ptr,
                   input_record* training_set,
                   int* sample_indices,
                   int num_samples,
                   int* available_attributes,
                   int num_available_attributes) {
    
    // no samples (shouldnt happen)
    if(num_samples == 0) {
        *node_ptr = NULL;
        free(sample_indices);
        return;
    }
    
    // LEAF COND1: all samples are from same class
    if(all_same_class(training_set, sample_indices, num_samples)) {
        int label = training_set[sample_indices[0]].label;
        *node_ptr = tree_create_leaf(label, sample_indices, num_samples);
        return;
    }
    
    // LEAF COND2: no more attributes, create leaf with majority class
    if(num_available_attributes == 0) {
        int majority = get_majority_class(training_set, sample_indices, num_samples);
        *node_ptr = tree_create_leaf(majority, sample_indices, num_samples);
        return;
    }
    
    // recursive case: find best attribute and split
    
    // need to extract actual records for information gain calculation
    input_record* current_records = (input_record*)malloc(num_samples * sizeof(input_record));
    if(current_records == NULL) {
        printf("Error: memory allocation failed\n");
        free(sample_indices);
        return;
    }
    get_records_by_indices(training_set, sample_indices, num_samples, current_records);
    
    int best_attr = ID3_find_best_attribute(current_records, num_samples, available_attributes, num_available_attributes);
    free(current_records);
    
    // LEAF COND3: no good attribute found
    if(best_attr < 0) {
        int majority = get_majority_class(training_set, sample_indices, num_samples);
        *node_ptr = tree_create_leaf(majority, sample_indices, num_samples);
        return;
    }
    
    // INTERNAL node creation
    *node_ptr = tree_create_internal(best_attr, sample_indices, num_samples);

    // children nodes (YES/NO) creation

    // dataset split for YES/NO branches

    // split samples by attribute value (YES/NO)
    int* yes_indices = (int*)malloc(num_samples * sizeof(int));
    int* no_indices = (int*)malloc(num_samples * sizeof(int));
    
    if(yes_indices == NULL || no_indices == NULL) {
        printf("Error: memory allocation failed\n");
        free(yes_indices);
        free(no_indices);
        return;
    }
    
    int yes_count = 0;
    int no_count = 0;
    
    for(int i = 0; i < num_samples; i++) {
        int idx = sample_indices[i];
        if(training_set[idx].attributes[best_attr] == YES) {
            yes_indices[yes_count] = idx;
            yes_count++;
        } else {
            no_indices[no_count] = idx;
            no_count++;
        }
    }
    
    int remaining_count = num_available_attributes - 1;
    
    // recursive calls for children (YES branch then NO branch)
    tree_node* yes_child = NULL;
    tree_node* no_child = NULL;
    
    // YES branch
    if(yes_count > 0) {
        // Create remaining_attrs array for this branch
        int* yes_remaining_attrs = (int*)malloc(remaining_count * sizeof(int));
        if(yes_remaining_attrs == NULL) {
            printf("Error: memory allocation failed\n");
            free(yes_indices);
            free(no_indices);
            return;
        }
        
        int idx = 0;
        for(int i = 0; i < num_available_attributes; i++) {
            if(available_attributes[i] != best_attr) {
                yes_remaining_attrs[idx] = available_attributes[i];
                idx++;
            }
        }
        
        ID3_train_rec(&yes_child, training_set, yes_indices, yes_count, yes_remaining_attrs, remaining_count);
        if(yes_child != NULL)
            tree_attach_child(*node_ptr, yes_child);
        
        free(yes_remaining_attrs);
    } else free(yes_indices);
    
    // NO branch
    if(no_count > 0) {
        // Create remaining_attrs array for this branch
        int* no_remaining_attrs = (int*)malloc(remaining_count * sizeof(int));
        if(no_remaining_attrs == NULL) {
            printf("Error: memory allocation failed\n");
            free(no_indices);
            return;
        }
        
        int idx = 0;
        for(int i = 0; i < num_available_attributes; i++) { 
            if(available_attributes[i] != best_attr) {
                no_remaining_attrs[idx] = available_attributes[i];
                idx++;
            }
        }
        
        ID3_train_rec(&no_child, training_set, no_indices, no_count, no_remaining_attrs, remaining_count);
        if(no_child != NULL)
            tree_attach_child(*node_ptr, no_child);
        
        free(no_remaining_attrs);
    } else free(no_indices);
}

float ID3_begin_testing(ID3_problem* problem, int test_size) {
    int correct_count = 0;
    for(int i = 0; i < test_size; i++) {
        int label = ID3_test_case(problem->root, &problem->testing_set[i]);
        if(label == problem->testing_set[i].label)
            correct_count++;
    }
    return (float)correct_count/test_size;
}

int ID3_test_case(tree_node* node, input_record* record) {
    if(node == NULL) {
        printf("Error at ID3_test_case: NULL node pointer.\n");
        return -1;
    }
    
    // If leaf, return class label
    if(node->kind == NODE_LEAF)
        return node->class_label;
    
    // Internal node: check attribute and traverse accordingly
    int attr_index = node->decision_attr_index;
    attribute_value attr_value = record->attributes[attr_index];
    
    // children[0] = YES branch, children[1] = NO branch
    tree_node* next_node = NULL;
    
    if(attr_value == YES) {
        if(node->children_count > 0 && node->children[0] != NULL)
            next_node = node->children[0];
    } else {
        if(node->children_count > 1 && node->children[1] != NULL)
            next_node = node->children[1];
    }
    
    // fallback condition
    if(next_node == NULL) {
        printf("Warning: missing child node for attribute %d=%s, using fallback.\n", 
               attr_index, attribute_value_to_string(attr_value));
        // Return a default
        return DEMOCRAT;
    }
    
    return ID3_test_case(next_node, record);
}
