/*!
*       \file BaseMesh.h
*       \brief Base Mesh Class for all types of Mesh Classes
*
*       This is the fundamental class for meshes
*       \author David Gu
*       \date 10/07/2010
*
*/

#ifndef _MESHLIB_BASE_MESH_H_
#define _MESHLIB_BASE_MESH_H_

#define MAX_LINE 2048

#include <math.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <map>
#include <set>

#include "../Geometry/Point.h"
#include "../Geometry/Point2.h"
#include "../parser/strutil.h"

namespace MeshLib {

    class CVertex {};
    class CEdge {};
    class CFace {};
    class CHalfEdge {};

    /*!
    * \brief CBaseMesh, base class for all types of mesh classes
    *
    *  This is the fundamental class for meshes. It includes a list of vertices,
    *  a list of edges, a list of faces, a list of halfedges. All the geometric objects are connected by pointers,
    *  vertex, edge, face are connected by halfedges. The mesh class has file IO functionalities,
    *  supporting .obj, .m and .off file formats. It offers Euler operators, each geometric primative
    *  can access its neighbors freely.
    *
    * \tparam V   vertex class, derived from MeshLib::CVertex   class
    * \tparam E     edge class, derived from MeshLib::CEdge     class
    * \tparam F     face class, derived from MeshLib::CFace     class
    * \tparam H halfedge class, derived from MeshLib::CHalfEdge class
    */
    template<typename V = CVertex, typename E = CEdge, typename F = CFace, typename H = CHalfEdge>
    class CBaseMesh
    {
    public:
        class CVertex;
        class CEdge;
        class CFace;
        class CHalfEdge;

        /*!
        \brief CVertex class, which is the base class of all kinds of vertex classes
        */
        class CVertex : public V
        {
        public:
            /*!
            CVertex constructor
            */
            CVertex() { m_halfedge = NULL; m_boundary = false; }
            /*!
            CVertex destructor
            */
            ~CVertex() {};

            /*! The point of the vertex
            */
            CPoint & point() { return m_point; }
            /*! The normal of the vertex
            */
            CPoint & normal() { return m_normal; }
            /*! The texutre coordinates of the vertex
            */
            CPoint2 & uv() { return m_uv; }

            /*! The most counter clockwise outgoing halfedge of the vertex .
            */
            CHalfEdge * most_ccw_out_halfedge()
            {
                //for interior vertex
                if (!m_boundary)
                {
                    return most_ccw_in_halfedge()->dual(); //most ccw out halfedge is the dual of the most ccw in halfedge
                }

                //for boundary vertex
                CHalfEdge * he = m_halfedge->next();
                //get the out halfedge which is the next halfedge of the most ccw in halfedge
                CHalfEdge * ne = he->ccw_rotate_about_source();
                //rotate ccwly around the source vertex
                while (ne != NULL)
                {
                    he = ne;
                    ne = he->ccw_rotate_about_source();
                }
                return he;
            }
            /*! The most clockwise outgoing halfedge of the vertex .
            */
            CHalfEdge * most_clw_out_halfedge()
            {
                //for interior vertex
                if (!m_boundary)
                {
                    return most_ccw_out_halfedge()->ccw_rotate_about_source();  //most ccw out halfedge rotate ccwly once about the source
                }
                //get one out halfedge
                CHalfEdge * he = m_halfedge->next();
                //rotate the out halfedge clwly about the source
                CHalfEdge * ne = he->clw_rotate_about_source();

                while (ne != NULL)
                {
                    he = ne;
                    ne = he->clw_rotate_about_source();
                }

                return he;
            }
            /*! The most counter clockwise incoming halfedge of the vertex.
            */
            CHalfEdge * most_ccw_in_halfedge()
            {
                //for interior vertex
                if (!m_boundary)
                {
                    return m_halfedge; //current half edge is the most ccw in halfedge
                }

                //for boundary vertex
                CHalfEdge * he = m_halfedge->ccw_rotate_about_target();
                //rotate to the most ccw in halfedge
                while (he != NULL)
                {
                    m_halfedge = he;
                    he = m_halfedge->ccw_rotate_about_target();
                }
                // the halfedge of the vertex becomes the most ccw in halfedge
                return m_halfedge;
            }
            /*! The most clockwise incoming halfedge of the vertex.
            */
            CHalfEdge * most_clw_in_halfedge()
            {
                //for interior vertex
                if (!m_boundary)
                {
                    return most_ccw_in_halfedge()->ccw_rotate_about_target(); //the most ccw in halfedge rotate ccwly once to get the most clw in halfedge
                }
                //for boundary vertex
                CHalfEdge * he = m_halfedge->clw_rotate_about_target();
                //rotate to the most clw in halfedge
                while (he != NULL)
                {
                    m_halfedge = he;
                    he = m_halfedge->clw_rotate_about_target();
                }

                return m_halfedge;
            }

            /*! One incoming halfedge of the vertex .
            */
            CHalfEdge * & halfedge() { return m_halfedge; }
            /*! the string of the vertex.
            */
            std::string & string() { return m_string; }

            bool & touched() { return m_touched; }

            /*! Vertex id.
            */
            int  & id() { return m_id; }
            /*! Whether the vertex is on the boundary.
            */
            bool & boundary() { return m_boundary; }

            /*! Convert vertex traits to string.
            */
            void _to_string() {}
            /*! Read traits from the string.
            */
            void _from_string() {}

