#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <tree.h>
#include <input.h>

tree_node* tree_create_leaf(int class_label, const int* sample_indices, int sample_count) {
    tree_node* node = (tree_node*)malloc(sizeof(tree_node));
    if(node == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    
    memset(node, 0, sizeof(tree_node));

    node->children = (tree_node**)malloc(INITIAL_MAX_CHILDREN * sizeof(tree_node*));
    if(node->children == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        free(node);
        return NULL;
    }

    memset(node->children, 0, INITIAL_MAX_CHILDREN * sizeof(tree_node*));
    node->max_children = INITIAL_MAX_CHILDREN;
    
    /* Configuring as Leaf */
    node->kind = NODE_LEAF;
    node->decision_attr_index = -1;
    node->class_label = class_label;
    
    /* Copiar índices de exemplos */
    node->sample_count = sample_count;
    if (sample_count > 0 && sample_indices != NULL) {
        node->sample_indices = (int*)malloc(sample_count * sizeof(int));
        if (node->sample_indices == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            free(node->children);
            free(node);
            return NULL;
        }
        memcpy(node->sample_indices, sample_indices, sample_count * sizeof(int));
    } else {
        node->sample_indices = NULL;
    }

    return node;
}

tree_node* tree_create_internal(int attribute_index, const int* sample_indices, int sample_count) {
    tree_node* node = (tree_node*)malloc(sizeof(tree_node));
    if(node == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    
    memset(node, 0, sizeof(tree_node));

    node->children = (tree_node**)malloc(INITIAL_MAX_CHILDREN * sizeof(tree_node*));
    if(node->children == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        free(node);
        return NULL;
    }

    memset(node->children, 0, INITIAL_MAX_CHILDREN * sizeof(tree_node*));
    node->max_children = INITIAL_MAX_CHILDREN;
    
    // Configuring as Internal Node
    node->kind = NODE_INTERNAL;
    node->decision_attr_index = attribute_index;
    node->class_label = -1;
    
    // Copy sample indices
    node->sample_count = sample_count;
    if (sample_count > 0 && sample_indices != NULL) {
        node->sample_indices = (int*)malloc(sample_count * sizeof(int));
        if (node->sample_indices == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            free(node->children);
            free(node);
            return NULL;
        }
        memcpy(node->sample_indices, sample_indices, sample_count * sizeof(int));
    } else {
        node->sample_indices = NULL;
    }

    return node;
}

void tree_delete(tree_node* root) {
    if(root == NULL)
        return;
    int i = 0;
    while(i < root->children_count) {
        tree_delete(root->children[i]);
        i++;
    }
    free(root->sample_indices);
    free(root->children);
    free(root);
}

int tree_attach_child(tree_node* parent, tree_node* child) {
    if (parent == NULL || child == NULL) {
        fprintf(stderr, "Invalid parent/child node\n");
        return -1;
    }

    // Reallocation with double the size
    if (parent->children_count == parent->max_children) {
        parent->max_children *= 2;
        tree_node** new_children = (tree_node**)realloc(parent->children, sizeof(tree_node*) * parent->max_children);
        if (!new_children) {
            fprintf(stderr, "Memory reallocation failed\n");
            parent->max_children /= 2;
            return -1;
        }
        parent->children = new_children;
    }

    parent->children[parent->children_count++] = child;
    return 0;
}

void tree_print(tree_node* root) {
    printf("--------------------------------------------------\n");
    printf("Decision Tree Structure\n\n");

    if(root == NULL) {
        printf("\t[Empty Tree...]\n");
    } else {
        if (root->kind == NODE_LEAF) {
            printf("      LEAF: class=%s\n", class_label_to_string(root->class_label));
        } else {
            printf("      SPLIT on attribute %d\n", root->decision_attr_index);
        }
        tree_print_rec(root, "");
    }
    
    printf("--------------------------------------------------\n");
}

void tree_print_rec(tree_node* node, char* prefix) {
    if(node == NULL) {
        return;
    }
    
    int i = 0;
    while(i < node->children_count) {
        // Create current number string
        char current_num[32];
        sprintf(current_num, "%s%d.", prefix, i + 1);
        
        // Print current child
        if (node->children[i]->kind == NODE_LEAF) {
            printf("%s      LEAF: class=%s\n", current_num, class_label_to_string(node->children[i]->class_label));
        } else {
            printf("%s      SPLIT on attribute %d\n", current_num, node->children[i]->decision_attr_index);
        }
        
        // Recursive call with extended prefix
        tree_print_rec(node->children[i], current_num);
        
        i++;
    }
}