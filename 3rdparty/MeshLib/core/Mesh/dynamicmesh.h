/*!
*      \file DynamicMesh.h
*      \brief Dynamic Mesh Class for edge swap, face split, edge split
*
*      Dynamic Mesh Class for edge swap, face split, edge split
*      \author David Gu
*      \date 10/07/2010
*
*/

#ifndef  _DYNAMIC_MESH_H_
#define  _DYNAMIC_MESH_H_

#include <map>
#include <vector>
#include <queue>

#include "mesh.h"
#include "boundary.h"


namespace MeshLib
{

    /*-------------------------------------------------------------------------------------------------------------------------------------

        Dynamic Mesh Class

    --------------------------------------------------------------------------------------------------------------------------------------*/
    /*! \brief CDynamicMesh class : Dynamic mesh
    *
    *  Mesh supports FaceSlit, EdgeSlit, EdgeSwap operations
    */
    template<typename V = CVertex, typename E = CEdge, typename F = CFace, typename H = CHalfEdge>
    class CDynamicMesh : public CBaseMesh<V, E, F, H>
    {
    public:
        using CMesh = CDynamicMesh<V, E, F, H>;
        using CVertex = typename CDynamicMesh::CVertex;
        using CEdge = typename CDynamicMesh::CEdge;
        using CFace = typename CDynamicMesh::CFace;
        using CHalfEdge = typename CDynamicMesh::CHalfEdge;

    public:
        /*! CDynamicMesh constructor */
        CDynamicMesh() { m_vertex_id = 0; m_face_id = 0; };
        CDynamicMesh(CBaseMesh<V,E,F,H> * mesh) : CBaseMesh<V,E,F,H>(*mesh)
        {
            m_vertex_id = 0;
            m_face_id = 0;

            for (CVertex * v : mesh->vertices())
            {
                if (v->id() >= m_vertex_id) m_vertex_id = v->id() + 1;
            }
            for (CFace * f : mesh->faces())
            {
                if (f->id() >= m_face_id) m_face_id = f->id() + 1;
            }
        }
        /*! CDynamicMesh destructor */
        ~CDynamicMesh();

        //dynamic mesh editing
        /*! Split a Face to three small faces
         * \param pFace the face to be split
         */
        CVertex *  splitFace(CFace * pFace);
        /*! Split one edge to two edges
         * \param pEdge the edge to be split
         */
        CVertex *  splitEdge(CEdge * pEdge);
        /*! Swap an edge
        * \param edge the edge to be swapped
        */
        void swapEdge(CEdge * edge);

        /*! Test if an edge is swapable
        * \param edge the edge to be swapped
        */
        bool swapable(CEdge * edge);

    protected:
        /*! attach halfeges to an edge
        * \param he0, he1 the halfedges
        * \param e edge
        */
        void __attach_halfedge_to_edge(CHalfEdge * he0, CHalfEdge * he1, CEdge * e);
        /*! next vertex id */
        int  m_vertex_id;
        /*! next face id */
        int  m_face_id;
    };

    /*---------------------------------------------------------------------------*/
    template<typename V, typename E, typename F, typename H>
    CDynamicMesh<V, E, F, H>::~CDynamicMesh()
    {
    }


    /*---------------------------------------------------------------------------*/