            /*! Adjacent edges
            */
            std::vector<CEdge*>   & edges()
            { 
                if (m_edges.empty())
                {
                    m_halfedges = this->halfedges();
                    for (CHalfEdge * he : m_halfedges) m_edges.push_back(he->edge());
                    if (this->boundary())
                    {
                        CHalfEdge * he = this->most_ccw_in_halfedge();
                        m_edges.push_back(he->edge());
                    }
                }
                return m_edges;
            }
            /*! Adjacent vertices
            */
            std::vector<CVertex*> & vertices()
            { 
                if (m_vertices.empty())
                {
                    m_halfedges = this->halfedges();
                    for (CHalfEdge * he : m_halfedges) m_vertices.push_back(he->target());
                    if (this->boundary())
                    {
                        CHalfEdge * he = this->most_ccw_in_halfedge();
                        m_vertices.push_back(he->source());
                    }
                }
                return m_vertices;
            }
            /* Adjacent faces
            */
            std::vector<CFace*>   & faces()
            {
                if (m_faces.empty())
                {
                    m_halfedges = this->halfedges();
                    for (CHalfEdge * he : m_halfedges) m_faces.push_back(he->face());
                }
                return m_faces;
            }

            /*! Adjacent halfedges, direction == 1 means out, direction == -1 means in
            */
            std::vector<CHalfEdge*> & halfedges(int direction = 1)
            {
                if (!m_halfedges.empty() && halfedge_direction == direction) return m_halfedges;
                m_halfedges.clear();
                if (direction == -1) // in halfedges
                {
                    CHalfEdge * he0 = this->most_ccw_in_halfedge();
                    CHalfEdge * he = he0;
                    m_halfedges.push_back(he);
                    while (he)
                    {
                        he = he->clw_rotate_about_target();
                        if (!he || he == he0) break;
                        m_halfedges.push_back(he);
                    }
                }
                else // out halfedge
                {
                    CHalfEdge * he0 = this->most_clw_out_halfedge();
                    CHalfEdge * he = he0;
                    m_halfedges.push_back(he);
                    while (he)
                    {
                        he = he->ccw_rotate_about_source();
                        if (!he || he == he0) break;
                        m_halfedges.push_back(he);
                    }
                }
                return m_halfedges;
            }

            std::list<CEdge*> & ledges() { return m_ledges; }

        protected:

            /*! Vertex ID.
            */
            int    m_id;
            /*! Vertex position point.
            */
            CPoint m_point;
            /*! Normal at the vertex.
            */
            CPoint m_normal;
            /*! Texture coordinates of the vertex.
            */
            CPoint2 m_uv;
            /*! The most CCW incoming halfedge of the vertex.
            */
            CHalfEdge *       m_halfedge;
            /*! Indicating if the vertex is on the boundary.
            */
            bool              m_boundary;
            /*! The string of the vertex, which stores the traits information.
            */
            std::string       m_string;

            /*! List of adjacent edges, such that current vertex is the end vertex of the edge with smaller id
            */
            std::vector<CEdge*>   m_edges;
            /*! Adjacent vertices, ccw order
            */
            std::vector<CVertex*> m_vertices;
            /*! Adjacent faces, ccw order
            */
            std::vector<CFace*>   m_faces;
            /*!
            */
            std::vector<CHalfEdge*> m_halfedges;
            int halfedge_direction = 1;

            /*! temp edge list when loading mesh
            */
            std::list<CEdge*> m_ledges;

            bool              m_touched;

        }; //class CVertex

        /*!
        \brief CEdge class, which is the base class of all kinds of edge classes
        */
        class CEdge : public E
        {
        public:
            /*!
            CEdge constructor, set both halfedge pointers to be NULL.
            */
            CEdge() { m_halfedge[0] = NULL; m_halfedge[1] = NULL; };
            /*!
            CEdge destructor.
            */
            ~CEdge() {};
            /*!
            The halfedge attached to the current edge
            \param id either 0 or 1
            \return the halfedge[id]
            */
            CHalfEdge * & halfedge(int id) { assert(0 <= id && id < 2); return m_halfedge[id]; };
            /*!
            The vertex of an edge.
            \param id either 0 or 1.
            \return the vertex[id] of e.
            */
            CVertex * vertex(int id)
            {
                assert(0 <= id && id < 2);
                assert(m_halfedge[0] != NULL);
                if (id == 0) return m_halfedge[0]->source();
                else return m_halfedge[0]->target();
            }
            /*!
            The first vertex of an edge.
            \param e the input edge.
            \return the first vertex of e.
            */
            CVertex * vertex1() { assert(m_halfedge[0] != NULL); return m_halfedge[0]->source(); }
            /*!
            The second vertex of an edge.
            \param e the input edge.
            \return the second vertex of e.
            */
            CVertex * vertex2() { assert(m_halfedge[0] != NULL); return m_halfedge[0]->target(); }

            //edge->face
            /*!
            The face attached to an edge.
            \param id either 0 or 1.
            \return the face[id] of e.
            */
            CFace * face(int id)
            {
                assert(0 <= id && id < 2);
                assert(m_halfedge[0] != NULL);
                if (id == 0) return m_halfedge[0]->face();
                else return m_halfedge[1]->face();
            }
            /*!
            The first face attaching to an edge.
            \param e the input edge.
            \return the first face attaching to e.
            */
            CFace * face1()
            {
                assert(m_halfedge[0] != NULL);
                return m_halfedge[0]->face();
            }
            /*!
            The second face attaching to an edge.
            \param e the input edge.
            \return the second face attaching to e.
            */
            CFace * face2()
            {
                if (!m_halfedge[1]) return NULL;
                return m_halfedge[1]->face();
            }

            /*!
            whether the edge is on the boundary.
            */
            bool boundary() { return (m_halfedge[0] == NULL) || (m_halfedge[1] == NULL); }
            /*!
            The dual halfedge to the input halfedge
            \param he halfedge attached to the current edge
            \return the other halfedge attached to the current edge
            */
            CHalfEdge * & other(CHalfEdge * he) { return (he != m_halfedge[0]) ? m_halfedge[0] : m_halfedge[1]; }
            /*!
            The string of the current edge.
            */
            std::string & string() { return m_string; }

            bool   & touched() { return m_touched; }
            double & length() { return m_length; }

