#include <vector>
#include <cmath>
#include "tgaimage.h"
#include "model.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
Model *model = nullptr;
const int width = 800;
const int height = 800;

void line(Vec2i p0, Vec2i p1, TGAImage &image, TGAColor color)
{
    bool steep = false;
    if (std::abs(p0.x - p1.x) < std::abs(p0.y - p1.y))
    {
        std::swap(p0.x, p0.y);
        std::swap(p1.x, p1.y);
        steep = true;
    }
    if (p0.x > p1.x)
    {
        std::swap(p0.x, p1.x);
        std::swap(p0.y, p1.y);
    }
    int dx = p1.x - p0.x;
    int dy = p1.y - p0.y;
    int derror2 = std::abs(dy) * 2;
    int error2 = 0;
    int y = p0.y;
    for (int x = p0.x; x <= p1.x; x++)
    {
        if (steep)
        {
            image.set(y, x, color);
        } else
        {
            image.set(x, y, color);
        }
        error2 += derror2;
        if (error2 > dx)
        {
            y += (p1.y > p0.y ? 1 : -1);
            error2 -= dx * 2;
        }
    }
}

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color)
{
    if (t0.y == t1.y && t0.y == t2.y)
    {
        return;
    }
    if (t0.y > t1.y)
    {
        std::swap(t0, t1);
    }
    if (t0.y > t2.y)
    {
        std::swap(t0, t2);
    }
    if (t1.y > t2.y)
    {
        std::swap(t1, t2);
    }
    int totalHeight = t2.y - t0.y;
    for (int i = 0; i < totalHeight; i++)
    {
        bool upperHalf = i > t1.y - t0.y || t1.y == t0.y;
        int segmentHeight = upperHalf ? t2.y - t1.y + 1 : t1.y - t0.y + 1;
        float alpha = (float) i / totalHeight;
        float beta = (float) (i - (upperHalf ? t1.y - t0.y : 0)) / segmentHeight;
        Vec2i A = t0 + (t2 - t0) * alpha;
        Vec2i B = upperHalf ? t1 + (t2 - t1) * beta : t0 + (t1 - t0) * beta;
        if (A.x>B.x)
        {
            std::swap(A, B);
        }
        for (int j = A.x; j <= B.x; j++)
        {
            image.set(j, t0.y + i, color);
        }
    }
}

int drawWireframe(int argc, char **argv)
{
    if (argc == 2)
    {
        model = new Model(argv[1]);
    } else
    {
        model = new Model("obj/african_head.obj");
    }

    TGAImage image(width, height, TGAImage::RGB);
    for (int i = 0; i < model->nfaces(); i++)
    {
        std::vector<int> face = model->face(i);
        for (int j = 0; j < 3; j++)
        {
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j + 1) % 3]);
            Vec2i p0;
            Vec2i p1;
            p0.x = static_cast<int>((v0.x + 1.) * width / 2.);
            p0.y = static_cast<int>((v0.y + 1.) * height / 2.);
            p1.x = static_cast<int>((v1.x + 1.) * width / 2.);
            p1.y = static_cast<int>((v1.y + 1.) * height / 2.);
            line(p0, p1, image, white);
        }
    }


    image.flip_vertically();
    image.write_tga_file("output.tga");
    delete model;
    return 0;
}

int main(int argc, char**argv)
{
    TGAImage image(width, height, TGAImage::RGB);
    Vec2i t0[3] = {Vec2i(10, 70), Vec2i(50, 160), Vec2i(70, 80)};
    Vec2i t1[3] = {Vec2i(180, 50), Vec2i(150, 1), Vec2i(70, 180)};
    Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)};
    triangle(t0[0], t0[1], t0[2], image, red);
    triangle(t1[0], t1[1], t1[2], image, white);
    triangle(t2[0], t2[1], t2[2], image, green);
    image.flip_vertically();
    image.write_tga_file("output.tga");
    return 0;
}