/*!
*      \file BaseTetMesh.h
*      \brief Base TetMesh Class for all types of Tetrahedron Mesh Classes
*
*		This is the fundamental class for tetrahedral meshes
*	   \author David Gu, Jerome Jiang
*      \date 10/01/2011
*	   \modified 4/16/2015
*
*/

#ifndef _TMESHLIB_TET_MESH_H_
#define _TMESHLIB_TET_MESH_H_

#include <cassert>
#include <list>
#include <cmath>
#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <map>

#include "../Geometry/Point.h"
#include "../parser/strutil.h"

#ifndef MAX_LINE 
#define MAX_LINE 2048
#endif

namespace TMeshLib
{
    using CPoint = MeshLib::CPoint;

    class CTVertex {};
    class CVertex {};
    class CTEdge {};
    class CEdge {};
    class CHalfEdge {};
    class CHalfFace {};
    class CFace {};
    class CTet {};

    /*!
    * \brief CBaseTMesh, base class for all types of tet-mesh classes
    *
    *  This is the fundamental class for tet-meshes. All the geometric objects are connected by pointers,
    *  vertex, edge, face, tet are connected by halffaces. The mesh class has file IO functionalities,
    *  supporting .tet file formats. It offers Euler operators, each geometric primative
    *  can access its neighbors freely.
    *
    * \tparam CVertex   vertex   class, derived from TMeshLib::CVertex     class
    * \tparam CTVertex  tetrahedron vertex   class, derived from TMeshLib::CTVertex   class
    * \tparam CHalfEdge halfedge class, derived from TMeshLib::CHalfEdge class
    * \tparam CTEdge	tetrahedron edge class, derived from TMeshLib::CTEdge class
    * \tparam CEdge     edge     class, derived from MeshLib::CEdge     class
    * \tparam CFace     face     class, derived from TMeshLib::CFace     class
    * \tparam CHalfFace half face     class, derived from TMeshLib::CHalfFace     class
    * \tparam CTet      tetrahedron class, derived from TMeshLib::CTet class
    */
    template <typename TV = CTVertex, typename V = CVertex, typename HE = CHalfEdge, typename TE = CTEdge, typename E = CEdge, typename HF = CHalfFace, typename F = CFace, typename T = CTet>
    class CBaseTMesh
    {
    public:
        class CTVertex;
        class CVertex;
        class CTEdge;
        class CEdge;
        class CHalfEdge;
        class CHalfFace;
        class CFace;
        class CTet;

        /*!
        * \brief CTVertex, base class for Tetrahedron vertex
        */
        class CTVertex : public TV
        {
        public:
            CTVertex() { m_pVertex = NULL; m_pTet = NULL; m_pHalfedge = NULL; };
            ~CTVertex() {};

            int & id() { return m_id; };

            CVertex   * & vertex() { return m_pVertex; };
            CTet      * & tet() { return m_pTet; };
            CHalfEdge * & halfedge() { return m_pHalfedge; };

            virtual void _from_string() { };
            virtual void _to_string() { };

        protected:
            int          m_id;			// vertex ID
            CVertex    * m_pVertex;
            CTet       * m_pTet;
            CHalfEdge  * m_pHalfedge;	// outgoing, halfedge start from this TVertex
        };

        /*!
        * \brief CVertex, base class for vertex
        */
        class CVertex : public V
        {
        public:
            CVertex() { m_id = 0; };
            ~CVertex() { m_pTVertices.clear(); };

            CPoint & point() { return m_point; };
            int    & id() { return m_id; };
            bool   & boundary() { return m_boundary; };

            std::list<CEdge*>     & edges() { return m_pEdges; };
            std::list<CTEdge*>    & tedges() { return m_pTEdges; };

            std::list<CHalfFace*> & halffaces() { return m_pHFaces; };
            std::list<CTVertex*>  & tvertices() { return m_pTVertices; };

            std::string & string() { return m_string; };

            virtual void _from_string() { };
            virtual void _to_string() { };

        protected:

            CPoint m_point;
            int    m_id;
            bool   m_boundary;

            std::list<CHalfFace*>  m_pHFaces;		//temporary HalfFace list, will be empty after loading the whole mesh 
            std::list<CTEdge*>     m_pTEdges;		//temporary TEdge list, will be empty after loading the whole mesh

            std::list<CTVertex*>   m_pTVertices;	//adjacent TVertecies
            std::list<CEdge*>      m_pEdges;	    //adjacent Edges;

            std::string m_string;
        };

