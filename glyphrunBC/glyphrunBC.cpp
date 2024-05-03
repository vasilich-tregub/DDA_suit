// glyphrunBC.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
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

#include "borderpolyline.h"

std::string textRun = u8"|-|";
const std::string fontPath = "./ARIAL.TTF";
const int fontSize = 24;

int main()
{
    std::size_t WIDTH = 160, HEIGHT = 160;
    std::vector<double> datavec(WIDTH * HEIGHT);
    constexpr double nan = std::numeric_limits<double>::quiet_NaN();
    for (int ix = 0; ix < WIDTH * HEIGHT; ++ix) datavec[ix] = nan;
    array2D<double> fld(datavec, WIDTH, HEIGHT);

    std::vector<std::pair<std::pair<int, int>, double>> va;

    cairo_surface_t* surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, WIDTH, HEIGHT);
    cairo_t* cr = cairo_create(surface);
    //cairo_set_tolerance(cr, 0.1);
    cairo_matrix_t matrix{ 1, 0, 0, 1, 0, HEIGHT / 2 };
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

    //cairo_arc(cr, WIDTH / 2, HEIGHT / 2, WIDTH / 4, 0, 2 * M_PI);

    cairo_path_t* path = cairo_copy_path_flat(cr);
    cairo_path_data_t* data;

    std::cout << path->num_data << '\n';

    double xbegin, ybegin, x, y, len2;

    for (int i = 0; i < path->num_data; i += path->data[i].header.length) {
        data = &path->data[i];
        switch (data->header.type) {
        case CAIRO_PATH_MOVE_TO:
            std::cout << "MOVE_TO " << data[1].point.x << ", " << data[1].point.y << '\n';
            x = xbegin = data[1].point.x;
            y = ybegin = data[1].point.y + HEIGHT/2;
            break;
        case CAIRO_PATH_LINE_TO:
            drawEdge(x, y, data[1].point.x, data[1].point.y + HEIGHT/2, va, fld);
            len2 = (x - data[1].point.x) * (x - data[1].point.x) + (y - HEIGHT/2 - data[1].point.y) * (y - HEIGHT/2 - data[1].point.y);
            std::cout << data[1].point.x << ", " << data[1].point.y << " len^2 = " << len2 << '\n';
            x = data[1].point.x;
            y = data[1].point.y + HEIGHT/2;
            break;
        case CAIRO_PATH_CURVE_TO:
            std::cout << "curve!\n";
            break;
        case CAIRO_PATH_CLOSE_PATH:
            drawEdge(x, y, xbegin, ybegin, va, fld);
            std::cout << "close!\n";
            break;
        }
    }

    // Draw original path
    cairo_set_source_rgb(cr, 1, 1, 1);

    cairo_set_line_width(cr, 1.);
    cairo_stroke(cr);

    cairo_surface_write_to_png(surface, "glyphrun.png");

    cairo_path_destroy(path);
    cairo_destroy(cr);
    cairo_surface_destroy(surface);

    display(WIDTH, HEIGHT, fld.buffer().data(), L"glyphrunBC.png");
    double fldvalsign = 1.0;
    for (int Y = 0; Y < HEIGHT; ++Y)
    {
        for (int X = 0; X < WIDTH; ++X)
        {
            if (!std::isnan(fld(X, Y)) && fld(X, Y) * fldvalsign <= 0)
            {
                fldvalsign = -fldvalsign;
            }
            if (std::isnan(fld(X, Y)))
            {
                fld(X, Y) = fldvalsign;
            }
        }
        fldvalsign = 1.0;
    }
    display(WIDTH, HEIGHT, fld.buffer().data(), L"glyphrunBC-filled.png");
}
