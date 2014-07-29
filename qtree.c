#include <stdlib.h>
#include "qtree.h"
#include "image.h"

// Add a node to the list
static QTreeNode* qtree_appendNode(QTree* tree)
{
    QTreeNode* n = tree->leaves;
    while(n->next) {
        n = n->next;
    }
    QTreeNode* newNode = (QTreeNode*)malloc(sizeof(QTreeNode));
    n->next = newNode;
    newNode->prev = n;
    newNode->next = NULL;
    return newNode;
}
// Remove a node from the list
static void qtree_removeNode(QTreeNode* n)
{
    if(n->prev) {
        n->prev->next = n->next;
    }
    if(n->next) {
        n->next->prev = n->prev;
    }
    free(n);    
}

QTree* qtree_make(const Image* img, int (*pred)(const Image* img, const QTreeNode* node))
{
    QTree* ret = (QTree*)malloc(sizeof(QTree));
    ret->leaves = (QTreeNode*)malloc(sizeof(QTreeNode));
    ret->leaves->prev = NULL;
    ret->leaves->next = NULL;
    ret->depth = 0;
    return ret;
}
