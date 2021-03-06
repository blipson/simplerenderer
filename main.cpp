#include <vector>
#include <cmath>
#include "geometry.h"
#include "tgaimage.h"
#include "model.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
Model *model = nullptr;
const int width = 800;
const int height = 800;

Vec3f barycentric(Vec2i *pts, Vec2i p)
{
    Vec3f u = cross(Vec3f(pts[2][0] - pts[0][0], pts[1][0] - pts[0][0], pts[0][0] - p[0]),
                    Vec3f(pts[2][1] - pts[0][1], pts[1][1] - pts[0][1], pts[0][1] - p[1]));
    if (std::abs(u[2]) < 1)
    {
        return Vec3f(-1, 1, 1);
    }
    return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

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
        }
        else
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

void triangle(Vec2i *pts, TGAImage &image, TGAColor color)
{
    Vec2i bboxmin(image.get_width() - 1, image.get_height() - 1);
    Vec2i bboxmax(0, 0);
    Vec2i clamp(image.get_width() - 1, image.get_height() - 1);
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            bboxmin[j] = std::max(0, std::min(bboxmin[j], pts[i][j]));
            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
        }
    }
    Vec2i p;
    for (p.x = bboxmin.x; p.x <= bboxmax.x; p.x++)
    {
        for (p.y = bboxmin.y; p.y <= bboxmax.y; p.y++)
        {
            Vec3f bcScreen = barycentric(pts, p);
            if (bcScreen.x < 0 || bcScreen.y < 0 || bcScreen.z < 0)
            {
                continue;
            }
            image.set(p.x, p.y, color);
        }
    }
}

int drawWireframe(int argc, char **argv)
{
    if (argc == 2)
    {
        model = new Model(argv[1]);
    }
    else
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

int drawTriangles(int argc, char **argv)
{
    if (argc == 2)
    {
        model = new Model(argv[1]);
    }
    else
    {
        model = new Model("obj/african_head.obj");
    }
    TGAImage image(width, height, TGAImage::RGB);
    Vec3f lightDir(0, 0, -1);
    for (int i = 0; i < model->nfaces(); i++)
    {
        std::vector<int> face = model->face(i);
        Vec2i screenCoords[3];
        Vec3f worldCoords[3];
        for (int j = 0; j < 3; j++)
        {
            Vec3f v = model->vert(face[j]);
            screenCoords[j] = Vec2i((v.x + 1.0) * width / 2.0, (v.y + 1.0) * height / 2.0);
            worldCoords[j] = v;
        }
        Vec3f base = worldCoords[2] - worldCoords[0];
        Vec3f exponent = worldCoords[1] - worldCoords[0];
        Vec3f n = Vec3f(base.y * exponent.z - base.z * exponent.y,
                        base.z * exponent.x - base.x * exponent.z,
                        base.x * exponent.y - base.y * exponent.x);
        n.normalize();
        float intensity = n.x * lightDir.x + n.y * lightDir.y + n.z * lightDir.z;
        if (intensity > 0)
        {
            Vec2i pts[3] = {Vec2i(screenCoords[0]), Vec2i(screenCoords[1]), Vec2i(screenCoords[2])};
            triangle(pts, image, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255));
        }
    }
    image.flip_vertically();
    image.write_tga_file("output.tga");
    return 0;
}

int main(int argc, char **argv)
{
    return drawTriangles(argc, argv);
}