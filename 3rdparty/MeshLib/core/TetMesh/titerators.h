/*!
*      \file titerators.h
*      \brief iterators for Tetrahedron Mesh Classes
*
*	   \author David Gu
*      \date 10/02/2011
*
*/

#ifndef _TMESHLIB_TITERATORS_H_
#define _TMESHLIB_TITERATORS_H_

#include <list>
#include <set>
#include "vertex.h"
#include "tvertex.h"
#include "edge.h"
#include "tedge.h"
#include "halfedge.h"
#include "face.h"
#include "halfface.h"
#include "tet.h"
#include "tmesh.h"


namespace TMeshLib
{
	/*!
	\brief TMeshVertex Iterator
	
	go through all the vertices in the TetMesh
	*/
	class TMeshVertexIterator
	{
	public:
		TMeshVertexIterator(CTMesh *  pMesh)
		{
			m_pMesh = pMesh;
			//m_id = 0;
			m_iter = pMesh->vertices().begin();
		};

		~TMeshVertexIterator(){};

		CVertex * value() { return *m_iter; };

		/*! iterate ++ */
		void operator++()
		{
			m_iter++;
		};
		/*! ++iterator */
		void operator++(int) { m_iter++; };
		/*! whether the iterator reaches the end */
		bool end(){ return m_iter == m_pMesh->vertices().end(); };
		/*! get the current vertex */
		CVertex * operator*() { return value(); };

	protected:
		/*! TetMesh pointer */
		CTMesh *        m_pMesh;
		/*! Current vertex list iterator */
		//int             m_id;
		typename std::list<CVertex*>::iterator m_iter;
	};

	/*!
	\brief VertexVertex Iterator

	go through all the adjacent vertices in of a vertex
	*/
	class TVertexVertexIterator
	{
	public:
		/*! Constructor */
		TVertexVertexIterator(CTMesh *  pMesh, CVertex *  pV)
		{
			m_pMesh = pMesh;
			m_vertex = pV;
			std::list<CEdge*> * pEdges = pMesh->VertexEdgeList(pV);
			std::set<CVertex*> vertices;

			for (std::list<CEdge*>::iterator eiter = (*pEdges).begin(); eiter != (*pEdges).end(); eiter++)
			{
				CEdge * e = *eiter;
				CVertex * v1 = pMesh->EdgeVertex1(e);
				CVertex * v2 = pMesh->EdgeVertex2(e);
				if (v1 != pV)
					m_pVertices.insert(v1);
				else
					m_pVertices.insert(v2);
			}
			m_iter = m_pVertices.begin();
		};
		/*! Destructor */
		~TVertexVertexIterator(){};
		/*! Iterator ++ */
		void operator++()
		{
			m_iter++;
		};
		/*! ++ Iterator */
		void operator++(int) { m_iter++; };
		/*! Verify if the iterator reaches the end */
		bool end(){ return m_iter == m_pVertices.end(); };
		/*! dereferencing */
		CVertex * operator*() { return *m_iter; };

	protected:
		/*! Pointer to the tmesh */
		CTMesh *  m_pMesh;
		/*! center vertex */
		CVertex *			m_vertex;
		/*! surrounding vertices */
		std::set<CVertex*>	m_pVertices;
		/*! set iterator */
		typename std::set<CVertex*>::iterator m_iter;
	};

	/*!
	\brief MeshTet Iterator
		
	go through all the Tets in a mesh
	*/
	class TMeshTetIterator
	{
	public:
		/*! Consructor */
		TMeshTetIterator(CTMesh *  pMesh)
		{
			m_pMesh = pMesh;
			//m_id = 0;
			m_iter = pMesh->tets().begin();
		};
		/*! Destructor */
		~TMeshTetIterator(){};
		/*! ++iterate */
		void operator++()
		{
			m_iter++;
		};
		/*! iterator ++ */
		void operator++(int) { m_iter++; };
		/*! verify if reaches the end */
		bool end(){ return m_iter == m_pMesh->tets().end(); };
		/*! dereferencing */
		CTet * operator*() { return *m_iter; };

	private:
		/*! pointer to a TetMesh */
		CTMesh *        m_pMesh;
		/*! tet iterator */
		typename std::list<CTet*>::iterator m_iter;
	};

