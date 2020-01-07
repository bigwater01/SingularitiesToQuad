#include "SingularitiesToQuad.h"

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        return -1;
    }

    string mesh_filename = string(argv[1]);
    string cut_filename = string(argv[2]);
    string output_filename;
    if (argc == 4)
        output_filename = string(argv[3]);
    else
    {
        auto pos = mesh_filename.find(".m");
        if (pos == mesh_filename.length())
            output_filename = mesh_filename + ".output.m";
        else
            output_filename = mesh_filename.substr(0, pos) + ".output.m";
    }

    SingularitiesToQuad* map = new SingularitiesToQuad();
    map->readMesh(mesh_filename);
    map->readCuts(cut_filename);
    map->traceAllComponents();
    map->writeMesh(output_filename);

    return 0;
}