            /*!
            Read the traits from the string.
            */
            void _from_string() {}
            /*!
            Save the traits to the string.
            */
            void _to_string() {}

        protected:
            /*!
            Pointers to the two halfedges attached to the current edge.
            */
            CHalfEdge      * m_halfedge[2];
            /*!
            The string associated to the current edge.
            */
            std::string      m_string;
            bool             m_touched;
            double           m_length;
        };

        /*!
        \brief CFace base class of all kinds of face classes
        */
        class CFace : public F
        {
        public:
            /*!
            CFace constructor
            */
            CFace() { m_halfedge = NULL; }
            /*!
            CFace destructor
            */
            ~CFace() {}
            /*!
            One of the halfedges attaching to the current face.
            */
            CHalfEdge * & halfedge() { return m_halfedge; }
            /*!
            The reference to the current face id
            */
            int & id() { return m_id; }
            /*!
            The value of the current face id.
            */
            const int id() const { return m_id; }
            /*!
            The string of the current face.
            */
            std::string & string() { return m_string; }
            bool & touched() { return m_touched; }

            /*!
            Convert face traits to the string.
            */
            void _to_string() {}
            /*!
            read face traits from the string.
            */
            void _from_string() {}

            std::vector<CHalfEdge*> & halfedges()
            {
                if (m_halfedges.empty())
                {
                    m_halfedge = this->halfedge();
                    m_halfedges.push_back(m_halfedge);
                    CHalfEdge * he = m_halfedge->next();
                    while (he != m_halfedge)
                    {
                        m_halfedges.push_back(he);
                        he = he->next();
                    }
                }
                return m_halfedges;
            }

            std::vector<CVertex*>   & vertices()
            {
                if (m_vertices.empty())
                {
                    m_halfedges = halfedges();
                    for (CHalfEdge* he : m_halfedges)
                    {
                        m_vertices.push_back(he->vertex());
                    }
                }
                return m_vertices; 
            }

            std::vector<CEdge*>     & edges()
            { 
                if (m_edges.empty())
                {
                    std::vector<CHalfEdge*> & m_halfedges = halfedges();
                    for (CHalfEdge* he : m_halfedges)
                    {
                        m_edges.push_back(he->edge());
                    }
                }
                return m_edges;
            }

        protected:
            /*!
            id of the current face
            */
            int         m_id;
            /*!
            One halfedge  attaching to the current face.
            */
            CHalfEdge * m_halfedge;
            /*!
            String of the current face.
            */
            std::string m_string;
            bool        m_touched;

            std::vector<CEdge*> m_edges;
            std::vector<CHalfEdge*> m_halfedges;
            std::vector<CVertex*>   m_vertices;
        };

        /*!
        \brief CHalfEdge Base class of all kinds of halfedges.
        */
        class CHalfEdge : public H
        {
        public:

            /*! Constructor, initialize all pointers to be NULL.
            */
            CHalfEdge() { m_edge = NULL; m_vertex = NULL; m_prev = NULL; m_next = NULL; m_face = NULL; };
            /*! Destructure.
            */
            ~CHalfEdge() {};

            /*! Pointer to the edge attaching to the current halfedge. */
            CEdge     * & edge() { return m_edge; }
            /*! Target vertex of the current halfedge. */
            CVertex   * & vertex() { return m_vertex; }
            /*! Target vertex of the current halfedge. */
            CVertex   * & target() { return m_vertex; }
            /*! Source vertex of the current halfedge. */
            CVertex   * & source() { return m_prev->vertex(); }
            /*! Previous halfedge of the current halfedge. */
            CHalfEdge * & prev() { return m_prev; }
            /*! Next halfedge of the current halfedge. */
            CHalfEdge * & next() { return m_next; }
            /*! The dual halfedge of the current halfedge. */
            CHalfEdge * & dual() { return m_edge->other(this); }
            /*! The face, to which the current halfedge attach. */
            CFace     * & face() { return m_face; }
            /*! whether a halfedge is on the boundary */
            bool  boundary() { return this->dual() == NULL; }
            /*! Rotate the halfedge about the target vertex ccwly.
            \return if the current halfedge is the most ccw in halfedge of its target vertex, which is on boundary, return NULL.
            */
            CHalfEdge * ccw_rotate_about_target() { return this->dual() ? this->dual()->prev() : NULL; }
            /*! Rotate the halfedge about the target vertex clwly.
            \return if the current halfedge is the most clw in halfedge of its target vertex, which is on boundary, return NULL.
            */
            CHalfEdge * clw_rotate_about_target() { return this->next()->dual(); }
            /*! Rotate the halfedge about the source vertex ccwly.
            \return if the current halfedge is the most ccw out halfedge of its source vertex, which is on boundary, return NULL.
            */
            CHalfEdge * ccw_rotate_about_source() { return this->prev()->dual(); }
            /*! Rotate the halfedge about the source vertex ccwly.
            \return if the current halfedge is the most clw out halfedge of its source vertex, which is on boundary, return NULL.
            */
            CHalfEdge * clw_rotate_about_source() { return this->dual() ? this->dual()->next() : NULL; }

            /*!
            The next Ccw Out HalfEdge
            \return the next Ccw Out HalfEdge, sharing the same source of he.
            */
            CHalfEdge * next_ccw_out_halfedge() { return ccw_rotate_about_source(); }
            /*!The next Clw Out HalfEdge
            \return the next Clw Out HalfEdge, sharing the same source of he.
            */
            CHalfEdge * next_clw_out_halfedge() { return clw_rotate_about_source(); }
            /*!
            The next Ccw In HalfEdge
            \return the next Ccw In HalfEdge, sharing the same target of he.
            */
            CHalfEdge * next_ccw_in_halfedge() { return ccw_rotate_about_target(); }
            /*!
            The next Clw In HalfEdge
            \return the next Clw In HalfEdge, sharing the same target of he.
            */
            CHalfEdge * next_clw_in_halfedge() { return clw_rotate_about_target(); }

