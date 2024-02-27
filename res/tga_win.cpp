// tga_win.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "tga.h"

#include <Windows.h>
#include <Gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

int main(int argc, char* argv[])
{
    //FILE* f = std::fopen("620_400.tga", "rb");
    if (argc < 2)
    {
        std::cout << "usage: >tga_win [imagename].tga";
        return 1;
    }
    const char* filename = argv[1];
    FILE* f;
    fopen_s(&f, filename, "rb");
    tga::StdioFileInterface file(f);
    tga::Decoder decoder(&file);
    tga::Header header;
    if (!decoder.readHeader(header))
        return 2;

    tga::Image image;
    image.bytesPerPixel = header.bytesPerPixel();
    image.rowstride = header.width * header.bytesPerPixel();

    std::vector<uint8_t> buffer(image.rowstride * header.height);
    image.pixels = &buffer[0];

    if (!decoder.readImage(header, image, nullptr))
        return 3;

    // Optional post-process to fix the alpha channel in
    // some TGA files where alpha=0 for all pixels when
    // it shouldn't.
    decoder.postProcessImage(header, image);

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    Gdiplus::Image* bmp = new Gdiplus::Bitmap(header.width, header.height);
    Gdiplus::Graphics g(bmp);
    Gdiplus::Bitmap bitmap(header.width, header.height, header.width * image.bytesPerPixel, PixelFormat32bppARGB, image.pixels);
    /*Gdiplus::Point destinationPoints[] = {
       Gdiplus::Point(0, header.height),   // destination for upper-left point of original
       Gdiplus::Point(header.width, header.height),  // destination for upper-right point of original
       Gdiplus::Point(0, 0) };  // destination for lower-left point of original
    g.DrawImage(&bitmap, destinationPoints, 3);
    */
    g.DrawImage(&bitmap, 0, 0); // , destinationPoints, 3);
    CLSID pngClsid;
    (void)CLSIDFromString(L"{557CF406-1A04-11D3-9A73-0000F81EF32E}", &pngClsid);
    const size_t len = strlen(filename) + 1;
    int wclen = MultiByteToWideChar(CP_UTF8, 0, filename, -1, NULL, 0);
    wchar_t* wcname = new wchar_t[wclen];
    MultiByteToWideChar(CP_UTF8, 0, filename, -1, wcname, wclen);
    std::wstring pngnamestring(wcname);
    pngnamestring += L".png";
    bmp->Save(pngnamestring.c_str(), &pngClsid, NULL);
    delete[] wcname;

    return 0;

}
