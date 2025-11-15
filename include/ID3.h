#ifndef ID3_H
#define ID3_H

#include <stdlib.h>

#include <input.h>
#include <tree.h>

#define TRAINING_SET_RATIO 0.8

// Problem struct for ID3 algorithm
// members:
// tree_node* root: Root of the decision tree
// input_record* training_set: Training dataset
// input_record* testing_set: Testing dataset
// int training_set_ratio: Ratio of training set size to total dataset size
typedef struct ID3_problem {
    tree_node* root;
    input_record* training_set;
    input_record* testing_set;
    int training_set_ratio;

} ID3_problem;

// Creates training and testing datasets from the given records.
// Shuffling is done using Fisher-Yates algorithm with a pointer array.
ID3_problem* ID3_create_problem(tree_node* root, input_record* training_set, input_record* testing_set, int training_set_ratio);

// Free problem struct
void ID3_free_problem(ID3_problem* problem);

// Generate problem from records
// Splits records into training and testing sets
// Initializes decision tree root as NULL
ID3_problem* ID3_generate_problem(input_record* records, int num_records);

// Calcula a entropia de um conjunto de registros
// Retorna valor entre 0.0 (puro) e 1.0 (máxima impureza)
float ID3_get_entropy(const input_record* records, int num_records);

// Calcula o ganho de informação para um atributo específico
float ID3_get_information_gain(const input_record* records, int num_records, int attribute_index);

// Encontra o melhor atributo (maior ganho de informação)
int ID3_find_best_attribute(const input_record* records, int num_records, const int* available_attributes, int num_available);

// Wrapper function (compatibilidade)
float ID3_get_gain(const input_record* records, int num_records);

// Trains the decision tree using ID3 algorithm on the problem's training set
// The root will be created and stored in problem->root
void ID3_begin_training(ID3_problem* problem, int train_size);

#endif