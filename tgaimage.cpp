#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>
#include <cmath>
#include "TgaImage.h"

TgaImage::TgaImage() : data(nullptr), width(0), height(0), bytesPerPixel(0)
{}

TgaImage::TgaImage(int w, int h, int bpp) : data(nullptr), width(w), height(h), bytesPerPixel(bpp)
{
    unsigned long numberOfBytes = width * height * bytesPerPixel;
    data = new unsigned char[numberOfBytes];
    memset(data, 0, numberOfBytes);
}

TgaImage::TgaImage(const TgaImage &img)
{
    width = img.width;
    height = img.height;
    bytesPerPixel = img.bytesPerPixel;
    unsigned long numberOfBytes = width * height * bytesPerPixel;
    data = new unsigned char[numberOfBytes];
    memcpy(data, img.data, numberOfBytes);
}

TgaImage::~TgaImage()
{
    delete[] data;
}

TgaImage &TgaImage::operator=(const TgaImage &img)
{
    if (this != &img)
    {
        delete[] data;
        width = img.width;
        height = img.height;
        bytesPerPixel = img.bytesPerPixel;
        unsigned long numberOfBytes = width * height * bytesPerPixel;
        data = new unsigned char[numberOfBytes];
        memcpy(data, img.data, numberOfBytes);
    }
    return *this;
}

int TgaImage::getWidth()
{
    return width;
}

int TgaImage::getHeight()
{
    return height;
}

int TgaImage::getBytesPerPixel()
{
    return bytesPerPixel;
}

unsigned char *TgaImage::buffer()
{
    return data;
}

void TgaImage::clear()
{
    memset((void *) data, 0, width * height * bytesPerPixel);
}

TgaColor TgaImage::get(int x, int y)
{
    if (!data || x < 0 || y < 0 || x >= width || y >= height)
    {
        return TgaColor();
    }
    return TgaColor(data + (x + y * width) * bytesPerPixel, bytesPerPixel);
}

bool TgaImage::set(int x, int y, TgaColor c)
{
    if (!data || x < 0 || y < 0 || x >= width || y >= height)
    {
        return false;
    }
    memcpy(data + (x + y * width) * bytesPerPixel, c.raw, bytesPerPixel);
    return true;
}

bool TgaImage::flipHorizontally()
{
    if (!data)
    {
        return false;
    }
    int half = width >> 1; // bit shift by 1 to divide by 2 without having to handle rounding manually.
    for (int i = 0; i < half; i++)
    {
        for (int j = 0; j < height; j++)
        {
            // Loop through each row, and flip the bits on the Y axis
            // going inwards from the outside.
            // [p1, p2, p3, p4,
            // p5, p6, p7, p8]
            // with width 4 and height 2 becomes
            // [p4, p3, p2, p1,
            //  p8, p7, p6, p5]
            TgaColor c1 = get(i, j);
            TgaColor c2 = get(width - 1 - i, j);
            set(i, j, c2);
            set(width - 1 - i, j, c1);
        }
    }
    return true;
}

bool TgaImage::flipVertically()
{
    if (!data)
    {
        return false;
    }
    unsigned long bytes_per_line = width * bytesPerPixel;
    auto *line = new unsigned char[bytes_per_line];
    int half = height >> 1; // bit shift by 1 to divide by 2 without having to handle rounding manually.
    for (int j = 0; j < half; j++)
    {
        // Flip on a per-row basis, going from outside in.
        // [p1, p2, p3, p4,
        // p5, p6, p7, p8]
        // with width 4 and height 2 becomes
        // [p5, p6, p7, p8,
        //  p1, p2, p3, p4]
        // Use line as a temporary storage so we can swap the lines.
        unsigned long l1 = j * bytes_per_line;
        unsigned long l2 = (height - 1 - j) * bytes_per_line;
        memmove((void *) line, (void *) (data + l1), bytes_per_line);
        memmove((void *) (data + l1), (void *) (data + l2), bytes_per_line);
        memmove((void *) (data + l2), (void *) line, bytes_per_line);
    }
    delete[] line;
    return true;
}

