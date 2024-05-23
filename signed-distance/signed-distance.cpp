// signed-distance.cpp : This file contains the 'main' function. Program execution begins and ends there.
// 
// full-blown optimization of distance transform with a "parabola intersections" method as presented in the paper
// Distance Transforms of Sampled Functions by Pedro F.Felzenszwalb and Daniel P.Huttenlocher.
// Complexity of naive DT implementation is O(width*width*height*height); an idea to store 
// vertical components (if computation starts with columns) improves performance giving O(width*height*height)
// and a "parabola intersections" method gives, as it seems, the best performance of O(width*height)
//

#include <iostream>
#include <vector>
#include <cairo/cairo.h>
#include <numbers>
#include "signed-distance.h"

#include <Windows.h>
#include <Gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

int main()
{
    int width = 1000, height = 1000;
    std::vector<int> fld(width * height);

    cairo_surface_t* surface = cairo_image_surface_create(CAIRO_FORMAT_A8, (int) width, (int) height);
    cairo_t* cr = cairo_create(surface);

    fld.assign(fld.size(), SHRT_MAX);

    cairo_arc(cr, 5.0 * width / 8, 3.0 * height / 8, width / 8.0, 0, 2 * std::numbers::pi);
    cairo_arc_negative(cr, width / 2.0, height / 2.0, 3.0 * width / 8, 2 * std::numbers::pi, 0);

    cairo_fill(cr);

    for (int ix = 0; ix < width * height; ++ix)
    {
        if (cairo_image_surface_get_data(surface)[ix] > 127)
        {
            fld[ix] = SHRT_MIN;
        }
    }

    dt(width, height, fld);

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    size_t cBuffer = fld.size();
    BYTE* bytes = new BYTE[4 * static_cast<size_t>(cBuffer)];
    for (ULONG ix = 0; ix < cBuffer; ++ix)
    {
        if (fld[ix] < 0)
        {
            bytes[4 * ix] = (BYTE)(255 - sqrt(-fld[ix]) * 255 / width * 2);
            bytes[4 * ix + 1] = (BYTE)(255 - sqrt(-fld[ix]) * 255 / width * 2);
            bytes[4 * ix + 2] = (BYTE)(0);
            bytes[4 * ix + 3] = (BYTE)(255);
        }
        else
        {
            bytes[4 * ix] = (BYTE)(255 - sqrt(fld[ix]) * 255 / width * 2);
            bytes[4 * ix + 1] = (BYTE)(0);
            bytes[4 * ix + 2] = (BYTE)(255 - sqrt(fld[ix]) * 255 / width * 2);
            bytes[4 * ix + 3] = (BYTE)(255);
        }
    }
    Gdiplus::Image* bmp = new Gdiplus::Bitmap((INT)width, (INT)height);
    Gdiplus::Graphics g(bmp);
    //g.TranslateTransform(0.f, -120.f);
    Gdiplus::Bitmap bitmap((INT)width, (INT)height, (INT)width * 4, PixelFormat32bppARGB, bytes);
    g.DrawImage(&bitmap, 0, 0);
    CLSID pngClsid;
    (void)CLSIDFromString(L"{557CF406-1A04-11D3-9A73-0000F81EF32E}", &pngClsid);
    bmp->Save(L"circleDT.png", &pngClsid, NULL);
}