            /*! String of the current halfedge. */
            std::string & string() { return m_string; }

            bool   & touched() { return m_touched; }
            double & length() { return m_length; }

            /*! Convert vertex traits to string.
            */
            void _to_string() {};
            /*! Read traits from the string.
            */
            void _from_string() {};

            CPoint2 & uv() { return m_uv; }

            CPoint & normal() { return m_normal; }

        protected:
            /*! Edge, current halfedge attached to. */
            CEdge       *     m_edge;
            /*! Face, current halfedge attached to. */
            CFace       *     m_face;
            /*! Target vertex of the current halfedge. */
            CVertex     *     m_vertex;
            /*! Previous halfedge of the current halfedge, in the same face. */
            CHalfEdge   *     m_prev;
            /*! Next halfedge of the current halfedge, in the same face. */
            CHalfEdge   *     m_next;
            /*! The string of the current halfedge. */
            std::string       m_string;
            bool              m_touched;
            double            m_length;
            /*! Texture coordinate on target. */
            CPoint2           m_uv;
            /*! normal */
            CPoint            m_normal;
        };

        //constructor and destructor
        /*!
        CBaseMesh constructor.
        */
        CBaseMesh() {};
        /*!
        CBaseMesh destructor
        */
        ~CBaseMesh();

        //file io
        /*!
        Read an .obj file.
        \param filename the input .obj file name
        */
        void read_obj(const char * filename);
        void read_obj(std::string filename) { read_obj(filename.c_str()); }
        /*!
        Write an .obj file.
        \param filename the output .obj file name
        */
        void write_obj(const char * filename);
        void write_obj(std::string filename) { write_obj(filename.c_str()); }
        /*!
        Read an .m file.
        \param filename the input obj file name
        */
        void read_m(const std::string & filename, const std::set<std::string> & traits = {});
        void read_m(const char * filename, const std::set<std::string> & traits = {}) { read_m(std::string(filename), traits); }
        /*!
        Write an .m file.
        \param filename the output .m file name
        */
        void write_m(const std::string & filename, const std::set<std::string> & traits = {});
        void write_m(const char * filename, const std::set<std::string> & traits = {}) { write_m(std::string(filename), traits); }
        /*!
        Read an .off file
        \param filename the input .off filename
        */
        void read_off(const char * filename);
        void read_off(std::string filename) { read_off(filename.c_str()); }
        /*!
        Write an .off file.
        \param filename the output .off file name
        */
        void write_off(const char * filename);
        void write_off(std::string filename) { write_off(filename.c_str()); }

        //number of vertices, faces, edges
        /*! number of vertices */
        int  num_vertices() { return m_verts.size(); }
        /*! number of edges */
        int  num_edges() { return m_edges.size(); }
        /*! number of faces */
        int  num_faces() { return m_faces.size(); }


        //acess vertex - id
        /*!
        Access a vertex by its id
        \param id the vertex id
        \return the vertex, whose ID equals to id. NULL, if there is no such a vertex.
        */
        CVertex * vertex(int id) { return m_map_vert[id]; }

        //access face - id
        /*!
        Access a face by its id
        \param id the face id
        \return the face, whose ID equals to id. NULL, if there is no such a face.
        */
        CFace *  face(int id) { return m_map_face[id]; }

        //access edge - edge key, vertex
        /*!
        Access an edge by its two end vertices
        \param v0 one vertex of the edge
        \param v1 the other vertex of the edge
        \return the edge connecting both v0 and v1, NULL if no such edge exists.
        */
        CEdge * edge(CVertex * v0, CVertex * v1)
        {
            CVertex * pV = (v0->id() < v1->id()) ? v0 : v1;
            for (CEdge * pE : pV->ledges())
            {
                CHalfEdge * pH = pE->halfedge(0);
                if (pH->source() == v0 && pH->target() == v1) return pE;
                if (pH->source() == v1 && pH->target() == v0) return pE;
            }
            return NULL;
        }

        //access halfedge - halfedge key, vertex
        /*!
        Access a halfedge by its two end vertices
        \param v0 one vertex of the halfedge
        \param v1 the other vertex of the halfedge
        \return the halfedge connecting both v0 and v1, NULL if no such edge exists.
        */
        CHalfEdge * halfedge(CVertex * v0, CVertex * v1)
        {
            CEdge * e = edge(v0, v1);
            assert(e);
            CHalfEdge * he = e->halfedge(0);
            if (he->vertex() == v1 && he->prev()->vertex() == v0) return he;
            he = e->halfedge(1);
            assert(he->vertex() == v1 && he->prev()->vertex() == v0);
            return he;
        }

        /*!
        Access a halfedge by its target vertex, and attaching face.
        \param v target vertex
        \param f attaching face
        \return halfedge, whose target is v, attaching face is f. NULL if no such an halfedge exists.
        */
        CHalfEdge * corner(CVertex * v, CFace * f)
        {
            CHalfEdge * he = f->halfedge();
            do {
                if (he->vertex() == v) return he;
                he = he->next();
            } while (he != f->halfedge());
            return NULL;
        }

        /*!
        List of the edges of the mesh.
        */
        std::list<CEdge*>     & edges() { return m_edges; }
        /*!
        List of the halfedges of the mesh.
        */
        std::list<CHalfEdge*> & halfedges() { return m_halfedges; }
        /*!
        List of the faces of the mesh.
        */
        std::list<CFace*>     & faces() { return m_faces; }
        /*!
        List of the vertices of the mesh.
        */
        std::list<CVertex*>   & vertices() { return m_verts; }