    //insert a vertex in the center of a face, split the face to 3 faces
    template<typename V, typename E, typename F, typename H>
    typename CDynamicMesh<V, E, F, H>::CVertex * CDynamicMesh<V,E,F,H>::splitFace(typename CDynamicMesh<V, E, F, H>::CFace * pFace)
    {

        CVertex * pV = this->create_vertex(++m_vertex_id);

        CVertex   *  v[3];
        CHalfEdge *  h[3];
        CHalfEdge *  hs[3];

        CEdge     *  eg[3];

        h[0] = pFace->halfedge();
        h[1] = h[0]->next();
        h[2] = h[1]->next();

        for (int i = 0; i < 3; i++)
        {
            v[i] = h[i]->target();
            eg[i] = h[i]->edge();
            hs[i] = h[i]->dual();
        }


        CFace * f = new CFace();
        assert(f != NULL);
        f->id() = ++m_face_id;
        this->m_faces.push_back(f);

        //create halfedges
        CHalfEdge * hes[3];
        for (int i = 0; i < 3; i++)
        {
            hes[i] = new CHalfEdge();
            assert(hes[i]);
        }

        //linking to each other
        for (int i = 0; i < 3; i++)
        {
            hes[i]->next() = hes[(i + 1) % 3];
            hes[i]->prev() = hes[(i + 2) % 3];
        }

        //linking to face
        for (int i = 0; i < 3; i++)
        {
            hes[i]->face() = f;
            f->halfedge() = hes[i];
        }


        f = new CFace();
        assert(f != NULL);
        f->id() = ++m_face_id;
        this->m_faces.push_back(f);

        //create halfedges
        CHalfEdge * hes2[3];

        for (int i = 0; i < 3; i++)
        {
            hes2[i] = new CHalfEdge;
            assert(hes2[i]);
        }

        //linking to each other
        for (int i = 0; i < 3; i++)
        {
            hes2[i]->next() = hes2[(i + 1) % 3];
            hes2[i]->prev() = hes2[(i + 2) % 3];
        }

        //linking to face
        for (int i = 0; i < 3; i++)
        {
            hes2[i]->face() = f;
            f->halfedge() = hes2[i];
        }

        CEdge * e[3];
        for (int i = 0; i < 3; i++)
        {
            e[i] = new CEdge();
            assert(e[i]);
            this->m_edges.push_back(e[i]);
        }

        __attach_halfedge_to_edge(h[1], hes[0], e[0]);
        __attach_halfedge_to_edge(hes[2], hes2[1], e[1]);
        __attach_halfedge_to_edge(h[2], hes2[0], e[2]);
        __attach_halfedge_to_edge(h[0], hs[0], eg[0]);
        __attach_halfedge_to_edge(hes[1], hs[1], eg[1]);
        __attach_halfedge_to_edge(hes2[2], hs[2], eg[2]);



        pV->halfedge() = h[1];


        h[1]->vertex() = pV;
        h[2]->vertex() = v[2];

        hes[0]->vertex() = v[0];
        hes[1]->vertex() = v[1];
        hes[2]->vertex() = pV;

        hes2[0]->vertex() = pV;
        hes2[1]->vertex() = v[1];
        hes2[2]->vertex() = v[2];

        for (int i = 0; i < 3; i++)
        {
            v[i]->halfedge() = hs[i]->dual();
        }
        return pV;

    };

    template<typename V, typename E, typename F, typename H>
    void CDynamicMesh<V, E, F, H>::swapEdge(CEdge * edge)
    {

        CHalfEdge * he_left = edge->halfedge(0);
        CHalfEdge * he_right = edge->halfedge(1);

        if (he_right == NULL)  return;

        CHalfEdge * ph[6];

        ph[0] = he_left;
        ph[1] = ph[0]->next();
        ph[2] = ph[1]->next();


        ph[3] = he_right;
        ph[4] = ph[3]->next();
        ph[5] = ph[4]->next();

        CVertex * pv[4];

        pv[0] = ph[0]->target();
        pv[1] = ph[1]->target();
        pv[2] = ph[2]->target();
        pv[3] = ph[4]->target();

        int     pi[6];
        CEdge * pe[6];

        for (int i = 0; i < 6; i++)
        {
            CHalfEdge *   he = ph[i];
            CEdge     *   e = he->edge();
            pe[i] = e;

            if (pe[i]->halfedge(0) == he)
            {
                pi[i] = 0;
            }
            else
            {
                assert(pe[i]->halfedge(1) == he);
                pi[i] = 1;
            }
        }


        //remove edge from edge list of the original vertex
        CVertex * vb = (pv[0]->id() < pv[2]->id()) ? pv[0] : pv[2];
        std::list<CEdge*> & ledges = (std::list<CEdge*> &) vb->edges();

        //add edge to the edge list of the new vertex
        CVertex * wb = (pv[1]->id() < pv[3]->id()) ? pv[1] : pv[3];
        std::list<CEdge*> & wedges = (std::list<CEdge*> &) wb->edges();

        for (typename std::list<CEdge*>::iterator eiter = wedges.begin(); eiter != wedges.end(); eiter++)
        {
            CEdge * pE = *eiter;

            CVertex * v1 = pE->vertex1();
            CVertex * v2 = pE->vertex2();

            if ((v1 == pv[0] && v2 == pv[2]) || (v1 == pv[2]) && (v2 == pv[0]))
            {
                std::cout << "DynamicMesh::SwapEdge::Warning Two edges share same both end vertices" << std::endl;
                //return;
            }
        }



        //relink the vertices

        ph[0]->target() = pv[1];
        ph[1]->target() = pv[2];
        ph[2]->target() = pv[3];
        ph[3]->target() = pv[3];
        ph[4]->target() = pv[0];
        ph[5]->target() = pv[1];

        for (int i = 0; i < 6; i++)
        {
            CHalfEdge * h = ph[i];
            CVertex   * v = h->target();
            v->halfedge() = h;
        }


        //relink the edge-halfedge pointers

        ph[1]->edge() = pe[2];
        pe[2]->halfedge(pi[2]) = ph[1];

        ph[2]->edge() = pe[4];
        pe[4]->halfedge(pi[4]) = ph[2];

        ph[4]->edge() = pe[5];
        pe[5]->halfedge(pi[5]) = ph[4];

        ph[5]->edge() = pe[1];
        pe[1]->halfedge(pi[1]) = ph[5];



        //remove edge from edge list of the original vertex
        typename std::list<CEdge*>::iterator pos = ledges.end();
        for (typename std::list<CEdge*>::iterator eiter = ledges.begin(); eiter != ledges.end(); eiter++)
        {
            CEdge * pE = *eiter;
            if (pE == edge)
            {
                pos = eiter;
                break;
            }
        }

        if (pos == ledges.end())
        {
            std::cout << "Error" << std::endl;
        }

        if (pos != ledges.end())
            ledges.erase(pos);

        //add edge to the edge list of the new vertex
        wedges.push_back(edge);

        for (int i = 0; i < 6; i++)
        {
            CHalfEdge * he = ph[i];
            CHalfEdge * sh = he->dual();
            assert(he->target() == sh->prev()->target());
            assert(he->prev()->target() == sh->target());
        }

        /*
          for(int i = 0; i < 6; i ++ )
          {
              CHalfEdge * he = ph[i];
              CEdge     * pe = halfedgeEdge( he );
              CVertex   * v1 = edgeVertex1( pe );
              CVertex   * v2 = edgeVertex2( pe );
              CEdge     * we = vertexEdge( v1, v2 );
              if( pe != we )
              {
                  std::cout << "Error" << std::endl;

                  for( int j = 0; j < 4; j ++ )
                  {
                      std::cout << pv[j]->id() << " ";
                  }
                  std::cout << std::endl;

                  for( std::list<CEdge*>::iterator eiter = ledges.begin(); eiter != ledges.end(); eiter ++ )
                  {
                      CEdge * e = *eiter;
                      CVertex * v1 = edgeVertex1( e );
                      CVertex * v2 = edgeVertex2( e );
                      std::cout << "(" << v1->id() << "," << v2->id() <<")" << std::endl;
                  }

                  std::cout << " Edge list " << std::endl;

                  for( std::list<CEdge*>::iterator eiter = wedges.begin(); eiter != wedges.end(); eiter ++ )
                  {
                      CEdge * e = *eiter;
                      CVertex * v1 = edgeVertex1( e );
                      CVertex * v2 = edgeVertex2( e );
                      std::cout << "(" << v1->id() << "," << v2->id() <<")" << std::endl;
                  }

                  write_m("test.m");
              }
              assert( pe == we );
          }

          static int id = 0;
          char name[1024];
          sprintf(name,"temp_%d.m", id++);

          write_m( name );

          */

    };