        /*!
        * \brief CHalfEdge, base class for halfedge
        */
        class CHalfEdge : public HE
        {
        public:
            CHalfEdge()
            {
                m_pSource = NULL;
                m_pTarget = NULL;
                m_pDual = NULL;
                m_pNext = NULL;
                m_pPrev = NULL;
                m_pTEdge = NULL;
                m_pHalfFace = NULL;
            };

            ~CHalfEdge() {};

            CVertex   * & source() { return m_pSource->vertex(); };
            CVertex   * & target() { return m_pTarget->vertex(); };

            CTVertex  * & tsource() { return m_pSource; };
            CTVertex  * & ttarget() { return m_pTarget; };

            CHalfEdge * & dual() { return m_pDual; };
            CHalfEdge * & next() { return m_pNext; };
            CHalfEdge * & prev() { return m_pPrev; };
            CTEdge    * & tedge() { return m_pTEdge; };
            CHalfFace * & halfface() { return m_pHalfFace; };

            virtual void _from_string() { };
            virtual void _to_string() { };

        protected:
            CTVertex   * m_pSource;
            CTVertex   * m_pTarget;
            CHalfEdge  * m_pDual;
            CHalfEdge  * m_pNext;
            CHalfEdge  * m_pPrev;
            CTEdge     * m_pTEdge;
            CHalfFace  * m_pHalfFace;
        };

        /*!
        * \brief CTEdge, base class for tedge, edge inside a tetrahedron
        */
        class CTEdge : public TE
        {
        public:

            CTEdge() { m_pLeft = NULL; m_pRight = NULL; };
            ~CTEdge() {};

            CHalfEdge * & left() { return m_pLeft; };
            CHalfEdge * & right() { return m_pRight; };
            CEdge     * & edge() { return m_pEdge; };
            CTet      * & tet() { return  m_pTet; };
            CTEdge    * & dual() { return  m_pLeft->prev()->dual()->next()->tedge(); } //tedge in the same tet, against the current one

            bool operator==(const CTEdge & e)
            {
                for (int i = 0; i < 2; i++)
                {
                    if (m_key[i] != e.m_key[i]) return false;
                }
                return true;
            }

            int & key(int k) { return m_key[k]; };

            virtual void _from_string() { };
            virtual void _to_string() { };

        protected:

            CHalfEdge    * m_pLeft;
            CHalfEdge    * m_pRight;
            CEdge        * m_pEdge;
            CTet         * m_pTet;
            int            m_key[2];

        };

        /*!
        * \brief CEdge, base class for edge, edge among tets
        */
        class CEdge : public E
        {
        public:

            CEdge() { m_vertices[0] = NULL; m_vertices[1] = NULL; };
            ~CEdge() { m_lTEdges.clear(); };

            bool   & boundary() { return m_boundary; };

            CVertex * & vertex1() { return m_vertices[0]; };
            CVertex * & vertex2() { return m_vertices[1]; };

            std::list<CTEdge*> & tedges() { return m_lTEdges; };

            virtual void _from_string() { };
            virtual void _to_string() { };

            std::string & string() { return m_string; };

        protected:
            std::list<CTEdge*> m_lTEdges;
            CVertex *   m_vertices[2];
            bool        m_boundary;
            std::string m_string;
        };

        /*!
        * \brief CHalfFace, base class for HalfFace
        */
        class CHalfFace : public HF
        {
        public:
            CHalfFace()
            {
                m_pHalfEdge = NULL;
                m_pFace = NULL;
                m_pTet = NULL;
                m_pDual = NULL;
            };

            ~CHalfFace() {};

            CHalfEdge * & halfedge() { return m_pHalfEdge; };
            CFace     * & face() { return m_pFace; };
            CTet      * & tet() { return m_pTet; };
            CHalfFace * & dual() { return m_pDual; };

            bool        & boundary() { return m_boundary; };
            int         & key(int k) { return m_key[k]; };

            bool operator==(const CHalfFace & f)
            {
                for (int i = 0; i < 3; i++)
                    if (m_key[i] != f.m_key[i]) return false;
                return true;
            };

        protected:
            CHalfEdge * m_pHalfEdge;
            CFace     * m_pFace;
            CTet      * m_pTet;
            CHalfFace * m_pDual;
            int         m_key[3];
            bool        m_boundary;
        };

        /*!
        * \brief CFace, base class for Face
        */
        class CFace : public F
        {
        public:
            CFace()
            {
                m_pLeft = NULL;
                m_pRight = NULL;
            };

            ~CFace() {};

            CHalfFace * & left() { return m_pLeft; };
            CHalfFace * & right() { return m_pRight; };

            bool & boundary() { return m_boundary; };
            std::string & string() { return m_string; };

