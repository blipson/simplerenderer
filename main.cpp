#include "TgaImage.h"
#include "Model.h"
#include <iostream>

const TgaColor white = TgaColor(255, 255, 255, 255);
const TgaColor red = TgaColor(255, 0, 0, 255);

void line(int x0, int y0, int x1, int y1, TgaImage &image, const TgaColor &color)
{
    int dx = x1 - x0;
    int dy = y1 - y0;
    float m = (float) dy / (float) dx;
    float b = float(y1) - (m * float(x1));

    int minX = std::min(x0, x1);
    int maxX = std::max(x0, x1);

    if (minX == maxX || std::abs(dx) < std::abs(dy))
    {
        int minY = std::min(y0, y1);
        int maxY = std::max(y0, y1);
        for (int y = minY; y <= maxY; y++)
        {
            int x = int(std::round((float(y) - b) / m));
            image.set(x, y, color);
        }
    }
    else
    {
        for (int x = minX; x <= maxX; x++)
        {
            int y = int(std::round((m * float(x)) + b));
            image.set(x, y, color);
        }
    }
}

void digitalDifferentialAnalyzerLine(int x0, int y0, int x1, int y1, TgaImage &image, const TgaColor &color)
{
    float x, y, dx, dy, step, i;

    dx = float(x1 - x0);
    dy = float(y1 - y0);

    float absDx = abs(dx);
    float absDy = abs(dy);

    if (absDx >= absDy)
    {
        step = absDx;
    }
    else
    {
        step = absDy;
    }
    dx = dx / step;
    dy = dy / step;
    x = float(x0);
    y = float(y0);
    i = 1;
    while (i <= step)
    {
        image.set(int(std::round(x)), int(std::round(y)), color);
        x = x + dx;
        y = y + dy;
        i = i + 1;
    }
}

void bresenhamLine(int x0, int y0, int x1, int y1, TgaImage &image, const TgaColor &color)
{
    bool steep = false;
    if (std::abs(x0 - x1) < std::abs(y0 - y1))
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1 - x0;
    int dy = y1 - y0;
    int dError = std::abs(dy) * 2;
    int error = 0;
    int y = y0;
    for (int x = x0; x <= x1; x++)
    {
        if (steep)
        {
            image.set(y, x, color);
        }
        else
        {
            image.set(x, y, color);
        }
        error += dError;
        if (error > dx)
        {
            y += (y1 > y0 ? 1 : -1);
            error -= dx * 2;
        }
    }
}


//returns fractional part of a number
float fPartOfNumber(float x)
{
    if (x > 0)
    {
        return x - std::round(x);
    }
    else
    {
        return x - std::round(x) + 1;
    }

}

float rfPartOfNumber(float x)
{
    return 1 - fPartOfNumber(x);
}


void xiaolinWuLine(int x0, int y0, int x1, int y1, TgaImage &image, const TgaColor &color)
{
    int steep = std::abs(y1 - y0) > std::abs(x1 - x0);

    // swap the co-ordinates if slope > 1 or we
    // draw backwards
    if (steep)
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }
    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    //compute the slope
    auto dx = float(x1 - x0);
    auto dy = float(y1 - y0);
    float gradient = dy / dx;
    if (dx == 0.0)
    {
        gradient = 1;
    }

    int xpxl1 = x0;
    int xpxl2 = x1;
    auto intersectY = float(y0);

    // main loop
    if (steep)
    {
        for (int x = xpxl1; x <= xpxl2; x += 1)
        {
            // pixel coverage is determined by fractional
            // part of y co-ordinate
            float firstCoef = rfPartOfNumber(intersectY);
            std::cout << firstCoef << std::endl;
            image.set(int(std::round(intersectY)), x,
                      TgaColor(
                              char(float(color.r) * firstCoef),
                              char(float(color.g) * firstCoef),
                              char(float(color.b) * firstCoef),
                              255
                      )
            );
            float secondCoef = fPartOfNumber(intersectY);
            std::cout << secondCoef << std::endl;
            image.set(int(std::round(intersectY)) - 1, x,
                      TgaColor(
                              char(float(color.r) * secondCoef),
                              char(float(color.g) * secondCoef),
                              char(float(color.b) * secondCoef),
                              255
                      )
            );
            intersectY += gradient;
        }
    }
    else
    {
        for (int x = xpxl1; x <= xpxl2; x += 1)
        {
            // pixel coverage is determined by fractional
            // part of y co-ordinate
            float firstCoef = rfPartOfNumber(intersectY);
            std::cout << firstCoef << std::endl;
            image.set(x, int(std::round(intersectY)),
                      TgaColor(
                              char(float(color.r) * firstCoef),
                              char(float(color.g) * firstCoef),
                              char(float(color.b) * firstCoef),
                              255
                      )
            );
            float secondCoef = fPartOfNumber(intersectY);
            std::cout << secondCoef << std::endl;
            image.set(x, int(std::round(intersectY)) - 1,
                      TgaColor(
                              char(float(color.r) * secondCoef),
                              char(float(color.g) * secondCoef),
                              char(float(color.b) * secondCoef),
                              255
                      )
            );
            intersectY += gradient;
        }
    }

}

int main(int argc, char **argv)
{
    auto *model = new Model("/Users/z003yw4/Dev/simplerenderer/input/head.obj");
    const int width = 500;
    const int height = 500;

    TgaImage image(width, height, TgaImage::RGB);
    for (int i = 0; i < model->nFaces(); i++)
    {
        std::vector<int> face = model->face(i);
        for (int j = 0; j < 3; j++)
        {
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j + 1) % 3]); // use the modulo to go back to 0 and connect the triangle

            // these constants change based on the size of the OBJ file
            int x0 = (v0.x + 1) * width / 2;
            int y0 = (v0.y + 1) * height / 2;
            int x1 = (v1.x + 1) * width / 2;
            int y1 = (v1.y + 1) * height / 2;
            xiaolinWuLine(x0, y0, x1, y1, image, white);
        }
    }

    image.flipVertically(); // i want to have the origin at the left bottom corner of the image
    image.writeTgaFile("output/testx.tga");
    delete model;
    return 0;

//    TgaImage image(100, 100, TgaImage::RGB);
//    for (int i = 0; i < 1000000; i++)
//    {
//        xiaolinWuLine(13, 20, 80, 40, image, white);
//        xiaolinWuLine(20, 13, 40, 80, image, red);
//        xiaolinWuLine(80, 40, 13, 20, image, red);
//    }
//    image.flipVertically(); // i want to have the origin at the left bottom corner of the image
//    image.writeTgaFile("output.tga");
//    return 0;
}
