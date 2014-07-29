#include <map>
#include <stdio.h>
#include <stdlib.h>

typedef std::map<int, std::pair<int, int> > QTreeMapInner_t;
typedef std::map<int, QTreeMapInner_t> QTreeMap_t;

typedef QTreeMap_t::iterator QTreeMapIter_t;
typedef QTreeMapInner_t::iterator QTreeMapInnerIter_t;


int main()
{
    QTreeMap_t nodes;

    nodes[10][20] = std::make_pair(1, 2);
    nodes[1][20] = std::make_pair(2, 3);
    nodes[0][30] = std::make_pair(3, 4);

    for(QTreeMapIter_t iter = nodes.begin(); iter != nodes.end(); iter++)
    {

        for(QTreeMapInnerIter_t iter1 = iter->second.begin(); iter1 != iter->second.end(); iter1++)
        {
            // Nice one C++
            printf("[%d][%d] = (%d,%d)\n", iter->first, iter1->first, iter1->second.first, iter1->second.second);
        }
    }


    return 0;
}