            virtual void _from_string() { };
            virtual void _to_string() { };

        protected:

            CHalfFace * m_pLeft;
            CHalfFace * m_pRight;
            bool		m_boundary;
            std::string m_string;
        };

        /*!
        * \brief CTet, base class for Tet
        */
        class CTet : public T
        {
        public:

            CTet() { m_id = 0; };
            ~CTet() {};

            CHalfFace * & halfface(int j) { return m_pHalfFace[j]; };
            CTVertex  * & tvertex(int j) { return m_pTVertex[j]; };
            CVertex   * & vertex(int j) { return m_pTVertex[j]->vertex(); };

            int  & id() { return m_id; };

            virtual void _from_string() { };
            virtual void _to_string() { };

            std::string & string() { return m_string; };

        protected:

            CHalfFace  * m_pHalfFace[4];
            CTVertex   * m_pTVertex[4];
            int          m_id;

            std::string  m_string;
        };


    public:
        /*!
            CBaseTMesh constructor
        */
        CBaseTMesh() {};
        /*!
            CBaseTMesh desctructor
        */
        ~CBaseTMesh() { _clear(); };
        /*!
            Load tet mesh from a ".tet" file
        */
        void _load(const char *);
        /*!
            Load tet mesh from a ".t" file
        */
        void _load_t(const char *);
        /*!
            Write tet mesh to a file
        */
        void _write(const char *);

        /*!
            Write tet mesh to a .t file
        */
        void _write_t(const char * filename);

        /*!
            access the list of halffaces
        */
        std::list<CHalfFace*> & halffaces() { return m_pHalfFaces; };
        /*!
            access the list of edges
        */
        std::list<CEdge*>     & edges() { return m_pEdges; };
        /*!
            access list of faces
        */
        std::list<CFace*>     & faces() { return m_pFaces; };
        /*!
            access list of vertices
        */
        std::list<CVertex*>   & vertices() { return m_pVertices; };

        /*! number of tets */
        int num_tets() { return m_nTets; };

        /*! number of vertices */
        int num_vertices() { return m_nVertices; };

        /*! max vertex id*/
        int max_vertex_id() { return m_maxVertexId; };

        /*! Access the array of tets */
        std::list<CTet*> & tets() { return m_pTets; };

        /*! access the vertex with ID */
        CVertex * vertex(int id) { return m_map_Vertices[id]; };

        /*! access the tet with ID */
        CTet    * tet(int id) { return m_map_Tets[id]; };

        /*! Vertex->Edge */
        CEdge   * edge(CVertex * v1, CVertex * v2)
        {
            std::list<CEdge*> & edges = v1->edges();
            for(auto pE : edges)
            {
                CVertex * w1 = pE->vertex1();
                CVertex * w2 = pE->vertex2();
                if (w1 == v1 && w2 == v2) return pE;
                if (w1 == v2 && w2 == v1) return pE;
            }
            return NULL;
        }


    protected:

        /*!
        construct tetrahedron
        \tparam v array of vertex ids
        \tparam pT retulting tetrahedron
        */

        void  _construct_tet(CTet* pT, int id, int * v);
        /*! construct faces */
        void  _construct_faces();
        /*! construct edges */
        void  _construct_edges();
        /*!  construct half face
        \tparam array of vertex pointers
        */
        CHalfFace * _construct_half_face(CTVertex **);

        /*! release all the memory allocations */
        void _clear();

    protected:
        /*!
        list of faces
        */
        std::list<CFace*>        m_pFaces;
        /*!
        list of half faces
        */
        std::list<CHalfFace*>	 m_pHalfFaces;
        /*!
        list of half edges
        */
        std::list<CHalfEdge*>	 m_pHalfEdges;
        /*!
        list of edges
        */
        std::list<CEdge*>        m_pEdges;
        /*!
        list of tetrahedra
        */
        std::list<CTEdge*>		 m_pTEdges;

        /*!
            array of vertices
            */
        std::list<CVertex*>		 m_pVertices;
        //CVertex *				 m_pVertices;

        /*!
        map of CVertex id and pointer
        */
        std::map<int, CVertex *> m_map_Vertices;

        /*!
        array of tets
        */
        std::list<CTet*>		 m_pTets;
        //CTet*                    m_pTets;

        std::map<int, CTet*>     m_map_Tets;

        /*! number of vertices */
        int m_nVertices;

        /*! number of tets */
        int m_nTets;

        /*! number of edges */
        int m_nEdges;

        /*! max vertex id */
        int m_maxVertexId;

    };