bool TgaImage::readTgaFile(const char *filename)
{
    // Make sure data is empty before we start.
    delete[] data;
    data = nullptr;

    // First, we open the file.
    std::ifstream in;
    in.open(filename, std::ios::binary);
    if (!in.is_open())
    {
        std::cerr << "can't open file " << filename << "\n";
        in.close();
        return false;
    }

    // Handle the header and set the appropriate fields on our class as we read them in.
    TgaHeader header;
    in.read((char *) &header, sizeof(header));
    if (!in.good())
    {
        in.close();
        std::cerr << "an error occured while reading the header\n";
        return false;
    }
    width = header.width;
    height = header.height;
    bytesPerPixel = header.bitsPerPixel >> 3;
    if (width <= 0 || height <= 0 || (bytesPerPixel != GRAYSCALE && bytesPerPixel != RGB && bytesPerPixel != RGBA))
    {
        in.close();
        std::cerr << "bad bpp (or width/height) value\n";
        return false;
    }

    // Handle the body as we read data in and allocate memory accordingly.
    unsigned long numberOfBytes = bytesPerPixel * width * height;
    data = new unsigned char[numberOfBytes];
    if (header.datatypeCode == 2 || header.datatypeCode == 3)
    {
        in.read((char *) data, numberOfBytes);
        if (!in.good())
        {
            in.close();
            std::cerr << "an error occured while reading the data\n";
            return false;
        }
    }
    else if (10 == header.datatypeCode || 11 == header.datatypeCode)
    {
        if (!loadRleData(in))
        {
            in.close();
            std::cerr << "an error occured while reading the data\n";
            return false;
        }
    }
    else
    {
        in.close();
        std::cerr << "unknown file format " << (int) header.datatypeCode << "\n";
        return false;
    }
    if (!(header.imageDescriptor & 0x20))
    {
        flipVertically();
    }
    if (header.imageDescriptor & 0x10)
    {
        flipHorizontally();
    }
    std::cerr << width << "x" << height << "/" << bytesPerPixel * 8 << "\n";
    in.close();
    return true;
}

bool TgaImage::loadRleData(std::ifstream &in)
{
    unsigned long pixelCount = width * height;
    unsigned long currentPixel = 0;
    unsigned long currentByte = 0;
    TgaColor colorBuffer;
    do
    {
        unsigned char chunkHeader = 0;
        chunkHeader = in.get();
        if (!in.good())
        {
            std::cerr << "an error occured while reading the data\n";
            return false;
        }
        if (chunkHeader < 128)
        {
            chunkHeader++;
            for (int i = 0; i < chunkHeader; i++)
            {
                in.read((char*) colorBuffer.raw, bytesPerPixel);
                if (!in.good())
                {
                    std::cerr << "an error occured while reading the header\n";
                    return false;
                }
                for (int t = 0; t < bytesPerPixel; t++)
                {
                    data[currentByte++] = colorBuffer.raw[t];
                }
                currentPixel++;
                if (currentPixel > pixelCount)
                {
                    std::cerr << "Too many pixels read\n";
                    return false;
                }
            }
        }
        else
        {
            chunkHeader -= 127;
            in.read((char *) colorBuffer.raw, bytesPerPixel);
            if (!in.good())
            {
                std::cerr << "an error occured while reading the header\n";
                return false;
            }
            for (int i = 0; i < chunkHeader; i++)
            {
                for (int t = 0; t < bytesPerPixel; t++)
                {
                    data[currentByte++] = colorBuffer.raw[t];
                }
                currentPixel++;
                if (currentPixel > pixelCount)
                {
                    std::cerr << "Too many pixels read\n";
                    return false;
                }
            }
        }
    }
    while (currentPixel < pixelCount);
    return true;
}