	/*!
	\brief TetHalfFace Iterator

	go through all the halfaces of a Tet in a mesh
	*/
	class TetHalfFaceIterator
	{
	public:
		/*! Constructor */
		TetHalfFaceIterator(CTMesh * pMesh, CTet * pT)
		{
			m_pMesh = pMesh;
			m_pTet = pT;
			m_id = 0;
		};
		/*! Destructor */
		~TetHalfFaceIterator(){};
		/*! iterator ++ */
		void operator++()
		{
			m_id++;
		};
		/*! ++iterator */
		void operator++(int) { m_id++; };
		/*! verify if the end has been reached */
		bool end(){ return m_id == 4; };
		/*! dereferencing */
		CHalfFace * operator*() { return m_pMesh->TetHalfFace(m_pTet, m_id); };

	private:
		/*! pointer to a TetMesh */
		CTMesh *  m_pMesh;
		/*! current tet */
		CTet *             m_pTet;
		/*! current half_face */
		int                m_id;
	};

	/*!
	\brief Mesh Edge Iterator

	go through all the edges of a Tet in a mesh
	*/
	class TMeshEdgeIterator
	{
	public:
		/*! Constructor */
		TMeshEdgeIterator(CTMesh * pMesh)
		{
			m_pMesh = pMesh;
			m_iter = m_pMesh->edges().begin();
		};
		/*! Destructor */
		~TMeshEdgeIterator(){};
		/*! ++ iterator */
		void operator++()
		{
			m_iter++;
		};
		/*! iterator++ */
		void operator++(int) { m_iter++; };
		/*! verify if the end has been reached */
		bool end() { return m_iter == m_pMesh->edges().end(); };
		/*! dereferencing */
		CEdge * operator*() { return *m_iter; };
	private:
		/*! pointer to the mesh */
		CTMesh * m_pMesh;
		/*! edge list iterator */
		typename std::list<CEdge*>::iterator m_iter;
	};

	/*!
	\brief Edge->TEdge Iterator

	go through all the tedges of an edge
	*/
	class EdgeTEdgeIterator
	{
	public:
		/*! constrructor */
		EdgeTEdgeIterator(CTMesh * pMesh, CEdge * pE)
		{
			m_pMesh = pMesh;
			m_pEdge = pE;
			m_iter = m_pMesh->EdgeTEdgeList(pE)->begin();
		};
		/*! Destructor */
		~EdgeTEdgeIterator(){};
		/*! ++iterator */
		void operator++(){ m_iter++; };
		/*! iterator++ */
		void operator++(int unused) { m_iter++; };
		/*! verify if the end has been reached */
		bool end(){ return m_iter == m_pMesh->EdgeTEdgeList(m_pEdge)->end(); };
		/*! dereferencing */
		CTEdge * operator*() { return *m_iter; };
	private:
		/*! pointer to mesh */
		CTMesh * m_pMesh;
		/*! pointer to the edge */
		CEdge  * m_pEdge;
		/*! TEdge list iterator */
		typename std::list<CTEdge*>::iterator m_iter;
	};

	/*!
	\brief Vertex->Edge Iterator

	go through all the edges of a vertex
	*/
	class TVertexEdgeIterator
	{
	public:
		/*! constructor */
		TVertexEdgeIterator(CTMesh * pMesh, CVertex * pV)
		{
			m_pMesh = pMesh; m_pV = pV; m_iter = m_pMesh->VertexEdgeList(pV)->begin();
		};
		/*! destructor */
		~TVertexEdgeIterator() {};
		/*! ++ iterator */
		void operator++(){ m_iter++; };
		/*! iterator ++ */
		void operator++(int unused) { m_iter++; };
		/*! verify if the end has been reached */
		bool end() { return m_iter == m_pMesh->VertexEdgeList(m_pV)->end(); };
		/*! dereferencing */
		CEdge * operator*() { return *m_iter; };

	private:
		/*! pointer to mesh */
		CTMesh * m_pMesh;
		/*! pointer to the vertex */
		CVertex * m_pV;
		/*! Edge list iterator */
		typename std::list<CEdge*>::iterator m_iter;
	};

