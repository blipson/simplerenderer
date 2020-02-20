#include "TgaImage.h"

const TgaColor white = TgaColor(255, 255, 255, 255);
const TgaColor red = TgaColor(255, 0, 0, 255);

int main(int argc, char **argv)
{
    TgaImage image(100, 100, TgaImage::RGB);
    image.set(52, 41, red);
    image.set(53, 41, red);
    image.set(54, 41, red);
    image.set(55, 41, red);
    image.set(56, 41, red);
    image.set(57, 41, red);
    image.set(52, 42, red);
    image.set(53, 42, red);
    image.set(54, 42, red);
    image.set(55, 42, red);
    image.set(56, 42, red);
    image.set(57, 42, red);
    image.set(52, 43, red);
    image.set(53, 43, red);
    image.set(54, 43, red);
    image.set(55, 43, red);
    image.set(56, 43, red);
    image.set(57, 43, red);
    image.flipVertically(); // I want to have the origin at the left bottom corner of the image.
    image.scale(500, 500);
    image.writeTgaFile("output.tga");
    return 0;
}
