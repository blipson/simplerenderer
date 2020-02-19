#ifndef SIMPLERENDERER_TGAIMAGE_H
#define SIMPLERENDERER_TGAIMAGE_H

#include <fstream>

#pragma pack(push,1)
struct TgaHeader
{
    char idLength;
    char colorMapType;
    char datatypeCode;
    short colorMapOrigin;
    short colorMapLength;
    char colorMapDepth;
    short xOrigin;
    short yOrigin;
    short width;
    short height;
    char  bitsPerPixel;
    char  imageDescriptor;
};
#pragma pack(pop)


struct TgaColor
{
    // This union is to make it so you can either do 4 defined chars,
    // or send 4 raw chars,
    // or simply define a single int value
    // all in the same field without allocating more memory than necessary
    union
    {
        struct
        {
            unsigned char b, g, r, a;
        };
        unsigned char raw[4];
        unsigned int val;
    };
    int bytesPerPixel;

    TgaColor() : val(0), bytesPerPixel(1)
    {}

    TgaColor(unsigned char R, unsigned char G, unsigned char B, unsigned char A) : b(B), g(G), r(R), a(A), bytesPerPixel(4)
    {}

    TgaColor(int v, int bpp) : val(v), bytesPerPixel(bpp)
    {}

    TgaColor(const unsigned char* p, int bpp) : val(0), bytesPerPixel(bpp)
    {
        for (int i = 0; i < bpp; i++)
        {
            raw[i] = p[i];
        }
    }

    TgaColor(const TgaColor &c) : val(c.val), bytesPerPixel(c.bytesPerPixel)
    {}

    TgaColor & operator =(const TgaColor &c)
    {
        if (this != &c)
        {
            bytesPerPixel = c.bytesPerPixel;
            val = c.val;
        }
        return *this;
    }
};


class TgaImage {
protected:
    unsigned char* data;
    int width;
    int height;
    int bytesPerPixel;

    bool loadRleData(std::ifstream &in);
    bool unloadRleData(std::ofstream &out);
public:
    enum Format
    {
        GRAYSCALE = 1,
        RGB = 3,
        RGBA = 4
    };

    TgaImage();
    TgaImage(int w, int h, int bpp);
    TgaImage(const TgaImage &img);
    ~TgaImage();

    TgaImage &operator =(const TgaImage &img);

    int getWidth();
    int getHeight();
    int getBytesPerPixel();
    unsigned char* buffer();
    void clear();

    TgaColor get(int x, int y);
    bool set(int x, int y, TgaColor c);

    bool flipHorizontally();
    bool flipVertically();
    bool readTgaFile(const char* filename);
    bool writeTgaFile(const char *filename, bool rle=true);
    bool scale(int w, int h);
};

#endif //SIMPLERENDERER_TGAIMAGE_H