	/*!
	\brief Tet->Edge Iterator

	go through all the edges of a Tet
	*/
	class TetEdgeIterator
	{
	public:
		/*! constructor */
		TetEdgeIterator(CTMesh * pMesh, CTet * pT)
		{
			m_pMesh = pMesh;
			m_pT = pT;
			for (int i = 0; i < 4; i++)
			{
				CHalfFace * pF = m_pMesh->TetHalfFace(pT, i);
				CHalfEdge * pH = m_pMesh->HalfFaceHalfEdge(pF);
				for (int j = 0; j < 3; j++)
				{
					CTEdge * pTE = m_pMesh->HalfEdgeTEdge(pH);
					CEdge  * pE = m_pMesh->TEdgeEdge(pTE);
					m_edges.insert(pE);
					pH = m_pMesh->HalfEdgeNext(pH);
				}
			}
			m_iter = m_edges.begin();
		};
		/*! destructor */
		~TetEdgeIterator() {};
		/*! iterator ++ */
		void operator++(){ m_iter++; };
		/*! ++ iterator */
		void operator++(int unused) { m_iter++; };
		/*! verify if the end of the list has been reached */
		bool end() { return m_iter == m_edges.end(); };
		/*! dereferencing */
		CEdge * operator*() { return *m_iter; };

	private:
		/*! Pointer to the mesh */
		CTMesh * m_pMesh;
		/*! Pointer to the tet */
		CTet * m_pT;
		/*! set of edges adjacent to the central vertex */
		std::set<CEdge*> m_edges;
		/*! edge set iterator */
		typename std::set<CEdge*>::iterator m_iter;
	};

	/*!
	\brief Mesh->Face Iterator

	go through all the faces of a Mesh
	*/
	class TMeshFaceIterator
	{
	public:
		/*! Constructor */
		TMeshFaceIterator(CTMesh *  pMesh)
		{
			m_pMesh = pMesh;
			m_iter = pMesh->faces().begin();
		};
		/*! Destructor */
		~TMeshFaceIterator(){};
		/*! ++ iterator */
		void operator++(){ m_iter++; };
		/*! iterator ++ */
		void operator++(int) { m_iter++; };
		/*! reach the end of face list */
		bool end(){ return m_iter == m_pMesh->faces().end(); };
		/*! dereferencing */
		CFace * operator*() { return *m_iter; };

	private:
		/*! pointer to the mesh */
		CTMesh *    m_pMesh;
		/*! list of face iterator */
		typename std::list<CFace*>::iterator m_iter;
	};

	/*!
	\brief Edge->Face Iterator

	go through all the faces adjacent to an edge
	*/
	class TEdgeFaceIterator
	{
	public:
		/*! constructor */
		TEdgeFaceIterator(CTMesh * pMesh, CEdge * pE)
		{
			m_pMesh = pMesh;
			std::list<CTEdge*> * TEdges = m_pMesh->EdgeTEdgeList(pE);
			for (std::list<CTEdge*>::iterator tit = TEdges->begin(); tit != TEdges->end(); tit++)
			{
				CTEdge * pT = *tit;

				CHalfEdge * pL = m_pMesh->TEdgeLeftHalfEdge(pT);
				CHalfEdge * pR = m_pMesh->TEdgeRightHalfEdge(pT);

				CHalfFace * pHF = m_pMesh->HalfEdgeHalfFace(pL);
				CFace *      pF = m_pMesh->HalfFaceFace(pHF);
				m_faces.insert(pF);

				pHF = m_pMesh->HalfEdgeHalfFace(pR);
				pF = m_pMesh->HalfFaceFace(pHF);
				m_faces.insert(pF);
			}

			m_iter = m_faces.begin();
		};

		/*! destructor */
		~TEdgeFaceIterator(){};
		/*! ++ iterator */
		void operator++() { m_iter++; };
		/*! iterator ++ */
		void operator++(int unused) { m_iter++; };
		/*! reach the end */
		bool end(){ return m_iter == m_faces.end(); };
		/*! dereferencing */
		CFace * operator*() { return *m_iter; };

	private:
		/*! pointer to the mesh */
		CTMesh *    m_pMesh;
		/*! face set */
		std::set<CFace*> m_faces;
		/*! face set iterator */
		typename std::set<CFace*>::iterator m_iter;
	};


