/*!
*      \file QuadrilateralBoundary.h
*      \brief Trace boundary loops
*      \author David Gu
*      \date 11/23/2012
*
*/
//the four corners of the boundary should be marked as ``marker''

#ifndef _QUADRILATERAL_BOUNDARY_H_
#define _QUADRILATERAL_BOUNDARY_H_

#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <list>
#include <set>

#include "mesh.h"
#include "boundary.h"

namespace MeshLib
{
    /*!
        \brief CLoop Boundary loop  class.
        \tparam CVertex Vertex type
        \tparam CEdge   Edge   type
        \tparam CFace   Face   type
        \tparam CHalfEdge HalfEdge type
    */
    template<typename V, typename E, typename F, typename H>
    class CQuadrilateralBoundary
    {
    public:
        using CMesh = CBaseMesh<V, E, F, H>;
        using CVertex = typename CMesh::CVertex;
        using CEdge = typename CMesh::CEdge;
        using CFace = typename CMesh::CFace;
        using CHalfEdge = typename CMesh::CHalfEdge;
        using TLoop = CLoop<V, E, F, H>;

    public:
        /*!
            Constructor of the CQuadrilateralBoundary
            \param pMesh  pointer to the current mesh
        */
        CQuadrilateralBoundary(CMesh * pMesh);
        /*!
           Destructor of CQuadrilateralBoundary.
        */
        ~CQuadrilateralBoundary();
        /*!
         * get array of segments
         */
        std::vector<std::list<CHalfEdge*>*> & segments() { return m_segments; };

    protected:
        /*!
         *  Pointer to the current mesh.
         */
        CMesh       * m_pMesh;
        /*!
         *  Segments
         */
        std::vector<std::list<CHalfEdge*>*> m_segments;
    };

    /*!
        CBoundary constructor
        \param pMesh the current mesh
    */
    template<typename V, typename E, typename F, typename H>
    CQuadrilateralBoundary<V,E,F,H>::CQuadrilateralBoundary(CMesh * pMesh)
    {
        m_pMesh = pMesh;
        CBoundary boundary(pMesh);

        //get the boundary half edge loop
        std::vector<TLoop*> & pLs = boundary.loops();
        assert(pLs.size() == 1);
        CLoop * pL = pLs[0];
        std::list<CHalfEdge*>   pHs;
        for (std::list<CHalfEdge*>::iterator hiter = pL->halfedges().begin(); hiter != pL->halfedges().end(); hiter++)
        {
            CHalfEdge * ph = *hiter;
            pHs.push_back(ph);
        }

        std::list<CVertex*> corners;
        for (std::list<CHalfEdge*>::iterator hiter = pHs.begin(); hiter != pHs.end(); hiter++)
        {
            CHalfEdge * ph = *hiter;
            CVertex * pv = m_pMesh->halfedgeSource(ph);
            //int valence = 0;
            //for( M::VertexEdgeIterator veiter( pv ); !veiter.end(); veiter ++ )
            //{
            //  E * pe = *veiter;
            //  valence += ( pe->sharp() )?1:0;
            //}
            //if( valence > 2 ) corners.push_back( pv );
            if (pv->isMarker()) corners.push_back(pv);
        }

        std::cout << "CQuadrilateralBoundary::CQuadrilateralBoundary find " << corners.size() << std::endl;

        if (corners.size() != 4)
        {
            std::cerr << "There should be 4 corners" << std::endl;
            return;
        }

        //find the lower left corner
        CVertex * pLL = corners.front();

        for (std::list<CVertex*>::iterator viter = corners.begin(); viter != corners.end(); viter++)
        {
            CVertex * pv = *viter;
            if (pv->z().real() < pLL->z().real())
            {
                pLL = pv;
                continue;
            }
            if (pv->z().real() > pLL->z().real()) continue;

            if (pv->z().imag() < pLL->z().imag())
                pLL = pv;
        }

        while (true)
        {
            CHalfEdge * ph = pHs.front();
            CVertex * pv = m_pMesh->halfedgeSource(ph);
            if (pv == pLL) break;
            pHs.pop_front();
            pHs.push_back(ph);
        }

        std::list<CHalfEdge*> * pSeg = NULL;
        while (!pHs.empty())
        {
            CHalfEdge * ph = pHs.front();
            CVertex * pv = m_pMesh->halfedgeSource(ph);
            pHs.pop_front();

            if (std::find(corners.begin(), corners.end(), pv) != corners.end())
            {
                pSeg = new std::list<CHalfEdge*>;
                assert(pSeg != NULL);
                m_segments.push_back(pSeg);
            }
            pSeg->push_back(ph);
        }
    };

    /*!
        CQuadrilateralBoundary destructor
    */
    template<typename V, typename E, typename F, typename H>
    CQuadrilateralBoundary<V, E, F, H>::~CQuadrilateralBoundary()
    {
        for (size_t i = 0; i < m_segments.size(); i++)
        {
            std::list<CHalfEdge*> * pSeg = m_segments[i];
            delete pSeg;
        }
        m_segments.clear();
    };

}
#endif

