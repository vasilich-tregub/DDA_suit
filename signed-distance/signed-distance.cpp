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

#include <Windows.h>
#include <Gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

int main()
{
    std::size_t width = 1000, height = 1000;
    std::vector<int> fld(width * height);
    std::vector<int> fldneg(width * height);
    std::vector<int>  coldistance(height);
    std::vector<int>  coldistanceneg(height);
    std::vector<int>  rowdistance(width);
    std::vector<int>  rowdistanceneg(width);

    cairo_surface_t* surface = cairo_image_surface_create(CAIRO_FORMAT_A8, (int) width, (int) height);
    cairo_t* cr = cairo_create(surface);

    for (int Y = 0; Y < height; ++Y)
    {
        for (int X = 0; X < width; ++X)
        {
            fld[Y * width + X] = SHRT_MAX;
            fldneg[Y * width + X] = SHRT_MAX;
        }
    }

    cairo_arc(cr, 5.0 * width / 8, 3.0 * height / 8, width / 8.0, 0, 2 * std::numbers::pi);
    cairo_arc_negative(cr, width / 2.0, height / 2.0, 3.0 * width / 8, 2 * std::numbers::pi, 0);

    cairo_fill(cr);

    for (int ix = 0; ix < width * height; ++ix)
    {
        if (cairo_image_surface_get_data(surface)[ix] == 0)
        {
            fld[ix] = 0;
        }
        if (cairo_image_surface_get_data(surface)[ix] != 0)
        {
            fldneg[ix] = 0;
        }
    }

    // vertical run: calculate vertical distances
    for (int X = 0; X < width; ++X)
    {
        // store a copy of column of distances 
        // because the next loop contains inner loop over the row index
        // and can modify the column data
        for (int Y = 0; Y < height; ++Y)
        {
            coldistance[Y] = fld[Y * width + X];
            coldistanceneg[Y] = fldneg[Y * width + X];
        }
        int* v = new int[height];
        int* vneg = new int[height];
        double* z = new double[height + 1];
        double* zneg = new double[height + 1];
        int k = 0;
        int kneg = 0;
        v[0] = 0;
        vneg[0] = 0;
        z[0] = SHRT_MIN;
        zneg[0] = SHRT_MIN;
        z[1] = SHRT_MAX;
        zneg[1] = SHRT_MAX;
        for (int q = 1; q <= height - 1; q++)
        {
            double s = ((coldistance[q] + q * q) - (coldistance[v[k]] + v[k] * v[k])) / (2 * q - 2 * v[k]);
            while (s <= z[k]) {
                k--;
                s = ((coldistance[q] + q * q) - (coldistance[v[k]] + v[k] * v[k])) / (2 * q - 2 * v[k]);
            }
            k++;
            v[k] = q;
            z[k] = s;
            z[k + 1] = SHRT_MAX;
            double sneg = ((coldistanceneg[q] + q * q) - (coldistanceneg[vneg[kneg]] + vneg[kneg] * vneg[kneg])) / (2 * q - 2 * vneg[kneg]);
            while (sneg <= zneg[kneg]) {
                kneg--;
                sneg = ((coldistanceneg[q] + q * q) - (coldistanceneg[vneg[kneg]] + vneg[kneg] * vneg[kneg])) / (2 * q - 2 * vneg[kneg]);
            }
            kneg++;
            vneg[kneg] = q;
            zneg[kneg] = sneg;
            zneg[kneg + 1] = SHRT_MAX;
        }

        k = 0;
        kneg = 0;
        for (int q = 0; q <= height - 1; q++)
        {
            while (z[k + 1] < q)
                k++;
            fld[q * width + X] = (q - v[k]) * (q - v[k]) + coldistance[v[k]];
            while (zneg[kneg + 1] < q)
                kneg++;
            fldneg[q * width + X] = (q - vneg[kneg]) * (q - vneg[kneg]) + coldistanceneg[vneg[kneg]];
        }

        delete[] v;
        delete[] z;
        delete[] vneg;
        delete[] zneg;
    }

    // horizontal run: process distances row-wise

    for (int Y = 0; Y < height; ++Y)
    {
        // store a copy of row of distances 
        // because the next loop contains inner loop over the column index
        // and can modify the row data
        for (int X = 0; X < width; ++X)
        {
            rowdistance[X] = fld[Y * width + X];
            rowdistanceneg[X] = fldneg[Y * width + X];
        }
        int* v = new int[width];
        int* vneg = new int[width];
        double* z = new double[width + 1];
        double* zneg = new double[width + 1];
        int k = 0;
        int kneg = 0;
        v[0] = 0;
        vneg[0] = 0;
        z[0] = SHRT_MIN;
        zneg[0] = SHRT_MIN;
        z[1] = SHRT_MAX;
        zneg[1] = SHRT_MAX;
        for (int q = 1; q <= width - 1; q++)
        {
            double s = ((rowdistance[q] + q * q) - (rowdistance[v[k]] + v[k] * v[k])) / (2 * q - 2 * v[k]);
            while (s <= z[k]) {
                k--;
                s = ((rowdistance[q] + q * q) - (rowdistance[v[k]] + v[k] * v[k])) / (2 * q - 2 * v[k]);
            }
            k++;
            v[k] = q;
            z[k] = s;
            z[k + 1] = SHRT_MAX;
            double sneg = ((rowdistanceneg[q] + q * q) - (rowdistanceneg[vneg[kneg]] + vneg[kneg] * vneg[kneg])) / (2 * q - 2 * vneg[kneg]);
            while (sneg <= zneg[kneg]) {
                kneg--;
                sneg = ((rowdistanceneg[q] + q * q) - (rowdistanceneg[vneg[kneg]] + vneg[kneg] * vneg[kneg])) / (2 * q - 2 * vneg[kneg]);
            }
            kneg++;
            vneg[kneg] = q;
            zneg[kneg] = sneg;
            zneg[kneg + 1] = SHRT_MAX;
        }

        k = 0;
        kneg = 0;
        for (int q = 0; q <= width - 1; q++)
        {
            while (z[k + 1] < q)
                k++;
            fld[Y * width + q] = (q - v[k]) * (q - v[k]) + rowdistance[v[k]];
            while (zneg[kneg + 1] < q)
                kneg++;
            fldneg[Y * width + q] = (q - vneg[kneg]) * (q - vneg[kneg]) + rowdistanceneg[vneg[kneg]];
        }

        delete[] v;
        delete[] z;
        delete[] vneg;
        delete[] zneg;
    }

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    size_t cBuffer = fld.size();
    BYTE* bytes = new BYTE[4 * static_cast<size_t>(cBuffer)];
    for (ULONG ix = 0; ix < cBuffer; ++ix)
    {
        if (fld[ix] == 0)
        {
            bytes[4 * ix] = (BYTE)(255 - sqrt(fldneg[ix]) * 255 / width * 2);
            bytes[4 * ix + 1] = (BYTE)(255 - sqrt(fldneg[ix]) * 255 / width * 2);
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