    protected:
        /*! list of edges */
        std::list<CEdge*>       m_edges;
        /*! list of edges */
        std::list<CHalfEdge*>   m_halfedges;
        /*! list of vertices */
        std::list<CVertex*>     m_verts;
        /*! list of faces */
        std::list<CFace*>       m_faces;

        //maps
        /*! map between vetex and its id*/
        std::map<int, CVertex*> m_map_vert;
        /*! map between face and its id*/
        std::map<int, CFace*>   m_map_face;

    public:
        /*! Create a vertex
        \param id Vertex id
        \return pointer to the new vertex
        */
        CVertex * create_vertex(int id = 0);
        /*! Create an edge
        \param v1 end vertex of the edge
        \param v2 end vertex of the edge
        \return pointer to the new edge
        */
        CEdge *   create_edge(CVertex * v1, CVertex * v2);
        /*! Create a face
        \param v an array of vertices
        \param id face id
        \return pointer to the new face
        */
        CFace *   create_face(std::vector<CVertex*> & v, int id); //create a triangle

        /*! delete one face
        \param pFace the face to be deleted
        */
        void      delete_face(CFace * pFace);

        /*! label boundary vertices, edges, faces */
        void      label_boundary();

    public:
        /*!
         *   the input traits of the mesh, there are 64 bits in total
         */
        static unsigned long long m_input_traits;
        /*!
         *   the output triats of the mesh, there are 64 bits in total
         */
        static unsigned long long m_output_traits;

    };


    /*!
     CBaseMesh destructor
     */
    template<typename V, typename E, typename F, typename H>
    inline CBaseMesh<V, E, F, H>::~CBaseMesh()
    {
        // remove vertices
        for (CVertex * v : m_verts) delete v;
        m_verts.clear();

        // remove faces
        for (CFace * f : m_faces) delete f;
        m_faces.clear();

        // remove edges
        for (CEdge * e : m_edges) delete e;
        m_edges.clear();

        // remove halfedges
        for (CHalfEdge * he : m_halfedges) delete he;
        m_halfedges.clear();

        // clear all the maps
        m_map_vert.clear();
        m_map_face.clear();
    };


    //create new gemetric simplexes
    /*! Create a vertex
    \param id Vertex id
    \return pointer to the new vertex
    */
    template<typename V, typename E, typename F, typename H>
    inline typename CBaseMesh<V, E, F, H>::CVertex * CBaseMesh<V, E, F, H>::create_vertex(int id)
    {
        CVertex * v = new CVertex();
        assert(v != NULL);
        v->id() = id;
        m_verts.push_back(v);
        m_map_vert.insert(std::pair<int, CVertex*>(id, v));
        return v;
    };

    /*! Create a face
    \param v an array of vertices
    \param id face id
    \return pointer to the new face
    */
    template<typename V, typename E, typename F, typename H>
    inline typename CBaseMesh<V, E, F, H>::CFace * CBaseMesh<V, E, F, H>::create_face(std::vector<CVertex*> & vs, int id)
    {
        CFace * f = new CFace();
        assert(f != NULL);
        f->id() = id;
        m_faces.push_back(f);
        m_map_face.insert(std::pair<int, CFace*>(id, f));

        //create halfedges
        std::vector<CHalfEdge*> hes;
        for (CVertex * v : vs)
        {
            CHalfEdge * he = new CHalfEdge();
            assert(he);
            he->vertex() = v;
            he->uv() = v->uv();
            he->normal() = v->normal();
            v->halfedge() = he;
            hes.push_back(he);
            m_halfedges.push_back(he);
        }

        //linking to each other
        for (size_t i = 0; i < hes.size(); i++)
        {
            hes[i]->next() = hes[(i + 1) % hes.size()];
            hes[i]->prev() = hes[(i + hes.size() - 1) % hes.size()];
        }

        //linking to face
        for (size_t i = 0; i < hes.size(); i++)
        {
            hes[i]->face() = f;
            f->halfedge() = hes[i];
        }

        //connecting with edge
        for (size_t i = 0; i < hes.size(); i++)
        {
            CEdge * e = create_edge(vs[i], vs[(i + hes.size() - 1) % hes.size()]);
            if (e->halfedge(0) == NULL)
            {
                e->halfedge(0) = hes[i];
            }
            else
            {
                assert(e->halfedge(1) == NULL);
                if (e->halfedge(1) != NULL)
                {
                    std::cout << "Illegal Face Construction " << id << std::endl;
                }
                e->halfedge(1) = hes[i];
            }
            hes[i]->edge() = e;
        }

        return f;
    };

    /*! Create an edge
    \param v1 end vertex of the edge
    \param v2 end vertex of the edge
    \return pointer to the new edge
    */
    template<typename V, typename E, typename F, typename H>
    inline typename CBaseMesh<V, E, F, H>::CEdge * CBaseMesh<V, E, F, H>::create_edge(CVertex * v1, CVertex * v2)
    {
        CVertex * pV = (v1->id() < v2->id()) ? v1 : v2;
        std::list<CEdge*> & ledges = pV->ledges();

        for (CEdge * pE : ledges)
        {
            CHalfEdge * pH = pE->halfedge(0);
            if (pH->source() == v1 && pH->target() == v2)
            {
                return pE;
            }
            if (pH->source() == v2 && pH->target() == v1)
            {
                return pE;
            }
        }

        //new edge
        CEdge * e = new CEdge();
        assert(e != NULL);
        m_edges.push_back(e);
        ledges.push_back(e);

        return e;
    };