    //construct faces
    template <typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
    void CBaseTMesh<TV, V, HE, TE, E, HF, F, T>::_construct_faces()
    {
        CVertex * pV = NULL;

        for (typename std::list<CVertex*>::iterator vIter = m_pVertices.begin(); vIter != m_pVertices.end(); vIter++)
        {
            pV = *vIter;

            std::list<CHalfFace*> & halffaces = pV->halffaces();

            while (!halffaces.empty())
            {
                CHalfFace * pF = halffaces.front();
                halffaces.remove(pF);
                CFace * f = new CFace();
                assert(f != NULL);
                m_pFaces.push_back(f);
                f->left() = pF;
                pF->face() = f;

                for(auto pH: halffaces)
                {
                    if (*pH == *pF)
                    {
                        pH->dual() = pF;
                        pF->dual() = pH;
                        f->right() = pH;
                        pH->face() = f;
                        break;
                    }
                }

                if (pF->dual() != NULL)
                {
                    halffaces.remove(pF->dual());
                }
            }
        }
    };

    //construct edges
    template <typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
    void CBaseTMesh<TV, V, HE, TE, E, HF, F, T>::_construct_edges()
    {
        for (typename std::list<CVertex*>::iterator vIter = m_pVertices.begin(); vIter != m_pVertices.end(); vIter++)
        {
            CVertex * pV = *vIter;
            std::list<CTEdge*> & pL = pV->tedges();

            while (!pL.empty())
            {
                CTEdge * pTE = pL.front();
                pL.remove(pTE);
                CEdge * e = new CEdge;
                assert(e != NULL);

                int id1 = pTE->key(0);
                CVertex * v1 = m_map_Vertices[id1];
                e->vertex1() = v1;

                int id2 = pTE->key(1);
                CVertex * v2 = m_map_Vertices[id2];
                e->vertex2() = v2;

                m_pEdges.push_back(e);

                e->tedges().push_back(pTE);
                pTE->edge() = e;

                std::list<CTEdge*> tmp_edges;
                for(auto pH: pL)
                {
                    if (*pH == *pTE)
                    {
                        pH->edge() = e;
                        tmp_edges.push_back(pH);
                    }
                }

                for(auto pH: tmp_edges)
                {
                    pL.remove(pH);
                    e->tedges().push_back(pH);
                }

            }

        }


        for (typename std::list<CEdge*>::iterator it = m_pEdges.begin(); it != m_pEdges.end(); it++)
        {
            CEdge * pE = *it;
            CVertex * v1 = pE->vertex1();
            CVertex * v2 = pE->vertex2();
            v1->edges().push_back(pE);
            v2->edges().push_back(pE);
        }
    };

    //construct tets
    template <typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
    void CBaseTMesh<TV, V, HE, TE, E, HF, F, T>::_construct_tet(CTet * pT, int id, int * v)
    {
        //set the tet->id

        pT->id() = id;

        //set TVertices of the Tet

        for (int k = 0; k < 4; k++)
        {
            CTVertex * pTV = new CTVertex();
            pT->tvertex(k) = pTV;
            pTV->id() = k;

            CVertex * pV = m_map_Vertices[v[k]];
            pTV->vertex() = pV;
            pV->tvertices().push_back(pTV);

            pTV->tet() = pT;
        }

        //set half faces
        int order[4][3] = { { 1, 2, 3 },{ 2, 0, 3 },{ 0, 1, 3 },{ 1, 0, 2 } };

        CTVertex   * pTV[3];
        CHalfFace * pHF[4];

        for (int i = 0; i < 4; i++)
        {
            for (int k = 0; k < 3; k++)
            {
                pTV[k] = pT->tvertex(order[i][k]);
            }
            pT->halfface(i) = _construct_half_face(pTV);
            pHF[i] = pT->halfface(i);
        }

        // connect the four half faces
        for (int i = 0; i < 4; i++)
        {
            pHF[i]->tet() = pT;
        }

        //Seting the dual half edges
        for (int i = 0; i < 3; i++)
        {
            CHalfEdge * pH0 = pHF[i]->halfedge();
            pH0 = pH0->next();
            CHalfEdge * pH1 = pHF[(i + 1) % 3]->halfedge();
            pH1 = pH1->prev();

            pH0->dual() = pH1;
            pH1->dual() = pH0;

            CTEdge * pTE = new CTEdge;
            assert(pTE != NULL);
            m_pTEdges.push_back(pTE);
            pTE->tet() = pT;
            pH0->tedge() = pTE;
            pH1->tedge() = pTE;

            if (pH0->source()->id() < pH0->target()->id())
            {
                pTE->left() = pH0;
                pTE->right() = pH1;
            }
            else
            {
                pTE->left() = pH0;
                pTE->right() = pH1;
            }

            pTE->key(0) = pTE->left()->source()->id();
            pTE->key(1) = pTE->left()->target()->id();

            CVertex * v = m_map_Vertices[pTE->key(0)];
            v->tedges().push_back(pTE);
        }

        CHalfEdge * pH0 = pHF[3]->halfedge();
        for (int i = 0; i < 3; i++)
        {
            CHalfEdge * pH1 = pHF[2 - i]->halfedge();
            pH0->dual() = pH1;
            pH1->dual() = pH0;

            CTEdge * pTE = new CTEdge();
            assert(pTE != NULL);
            //save TEdges to the TEdge list
            m_pTEdges.push_back(pTE);
            //set TEdge->Tet
            pTE->tet() = pT;
            //set HalfEdge->TEdge
            pH0->tedge() = pTE;
            pH1->tedge() = pTE;

            if (pH0->source()->id() < pH0->target()->id())
            {
                pTE->left() = pH0;
                pTE->right() = pH1;
            }
            else
            {
                pTE->left() = pH0;
                pTE->right() = pH1;
            }
            pTE->key(0) = pTE->left()->source()->id();
            pTE->key(1) = pTE->left()->target()->id();

            CVertex * v = m_map_Vertices[pTE->key(0)];
            v->tedges().push_back(pTE);

            pH0 = pH0->next();
        }
    };


