#ifndef ID3_H
#define ID3_H

#include <input.h>
#include <stdlib.h>

#define TRAINING_SET_RATIO 0.8

// Splits the dataset randomly into training and testing sets based on TRAINING_SET_RATIO
// allocates the dataset in training_set and testing_set, 
// allowing the caller to *free records array* if desired.
void ID3_build_datasets(input_record* records, int num_records, input_record** training_set, input_record** testing_set);

#endif