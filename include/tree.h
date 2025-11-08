#ifndef TREE_H
#define TREE_H

#define INITIAL_MAX_CHILDREN 2

typedef struct tree_node {
    int    children_count;
    int    max_children;
    float  data;
    struct tree_node**  children;
} tree_node;

tree_node* tree_create_node(float data);

void tree_delete(tree_node* root);

int tree_attach_child(tree_node* parent, tree_node* child);

void tree_print(tree_node* root);

void tree_print_rec(tree_node* node, char* prefix);

#endif