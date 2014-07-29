#ifndef qtreeH
#define qtreeH


#include <map>
#include "image.h"

// Quick'n'dirty quadtree implementation, no hashing just a
// simple likned list of nodes.
typedef struct _QTreeNode
{
    int dims; // dimensions of the quadrant
    int top, left; // top-left coordinate in the image
    void* data; // The data associated with this node (region stats, whatever)
} QTreeNode;


typedef std::map< std::map< std::pair<int, void*> > > QTreeMap;
struct QTree
{
    QTreeMap map;
    make(Image
};

// make a quadtree and return the list of leaf nodes
// Takes an image and a predicate function. The predicate
// function has the signature:
// 
// int pred(const Image* img, const QTreeNode* node);
// 
// It should return true to split the node, false to keep the node as-is.
QTree* qtree_make(const Image* img, bool (*pred)(const Image* img, const QTreeNode* node));

// Free the quadtree given any node
void qtree_free(QTree* node); 

// Find the node matching the top and left coordinates
// Returns an invalid node if not found
QTreeNode* qtree_find(const QTree* tree, int top, int left);

// Search the tree for a node using the given predicate. 
// The signature for the predicate is:
// 
// bool pred(const QTreeNode* node)
//
// It should return true for a match, false otherwise.
QTreeNode* qtree_find(const QTree* tree, bool(*pred)(const QTreeNode* node));;


    
#endif // qtreeH

