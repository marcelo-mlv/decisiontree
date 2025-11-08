#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <tree.h>

tree_node* tree_create_node(float data) {
    tree_node* root = (tree_node*)malloc(sizeof(tree_node));
    if(root == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    
    memset(root, 0, sizeof(tree_node));

    root->children = (tree_node**)malloc(INITIAL_MAX_CHILDREN * sizeof(tree_node*));
    if(root->children == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    memset(root->children, 0, INITIAL_MAX_CHILDREN * sizeof(tree_node*));
    root->max_children = INITIAL_MAX_CHILDREN;
    root->data = data;

    return root;
}

void tree_delete(tree_node* root) {
    if(root == NULL)
        return;
    int i = 0;
    while(i < root->children_count) {
        tree_delete(root->children[i]);
        i++;
    }
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
    printf("Tree printing via hierarchical listing\n\n");

    if(root == NULL) {
        printf("\t[Empty Tree...]\n");
    } else {
        printf("      %.2f\n", root->data);
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
        printf("%s      %.2f\n", current_num, node->children[i]->data);
        
        // Recursive call with extended prefix
        tree_print_rec(node->children[i], current_num);
        
        i++;
    }
}