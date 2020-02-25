#include "TgaImage.h"
#include "Model.h"
#include <iostream>

const TgaColor white = TgaColor(255, 255, 255, 255);

void line(int x0, int y0, int x1, int y1, TgaImage &image, const TgaColor &color)
{
    int dx = x1 - x0;
    int dy = y1 - y0;
    float m = (float) dy / (float) dx;
    float b = y1 - (m * x1);

    int minX = std::min(x0, x1);
    int maxX = std::max(x0, x1);

    if (minX == maxX)
    {
        int minY = std::min(y0, y1);
        int maxY = std::max(y0, y1);
        for (float y = minY; y <= maxY; y += 0.001)
        {
            image.set(minX, y, color);
        }
    }
    else
    {
        for (float x = minX; x <= maxX; x += 0.001)
        {
            int y = (m * x) + b;
            image.set(x, y, color);
        }
    }
}

int main(int argc, char** argv)
{
    auto *model = new Model("head.obj");
    const int width = 800;
    const int height = 800;

    TgaImage image(width, height, TgaImage::RGB);
    for (int i = 0; i < model->nFaces(); i++)
    {
        std::vector<int> face = model->face(i);
        for (int j = 0; j < 3; j++)
        {
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j + 1) % 3]); // use the modulo to go back to 0 and connect the triangle
            int x0 = (v0.x + 1) * width / 2;
            int y0 = (v0.y + 1) * height / 2;
            int x1 = (v1.x + 1) * width / 2;
            int y1 = (v1.y + 1) * height / 2;
            line(x0, y0, x1, y1, image, white);
        }
    }

    image.flipVertically(); // i want to have the origin at the left bottom corner of the image
    image.writeTgaFile("output.tga");
    delete model;
    return 0;
}