    /*---------------------------------------------------------------------------*/
    template<typename V, typename E, typename F, typename H>
    bool CDynamicMesh<V,E,F,H>::swapable(typename CDynamicMesh<V, E, F, H>::CEdge * edge)
    {

        CHalfEdge * he_left = edge->halfedge(0);
        CHalfEdge * he_right = edge->halfedge(1);

        if (he_right == NULL)  return false;

        CHalfEdge * ph[6];

        ph[0] = he_left;
        ph[1] = ph[0]->next();
        ph[2] = ph[1]->next();


        ph[3] = he_right;
        ph[4] = ph[3]->next();
        ph[5] = ph[4]->next();

        CVertex * pv[4];

        pv[0] = ph[0]->target();
        pv[1] = ph[1]->target();
        pv[2] = ph[2]->target();
        pv[3] = ph[4]->target();

        int     pi[6];
        CEdge * pe[6];

        for (int i = 0; i < 6; i++)
        {
            CHalfEdge *   he = ph[i];
            CEdge     *   e = he->edge();
            pe[i] = e;

            if (pe[i]->halfedge(0) == he)
            {
                pi[i] = 0;
            }
            else
            {
                assert(pe[i]->halfedge(1) == he);
                pi[i] = 1;
            }
        }


        //remove edge from edge list of the original vertex
        CVertex * vb = (pv[0]->id() < pv[2]->id()) ? pv[0] : pv[2];
        std::list<CEdge*> & ledges = (std::list<CEdge*> &) vb->edges();

        //add edge to the edge list of the new vertex
        CVertex * wb = (pv[1]->id() < pv[3]->id()) ? pv[1] : pv[3];
        std::list<CEdge*> & wedges = (std::list<CEdge*> &) wb->edges();

        for (typename std::list<CEdge*>::iterator eiter = wedges.begin(); eiter != wedges.end(); eiter++)
        {
            CEdge * pE = *eiter;

            CVertex * v1 = pE->vertex1();
            CVertex * v2 = pE->vertex2();

            if ((v1 == pv[0] && v2 == pv[2]) || (v1 == pv[2]) && (v2 == pv[0]))
            {
                std::cout << "DynamicMesh::SwapEdge::Warning Two edges share same both end vertices" << std::endl;
                return false;
            }
        }

        return true;
    };

