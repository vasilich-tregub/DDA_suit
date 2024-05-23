// glyphrun-rendered.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include "display.h"
#include <cairo/cairo.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H

#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>

#ifndef HB_H_IN
#define HB_H_IN
#endif // !HB_H_IN
#include <harfbuzz/hb-unicode.h>

#include <pango-1.0/pango/pangocairo.h>
#include <cairo/cairo-ft.h>

#include "signed-distance.h"

std::string textRun = u8"8.8.8";
const std::string fontPath = "./ARIAL.TTF";
const int fontSize = 72;

int main()
{
    int width = 320, height = 120;
    std::vector<int> fld(width * height);

    fld.assign(fld.size(), SHRT_MAX);

    cairo_surface_t* surface = cairo_image_surface_create(CAIRO_FORMAT_A8, width, height);
    cairo_t* cr = cairo_create(surface);
    //cairo_set_tolerance(cr, 0.5);
    cairo_matrix_t matrix{ 1, 0, 0, 1, 0, 3 * height / 4 };
    cairo_transform(cr, &matrix);

    hb_buffer_t* buf0 = hb_buffer_create();

    hb_unicode_funcs_t* unicode = hb_buffer_get_unicode_funcs(buf0);

    double textRunPos = 0;

    FT_Library    library;

    int ft_error = FT_Init_FreeType(&library);
    FT_Face face;

    const char* text = reinterpret_cast<const char*>(textRun.data());

    hb_buffer_t* buf = hb_buffer_create();

    hb_buffer_add_utf8(buf, text, (int)strlen(text), 0, (int)strlen(text));

    hb_buffer_guess_segment_properties(buf);

    ft_error = FT_New_Face(library, fontPath.c_str(), 0, &face);

    ft_error = FT_Set_Pixel_Sizes(face, 0, fontSize);
    hb_font_t* hb_font = hb_ft_font_create(face, 0);
    hb_shape(hb_font, buf, nullptr, 0);
    cairo_font_face_t* crFontFace = cairo_ft_font_face_create_for_ft_face(face, 0);
    cairo_set_font_face(cr, crFontFace);

    unsigned int glyph_count = 0;
    hb_glyph_info_t* glyph_info = hb_buffer_get_glyph_infos(buf, &glyph_count);
    hb_glyph_position_t* glyph_pos = hb_buffer_get_glyph_positions(buf, &glyph_count);
    cairo_set_font_size(cr, fontSize);

    cairo_glyph_t* crglyphs = new cairo_glyph_t[glyph_count];

    double xadv = 0;
    for (unsigned int glix = 0; glix < glyph_count; ++glix)
    {
        crglyphs[glix].index = glyph_info[glix].codepoint;
        crglyphs[glix].x = glyph_pos[glix].x_offset + xadv + textRunPos;
        xadv += glyph_pos[glix].x_advance / 64.0;
        crglyphs[glix].y = glyph_pos[glix].y_offset;
    }
    textRunPos += xadv;

    cairo_glyph_path(cr, crglyphs, glyph_count);

    delete[] crglyphs;

    hb_font_destroy(hb_font);

    hb_buffer_destroy(buf);

    FT_Done_Face(face);
    FT_Done_FreeType(library);

    //cairo_arc(cr, width / 2, height / 2, width / 4, 0, 2 * M_PI);

    cairo_fill(cr);

    cairo_surface_write_to_png(surface, "glyphrun.png");

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
    bmp->Save(L"glyphrun-filled.png", &pngClsid, NULL);

    cairo_destroy(cr);
    cairo_surface_destroy(surface);

}