    template <typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
    void CBaseTMesh<TV, V, HE, TE, E, HF, F, T>::_clear()
    {
        for (std::list<CEdge*>::iterator eit = m_pEdges.begin(); eit != m_pEdges.end(); eit++)
        {
            CEdge * pE = *eit;
            delete pE;
        }

        for (std::list<CTEdge*>::iterator eit = m_pTEdges.begin(); eit != m_pTEdges.end(); eit++)
        {
            CTEdge * pE = *eit;
            delete pE;
        }

        for (std::list<CHalfEdge*>::iterator hit = m_pHalfEdges.begin(); hit != m_pHalfEdges.end(); hit++)
        {
            CHalfEdge * pH = *hit;
            delete pH;
        }


        for (std::list<CFace*>::iterator fit = m_pFaces.begin(); fit != m_pFaces.end(); fit++)
        {
            CFace * pF = *fit;
            delete pF;
        }

        for (std::list<CHalfFace*>::iterator fit = m_pHalfFaces.begin(); fit != m_pHalfFaces.end(); fit++)
        {
            CHalfFace * pF = *fit;
            delete pF;
        }

        m_pVertices.clear();
        m_pTets.clear();
        //delete[] m_pTVertices;
    };

    template <typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
    void CBaseTMesh<TV, V, HE, TE, E, HF, F, T>::_load(const char * input)
    {
        m_maxVertexId = 0;

        std::ifstream is(input);

        if (is.fail())
        {
            fprintf(stderr, "Error in opening file %s\n", input);
            return;
        }

        char buffer[MAX_LINE];
        {
            //read in the first line
            is.getline(buffer, MAX_LINE);
            //read in the number of vertices: "number vertices"
            std::string line(buffer);
            line = strutil::trim(line);
            strutil::Tokenizer stokenizer(line, " \r\n");
            stokenizer.nextToken();
            std::string token = stokenizer.getToken();
            m_nVertices = strutil::parseString<int>(token);
            stokenizer.nextToken();
            token = stokenizer.getToken();
            std::cout << token << std::endl;
            if (token != "vertices")
            {
                fprintf(stderr, "Error in vertex format %s\n", input);
                return;
            }
        }

        //read in the second line
        is.getline(buffer, MAX_LINE);
        //read in the number of vertices: "number tets"
        std::string line(buffer);
        line = strutil::trim(line);
        strutil::Tokenizer stokenizer(line, " \r\n");
        stokenizer.nextToken();
        std::string token = stokenizer.getToken();
        m_nTets = strutil::parseString<int>(token);
        stokenizer.nextToken();
        token = stokenizer.getToken();
        if (token != "tets")
        {
            fprintf(stderr, "Error in tet format %s\n", input);
            return;
        }

        //read in the vertices
        for (int i = 0; i < m_nVertices && is.getline(buffer, MAX_LINE); i++)
        {
            std::string line(buffer);
            line = strutil::trim(line);
            strutil::Tokenizer stokenizer(line, " \r\n");

            CPoint p;
            for (int k = 0; k < 3; k++)
            {
                stokenizer.nextToken();
                std::string token = stokenizer.getToken();
                p[k] = strutil::parseString<float>(token);
            }

            CVertex * v = new CVertex();
            //CVertex & v = m_pVertices[i];
            v->id() = i;
            v->point() = p;

            m_pVertices.push_back(v);
            m_map_Vertices.insert(std::pair<int, CVertex*>(i, v));

            if (!stokenizer.nextToken("\t\r\n")) continue;
            std::string token = stokenizer.getToken();

            int sp = (int)token.find("{");
            int ep = (int)token.find("}");

            if (sp >= 0 && ep >= 0)
            {
                v->string() = token.substr(sp + 1, ep - sp - 1);
            }
        }


        //read in tets 
        for (int id = 0; id < m_nTets && is.getline(buffer, MAX_LINE); id++)
        {
            int vId[4];

            std::string line(buffer);
            line = strutil::trim(line);
            strutil::Tokenizer stokenizer(line, " \r\n");

            //skip the first "4" in the line
            stokenizer.nextToken();
            std::string token = stokenizer.getToken();

            for (int k = 0; k < 4; k++)
            {
                stokenizer.nextToken();
                std::string token = stokenizer.getToken();
                vId[k] = strutil::parseString<int>(token);
            }

            CTet * pT = new CTet();
            m_pTets.push_back(pT);
            m_map_Tets.insert(std::pair<int, CTet *>(id, pT));

            _construct_tet(pT, id, vId);
        }

        _construct_faces();
        _construct_edges();

        is.close();

        for (typename std::list<CVertex*>::iterator vIter = m_pVertices.begin(); vIter != m_pVertices.end(); vIter++)
        {
            CVertex * pV = *vIter;
            if (pV->id() > m_maxVertexId)
            {
                m_maxVertexId = pV->id();
            }
        }

    };

