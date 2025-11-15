#ifndef TREE_H
#define TREE_H

#define INITIAL_MAX_CHILDREN 2

// Node enum for decision tree
// members:
// NODE_INTERNAL: Node that splits by attribute
// NODE_LEAF: Terminal node with classification
typedef enum {
    NODE_INTERNAL,
    NODE_LEAF
} node_kind;

// Decision Tree node struct
// members:
//   int children_count: Current number of children
//   int max_children: Maximum allocated children
//   tree_node** children: Array of child nodes
//   node_kind kind: Type of node (internal/leaf)
//   int attribute_index: Attribute index for split (valid if INTERNAL)
//   int class_label: Final classification (valid if LEAF)
//   int* sample_indices: Array containing indices of samples reaching this node
//   int sample_count: Number of samples
typedef struct tree_node {
    int    children_count;
    int    max_children;
    struct tree_node**  children;
    
    node_kind kind;
    int       attribute_index;
    int       class_label;
    int*      sample_indices;
    int       sample_count;
} tree_node;

// create leaf node
tree_node* tree_create_leaf(int class_label, const int* sample_indices, int sample_count);

// create internal node (split by attribute)
tree_node* tree_create_internal(int attribute_index, const int* sample_indices, int sample_count);

// add already created child to parent
int tree_attach_child(tree_node* parent, tree_node* child);

void tree_delete(tree_node* root);

void tree_print(tree_node* root);

void tree_print_rec(tree_node* node, char* prefix);

#endif