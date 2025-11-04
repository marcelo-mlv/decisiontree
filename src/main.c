#include <stdio.h>

#include <tree.h>

int main() {
    tree_node* root;
    tree_node *p, *q, *r;
    
    root = tree_create_node(2);

    p = tree_addchild(root, 3);
    p = tree_addchild(root, 4);
    p = tree_addchild(root, 5);
    p = tree_addchild(root, 6);
    p = tree_addchild(root, 7);
    p = tree_addchild(root, 8);

    r = tree_addchild(p, 83);
    r = tree_addchild(p, 86);
    
    q = tree_addchild(r, 868);

    q = tree_addchild(q, 8681);

    tree_print(root);
    tree_delete(root);
}