    /*!
    Read an .obj file.
    \param filename the filename .obj file name
    */
    template<typename V, typename E, typename F, typename H>
    inline void CBaseMesh<V, E, F, H>::read_obj(const char * filename)
    {

        std::fstream f(filename, std::fstream::in);
        if (f.fail()) return;

        char cmd[1024];

        int  vid = 1;
        int  fid = 1;

        bool with_uv = false;
        bool with_normal = false;

        std::vector<CPoint2> uvs;
        std::vector<CPoint> normals;


        while (f.getline(cmd, 1024))
        {
            std::string line(cmd);
            line = strutil::trim(line);

            strutil::Tokenizer stokenizer(line, " \t\r\n");

            stokenizer.nextToken();
            std::string token = stokenizer.getToken();

            if (token == "v")
            {
                CPoint p;
                for (int i = 0; i < 3; i++)
                {
                    stokenizer.nextToken();
                    token = stokenizer.getToken();
                    p[i] = strutil::parseString<float>(token);
                }

                CVertex * v = create_vertex(vid);
                v->point() = p;
                vid++;
                continue;
            }


            if (token == "vt")
            {
                with_uv = true;
                CPoint2 uv;
                for (int i = 0; i < 2; i++)
                {
                    stokenizer.nextToken();
                    token = stokenizer.getToken();
                    uv[i] = strutil::parseString<float>(token);
                }
                uvs.push_back(uv);
                continue;
            }

            if (token == "vn")
            {
                with_normal = true;

                CPoint n;
                for (int i = 0; i < 3; i++)
                {
                    stokenizer.nextToken();
                    token = stokenizer.getToken();
                    n[i] = strutil::parseString<float>(token);
                }
                normals.push_back(n);
                continue;
            }

            if (token == "f")
            {
                std::vector<CVertex*> vs;
                for (int i = 0; i < 3; i++)
                {
                    stokenizer.nextToken();
                    token = stokenizer.getToken();
                    strutil::Tokenizer tokenizer(token, " /\t\r\n");

                    int ids[3];
                    int k = 0;
                    while (tokenizer.nextToken())
                    {
                        std::string token = tokenizer.getToken();
                        ids[k] = strutil::parseString<int>(token);
                        k++;
                    }

                    CVertex * vi = m_map_vert[ids[0]];
                    if (with_uv)
                        vi->uv() = uvs[ids[1] - 1];
                    if (with_normal)
                        vi->normal() = normals[ids[2] - 1];
                    vs.push_back(vi);
                }
                create_face(vs, fid++);
            }
        }

        f.close();

        label_boundary();
    }

    /*!
        Read an .m file.
        \param input the input obj file name
    */
    template<typename V, typename E, typename F, typename H>
    inline void CBaseMesh<V, E, F, H>::read_m(const std::string & input, const std::set<std::string> & traits)
    {
        std::fstream is(input, std::fstream::in);

        if (is.fail())
        {
            std::cerr << "error in opening file " << input << std::endl;
            return;
        }

        char buffer[MAX_LINE];
        int id;

        while (is.getline(buffer, MAX_LINE))
        {

            std::string line(buffer);
            line = strutil::trim(line);

            strutil::Tokenizer stokenizer(line, " \r\n");

            stokenizer.nextToken();
            std::string token = stokenizer.getToken();

            if (token == "Vertex")
            {
                stokenizer.nextToken();
                token = stokenizer.getToken();
                id = strutil::parseString<int>(token);

                CPoint p;
                for (int i = 0; i < 3; i++)
                {
                    stokenizer.nextToken();
                    token = stokenizer.getToken();
                    p[i] = strutil::parseString<float>(token);
                }

                CVertex * v = create_vertex(id);
                v->point() = p;

                if (!stokenizer.nextToken("\t\r\n")) continue;
                token = stokenizer.getToken();

                int sp = (int)token.find("{");
                int ep = (int)token.find("}");

                if (sp >= 0 && ep >= 0)
                {
                    v->string() = token.substr(sp + 1, ep - sp - 1);
                }
                continue;
            }


            if (token == "Face")
            {

                stokenizer.nextToken();
                token = stokenizer.getToken();
                id = strutil::parseString<int>(token);

                std::vector<CVertex*> vs;
                while (stokenizer.nextToken())
                {
                    token = stokenizer.getToken();
                    if (strutil::startsWith(token, "{")) break;
                    int vid = strutil::parseString<int>(token);
                    vs.push_back(this->vertex(vid));
                }

                CFace * f = create_face(vs, id);

                if (strutil::startsWith(token, "{"))
                {
                    f->string() = strutil::trim(token, "{}");
                }
                continue;
            }

            //read in edge attributes
            if (token == "Edge")
            {
                stokenizer.nextToken();
                token = stokenizer.getToken();
                int id0 = strutil::parseString<int>(token);

                stokenizer.nextToken();
                token = stokenizer.getToken();
                int id1 = strutil::parseString<int>(token);

                CVertex * v0 = this->vertex(id0);
                CVertex * v1 = this->vertex(id1);

                CEdge * edge = this->edge(v0, v1);

                if (!stokenizer.nextToken("\t\r\n")) continue;
                token = stokenizer.getToken();

                int sp = (int)token.find("{");
                int ep = (int)token.find("}");

                if (sp >= 0 && ep >= 0)
                {
                    edge->string() = token.substr(sp + 1, ep - sp - 1);
                }
                continue;
            }

            //read in edge attributes
            if (token == "Corner")
            {
                stokenizer.nextToken();
                token = stokenizer.getToken();
                int vid = strutil::parseString<int>(token);

                stokenizer.nextToken();
                token = stokenizer.getToken();
                int fid = strutil::parseString<int>(token);


                CVertex * v = this->vertex(vid);
                CFace   * f = this->face(fid);
                CHalfEdge * he = this->corner(v, f);


                if (!stokenizer.nextToken("\t\r\n")) continue;
                token = stokenizer.getToken();

                int sp = (int)token.find("{");
                int ep = (int)token.find("}");

                if (sp >= 0 && ep >= 0)
                {
                    he->string() = token.substr(sp + 1, ep - sp - 1);
                }
                continue;
            }
        }

        //labelBoundary();

        //Label boundary edges
        for (CEdge * edge : m_edges)
        {
            CHalfEdge * he[2];
            he[0] = edge->halfedge(0);
            he[1] = edge->halfedge(1);
            assert(he[0] != NULL);

            if (he[1] != NULL)
            {
                assert(he[0]->target() == he[1]->source() && he[0]->source() == he[1]->target());

                if (he[0]->target()->id() < he[0]->source()->id())
                {
                    edge->halfedge(0) = he[1];
                    edge->halfedge(1) = he[0];
                }

                assert(edge->vertex1()->id() < edge->vertex2()->id());
            }
            else
            {
                he[0]->vertex()->boundary() = true;
                he[0]->prev()->vertex()->boundary() = true;
            }

        }

        std::list<CVertex*> dangling_verts;
        //Label boundary edges
        for (CVertex * v : m_verts)
        {
            if (v->halfedge() != NULL) continue;
            dangling_verts.push_back(v);
        }

        for (CVertex * v : dangling_verts)
        {
            m_verts.remove(v);
            delete v;
            v = NULL;
        }

        //Arrange the boundary half_edge of boundary vertices, to make its halfedge
        //to be the most ccw in half_edge
        for (CVertex * v : m_verts)
        {
            if (!v->boundary()) continue;

            CHalfEdge * he = v->most_ccw_in_halfedge();
            while (he->dual() != NULL)
            {
                he = he->next_ccw_in_halfedge();
            }
            v->halfedge() = he;
        }

        //read in the traits
        for (CVertex * v : m_verts) v->_from_string();
        for (CEdge * e : m_edges) e->_from_string();
        for (CFace * f : m_faces) f->_from_string();
        for (CHalfEdge * he : m_halfedges) he->_from_string();

    };

