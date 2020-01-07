#include "SingularitiesToQuad.h"

#include <fstream>
#include <queue>


SingularitiesToQuad::SingularitiesToQuad()
{
    mesh = new CMesh();
}


SingularitiesToQuad::~SingularitiesToQuad()
{
}

int SingularitiesToQuad::readMesh(const string& filename)
{
    mesh->read_m(filename);
    return 0;
}

int SingularitiesToQuad::readCuts(const string& filename)
{
    if (!mesh)
    {
        cerr << "read mesh first" << endl;
        exit(-1);
    }

    ifstream file(filename);
    if (file.good())
    {
        string line;
        while (getline(file, line))
        {
            std::istringstream iss(line);
            string type;
            int cid, x, y;
            double len;
            iss >> type;
            if (type.length() == 0 || type[0] == '#') continue;
            if (type == "Cut")
            {
                iss >> cid;
                vector<CVertex*> cut;
                int vid = -1;
                while (iss >> vid)
                {
                    cut.push_back(mesh->vertex(vid));
                }
                if (cut.empty())
                {
                    cerr << "cut can not be empty" << endl;
                    exit(-1);
                }
                cuts[cid] = cut;
            }
            if (type == "Seed")
            {
                int sid, seed;
                iss >> sid >> seed;
                seeds[sid] = mesh->vertex(seed);
            }
        }
    }
    else
    {
        cout << "can't open graph file: " << filename << endl;
        exit(-1);
    }

    for (CVertex* v : mesh->vertices())
    {
        v->fixed() = false;
        v->cut() = false;
        v->touched() = false;
    }
    for (CEdge* e : mesh->edges())
    {
        e->touched() = false;
        e->cut() = false;
    }

    for (auto c : cuts)
    {
        for (size_t i = 0; i < c.second.size(); ++i)
        {
            CVertex* source = c.second[i];
            CVertex* target = (i == c.second.size() - 1) ? c.second[0] : c.second[i+1];
            source->cut() = true;
            for (CHalfEdge* vhe : source->halfedges())
            {
                if (vhe->target()->id() == target->id())
                    vhe->edge()->cut() = true;
            }
        }
    }
}

int SingularitiesToQuad::traceAllComponents()
{   
    for (CVertex* v : mesh->vertices())
    {
        v->component_id() = -1;
    }
    for (auto s : seeds)
    {
        int id = s.first;
        CVertex* seed = s.second;
        vector<CVertex*> component;
        traceComponent(id, seed, component);
        components[id] = component;
    }
    for (CVertex* v : mesh->vertices())
    {
        if (v->component_id() == -1 && !v->cut())
        {
            cout << "Untouched vertex " << v->id() << endl;
        }
        assert(v->component_id() != -1 || v->cut());
    }
    for (auto c : cuts)
    {
        for (CVertex* vi : c.second)
        {
            vi->component_id() = -1;
        }
    }

    for (CVertex* v : mesh->vertices())
    {
        ostringstream oss;
        oss << "vcid=(" << v->component_id() << ")";
        v->string() = oss.str();
    }


    for (CFace* f : mesh->faces())
    {
        f->touched() = false;
        f->component_id() = -1;
    }

    for (auto comp : components)
    {
        for (CVertex* vi : comp.second)
        {
            if (vi->component_id() == -1)
                continue;

            for (CFace* fi : vi->faces())
            {
                ostringstream oss;
                fi->component_id() = vi->component_id();
                oss << "fcid=(" << fi->component_id() << ")";
                fi->string() = oss.str();
                fi->touched() = true;
            }
        }
    }

    for (CFace* f : mesh->faces())
    {
        if (f->touched())
            continue;

        for (CHalfEdge* he : f->halfedges())
        {
            if (he->edge()->cut())
                continue;
            he->face()->component_id() = he->dual()->face()->component_id();

            ostringstream oss;
            oss << "fcid=(" << he->face()->component_id() << ")";
            he->face()->string() = oss.str();
            he->face()->touched() = true;
        }
    }

    return 0;
}

int SingularitiesToQuad::traceComponent(int id, CVertex* seed, vector<CVertex*>& component)
{
    for (CVertex* v : mesh->vertices()) v->touched() = false;

    queue<CVertex*> qe;
    qe.push(seed);
    component.clear();
    while (!qe.empty())
    {
        CVertex* v = qe.front();
        qe.pop();
        component.push_back(v);
        v->touched() = true;
        v->component_id() = id;
        int vid = v->id();
        bool isCut = v->cut();
        for (CVertex* vj : v->vertices())
        {
            if (vj->touched()) continue;
            bool isCut = vj->cut();
            if (!isCut) qe.push(vj);
            else component.push_back(vj);
            vj->touched() = true;
        }
    }
    return 0;
}

int SingularitiesToQuad::writeMesh(string filename)
{
    mesh->write_m(filename);
    return 0;
}
