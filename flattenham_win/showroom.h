#pragma once
#include "flattenham_win.h"

unsigned char* showroom(int WIDTH, int HEIGHT, double* fld)
{
    double pxlmaxval = 0;
    for (int ix = 0; ix < WIDTH * HEIGHT; ++ix)
    {
        if (!std::isnan(fld[ix]))
        {
            if (abs(fld[ix]) > pxlmaxval) pxlmaxval = abs(fld[ix]);
        }
    }

    /*Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);*/
    int cBuffer = WIDTH * HEIGHT;
    if (WIDTH < 512)
        cBuffer = 512 * 512;
    unsigned char* bytes = new unsigned char[4 * static_cast<size_t>(cBuffer)];
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
                            bytes[4 * iblkx] = (unsigned char)(255);
                            bytes[4 * iblkx + 1] = (unsigned char)(255);
                            bytes[4 * iblkx + 2] = (unsigned char)(255);
                            bytes[4 * iblkx + 3] = (unsigned char)(255);
                        }
                        else if (fldval > 0)
                        {
                            bytes[4 * iblkx] = (unsigned char)(fldval);
                            bytes[4 * iblkx + 1] = 0;
                            bytes[4 * iblkx + 2] = (unsigned char)(fldval);
                            bytes[4 * iblkx + 3] = (unsigned char)(255);
                        }
                        else
                        {
                            bytes[4 * iblkx] = (unsigned char)(-fldval);
                            bytes[4 * iblkx + 1] = (unsigned char)(-fldval);
                            bytes[4 * iblkx + 2] = 0;
                            bytes[4 * iblkx + 3] = (unsigned char)(255);
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
                bytes[4 * ix] = (unsigned char)(255);
                bytes[4 * ix + 1] = (unsigned char)(255);
                bytes[4 * ix + 2] = (unsigned char)(255);
                bytes[4 * ix + 3] = (unsigned char)(255);
            }
            else if (fldval > 0)
            {
                bytes[4 * ix] = (unsigned char)(fldval);
                bytes[4 * ix + 1] = 0;
                bytes[4 * ix + 2] = (unsigned char)(fldval);
                bytes[4 * ix + 3] = (unsigned char)(255);
            }
            else
            {
                bytes[4 * ix] = (unsigned char)(-fldval);
                bytes[4 * ix + 1] = (unsigned char)(-fldval);
                bytes[4 * ix + 2] = 0;
                bytes[4 * ix + 3] = (unsigned char)(255);
            }
        }
    }
    int width = WIDTH, height = HEIGHT;
    if (WIDTH < 512)
    {
        width = 512;
        height = 512 * HEIGHT / WIDTH;
    }

    for (int Y = 0; Y < 512; ++Y)
    {
        for (int X = 0; X < WIDTH; ++X)
        {
            int ix = Y * 512 + X * 512 / WIDTH;
            bytes[4 * ix] = (unsigned char)(0);
            bytes[4 * ix + 1] = (unsigned char)(255);
            bytes[4 * ix + 2] = (unsigned char)(0);
            bytes[4 * ix + 3] = (unsigned char)(255);
        }
    }
    for (int Y = 0; Y < HEIGHT; ++Y)
    {
        for (int X = 0; X < 512; ++X)
        {
            int ix = Y * 512 / HEIGHT * 512 + X;
            bytes[4 * ix] = (unsigned char)(0);
            bytes[4 * ix + 1] = (unsigned char)(255);
            bytes[4 * ix + 2] = (unsigned char)(0);
            bytes[4 * ix + 3] = (unsigned char)(255);
        }
    }
    return bytes;
}