    /*!
        Write an .m file.
        \param output the output .m file name
    */
    template<typename V, typename E, typename F, typename H>
    inline void CBaseMesh<V, E, F, H>::write_m(const std::string & output, const std::set<std::string> & traits)
    {
        // write traits to string
        for (CVertex * v : m_verts) v->_to_string();
        for (CEdge * e : m_edges) e->_to_string();
        for (CFace * f : m_faces) f->_to_string();
        for (CHalfEdge * he : m_halfedges) he->_to_string();

        std::fstream _os(output, std::fstream::out);
        if (_os.fail())
        {
            std::cerr << "error in opening file " << output << std::endl;
            return;
        }

        // write vertices
        for (CVertex * v : m_verts)
        {
            _os << "Vertex " << v->id();
            _os << " " << v->point();

            if (v->string().size() > 0)
            {
                _os << " " << "{" << v->string() << "}";
            }
            _os << std::endl;
        }

        for (CFace * f : m_faces)
        {
            _os << "Face " << f->id();
            for (CVertex * v : f->vertices())
            {
                _os << " " << v->id();
            }			

            if (f->string().size() > 0)
            {
                _os << " " << "{" << f->string() << "}";
            }
            _os << std::endl;
        }

        for (CEdge * e : m_edges)
        {
            if (e->string().size() > 0)
            {
                _os << "Edge " << e->vertex(0)->id() << " " << e->vertex(1)->id() << " ";
                _os << "{" << e->string() << "}" << std::endl;
            }
        }

        for (CHalfEdge * he : m_halfedges)
        {
            if (he->string().size() > 0)
            {
                _os << "Corner " << he->vertex()->id() << " " << he->face()->id() << " ";
                _os << "{" << he->string() << "}" << std::endl;
            }
        }

        _os.close();
    };

    //assume the mesh is with uv coordinates and normal vector for each vertex
    /*!
        Write an .obj file.
        \param output the output .obj file name
    */
    template<typename V, typename E, typename F, typename H>
    inline void CBaseMesh<V, E, F, H>::write_obj(const char * output)
    {
        std::fstream _os(output, std::fstream::out);
        if (_os.fail())
        {
            std::cerr << "error in opening file " << output << std::endl;
            return;
        }

        int vid = 0;
        for (CVertex * v : m_verts) v->id() = ++vid;
        for (CVertex * v : m_verts)
        {
            _os << "v " << v->point() << std::endl;
        }
        for (CVertex * v : m_verts)
        {
            _os << "vt " << v->uv() << std::endl;
        }
        for (CVertex * v : m_verts)
        {
            _os << "vn " << v->normal() << std::endl;
        }

        for (CFace * f : m_faces)
        {
            _os << "f";
            CHalfEdge *he = f->halfedge();

            do {
                int vid = he->target()->id();
                _os << " " << vid << "/" << vid << "/" << vid;
                he = he->next();
            } while (he != f->halfedge());
            _os << std::endl;
        }

        _os.close();
    };

    /*!
        Write an .off file.
        \param output the output .off file name
    */
    template<typename V, typename E, typename F, typename H>
    inline void CBaseMesh<V, E, F, H>::write_off(const char * output)
    {
        std::fstream _os(output, std::fstream::out);
        if (_os.fail())
        {
            std::cerr << "error in opening file " << output << std::endl;
            return;
        }

        _os << "OFF" << std::endl;
        _os << m_verts.size() << " " << m_faces.size() << " " << m_edges.size() << std::endl;

        int vid = 0;
        for (CVertex * v : m_verts) v->id() = vid++;
        for (CVertex * v : m_verts) _os << v->point() << std::endl;

        for (CFace * f : m_faces)
        {
            _os << "3";
            CHalfEdge * he = f->halfedge();
            do {
                int vid = he->target()->id();
                _os << " " << vid;
                he = he->next();
            } while (he != f->halfedge());
            _os << std::endl;
        }

        _os.close();
    };