	/*!
	\brief HalfFace->vertex Iterator

	go through all the vertices of a HalfFace
	*/
	class HalfFaceVertexIterator
	{
	public:
		/*! constructor */
		HalfFaceVertexIterator(CTMesh * pMesh, CHalfFace * pF)
		{
			m_pMesh = pMesh;
			m_pF = pF;
			m_pH = pMesh->HalfFaceHalfEdge(pF);
		};
		/*! destructor */
		~HalfFaceVertexIterator() {};
		/*! iterator ++ */
		void operator++()
		{
			m_pH = m_pMesh->HalfEdgeNext(m_pH);
			if (m_pH == m_pMesh->HalfFaceHalfEdge(m_pF))
			{
				m_pH = NULL;
			}
		};
		/*! ++ iterator */
		void operator++(int)
		{
			m_pH = m_pMesh->HalfEdgeNext(m_pH);
			if (m_pH == m_pMesh->HalfFaceHalfEdge(m_pF))
			{
				m_pH = NULL;
			}
		};
		/*! verify if the end of the list has been reached */
		bool end() { return m_pH == NULL; };
		/*! dereferencing */
		CVertex * operator*() { return m_pMesh->HalfEdgeTarget(m_pH); };

	private:
		/*! Pointer to the mesh */
		CTMesh * m_pMesh;
		/*! Pointer to the halfface */
		CHalfFace * m_pF;
		/*! Pointer to the halfedge */
		CHalfEdge * m_pH;
	};

	/*!
	\brief HalfFace->HalfEdge Iterator

	go through all the halfedges of a HalfFace
	*/
	class HalfFaceHalfEdgeIterator
	{
	public:
		/*! constructor */
		HalfFaceHalfEdgeIterator(CTMesh * pMesh, CHalfFace * pF)
		{
			m_pMesh = pMesh;
			m_pF = pF;
			m_pH = pMesh->HalfFaceHalfEdge(pF);
		};
		/*! destructor */
		~HalfFaceHalfEdgeIterator() {};
		/*! iterator ++ */
		void operator++()
		{
			m_pH = m_pMesh->HalfEdgeNext(m_pH);
			if (m_pH == m_pMesh->HalfFaceHalfEdge(m_pF))
			{
				m_pH = NULL;
			}
		};
		/*! ++ iterator */
		void operator++(int unused)
		{
			m_pH = m_pMesh->HalfEdgeNext(m_pH);
			if (m_pH == m_pMesh->HalfFaceHalfEdge(m_pF))
			{
				m_pH = NULL;
			}
		};
		/*! verify if the end of the list has been reached */
		bool end() { return m_pH == NULL; };
		/*! dereferencing */
		CHalfEdge * operator*() { return m_pH; };

	protected:
		/*! Pointer to the mesh */
		CTMesh * m_pMesh;
		/*! Pointer to the halfface */
		CHalfFace * m_pF;
		/*! Pointer to the halfedge */
		CHalfEdge * m_pH;
	};


	/*!
	\brief Face->vertex Iterator

	go through all the vertices of a Face
	*/
	class FaceVertexIterator
	{
	public:
		/*! constructor */
		FaceVertexIterator(CTMesh * pMesh, CFace * pF)
		{
			m_pMesh = pMesh;
			m_pF = pF;
			m_pHF = m_pMesh->FaceLeftHalfFace(m_pF);
			m_pH = pMesh->HalfFaceHalfEdge(m_pHF);
		};
		/*! destructor */
		~FaceVertexIterator() {};
		/*! iterator ++ */
		void operator++()
		{
			m_pH = m_pMesh->HalfEdgeNext(m_pH);
			if (m_pH == m_pMesh->HalfFaceHalfEdge(m_pHF))
			{
				m_pH = NULL;
			}
		};
		/*! ++ iterator */
		void operator++(int)
		{
			m_pH = m_pMesh->HalfEdgeNext(m_pH);
			if (m_pH == m_pMesh->HalfFaceHalfEdge(m_pHF))
			{
				m_pH = NULL;
			}
		};
		/*! verify if the end of the list has been reached */
		bool end() { return m_pH == NULL; };
		/*! dereferencing */
		CVertex * operator*() { return m_pMesh->HalfEdgeTarget(m_pH); };

	private:
		/*! Pointer to the mesh */
		CTMesh * m_pMesh;
		/*! Pointer to the face */
		CFace * m_pF;
		/*! Pointer to the halfface */
		CHalfFace * m_pHF;
		/*! Pointer to the halfedge */
		CHalfEdge * m_pH;
	};


