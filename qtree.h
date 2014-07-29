#ifndef qtreeH
#define qtreeH


#include <map>
#include "image.h"


// This is a transient structure that is only used when
// constructing the tree or returning node information.
// It is not stored in the tree
template<class DataType>
struct QTreeNode
{
    int top, left;
    int width, height;
    DataType data;
    QTreeNode()
    {
        left = top = width = height = -1;
    }
    QTreeNode(int t, int l, int w, int h)
    {
        top = t;
        left = l;
        width = w; 
        height = h;
    }

    QTreeNode(int t, int l, int w, int h, DataType d)
    {
        top = t;
        left = l;
        width = w; 
        height = h;
        data = d;
    }
    bool isValid() const
    {
        return top >= 0 && left >= 0 && width > 0 && height > 0;
    }
    int size() const
    {
        return width * height;
    }
    int right() {
        return left + width;
    }
    int bottom() {
        return top + height;
    }

};


// A quadtree with T data associated with each node.
//
// The quadtree is stored as:
// map: [int][int] = pair<int, void*>
// i.e. [top][left].first = size
// i.e. [top][left].second = void* user data
template<class DataType>
struct QTree
{

    typedef std::map<int, QTreeNode<DataType> > QTreeMapInner_t;
    typedef std::map<int, QTreeMapInner_t> QTreeMap_t;

    typedef typename QTreeMap_t::iterator QTreeMapIter_t;
    typedef typename QTreeMapInner_t::iterator QTreeMapInnerIter_t;

    QTreeMap_t nodes;

    void release()
    {
        nodes.clear();
    }

    template<class SplitPred>
    void make(const Image& img, SplitPred pred)
    {
        // Start with the entire image and start splitting the node
        QTreeNode<DataType> firstnode(0, 0, img.width, img.height);
        this->make<SplitPred>(img, pred, firstnode);
    }
    void setData(int top, int left, const DataType& data)
    {
        if(isnode(top,left))
        {
            this->nodes[top][left].data = data;
        }
    }

    DataType getData(int top, int left)
    {
        if(isnode(top, left))
        {
            return this->nodes[top][left].data;
        }
        return DataType();

    }

    QTreeNode<DataType> getNode(int top, int left)
    {
        if(isnode(top, left))
        {
            return nodes[top][left];
        }
        return QTreeNode<DataType>();
    }

    bool isnode(int top, int left)
    {
        return (this->nodes.find(top) != this->nodes.end()) &&
               (this->nodes[top].find(left) != this->nodes[top].end());
    }

    // Visit all nodes calling the functor for each. Functor will
    // be called for the following operator:
    // bool func.operator()(QTreeNode<DataType>& node);
    // the following operator defined
    template<class VisitFunc>
    void foreach(VisitFunc func)
    {
        for(QTreeMapIter_t iter = this->nodes.begin(); iter != this->nodes.end(); iter++)
        {
            for(QTreeMapInnerIter_t iter1 = iter->second.begin(); iter1 != iter->second.end(); iter1++)
            {
                bool result = func(iter1->second);
                if(!result)
                {
                    return;
                }
            }
        }
    }

    // Search for the node with data equal to the given needle.
    // Assumes T has equality operator== defined.
    // Functor will be called using the folowing overload
    // func.operator()(const QTreeNode<DataType>& node)
    template<class FindPred>
    QTreeNode<DataType> findNode(FindPred pred)
    {
        for(QTreeMapIter_t iter = this->nodes.begin(); iter != this->nodes.end(); iter++)
        {
            for(QTreeMapInnerIter_t iter1 = iter->second.begin(); iter1 != iter->second.end(); iter1++)
            {
                QTreeNode<DataType> node = iter1->second;
                bool result = pred(node);
                if(result)
                {
                    return node;
                }
            }
        }
        return QTreeNode<DataType>();
    }

    // Render the quads to an image
    void drawQuads(Image& outimg)
    {

        for(QTreeMapIter_t iter = this->nodes.begin(); iter != this->nodes.end(); iter++)
        {
            for(QTreeMapInnerIter_t iter1 = iter->second.begin(); iter1 != iter->second.end(); iter1++)
            {
                QTreeNode<DataType>& node = iter->second;
                uint8_t* lineTop = outimg.scanline(node.top);
                uint8_t* lineBottom = outimg.scanline(node.bottom()-1);
                for(int x = node.left; x < node.right(); ++x)
                {
                    lineTop[x] = 255;
                    lineBottom[x] = 255;
                }

                for(int y = node.top; y < node.bottom(); ++y)
                {
                    outimg.pixels[node.left + y * outimg.width] = 255;
                    outimg.pixels[node.right()-1 + y * outimg.width] = 255;
                }
            }
        }
    }
private:

    template<class SplitPred>
    void make(const Image& img, SplitPred pred, QTreeNode<DataType>& node)
    {
        if(pred(img, node))
        {
            // Split this node
            int wOn2= node.width / 2;
            int hOn2 = node.height / 2;

            QTreeNode<DataType> topleft(node.top, node.left, wOn2, hOn2, node.data);
            this->make(img, pred, topleft);
            
            QTreeNode<DataType> topright(node.top, node.left + wOn2, wOn2, hOn2, node.data);
            this->make(img, pred, topright);

            QTreeNode<DataType> bottomleft(node.top + hOn2, node.left, wOn2, hOn2, node.data);
            this->make(img, pred, bottomleft);
            
            QTreeNode<DataType> bottomright(node.top + hOn2, node.left + wOn2, wOn2, hOn2, node.data);
            this->make(img, pred, bottomright);
            
        }
        // Not splitting, add this node to the tree
        this->nodes[node.top][node.left] = node;
    }
};




#endif // qtreeH

#if 0
// goddam ugly C++!
// Exmaples of the functors for make, findNode and foreach
//
// The Make functor, built for QTreeNodes with integer data
// This will result in a tree with nodes containing no less
// than 100 pixels.
struct SplitPred
{
    bool operator()(const Image& img, QTreeNode<int>& node)
    {
        // example of setting node data in the make functor
        node.data = img.size; 
        return node.size() > 100;
    }
}

// VisitFunctor
// A foreach functor for QTreeNodes of double data type
struct VisitFunctor
{
    bool operator()(QTreeNode<double>& node)
    {
        // if the data is < 10 we stop the foreach loop
        return node.data >= 10;
    }
}

// A find functor for QTreeNodes of any type
template<class DataType>
struct FindPred 
{
    DataType needle;
    explicit FindPred(DataType n) {needle = n;}
    bool operator()(const QTreeNode<DataType>& node)
    {
        return node.data == needle;
    }
}

#endif
