/*!
*      \file Boundary.h
*      \brief Trace boundary loops
*      \author David Gu
*      \date 10/07/2010
*
*/


#ifndef _BOUNDARY_H_
#define _BOUNDARY_H_

#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <list>
#include <set>
#include <queue>
#include "mesh.h"

namespace MeshLib
{
    /*!
        \brief CLoopSegment Boundary loop  segment class.
        \tparam CVertex Vertex type
        \tparam CEdge   Edge   type
        \tparam CFace   Face   type
        \tparam CHalfEdge HalfEdge type
    */
    template<typename V, typename E, typename F, typename H>
    class CLoopSegment
    {
    public:
        using CMesh = CBaseMesh<V, E, F, H>;
        using CVertex = typename CMesh::CVertex;
        using CEdge = typename CMesh::CEdge;
        using CFace = typename CMesh::CFace;
        using CHalfEdge = typename CMesh::CHalfEdge;
        /*!
            Constructor of the CLoopSegment
            \param pMesh  pointer to the current mesh
            \param pH halfedge on the boundary loop
        */
        CLoopSegment(CMesh * pMesh, std::vector<CHalfEdge*> & pHs)
        {
            m_pMesh = pMesh;

            for (size_t i = 0; i < pHs.size(); i++)
            {
                m_halfedges.push_back(pHs[i]);
            }
        }
        /*!
           Destructor of CLoop.
        */
        ~CLoopSegment()
        {
            m_halfedges.clear();
        };

        /*!
        The vector of haledges on the current boundary loop segment.
        */
        std::vector<CHalfEdge*>  & halfedges()
        {
            return m_halfedges;
        }
        /*!
            Starting vertex
        */
        CVertex * start()
        {
            if (m_halfedges.size() == 0) return NULL;
            CHalfEdge * he = m_halfedges[0];
            return m_pMesh->halfedgeSource(he);
        }
        /*!
            ending vertex
        */
        CVertex * end()
        {
            if (m_halfedges.size() == 0) return NULL;
            size_t n = m_halfedges.size();
            CHalfEdge * he = m_halfedges[n - 1];
            return m_pMesh->halfedgeTarget(he);
        }

    protected:
        /*!
            The mesh pointer
        */
        CMesh		* m_pMesh;

        /*!
            The vector of consecutive halfedges along the boundary loop.
        */
        std::vector<CHalfEdge*>							  m_halfedges;

    };

    /*!
        \brief CLoop Boundary loop  class.
        \tparam CVertex Vertex type
        \tparam CEdge   Edge   type
        \tparam CFace   Face   type
        \tparam CHalfEdge HalfEdge type
    */    
    template<typename V, typename E, typename F, typename H>
    class CLoop
    {
        using CMesh = CBaseMesh<V, E, F, H>;
        using CVertex = typename CMesh::CVertex;
        using CEdge = typename CMesh::CEdge;
        using CFace = typename CMesh::CFace;
        using CHalfEdge = typename CMesh::CHalfEdge;
        using TSegment = CLoopSegment<V, E, F, H>;

    public:
        /*!
            Constructor of the CLoop
            \param pMesh  pointer to the current mesh
            \param pH halfedge on the boundary loop
        */
        CLoop(CMesh * pMesh, CHalfEdge * pH);
        /*!
            Constructor of the CLoop
            \param pMesh  pointer to the current mesh
        */
        CLoop(CMesh * pMesh) { m_pMesh = pMesh; m_length = 0; m_pHalfedge = NULL; };
        /*!
           Destructor of CLoop.
        */
        ~CLoop();

        /*!
        The list of haledges on the current boundary loop.
        */
        std::list<CHalfEdge*>  & halfedges()
        {
            return m_halfedges;
        }

        /*!
            The length of the current boundary loop.
        */
        double length()
        {
            return m_length;
        }
        /*!
         *  Output to a file
         */
        void write(const char * file);
        /*!
         *  Input from a file
         */
        void read(const char * file);
        /*!
            The vector of segments on this loop
        */
        std::vector<TSegment*>  & segments()
        {
            return m_segments;
        }
        /*!
            divide the loop to segments
            \param markers, the array of markers, the first marker is the starting one
        */
        void divide(std::vector<CVertex*> & markers);

