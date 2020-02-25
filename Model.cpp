#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "Model.h"

Model::Model(const char *filename) : verts(), faces()
{
    // open the file and read the lines
    std::ifstream in;
    in.open(filename, std::ifstream::in);
    if (in.fail())
    {
        return;
    }
    std::string line;
    while (!in.eof())
    {
        std::getline(in, line);
        std::istringstream iss(line);
        char trash;
        // check if it's a vertex (v, vt, or vn), or a face (f)
        if (!line.compare(0, 2, "v "))
        {
            iss >> trash;
            Vec3f v;
            for (float &i : v.raw)
            {
                iss >> i;
            }
            verts.push_back(v); // if it's a vertex then read in the raw geomtry data and add it to our vertex list
        }
        else if (!line.compare(0, 2, "f "))
        {
            std::vector<int> f;
            int itrash, idx;
            iss >> trash;
            while (iss >> idx >> trash >> itrash >> trash >> itrash)
            {
                idx--; // in wavefront obj all indices start at 1, not zero
                f.push_back(idx);
            }
            faces.push_back(f); // if it's a face then read in the raw face data, ignoring the slashes between values, and add it to our faces list
        }
    }
    std::cerr << "# v# " << verts.size() << " f# " << faces.size() << std::endl;
}

Model::~Model()
= default;

int Model::nVerts()
{
    return (int) verts.size();
}

int Model::nFaces()
{
    return (int) faces.size();
}

std::vector<int> Model::face(int i)
{
    return faces[i];
}

Vec3f Model::vert(int i)
{
    return verts[i];
}
