#pragma once

#include "Mesh/mesh.h"
#include "Mesh/dynamicmesh.h"
#include "Mesh/boundary.h"

#include <map>

#ifndef EPS
#define EPS 1e-6
#endif // !EPS

#define ADD_PROPERTY(T, x) \
private:\
    T m_##x; \
public:\
    inline T & x() { return m_##x; } \

class CTarget;

class CHVertex
{
    ADD_PROPERTY(int, index)
    ADD_PROPERTY(bool, fixed)
    ADD_PROPERTY(bool, cut)
    ADD_PROPERTY(int, component_id)
};
class CHEdge
{
    ADD_PROPERTY(bool, cut)
};
class CHFace
{
    ADD_PROPERTY(int, index)
    ADD_PROPERTY(int, component_id)
};
class CHHalfEdge
{

};

using CMesh = MeshLib::CBaseMesh<CHVertex, CHEdge, CHFace, CHHalfEdge>;
using CVertex = CMesh::CVertex;
using CEdge = CMesh::CEdge;
using CFace = CMesh::CFace;
using CHalfEdge = CMesh::CHalfEdge;
using namespace std;

typedef map<int, vector<CVertex*>> Cut;
typedef map<int, CVertex*> Seed;
typedef map<int, vector<CVertex*>> Component;

class SingularitiesToQuad
{
public:
    SingularitiesToQuad();
    ~SingularitiesToQuad();

    int readMesh(const string& filename);
    int readCuts(const string& filename);

    int traceAllComponents();
    int traceComponent(int id, CVertex* seed, vector<CVertex*>& component);

    int writeMesh(string filename);

private:
    CMesh* mesh;
    Cut cuts;
    Seed seeds;
    Component components;
};