    /*---------------------------------------------------------------------------*/
    //insert a vertex in the center of an edge, split each neighboring face into 2 faces
    template<typename V, typename E, typename F, typename H>
    typename CDynamicMesh<V, E, F, H>::CVertex * CDynamicMesh<V,E,F,H>::splitEdge(typename CDynamicMesh<V, E, F, H>::CEdge * pEdge)
    {

        CVertex * pV = this->create_vertex(++m_vertex_id);


        CHalfEdge *  h[12];
        CHalfEdge *  s[6];
        CVertex   *  v[6];
        CEdge     * eg[6];

        h[0] = pEdge->halfedge(0);
        h[1] = h[0]->next();
        h[2] = h[1]->next();

        h[3] = pEdge->halfedge(1);
        h[4] = h[3]->next();
        h[5] = h[4]->next();


        CFace * f[4];
        f[0] = h[0]->face();
        f[1] = h[3]->face();

        for (int i = 0; i < 6; i++)
        {
            eg[i] = h[i]->edge();
            v[i] = h[i]->vertex();
            s[i] = h[i]->dual();
        }

        f[2] = new CFace();
        assert(f[2] != NULL);
        f[2]->id() = ++m_face_id;
        this->m_faces.push_back(f[2]);

        //create halfedges
        for (int i = 6; i < 9; i++)
        {
            h[i] = new CHalfEdge;
            assert(h[i]);
        }

        //linking to each other
        for (int i = 0; i < 3; i++)
        {
            h[i + 6]->next() = h[6 + (i + 1) % 3];
            h[i + 6]->prev() = h[6 + (i + 2) % 3];
        }

        //linking to face
        for (int i = 6; i < 9; i++)
        {
            h[i]->face() = f[2];
            f[2]->halfedge() = h[i];
        }


        f[3] = new CFace();
        assert(f[3] != NULL);
        f[3]->id() = ++m_face_id;
        this->m_faces.push_back(f[3]);

        //create halfedges
        for (int i = 9; i < 12; i++)
        {
            h[i] = new CHalfEdge();
            assert(h[i]);
        }

        //linking to each other
        for (int i = 0; i < 3; i++)
        {
            h[i + 9]->next() = h[9 + (i + 1) % 3];
            h[i + 9]->prev() = h[9 + (i + 2) % 3];
        }

        //linking to face
        for (int i = 9; i < 12; i++)
        {
            h[i]->face() = f[3];
            f[3]->halfedge() = h[i];
        }

        CEdge * e[3];

        for (int i = 0; i < 3; i++)
        {
            e[i] = new CEdge();
            this->m_edges.push_back(e[i]);
            assert(e[i]);
        }

        __attach_halfedge_to_edge(h[2], h[6], e[0]);
        __attach_halfedge_to_edge(h[8], h[9], e[1]);
        __attach_halfedge_to_edge(h[4], h[11], e[2]);

        __attach_halfedge_to_edge(h[0], h[3], eg[0]);
        __attach_halfedge_to_edge(h[1], s[1], eg[1]);
        __attach_halfedge_to_edge(h[5], s[5], eg[5]);

        __attach_halfedge_to_edge(h[7], s[2], eg[2]);
        __attach_halfedge_to_edge(h[10], s[4], eg[4]);


        h[0]->vertex() = v[0];
        h[1]->vertex() = v[1];
        h[2]->vertex() = pV;
        h[3]->vertex() = pV;
        h[4]->vertex() = v[4];
        h[5]->vertex() = v[5];
        h[6]->vertex() = v[1];
        h[7]->vertex() = v[2];
        h[8]->vertex() = pV;
        h[9]->vertex() = v[2];
        h[10]->vertex() = v[4];
        h[11]->vertex() = pV;



        v[0]->halfedge() = h[0];
        v[1]->halfedge() = h[1];
        v[2]->halfedge() = h[7];
        v[4]->halfedge() = h[4];
        pV->halfedge() = h[3];

        for (int k = 0; k < 4; k++)
        {
            CHalfEdge * pH = f[k]->halfedge();
            for (int i = 0; i < 3; i++)
            {
                assert(pH->vertex() == pH->dual()->prev()->vertex());
                pH = pH->next();
            }
        }
        return pV;

    };

    /*---------------------------------------------------------------------------*/
    template<typename V, typename E, typename F, typename H>
    void CDynamicMesh<V, E, F, H>::__attach_halfedge_to_edge(typename CDynamicMesh<V, E, F, H>::CHalfEdge * he0, typename CDynamicMesh<V, E, F, H>::CHalfEdge * he1, typename CDynamicMesh<V, E, F, H>::CEdge * e)
    {
        e->halfedge(0) = he0;
        e->halfedge(1) = he1;

        he0->edge() = e;
        he1->edge() = e;

    };



}
#endif  //_DYNAMIC_MESH_H_