    template <typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
    void CBaseTMesh<TV, V, HE, TE, E, HF, F, T>::_load_t(const char * input)
    {
        m_maxVertexId = 0;

        std::fstream is(input, std::fstream::in);

        if (is.fail())
        {
            fprintf(stderr, "Error in opening file %s\n", input);
            return;
        }

        char buffer[MAX_LINE];

        m_nVertices = 0;
        m_nTets = 0;
        m_nEdges = 0;

        while (!is.eof())
        {
            is.getline(buffer, MAX_LINE);
            std::string line(buffer);
            line = strutil::trim(line);
            strutil::Tokenizer stokenizer(line, " \r\n");

            stokenizer.nextToken();
            std::string token = stokenizer.getToken();

            if (token == "Vertex") m_nVertices++;
            if (token == "Tet") m_nTets++;
            if (token == "Edge") m_nEdges++;
        }

        is.clear();              // forget we hit the end of file
        is.seekg(0, std::ios::beg);   // move to the start of the file

        //read in the vertices
        for (int i = 0; i < m_nVertices && is.getline(buffer, MAX_LINE); i++)
        {
            std::string line(buffer);
            line = strutil::trim(line);
            strutil::Tokenizer stokenizer(line, " \r\n");

            stokenizer.nextToken();
            std::string token = stokenizer.getToken();

            if (token != "Vertex")
            {
                fprintf(stderr, "Error in vertex format\n");
                return;
            }


            stokenizer.nextToken();
            token = stokenizer.getToken();
            int vid = strutil::parseString<int>(token);

            CPoint p;
            for (int k = 0; k < 3; k++)
            {
                stokenizer.nextToken();
                std::string token = stokenizer.getToken();
                p[k] = strutil::parseString<float>(token);
            }

            CVertex * v = new CVertex();
            //CVertex & v = m_pVertices[i];
            v->id() = vid;
            v->point() = p;

            m_pVertices.push_back(v);

            m_map_Vertices.insert(std::pair<int, CVertex *>(vid, v));

            if (!stokenizer.nextToken("\t\r\n")) continue;
            token = stokenizer.getToken();

            int sp = (int)token.find("{");
            int ep = (int)token.find("}");

            if (sp >= 0 && ep >= 0)
            {
                v->string() = token.substr(sp + 1, ep - sp - 1);
            }
        }


        //read in tets 
        for (int id = 0; id < m_nTets && is.getline(buffer, MAX_LINE); id++)
        {
            int vId[4];

            std::string line(buffer);
            line = strutil::trim(line);
            strutil::Tokenizer stokenizer(line, " \r\n");

            stokenizer.nextToken();
            std::string token = stokenizer.getToken();

            if (token != "Tet")
            {
                fprintf(stderr, "Error in tet format\n");
                return;
            }

            //skip the first "4" in the line
            stokenizer.nextToken();
            token = stokenizer.getToken();
            int tid = strutil::parseString<int>(token);


            for (int k = 0; k < 4; k++)
            {
                stokenizer.nextToken();
                std::string token = stokenizer.getToken();
                vId[k] = strutil::parseString<int>(token);
            }



            CTet * pT = new CTet();
            m_pTets.push_back(pT);
            m_map_Tets.insert(std::pair<int, CTet *>(tid, pT));

            _construct_tet(pT, tid, vId);

            // read in string
            if (!stokenizer.nextToken("\t\r\n")) continue;
            token = stokenizer.getToken();

            int sp = (int)token.find("{");
            int ep = (int)token.find("}");

            if (sp >= 0 && ep >= 0)
            {
                pT->string() = token.substr(sp + 1, ep - sp - 1);
            }
        }

        _construct_faces();
        _construct_edges();

        for (int id = 0; id < m_nEdges && is.getline(buffer, MAX_LINE); id++)
        {
            std::string line(buffer);
            line = strutil::trim(line);
            strutil::Tokenizer stokenizer(line, " \r\n");

            stokenizer.nextToken();
            std::string token = stokenizer.getToken();

            if (token != "Edge")
            {
                fprintf(stderr, "Error in edge format\n");
                return;
            }

            stokenizer.nextToken();
            token = stokenizer.getToken();
            int id1 = strutil::parseString<int>(token);

            stokenizer.nextToken();
            token = stokenizer.getToken();
            int id2 = strutil::parseString<int>(token);

            CVertex * pV1 = this->vertex(id1);
            CVertex * pV2 = this->vertex(id2);

            CEdge * pE = this->edge(pV1, pV2);

            if (!stokenizer.nextToken("\t\r\n"))
            {
                continue;
            }

            token = stokenizer.getToken();

            int sp = (int)token.find("{");
            int ep = (int)token.find("}");

            if (sp >= 0 && ep >= 0)
            {
                pE->string() = token.substr(sp + 1, ep - sp - 1);
            }
        }

        m_nEdges = (int)m_pEdges.size();

        is.close();

        for (typename std::list<CVertex*>::iterator vIter = m_pVertices.begin(); vIter != m_pVertices.end(); vIter++)
        {
            CVertex * pV = *vIter;
            if (pV->id() > m_maxVertexId)
            {
                m_maxVertexId = pV->id();
            }
        }

        // read in traits
        for (typename std::list<CVertex*>::iterator vIter = m_pVertices.begin(); vIter != m_pVertices.end(); vIter++)
        {
            CVertex * pV = *vIter;
            pV->_from_string();
        }

        for (typename std::list<CTet *>::iterator tIter = m_pTets.begin(); tIter != m_pTets.end(); tIter++)
        {
            CTet * pT = *tIter;
            pT->_from_string();
        }

        for (typename std::list<CEdge*>::iterator eIter = m_pEdges.begin(); eIter != m_pEdges.end(); eIter++)
        {
            CEdge * pE = *eIter;
            pE->_from_string();
        }

    };

