# MeshLib
redesined Mesh class

## usage
* for simplest case, use default:
```c++
#include "Mesh/mesh.h"

using CMesh = MeshLib::CBaseMesh<>;
using CVertex = typename CMesh::CVertex;
using CEdge = typename CMesh::CEdge;
using CFace = typename CMesh::CFace;
using CHalfEdge = typename CMesh::CHalfEdge;

CMesh * mesh = new CMesh();

```
* we can define custom classes for template parameters (CVertex, CEdge, CFace, CHalfEdge) of CBaseMesh, they are served as base class for CBaseMesh::CVertex, CBaseMesh::CFace, etc..  

```c++
#include <string>
#include "Mesh/mesh.h"
#include "Mesh/boundary.h"

using namespace std;

#define ADD_PROPERTY(T, x) \
private:\
    T m_##x; \
public:\
    inline T & x() { return m_##x; } \

class CBVertex
{
    ADD_PROPERTY(int, index)
    // add curvature property to vertex
    ADD_PROPERTY(double, curvature)
};
class CBEdge
{
    // edge weight
    ADD_PROPERTY(double, weight)
};
class CBFace
{
    ADD_PROPERTY(int, index)
};
class CBHalfEdge
{
    // opposite angle of this halfedge
    ADD_PROPERTY(double, angle)
};

using CMesh = MeshLib::CBaseMesh<CBVertex, CBEdge, CBFace, CBHalfEdge>;
// CMesh::CVertex is inherited from CBVertex
using CVertex = typename CMesh::CVertex;
using CEdge = typename CMesh::CEdge;
using CFace = typename CMesh::CFace;
using CHalfEdge = typename CMesh::CHalfEdge;
using CPoint = MeshLib::CPoint;
using CPoint2 = MeshLib::CPoint2;
using CLoop = MeshLib::CLoop<CBVertex, CBEdge, CBFace, CBHalfEdge>;
using CBoundary = MeshLib::CBoundary<CBVertex, CBEdge, CBFace, CBHalfEdge>;

```
* avoid define properties that already present in inherited class.  
if a property presented in both base class (say CBVertex) and inherited class (CMesh::CVertex), a pointer of that class will visit corresponding property in that class, for example:
```c++
CVertex * v = new CVertex();
int id = v->id();
CBVertex * v2 = v;
int id2 = v2->id(); // if id is defined in CBVertex, otherwise compiling error
```

* there is no need to import namespace MeshLib, since all names are already available

* all old iterators like "MeshVertexIterator" are not available, use "mesh->vertices()" instead, see following for an example

* with above defines, we can use CMesh class as follows:

```c++
int main()
{
    CMesh * mesh = new CMesh();
    mesh->read_m("data/eight.m");
    // renumber vertex, note that ->id() is used for input/output, if we do not
    // want to change that, use ->index() instead
    int k = 0;
    for (CVertex * v : mesh->vertices())
    {
        // can use base class point to child class
        CBVertex * vb = v;
        v->index() = ++k;
        v->string() = "index=(" + to_string(k) + " )";
        // visit vertex neighbors of this vertex
        for (CVertex * vj : v->vertices())
        {
            CEdge * e = mesh->edge(v, vj);
            e->weight() = 1;
        }
    }
    mesh->write_m("data/eight.2.m");

    return 0;
}
```