    //template pointer converting to base class pointer is OK (BasePointer) = (TemplatePointer)
    //(TemplatePointer)=(BasePointer) is incorrect
    /*! delete one face
    \param pFace the face to be deleted
    */
    template<typename V, typename E, typename F, typename H>
    inline void CBaseMesh<V, E, F, H>::delete_face(CFace * pFace)
    {
        typename std::map<int, CFace*>::iterator fiter = m_map_face.find(pFace->id());
        if (fiter != m_map_face.end())
        {
            m_map_face.erase(fiter);
        }
        m_faces.remove(pFace);

        //create halfedges
        CHalfEdge * hes[3];

        hes[0] = pFace->halfedge();
        hes[1] = hes[0]->next();
        hes[2] = hes[1]->next();

        for (int i = 0; i < 3; i++)
        {
            //connection with edge
            CHalfEdge * pH = hes[i];

            //connection to target
            CVertex * pV = pH->target();
            if (pV->halfedge() == pH)
            {
                if (pH->next()->dual() != NULL)
                    pV->halfedge() = pH->next()->dual();
                else
                {
                    assert(pH->dual() != NULL); //otherwise the mesh is not a manifold
                    pV->halfedge() = pH->next()->dual();
                }
            }
        }

        for (int i = 0; i < 3; i++)
        {
            //connection with edge
            CHalfEdge * pH = hes[i];
            CHalfEdge * pS = pH->dual();
            CEdge * pE = pH->edge();

            pE->halfedge(0) = pS;
            pE->halfedge(1) = NULL;

            if (pS == NULL)
            {
                m_edges.remove(pE);
                CVertex * v0 = pH->source();
                CVertex * v1 = pH->target();
                delete pE;
            }
        }

        //remove half edges
        for (int i = 0; i < 3; i++)
        {
            delete hes[i];
        }

        delete pFace;
    };

    /*!
        Read an .off file
        \param input the input .off filename
    */
    template<typename V, typename E, typename F, typename H>
    inline void CBaseMesh<V, E, F, H>::read_off(const char * input)
    {
        std::fstream is(input, std::fstream::in);

        if (is.fail())
        {
            std::cerr << "error in opening file " << input << std::endl;
            return;
        }

        char buffer[MAX_LINE];

        //read in the first line "OFF"

        while (is.getline(buffer, MAX_LINE))
        {
            std::string line(buffer);

            strutil::Tokenizer stokenizer(line, " \r\n");

            stokenizer.nextToken();
            std::string token = stokenizer.getToken();
            if (token == "OFF" || token == "COFF" || token == "NOFF") break;
        }

        int nVertices, nFaces, nEdges;

        //read in Vertex Number, Face Number, Edge Number

        is.getline(buffer, MAX_LINE);
        std::string line(buffer);

        strutil::Tokenizer stokenizer(line, " \r\n");

        stokenizer.nextToken();
        std::string token = stokenizer.getToken();
        nVertices = strutil::parseString<int>(token);

        stokenizer.nextToken();
        token = stokenizer.getToken();
        nFaces = strutil::parseString<int>(token);

        stokenizer.nextToken();
        token = stokenizer.getToken();
        nEdges = strutil::parseString<int>(token);

        for (int id = 0; id < nVertices; id++)
        {
            is.getline(buffer, MAX_LINE);
            std::string line(buffer);

            strutil::Tokenizer stokenizer(line, " \r\n");
            CPoint p;
            for (int j = 0; j < 3; j++)
            {
                stokenizer.nextToken();
                std::string token = stokenizer.getToken();
                p[j] = strutil::parseString<float>(token);
            }

            CVertex * v = create_vertex(id + 1);
            v->point() = p;
        }

        for (int id = 0; id < nFaces; id++)
        {
            is.getline(buffer, MAX_LINE);
            std::string line(buffer);

            strutil::Tokenizer stokenizer(line, " \r\n");
            stokenizer.nextToken();
            std::string token = stokenizer.getToken();

            int n = strutil::parseString<int>(token);
            assert(n == 3);

            std::vector<CVertex*> vs;
            for (int j = 0; j < 3; j++)
            {
                stokenizer.nextToken();
                std::string token = stokenizer.getToken();
                int vid = strutil::parseString<int>(token);
                vs.push_back(this->vertex(vid + 1));
            }
            create_face(vs, id + 1);
        }

        is.close();

        label_boundary();

    };

    /*!
        Label boundary edges, vertices
    */
    template<typename V, typename E, typename F, typename H>
    inline void CBaseMesh<V, E, F, H>::label_boundary(void)
    {
        //Label boundary edges
        for (CEdge * e : m_edges)
        {
            CHalfEdge * he[2];
            he[0] = e->halfedge(0);
            he[1] = e->halfedge(1);
            assert(he[0] != NULL);

            if (he[1] != NULL)
            {
                assert(he[0]->target() == he[1]->source() && he[0]->source() == he[1]->target());
                if (he[0]->target()->id() < he[0]->source()->id())
                {
                    e->halfedge(0) = he[1];
                    e->halfedge(1) = he[0];
                }
                assert(e->vertex1()->id() < e->vertex2()->id());
            }
            else
            {
                he[0]->vertex()->boundary() = true;
                he[0]->prev()->vertex()->boundary() = true;
            }
        }

        std::list<CVertex*> dangling_verts;
        //Label boundary edges
        for (CVertex * v : m_verts)
        {
            if (v->halfedge() != NULL) continue;
            dangling_verts.push_back(v);
        }

        for (CVertex * v : dangling_verts)
        {
            m_verts.remove(v);
            delete v;
            v = NULL;
        }

        //Arrange the boundary half_edge of boundary vertices, to make its halfedge
        //to be the most ccw in half_edge
        for (CVertex * v : m_verts)
        {
            if (!v->boundary()) continue;

            CHalfEdge * he = v->halfedge();
            while (he->dual() != NULL)
            {
                he = he->ccw_rotate_about_target();
            }
            v->halfedge() = he;
        }

    };


}//name space MeshLib

#endif //_MESHLIB_BASE_MESH_H_ defined