    protected:
        /*!
            Pointer to the current mesh.
        */
        CMesh		         * m_pMesh;
        /*! The length of the current boundary loop.
        */
        double				   m_length;
        /*!
            Starting halfedge of the current boundary loop.
        */
        CHalfEdge            * m_pHalfedge;
        /*!
            List of consecutive halfedges along the boundary loop.
        */
        std::list<CHalfEdge*>  m_halfedges;
        /*!
            Vector of segments
        */
        std::vector<TSegment*> m_segments;
    };

    /*!
        \brief CBoundary Boundary  class.
        \tparam CVertex Vertex type
        \tparam CEdge   Edge   type
        \tparam CFace   Face   type
        \tparam CHalfEdge HalfEdge type
    */
    template<typename V, typename E, typename F, typename H>
    class CBoundary
    {
        using CMesh = CBaseMesh<V, E, F, H>;
        using CVertex = typename CMesh::CVertex;
        using CEdge = typename CMesh::CEdge;
        using CFace = typename CMesh::CFace;
        using CHalfEdge = typename CMesh::CHalfEdge;
        using TLoop = CLoop<V, E, F, H>;

    public:
        /*!
        CBoundary constructor
        \param pMesh pointer to the current mesh
        */
        CBoundary(CMesh * pMesh);
        /*!
        CBoundary destructor
        */
        ~CBoundary();
        /*!
        The list of boundary loops.
        */
        std::vector<TLoop*> & loops()
        {
            return m_loops;
        }

    protected:
        /*!
            Pointer to the current mesh.
        */
        CMesh * m_pMesh;
        /*!
            List of boundary loops.
        */
        typename std::vector<TLoop*> m_loops;
        /*!
            Bubble sort the loops
            \param loops the vector of loops
        */
        void _bubble_sort(std::vector<TLoop*> & loops);
    };

    /*!
        CLoop constructure, trace the boundary loop, starting from the halfedge pH.
        \param pMesh pointer to the current mesh
        \param pH  halfedge on the current boundary loop
    */
    template<typename V, typename E, typename F, typename H>
    CLoop<V, E, F, H>::CLoop(CMesh * pMesh, CHalfEdge * pH)
    {
        m_pMesh = pMesh;
        m_pHalfedge = pH;

        m_length = 0;
        CHalfEdge * he = pH;
        //trace the boundary loop
        do {
            CVertex * v = he->target();
            he = v->most_clw_out_halfedge();
            m_halfedges.push_back(he);
            m_length += he->edge()->length();
        } while (he != m_pHalfedge);
    }

    /*!
    CLoop destructor, clean up the list of halfedges in the loop
    */
    template<typename V, typename E, typename F, typename H>
    CLoop<V, E, F, H>::~CLoop()
    {
        m_halfedges.clear();

        for (size_t i = 0; i < m_segments.size(); i++)
        {
            delete m_segments[i];
        }
        m_segments.clear();
    }


    /*!
        _bubble_sort
        bubble sort  a vector of boundary loop objects, according to their lengths
        \param loops vector of loops
    */
    template<typename V, typename E, typename F, typename H>
    void CBoundary<V, E, F, H>::_bubble_sort(std::vector<TLoop*> & loops)
    {
        int i, j, flag = 1;    // set flag to 1 to start first pass
        CLoop<V, E, F, H> * temp;             // holding variable
        int numLength = (int)loops.size();
        for (i = 1; (i <= numLength) && flag; i++)
        {
            flag = 0;
            for (j = 0; j < (numLength - 1); j++)
            {
                if (loops[j + 1]->length() > loops[j]->length())      // ascending order simply changes to <
                {
                    temp = loops[j];								// swap elements
                    loops[j] = loops[j + 1];
                    loops[j + 1] = temp;
                    flag = 1;										// indicates that a swap occurred.
                }
            }
        }
    }

