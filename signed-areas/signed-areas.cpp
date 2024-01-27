// signed-areas.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <cairo/cairo.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>

constexpr int WIDTH = 512;
constexpr int HEIGHT = 512;

double fld[WIDTH * HEIGHT]{};

int main()
{
    cairo_surface_t* surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, WIDTH, HEIGHT);
    cairo_t* cr = cairo_create(surface);
    //cairo_set_tolerance(cr, 0.1);
    //cairo_matrix_t matrix{ 1, 0, 0, 1, 0, HEIGHT / 2 };
    //cairo_transform(cr, &matrix);

    cairo_arc(cr, WIDTH / 2, HEIGHT / 2, WIDTH / 4, 0, 2 * M_PI);
    cairo_path_t* path = cairo_copy_path_flat(cr);

    cairo_path_data_t* data;

    std::cout << "0: " << path->num_data << '\n';

    double xbegin, ybegin, x, y;

    std::vector<std::pair<double, double>> polyline{};
    std::vector<std::vector<std::pair<double, double>>> lines{};

    for (int i = 0; i < path->num_data; i += path->data[i].header.length) {
        data = &path->data[i];
        switch (data->header.type) {
        case CAIRO_PATH_MOVE_TO:
            std::cout << data[1].point.x << ", " << data[1].point.y << '\n';
            polyline.clear();
            polyline.push_back({ data[1].point.x, data[1].point.y });
            x = xbegin = data[1].point.x;
            y = ybegin = data[1].point.y + HEIGHT / 2;
            break;
        case CAIRO_PATH_LINE_TO:
            //boundaryLine(x, y, data[1].point.x, data[1].point.y + HEIGHT / 2);
            polyline.push_back({ data[1].point.x, data[1].point.y });
            x = data[1].point.x;
            y = data[1].point.y + HEIGHT / 2;
            break;
        case CAIRO_PATH_CURVE_TO:
            std::cout << "curve!\n";
            break;
        case CAIRO_PATH_CLOSE_PATH:
            //boundaryLine(x, y, xbegin, ybegin);
            std::cout << "close!\n";
            break;
        }
    }
    if (!polyline.empty())
        lines.push_back(polyline);

    // Draw original path
    cairo_set_source_rgb(cr, 1, 1, 1);

    cairo_set_line_width(cr, 3.);
    cairo_stroke(cr);

    cairo_arc_negative(cr, WIDTH / 2, HEIGHT / 2, 3 * WIDTH / 16, 2 * M_PI, 0);
    path = cairo_copy_path_flat(cr);

    std::cout << "1: " << path->num_data << '\n';

    for (int i = 0; i < path->num_data; i += path->data[i].header.length) {
        data = &path->data[i];
        switch (data->header.type) {
        case CAIRO_PATH_MOVE_TO:
            std::cout << data[1].point.x << ", " << data[1].point.y << '\n';
            polyline.clear();
            polyline.push_back({ data[1].point.x, data[1].point.y });
            x = xbegin = data[1].point.x;
            y = ybegin = data[1].point.y + HEIGHT / 2;
            break;
        case CAIRO_PATH_LINE_TO:
            //boundaryLine(x, y, data[1].point.x, data[1].point.y + HEIGHT / 2);
            polyline.push_back({ data[1].point.x, data[1].point.y });
            x = data[1].point.x;
            y = data[1].point.y + HEIGHT / 2;
            break;
        case CAIRO_PATH_CURVE_TO:
            std::cout << "curve!\n";
            break;
        case CAIRO_PATH_CLOSE_PATH:
            //boundaryLine(x, y, xbegin, ybegin);
            std::cout << "close!\n";
            break;
        }
    }
    if (!polyline.empty())
        lines.push_back(polyline);

    // Draw original path
    cairo_set_source_rgb(cr, 1, 1, 1);

    cairo_set_line_width(cr, 3.);
    cairo_stroke(cr);

    cairo_surface_write_to_png(surface, "signed-areas.png");

    cairo_path_destroy(path);
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
}

