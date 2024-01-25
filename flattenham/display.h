#pragma once
#include <Windows.h>
#include <Gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

void display(int WIDTH, int HEIGHT, double* fld, const wchar_t* filename)
{
    double pxlmaxval = 0;
    for (int ix = 0; ix < WIDTH * HEIGHT; ++ix)
    {
        if (!std::isnan(fld[ix]))
        {
            if (abs(fld[ix]) > pxlmaxval) pxlmaxval = abs(fld[ix]);
        }
    }

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    int cBuffer = WIDTH * HEIGHT;
    if (WIDTH < 512)
        cBuffer = 512 * 512;
    BYTE* bytes = new BYTE[4 * static_cast<size_t>(cBuffer)];
    if (WIDTH < 512)
    {
        for (int Y = 0; Y < HEIGHT; ++Y)
        {
            for (int X = 0; X < WIDTH; ++X)
            {
                int iX = Y * WIDTH + X;
                int fldval = (int)(fld[iX] / pxlmaxval * 255.0);
                for (int blkY = 0; blkY < ceil(512.f / HEIGHT); ++blkY)
                {
                    for (int blkX = 0; blkX < ceil(512.f / WIDTH); ++blkX)
                    {
                        int iblkx = ((Y * 512 / WIDTH) + blkY) * 512 + X * 512 / WIDTH + blkX;
                        if (std::isnan(fld[iX]))
                        {
                            bytes[4 * iblkx] = (BYTE)(255);
                            bytes[4 * iblkx + 1] = (BYTE)(255);
                            bytes[4 * iblkx + 2] = (BYTE)(255);
                            bytes[4 * iblkx + 3] = (BYTE)(255);
                        }
                        else if (fldval > 0)
                        {
                            bytes[4 * iblkx] = (BYTE)(fldval);
                            bytes[4 * iblkx + 1] = 0;
                            bytes[4 * iblkx + 2] = (BYTE)(fldval);
                            bytes[4 * iblkx + 3] = (BYTE)(255);
                        }
                        else
                        {
                            bytes[4 * iblkx] = (BYTE)(-fldval);
                            bytes[4 * iblkx + 1] = (BYTE)(-fldval);
                            bytes[4 * iblkx + 2] = 0;
                            bytes[4 * iblkx + 3] = (BYTE)(255);
                        }
                    }
                }
            }
        }
    }
    else
    {
        for (int ix = 0; ix < cBuffer; ++ix)
        {
            int fldval = (int)(fld[ix] / pxlmaxval * 255.0);
            if (std::isnan(fld[ix]))
            {
                bytes[4 * ix] = (BYTE)(255);
                bytes[4 * ix + 1] = (BYTE)(255);
                bytes[4 * ix + 2] = (BYTE)(255);
                bytes[4 * ix + 3] = (BYTE)(255);
            }
            else if (fldval > 0)
            {
                bytes[4 * ix] = (BYTE)(fldval);
                bytes[4 * ix + 1] = 0;
                bytes[4 * ix + 2] = (BYTE)(fldval);
                bytes[4 * ix + 3] = (BYTE)(255);
            }
            else
            {
                bytes[4 * ix] = (BYTE)(-fldval);
                bytes[4 * ix + 1] = (BYTE)(-fldval);
                bytes[4 * ix + 2] = 0;
                bytes[4 * ix + 3] = (BYTE)(255);
            }
        }
    }
    int width = WIDTH, height = HEIGHT;
    if (WIDTH < 512)
    {
        width = 512;
        height = 512 * HEIGHT / WIDTH;
    }
    Gdiplus::Image* bmp = new Gdiplus::Bitmap(width, height);
    Gdiplus::Graphics g(bmp);
    Gdiplus::Bitmap bitmap(width, height, width * 4, PixelFormat32bppARGB, bytes);
    Gdiplus::Point destinationPoints[] = {
       Gdiplus::Point(0, height),   // destination for upper-left point of original
       Gdiplus::Point(width, height),  // destination for upper-right point of original
       Gdiplus::Point(0, 0) };  // destination for lower-left point of original
    g.DrawImage(&bitmap, destinationPoints, 3);
    //g.DrawImage(&bitmap, 0, 0);
    CLSID pngClsid;
    (void)CLSIDFromString(L"{557CF406-1A04-11D3-9A73-0000F81EF32E}", &pngClsid);
    bmp->Save(filename, &pngClsid, NULL);
}