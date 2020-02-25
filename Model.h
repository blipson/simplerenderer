
#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "Geometry.h"

class Model
{
  private:
    std::vector<Vec3f> verts;
    std::vector<std::vector<int> > faces;
  public:
    Model(const char *filename);

    ~Model();

    int nVerts();

    int nFaces();

    Vec3f vert(int i);

    std::vector<int> face(int i);
};

#endif //__MODEL_H__
