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
        v->string() = "index=(" + to_string(k) + ")";
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