bool TgaImage::writeTgaFile(const char *filename, bool rle)
{
    unsigned char developer_area_ref[4] = {0, 0, 0, 0};
    unsigned char extension_area_ref[4] = {0, 0, 0, 0};
    unsigned char footer[18] = {'T', 'R', 'U', 'E', 'V', 'I', 'S', 'I', 'O', 'N', '-', 'X', 'F', 'I', 'L', 'E', '.',
                                '\0'};
    std::ofstream out;
    out.open(filename, std::ios::binary);
    if (!out.is_open())
    {
        std::cerr << "can't open file " << filename << "\n";
        out.close();
        return false;
    }
    TgaHeader header;
    memset((void *) &header, 0, sizeof(header));
    header.bitsPerPixel = bytesPerPixel << 3;
    header.width = width;
    header.height = height;
    header.datatypeCode = (bytesPerPixel == GRAYSCALE ? (rle ? 11 : 3) : (rle ? 10 : 2));
    header.imageDescriptor = 0x20; // top-left origin
    out.write((char *) &header, sizeof(header));
    if (!out.good())
    {
        out.close();
        std::cerr << "can't dump the tga file\n";
        return false;
    }
    if (!rle)
    {
        out.write((char *) data, width * height * bytesPerPixel);
        if (!out.good())
        {
            std::cerr << "can't unload raw data\n";
            out.close();
            return false;
        }
    } else
    {
        if (!unloadRleData(out))
        {
            out.close();
            std::cerr << "can't unload rle data\n";
            return false;
        }
    }
    out.write((char *) developer_area_ref, sizeof(developer_area_ref));
    if (!out.good())
    {
        std::cerr << "can't dump the tga file\n";
        out.close();
        return false;
    }
    out.write((char *) extension_area_ref, sizeof(extension_area_ref));
    if (!out.good())
    {
        std::cerr << "can't dump the tga file\n";
        out.close();
        return false;
    }
    out.write((char *) footer, sizeof(footer));
    if (!out.good())
    {
        std::cerr << "can't dump the tga file\n";
        out.close();
        return false;
    }
    out.close();
    return true;
}

// TODO: it is not necessary to break a raw chunk for two equal pixels (for the matter of the resulting size)
bool TgaImage::unloadRleData(std::ofstream &out)
{
    const unsigned char maxChunkLength = 128;
    unsigned long nPixels = width * height;
    unsigned long currentPixel = 0;
    while (currentPixel < nPixels)
    {
        unsigned long chunkStart = currentPixel * bytesPerPixel;
        unsigned long currentByte = currentPixel * bytesPerPixel;
        unsigned char runLength = 1;
        bool raw = true;
        while (currentPixel + runLength < nPixels && runLength < maxChunkLength)
        {
            bool success = true;
            for (int t = 0; success && t < bytesPerPixel; t++)
            {
                success = (data[currentByte + t] == data[currentByte + t + bytesPerPixel]);
            }
            currentByte += bytesPerPixel;
            if (1 == runLength)
            {
                raw = !success;
            }
            if (raw && success)
            {
                runLength--;
                break;
            }
            if (!raw && !success)
            {
                break;
            }
            runLength++;
        }
        currentPixel += runLength;
        out.put(raw ? runLength - 1 : runLength + 127);
        if (!out.good())
        {
            std::cerr << "can't dump the tga file\n";
            return false;
        }
        out.write((char *) (data + chunkStart), (raw ? runLength * bytesPerPixel : bytesPerPixel));
        if (!out.good())
        {
            std::cerr << "can't dump the tga file\n";
            return false;
        }
    }
    return true;
}

bool TgaImage::scale(int w, int h)
{
    if (w <= 0 || h <= 0 || !data)
    {
        return false;
    }
    auto* tData = new unsigned char[w * h * bytesPerPixel];
    int nScanLine = 0;
    int oScanLine = 0;
    int yErr = 0;
    int nLineBytes = w * bytesPerPixel;
    int oLineBytes = width * bytesPerPixel;
    for (int j = 0; j < height; j++)
    {
        int xErr = width - w;
        int nx = -bytesPerPixel;
        int ox = -bytesPerPixel;
        for (int i = 0; i < width; i++)
        {
            ox += bytesPerPixel;
            xErr += w;
            while (xErr >= (int) width)
            {
                xErr -= width;
                nx += bytesPerPixel;
                memcpy(tData + nScanLine + nx, data + oScanLine + ox, bytesPerPixel);
            }
        }
        yErr += h;
        oScanLine += oLineBytes;
        while (yErr >= (int) height)
        {
            if (yErr >= (int) height << 1) // it means we jump over a scanline
            {
                memcpy(tData + nScanLine + nLineBytes, tData + nScanLine, nLineBytes);
            }
            yErr -= height;
            nScanLine += nLineBytes;
        }
    }
    delete [] data;
    data = tData;
    width = w;
    height = h;
    return true;
}