	/*!
	\brief TVertex->InHalfEdge Iterator

	go through all the InHalfEdge of a TVertex
	*/
	class TVertexInHalfEdgeIterator
	{
	public:
		/*! constructor */
		TVertexInHalfEdgeIterator(CTMesh * pMesh, CTVertex * pTV)
		{
			m_pMesh = pMesh;
			m_pTV = pTV;
			m_pH = pMesh->TVertexHalfEdge(pTV);
			m_pHF = m_pMesh->HalfEdgeHalfFace(m_pH);
		};
		/*! destructor */
		~TVertexInHalfEdgeIterator() {};
		/*! iterator ++ */
		void operator++()
		{
			m_pH = m_pMesh->HalfEdgeNext(m_pH);
			if (m_pH == m_pMesh->HalfFaceHalfEdge(m_pHF))
			{
				m_pH = NULL;
			}
		};
		/*! ++ iterator */
		void operator++(int unused)
		{
			CHalfEdge * pN = m_pMesh->HalfEdgeNext(m_pH);
			m_pH = m_pMesh->HalfEdgeDual(pN);
			if (m_pH == m_pMesh->TVertexHalfEdge(m_pTV))
			{
				m_pH = NULL;
			}
		};
		/*! verify if the end of the list has been reached */
		bool end() { return m_pH == NULL; };
		/*! dereferencing */
		CHalfEdge * operator*() { return m_pH; };

	private:
		/*! Pointer to the mesh */
		CTMesh * m_pMesh;
		/*! Pointer to the TVertex */
		CTVertex * m_pTV;
		/*! Pointer to the halfedge */
		CHalfEdge * m_pH;
		/*! Pointer to the halfface */
 		CHalfFace * m_pHF;
	};

	/*!
	\brief TVertex->TEdge Iterator

	go through all the TEdge of a TVertex
	*/
	class TVertexTEdgeIterator
	{
	public:
		/*! constructor */
		TVertexTEdgeIterator(CTMesh * pMesh, CTVertex * pTV)
		{
			m_pMesh = pMesh;
			m_pTV = pTV;
			m_pH = pMesh->TVertexHalfEdge(pTV);
			m_pHF = m_pMesh->HalfEdgeHalfFace(m_pH);
		};
		/*! destructor */
		~TVertexTEdgeIterator() {};
		/*! iterator ++ */
		void operator++()
		{
			m_pH = m_pMesh->HalfEdgeNext(m_pH);
			if (m_pH == m_pMesh->HalfFaceHalfEdge(m_pHF))
			{
				m_pH = NULL;
			}
		};
		/*! ++ iterator */
		void operator++(int unused)
		{
			CHalfEdge * pN = m_pMesh->HalfEdgeNext(m_pH);
			m_pH = m_pMesh->HalfEdgeDual(pN);
			if (m_pH == m_pMesh->TVertexHalfEdge(m_pTV))
			{
				m_pH = NULL;
			}
		};
		/*! verify if the end of the list has been reached */
		bool end() { return m_pH == NULL; };
		/*! dereferencing */
		CTEdge * operator*() { return m_pMesh->HalfEdgeTEdge(m_pH); };

	private:
		/*! Pointer to the mesh */
		CTMesh * m_pMesh;
		/*! Pointer to the TVertex */
		CTVertex * m_pTV;
		/*! Pointer to the halfedge */
		CHalfEdge * m_pH;
		/*! Pointer to the halfface */
 		CHalfFace * m_pHF;
	};

	/*!
	\brief Vertex->TVertex Iterator
	
	go through all the tvertices of a vertex
	*/
	class VertexTVertexIterator
	{
	public:
		/*! constrructor */
		VertexTVertexIterator(CTMesh * pMesh, CVertex * pV)
		{
			m_pMesh = pMesh;
			m_pVert = pV;
			m_iter = m_pMesh->VertexTVertexList(pV)->begin();
		};
		/*! Destructor */
		~VertexTVertexIterator(){};
		/*! ++iterator */
		void operator++(){ m_iter++; };
		/*! iterator++ */
		void operator++(int unused) { m_iter++; };
		/*! verify if the end has been reached */
		bool end(){ return m_iter == m_pMesh->VertexTVertexList(m_pVert)->end(); };
		/*! dereferencing */
		CTVertex * operator*() { return *m_iter; };
	private:
		/*! pointer to mesh */
		CTMesh * m_pMesh;
		/*! pointer to the vertex */
		CVertex  * m_pVert;
		/*! TVertex list iterator */
		typename std::list<CTVertex*>::iterator m_iter;
	};

};
#endif