    template <typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
    typename CBaseTMesh<TV, V, HE, TE, E, HF, F, T>::CHalfFace* CBaseTMesh<TV, V, HE, TE, E, HF, F, T>::_construct_half_face(CTVertex ** pTV)
    {
        CHalfFace * pHF = new CHalfFace();
        assert(pHF != NULL);
        m_pHalfFaces.push_back(pHF);

        CVertex * pV[3];

        for (int i = 0; i < 3; i++)
        {
            pV[i] = pTV[i]->vertex();
        }

        CHalfEdge * pH[3];
        for (int i = 0; i < 3; i++)
        {
            pH[i] = new CHalfEdge();
            assert(pH[i] != NULL);
            m_pHalfEdges.push_back(pH[i]);

            pH[i]->halfface() = pHF;
            pH[i]->tsource() = pTV[i];
            pH[i]->ttarget() = pTV[(i + 1) % 3];
            pTV[i]->halfedge() = pH[i];
        }

        for (int i = 0; i < 3; i++)
        {
            pH[i]->next() = pH[(i + 1) % 3];
            pH[i]->prev() = pH[(i + 2) % 3];
        }

        pHF->halfedge() = pH[0];

        for (int i = 0; i < 3; i++)
        {
            pHF->key(i) = pV[i]->id();
        }

        //bubble
        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 2 - i; j++)
            {
                if (pHF->key(j) > pHF->key(j + 1))
                {
                    int tmp = pHF->key(j);
                    pHF->key(j) = pHF->key(j + 1);
                    pHF->key(j + 1) = tmp;
                }
            }
        }

        assert(pHF->key(0) < pHF->key(1));
        assert(pHF->key(1) < pHF->key(2));

        CVertex * pv = m_map_Vertices[pHF->key(0)];

        pv->halffaces().push_back(pHF);

        return pHF;
    };


    //write tet mesh to the file
    template <typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
    void CBaseTMesh<TV, V, HE, TE, E, HF, F, T>::_write(const char * output)
    {

        std::fstream _os(output, std::fstream::out);
        if (_os.fail())
        {
            fprintf(stderr, "Error is opening file %s\n", output);
            return;
        }
        _os << m_nVertices << " vertices" << std::endl;
        _os << m_nTets << " tets" << std::endl;

        for (typename std::list<CVertex*>::iterator vIter = m_pVertices.begin(); vIter != m_pVertices.end(); vIter++)
        {
            CVertex * pV = *vIter;
            CPoint  p = pV->point();
            for (int k = 0; k < 3; k++)
            {
                _os << " " << p[k];
            }
            if (pV->string().size() > 0)
            {
                _os << " " << "{" << pV->string() << "}";
            }
            _os << std::endl;
        }

        for (int i = 0; i < m_nTets; i++)
        {
            CTet * pt = m_map_Tets[i];
            _os << 4;
            for (int k = 0; k < 4; k++)
            {
                _os << " " << pt->vertex(k)->id();
            }
            //if( pt->string().size() > 0 )
            //{
            //	_os << " " << "{"<< pt->string() << "}";
            //}
            _os << std::endl;
        }

        for (typename std::list<CEdge*>::iterator eIter = m_pEdges.begin(); eIter != m_pEdges.end(); eIter++)
        {
            CEdge * pE = *eIter;
            if (pE->string().size() > 0)
            {
                _os << "Edge " << pE->vertex1()->id() << " " << pE->vertex2()->id() << " ";
                _os << "{" << pE->string() << "}" << std::endl;
            }
        }

        _os.close();
    };

    template <typename TV, typename V, typename HE, typename TE, typename E, typename HF, typename F, typename T>
    void CBaseTMesh<TV, V, HE, TE, E, HF, F, T>::_write_t(const char * output)
    {
        //write traits to string, add by Wei Chen, 11/23/2015
        for (typename std::list<CVertex*>::iterator vIter = m_pVertices.begin(); vIter != m_pVertices.end(); vIter++)
        {
            CVertex * pV = *vIter;
            pV->_to_string();
        }

        for (typename std::list<CTet *>::iterator tIter = m_pTets.begin(); tIter != m_pTets.end(); tIter++)
        {
            CTet * pT = *tIter;
            pT->_to_string();
        }

        for (typename std::list<CEdge*>::iterator eIter = m_pEdges.begin(); eIter != m_pEdges.end(); eIter++)
        {
            CEdge * pE = *eIter;
            pE->_to_string();
        }
        //write traits end

        std::fstream _os(output, std::fstream::out);
        if (_os.fail())
        {
            fprintf(stderr, "Error while opening file %s\n", output);
            return;
        }

        for (typename std::list<CVertex*>::iterator vIter = m_pVertices.begin(); vIter != m_pVertices.end(); vIter++)
        {
            CVertex * pV = *vIter;
            CPoint p = pV->point();
            _os << "Vertex " << pV->id();
            for (int k = 0; k < 3; k++)
            {
                _os << " " << p[k];
            }
            if (pV->string().size() > 0)
            {
                _os << " " << "{" << pV->string() << "}";
            }
            _os << std::endl;
        }

        for (typename std::list<CTet *>::iterator tIter = m_pTets.begin(); tIter != m_pTets.end(); tIter++)
        {
            CTet * pT = *tIter;
            _os << "Tet " << pT->id();
            for (int k = 0; k < 4; k++)
            {
                _os << " " << pT->vertex(k)->id();
            }
            if (pT->string().size() > 0)
            {
                _os << " " << "{" << pT->string() << "}";
            }
            _os << std::endl;
        }

        for (typename std::list<CEdge*>::iterator eIter = m_pEdges.begin(); eIter != m_pEdges.end(); eIter++)
        {
            CEdge * pE = *eIter;
            if (pE->string().size() > 0)
            {
                _os << "Edge " << pE->vertex1()->id() << " " << pE->vertex2()->id() << " ";
                _os << "{" << pE->string() << "}" << std::endl;
            }
        }

        _os.close();
    }


};
#endif // _TMESHLIB_TET_MESH_H_