    /*!
        CBoundary constructor
        \param pMesh the current mesh
    */
    template<typename V, typename E, typename F, typename H>
    CBoundary<V, E, F, H>::CBoundary(CMesh * pMesh)
    {
        m_pMesh = pMesh;
        //collect all boundary halfedges
        std::set<CHalfEdge*> boundary_hes;
        for (CEdge * e : m_pMesh->edges())        
        {
            if (!e->boundary()) continue;
            CHalfEdge * he = e->halfedge(0);
            boundary_hes.insert(he);
        }
        //trace all the boundary loops
        while (!boundary_hes.empty())
        {
            //get the first boundary halfedge
            CHalfEdge * he = *boundary_hes.begin();
            //trace along this boundary halfedge
            TLoop * pL = new TLoop(m_pMesh, he);
            assert(pL);
            m_loops.push_back(pL);
            //remove all the boundary halfedges, which are in the same boundary loop as the head, from the halfedge list
            for (CHalfEdge * he : pL->halfedges())
            {
                typename std::set<CHalfEdge*>::iterator siter = boundary_hes.find(he);
                if (siter == boundary_hes.end()) continue;
                boundary_hes.erase(siter);
            }
        }

        //std::sort( vlps.begin(), vlps.end(), loop_compare<CVertex,CFace,CEdge,CHalfEdge> );
        _bubble_sort(m_loops);
    }

    /*!	CBoundary destructor, delete all boundary loop objects.
    */
    template<typename V, typename E, typename F, typename H>
    CBoundary<V, E, F, H>::~CBoundary()
    {
        for (TLoop * pL : m_loops) delete pL;
        m_loops.clear();
    };

    /*!
        Output the loop to a file
        \param file_name the name of the file
    */
    template<typename V, typename E, typename F, typename H>
    void CLoop<V, E, F, H>::write(const char * file_name)
    {
        std::ofstream ofs;
        ofs.open(file_name);
        for (CHalfEdge * pH : m_halfedges)        
        {
            CVertex * pV = pH->source();
            CVertex * pW = pH->target();
            ofs << pV->id() << " " << pW->id() << std::endl;
        }
        ofs.close();
    };

    /*!
        Output the loop to a file
        \param file_name the name of the file
    */
    template<typename V, typename E, typename F, typename H>
    void CLoop<V, E, F, H>::read(const char * file_name)
    {
        std::ifstream ifs;
        ifs.open(file_name);

        if (ifs.is_open())
        {
            while (ifs.good())
            {
                int source, target;
                ifs >> source >> target;
                CVertex * pS = m_pMesh->vertex(source);
                CVertex * pT = m_pMesh->vertex(target);
                CEdge   * pE = m_pMesh->edge(pS, pT);
                CHalfEdge * pH = pE->halfedge(0);
                m_halfedges.push_back(pH);
            }
            ifs.close();
        }

    };


    /*!
        Divide the loop to segments
        \param markers
    */
    template<typename V, typename E, typename F, typename H>
    void CLoop<V, E, F, H>::divide(std::vector<CVertex*> & markers)
    {
        std::deque<CHalfEdge*> queue;
        for (CHalfEdge * pH : m_halfedges)  queue.push_back(pH);
        
        size_t n = 0;
        while (true)
        {
            CHalfEdge * pH = queue.front();
            if (pH->source() == markers[0]) break;
            queue.pop_front();
            queue.push_back(pH);
            n++;
            if (n > queue.size()) break;
        }

        if (n > queue.size())
        {
            std::cerr << "CLoop::divide: can not find the starting vertex " << std::endl;
            return;
        }

        for (size_t i = 0; i < markers.size(); i++)
        {
            std::vector<CHalfEdge*> hes;
            CHalfEdge * ph = queue.front();
            queue.pop_front();
            assert(ph->source() == markers[i]);
            hes.push_back(ph);

            while (ph->target() != markers[(i + 1) % markers.size()])
            {
                if (queue.empty())
                {
                    std::cerr << "CLoop::divide error" << std::endl;
                    return;
                }
                ph = queue.front();
                queue.pop_front();
                hes.push_back(ph);
            }

            TSegment * pS = new TSegment(m_pMesh, hes);
            assert(pS != NULL);
            m_segments.push_back(pS);
        }
    };


}
#endif
