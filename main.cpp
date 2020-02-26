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
    float b = y1 - (m * x1);

    int minX = std::min(x0, x1);
    int maxX = std::max(x0, x1);

    if (minX == maxX || std::abs(dx) < std::abs(dy))
    {
        int minY = std::min(y0, y1);
        int maxY = std::max(y0, y1);
        for (int y = minY; y <= maxY; y++)
        {
            int x = (y - b) / m;
            image.set(x, y, color);
        }
    }
    else
    {
        for (int x = minX; x <= maxX; x++)
        {
            int y = (m * x) + b;
            image.set(x, y, color);
        }
    }
}

void digitalDifferentialAnalyzerLine(int x0, int y0, int x1, int y1, TgaImage &image, const TgaColor &color)
{
    float x, y, dx, dy, step, i;

    dx = (x1 - x0);
    dy = (y1 - y0);
    if (abs(dx) >= abs(dy))
    {
        step = abs(dx);
    }
    else
    {
        step = abs(dy);
    }
    dx = dx / step;
    dy = dy / step;
    x = x0;
    y = y0;
    i = 1;
    while (i <= step) {
        image.set(x, y, color);
        x = x + dx;
        y = y + dy;
        i = i + 1;
    }
}

void bresenhamLine(int x0, int y0, int x1, int y1, TgaImage &image, const TgaColor &color)
{
    bool steep = false;
    if (std::abs(x0-x1)<std::abs(y0-y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0>x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1-x0;
    int dy = y1-y0;
    int derror2 = std::abs(dy)*2;
    int error2 = 0;
    int y = y0;
    for (int x=x0; x<=x1; x++) {
        if (steep) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }
        error2 += derror2;
        if (error2 > dx) {
            y += (y1>y0?1:-1);
            error2 -= dx*2;
        }
    }
}

int iPartOfNumber(float x)
{
    return (int)x;
}

//rounds off a number
int roundNumber(float x)
{
    return iPartOfNumber(x + 0.5) ;
}

//returns fractional part of a number
float fPartOfNumber(float x)
{
    if (x>0) return x - iPartOfNumber(x);
    else return x - (iPartOfNumber(x)+1);

}

float rfPartOfNumber(float x)
{
    return 1 - fPartOfNumber(x);
}


void xiaolinWuLine(int x0 , int y0 , int x1 , int y1, TgaImage &image, const TgaColor &color)
{
    int steep = std::abs(y1 - y0) > std::abs(x1 - x0) ;

    // swap the co-ordinates if slope > 1 or we
    // draw backwards
    if (steep)
    {
        std::swap(x0 , y0);
        std::swap(x1 , y1);
    }
    if (x0 > x1)
    {
        std::swap(x0 ,x1);
        std::swap(y0 ,y1);
    }

    //compute the slope
    float dx = x1-x0;
    float dy = y1-y0;
    float gradient = dy/dx;
    if (dx == 0.0)
        gradient = 1;

    int xpxl1 = x0;
    int xpxl2 = x1;
    float intersectY = y0;

    // main loop
    if (steep)
    {
        for (int x = xpxl1 ; x <=xpxl2 ; x += 1)
        {
            // pixel coverage is determined by fractional
            // part of y co-ordinate
            float firstCoef = rfPartOfNumber(intersectY);
            std::cout << firstCoef << std::endl;
            image.set(iPartOfNumber(intersectY), x,
                      TgaColor(color.r * firstCoef, color.g * firstCoef, color.b * firstCoef, 255));
            float secondCoef = fPartOfNumber(intersectY);
            std::cout << secondCoef << std::endl;
            image.set(iPartOfNumber(intersectY)-1, x,
                      TgaColor(color.r * secondCoef, color.g * secondCoef, color.b * secondCoef, 255));
            intersectY += gradient;
        }
    }
    else
    {
        for (int x = xpxl1 ; x <=xpxl2 ; x += 1)
        {
            // pixel coverage is determined by fractional
            // part of y co-ordinate
            float firstCoef = rfPartOfNumber(intersectY);
            std::cout << firstCoef << std::endl;
            image.set(x, iPartOfNumber(intersectY),
                      TgaColor(color.r * firstCoef, color.g * firstCoef, color.b * firstCoef, 255));
            float secondCoef = fPartOfNumber(intersectY);
            std::cout << secondCoef << std::endl;
            image.set(x, iPartOfNumber(intersectY)-1,
                      TgaColor(color.r * secondCoef, color.g * secondCoef, color.b * secondCoef, 255));
            intersectY += gradient;
        }
    }

}

int main(int argc, char** argv)
{
    auto *model = new Model("/Users/z003yw4/Dev/simplerenderer/targetitem.obj");
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
            int x0 = (v0.x + 17) * width / 35;
            int y0 = (v0.y + 2) * height / 35;
            int x1 = (v1.x + 17) * width / 35;
            int y1 = (v1.y + 2) * height / 35;
            line(x0, y0, x1, y1, image, white);
        }
    }

    image.flipVertically(); // i want to have the origin at the left bottom corner of the image
    image.writeTgaFile("targetitem.tga");
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
