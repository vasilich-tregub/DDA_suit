// signed-areas.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <cairo/cairo.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>

#include "signed-areas.h"
#include "display.h"

constexpr int WIDTH = 2048;
constexpr int HEIGHT = 2048;

double fld[WIDTH * HEIGHT]{};

void setPixel(int x, int y, double Y = 0)
{
    if (std::isnan(fld[x + y * WIDTH]) || (abs(Y) < abs(fld[x + y * WIDTH])))
    {
        fld[x + y * WIDTH] = Y;
    }
}

enum celledge { LEFT, BOTTOM, RIGHT, TOP };
struct inode { int x; int y; celledge entry; }; // left/bottom coords + the border where the line enters the cell
struct vec
{
    double x; double y;
    vec operator*(double z)
    {
        return { x * z, y * z };
    };
    vec operator/(double z)
    {
        return { x / z, y / z };
    };
};
double cross(vec v1, vec v2) // z-component of cross product, I use it to compute signed distance
{
    return (v1.x * v2.y - v1.y * v2.x);
}
vec tricross(vec v1, vec v2, vec v3) // triple cross product, I use it to find projection point, to verify if it falls inside line segment
{
    return { (v2.x * v3.y - v2.y * v3.x) * v1.y, (v2.y * v3.x - v2.x * v3.y) * v1.x };
}

vec priorlinevec{ 0, 0 }; // TODO: replace 'priorlinevec' tinkering with a test on position of a foot of normal vector from a current node to the line

void boundaryLine(double x0, double y0, double x1, double y1)
{
    if (x0 == x1 && y0 == y1)
        return;
    int ix0 = (int)std::floor(x0);
    int iy0 = (int)std::floor(y0);
    int ix1 = (int)std::floor(x1);
    int iy1 = (int)std::floor(y1);
    vec linevec = { (x1 - x0), (y1 - y0) };
    double seglen2 = linevec.x * linevec.x + linevec.y * linevec.y;// dx* dx + dy * dy;
    double seglen = sqrt(seglen2);
    inode nextnode{};
    // 1st endpoint
    double convexconcave = copysign(1.0, cross(priorlinevec, linevec));
    for (int j = 0; j < 2; ++j)
        for (int i = 0; i < 2; ++i)
        {
            //std::cout << i << j << std::endl;
            vec nv0{ (ix0 + i - x0), (iy0 + j - y0) }; // 2D vector from {ix0 + i, iy0 + j} to {x0,y0}
            // cross product, the (signed) area and the distance to the line (linevec/seglen is unit vector)
            double d2l = cross(nv0, linevec) / seglen;
            //std::cout << "d2l: " << d2l << std::endl;
            // triple cross product and the vector to nv0 proj to linevec wrt {ix0+i,iy0+j}
            vec proj = tricross(linevec, linevec, nv0) / seglen2;
            //std::cout << proj.x << ", " << proj.y << std::endl;
            if (((proj.x < (x0 - ix0 - i) && proj.x < (x1 - ix0 - i)) || (proj.x > (x0 - ix0 - i) && proj.x > (x1 - ix0 - i))) ||
                ((proj.y < (y0 - iy0 - j) && proj.y < (y1 - iy0 - j)) || (proj.y > (y0 - iy0 - j) && proj.y > (y1 - iy0 - j))))
            {

                double d2lsign = copysign(1.0, d2l);
                if (d2lsign != convexconcave)
                    continue; // to skip, as the node with a normal vector's foot situated within the segment will be processed in the between-eps iteration
                d2l = copysign(
                    sqrt((x0 - ix0 - i) * (x0 - ix0 - i) + (y0 - iy0 - j) * (y0 - iy0 - j)),
                    d2l);
            }
            //std::cout << "d2ep0: " << d2l << std::endl;
            setPixel(ix0 + i, iy0 + j, d2l);
        }
    // 2nd endpoint
    for (int j = 0; j < 2; ++j)
        for (int i = 0; i < 2; ++i)
        {
            //std::cout << i << j << std::endl;
            vec nv1{ (ix1 + i - x1), (iy1 + j - y1) }; // 2D vector from {ix1 + i, iy1 + j} to {x1,y1}
            // cross product, the (signed) area and the distance to the line (linevec/seglen is unit vector)
            double d2l = cross(nv1, linevec) / seglen;
            //std::cout << "d2l: " << d2l << std::endl;
            // triple cross product and the vector to nv0 proj to linevec wrt {ix0+i,iy0+j}
            vec proj = tricross(linevec, linevec, nv1) / seglen2;
            //std::cout << proj.x << ", " << proj.y << std::endl;
            if (((proj.x < (x1 - ix1 - i) && proj.x < (x0 - ix1 - i)) || (proj.x > (x1 - ix1 - i) && proj.x > (x0 - ix1 - i))) ||
                ((proj.y < (y1 - iy1 - j) && proj.y < (y0 - iy1 - j)) || (proj.y > (y1 - iy1 - j) && proj.y > (y0 - iy1 - j))))
            {
                double d2lsign = copysign(1.0, d2l);
                if (d2lsign != convexconcave)
                    continue;
                d2l = copysign(
                    sqrt((x0 - ix0 - i) * (x0 - ix0 - i) + (y0 - iy0 - j) * (y0 - iy0 - j)),
                    d2l);
            }
            //std::cout << "d2ep0: " << d2l << std::endl;
            setPixel(ix1 + i, iy1 + j, d2l);
        }

    if (ix0 == ix1 && iy0 == iy1)
        return;
    // connecting line
    // line intersection with grid
    double yintersect = HEIGHT;
    double xintersect = WIDTH;
    bool intersectfound = false;
    if (linevec.x != 0)
    {
        // check if left boundary is crossed EXCEPTIONAL CONDITION STRICTLY LESS !!!!!!!!!!!
        if ((x1 <= ix0 && ix0 <= x0))
        {
            yintersect = linevec.y / linevec.x * (ix0 - x0) + y0;
            xintersect = ix0;
            if (yintersect >= iy0 && yintersect <= iy0 + 1)
            {
                intersectfound = true;
                nextnode = { ix0 - 1, iy0, RIGHT };
            }
        }
        // check if right boundary is crossed
        if (!intersectfound && (x0 <= ix0 + 1 && ix0 + 1 <= x1))
        {
            yintersect = linevec.y / linevec.x * (ix0 + 1 - x0) + y0;
            xintersect = ix0 + 1;
            if (yintersect >= iy0 && yintersect <= iy0 + 1)
            {
                intersectfound = true;
                nextnode = { ix0 + 1, iy0, LEFT };
            }
        }
        if (!intersectfound)
        {
            xintersect = WIDTH;
            yintersect = HEIGHT;
            nextnode = { WIDTH, HEIGHT };
        }
    }
    if (linevec.y != 0)
    {
        // check if bottom boundary is crossed EXCEPTIONAL CONDITION STRICTLY LESS !!!!!!!!!!!
        if ((y1 <= iy0 && iy0 <= y0))
        {
            xintersect = linevec.x / linevec.y * (iy0 - y0) + x0;
            yintersect = iy0;
            if (xintersect >= ix0 && xintersect < ix0 + 1)
            {
                intersectfound = true;
                nextnode = { ix0, iy0 - 1, TOP };
            }
        }
        // check if top boundary is crossed
        if (!intersectfound && ((y0 <= iy0 + 1 && iy0 + 1 <= y1)))
        {
            xintersect = linevec.x / linevec.y * (iy0 + 1 - y0) + x0;
            yintersect = iy0 + 1;
            if (xintersect >= ix0 && xintersect <= ix0 + 1)
            {
                intersectfound = true;
                nextnode = { ix0, iy0 + 1, BOTTOM };
            }
        }
        if (!intersectfound)
        {
            xintersect = WIDTH;
            yintersect = HEIGHT;
            nextnode = { WIDTH, HEIGHT };
        }
    }
    //std::cout << "Next node: " << nextnode.x << " " << nextnode.y << std::endl;

    // compute pixels between endpoints
    // 
    while ((linevec.x > 0 && nextnode.x < ix1) || (linevec.x < 0 && nextnode.x > ix1) ||
        (linevec.y > 0 && nextnode.y < iy1) || (linevec.y < 0 && nextnode.y > iy1))
    {
        int ix = nextnode.x, iy = nextnode.y;
        celledge edge = nextnode.entry;

        auto nodepair = std::pair<inode, inode>{ nextnode, nextnode };
        switch (edge)
        {
        case LEFT:
            nodepair.second = { ix, iy + 1 };
            break;
        case BOTTOM:
            nodepair.second = { ix + 1, iy };
            break;
        case RIGHT:
            nodepair = { {ix + 1, iy}, {ix + 1, iy + 1} };
            break;
        case TOP:
            nodepair = { {ix, iy + 1}, {ix + 1, iy + 1} };
            break;
        }

        for (inode sidenode : {nodepair.first, nodepair.second})
        {
            //TODO: skip the node of a pair that's been already visited in the preceding step
            vec nv{ (sidenode.x - x0), (sidenode.y - y0) }; // 2D vector from {ix + i, iy + j} to {x0,y0}
            // cross product, the (signed) area and the distance to the line (linevec/seglen is unit vector)
            double d2l = cross(nv, linevec) / seglen;
            setPixel(sidenode.x, sidenode.y, d2l);
        }
        intersectfound = false;
        if (linevec.x != 0)
        {
            if ((edge != LEFT) && ((x0 < ix && ix <= x1)))
            {
                yintersect = linevec.y / linevec.x * (ix - x0) + y0;
                xintersect = ix;
                if (yintersect > iy && yintersect < iy + 1)
                {
                    intersectfound = true;
                    nextnode = { ix - 1, iy, RIGHT };
                }
            }
            if ((edge != LEFT) && ((x1 <= ix && ix <= x0)))
            {
                yintersect = linevec.y / linevec.x * (ix - x0) + y0;
                xintersect = ix;
                if (yintersect >= iy && yintersect <= iy + 1)
                {
                    intersectfound = true;
                    nextnode = { ix - 1, iy, RIGHT };
                }
            }
            // check if right boundary is crossed
            if (!intersectfound && ((edge != RIGHT) && ((x0 <= ix + 1 && ix + 1 <= x1))))
            {
                yintersect = linevec.y / linevec.x * (ix + 1 - x0) + y0;
                xintersect = ix + 1;
                if (yintersect >= iy && yintersect <= iy + 1)
                {
                    intersectfound = true;
                    nextnode = { ix + 1, iy, LEFT };
                }
            }
            if (!intersectfound && ((edge != RIGHT) && ((x1 <= ix + 1 && ix + 1 <= x0))))
            {
                yintersect = linevec.y / linevec.x * (ix + 1 - x0) + y0;
                xintersect = ix + 1;
                if (yintersect > iy && yintersect < iy + 1)
                {
                    intersectfound = true;
                    nextnode = { ix + 1, iy, LEFT };
                }
            }
            if (!intersectfound)
            {
                xintersect = WIDTH;
                yintersect = HEIGHT;
                nextnode = { WIDTH, HEIGHT };
            }
        }
        if (linevec.y != 0)
        {
            // check if bottom boundary is crossed EXCEPTIONAL CONDITION STRICTLY LESS !!!!!!!!!!!
            if ((edge != BOTTOM) && ((y0 < iy && iy <= y1)))
            {
                xintersect = linevec.x / linevec.y * (iy - y0) + x0;
                yintersect = iy;
                if (xintersect > ix && xintersect < ix + 1)
                {
                    intersectfound = true;
                    nextnode = { ix, iy - 1, TOP };
                }
            }
            if ((edge != BOTTOM) && ((y1 <= iy && iy <= y0)))
            {
                xintersect = linevec.x / linevec.y * (iy - y0) + x0;
                yintersect = iy;
                if (xintersect >= ix && xintersect <= ix + 1)
                {
                    intersectfound = true;
                    nextnode = { ix, iy - 1, TOP };
                }
            }
            // check if top boundary is crossed
            if (!intersectfound && ((edge != TOP) && ((y0 <= iy + 1 && iy + 1 <= y1))))
            {
                xintersect = linevec.x / linevec.y * (iy + 1 - y0) + x0;
                yintersect = iy + 1;
                if (xintersect >= ix && xintersect <= ix + 1)
                {
                    intersectfound = true;
                    nextnode = { ix, iy + 1, BOTTOM };
                }
            }
            if (!intersectfound && ((edge != TOP) && ((y1 <= iy + 1 && iy + 1 <= y0))))
            {
                xintersect = linevec.x / linevec.y * (iy + 1 - y0) + x0;
                yintersect = iy + 1;
                if (xintersect > ix && xintersect < ix + 1)
                {
                    intersectfound = true;
                    nextnode = { ix, iy + 1, BOTTOM };
                }
            }
            if (!intersectfound)
            {
                xintersect = WIDTH;
                yintersect = HEIGHT;
                nextnode = { WIDTH, HEIGHT };
            }
        }
        //std::cout << "Next node: " << nextnode.x << " " << nextnode.y << std::endl;
    }
    priorlinevec = linevec;
}

int main()
{
    cairo_surface_t* surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, WIDTH, HEIGHT);
    cairo_t* cr = cairo_create(surface);
    //cairo_set_tolerance(cr, 0.1);
    //cairo_matrix_t matrix{ 1, 0, 0, 1, 0, HEIGHT / 2 };
    //cairo_transform(cr, &matrix);

    cairo_arc(cr, WIDTH / 2, HEIGHT / 2, WIDTH / 4, 0, 2 * M_PI);
    cairo_new_sub_path(cr);
    cairo_arc_negative(cr, WIDTH / 2, HEIGHT / 2, 3 * WIDTH / 16, 2 * M_PI, 0);

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
    if (!polyline.empty())
        lines.push_back(polyline);
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

    constexpr double nan = std::numeric_limits<double>::quiet_NaN();
    for (int ix = 0; ix < WIDTH * HEIGHT; ++ix) fld[ix] = nan;
    double polyarea = 0;
    for (auto poly : lines)
    {
        polyarea = 0;
        for (int i = 1; i < poly.size(); ++i)
        {
            boundaryLine(poly[i - 1].first, poly[i - 1].second, poly[i].first, poly[i].second);
            polyarea += (poly[i].first - poly[i - 1].first) * (poly[i].second + poly[i - 1].second);
        }
        polyarea += (poly[0].first - poly[poly.size() - 1].first) * (poly[0].second - poly[poly.size() - 1].second);
        std::cout << polyarea << std::endl;
    }
    display(WIDTH, HEIGHT, fld, L"signed-areas-BC.png");

    double fldvalsign = 1.0; // sqrt(2);
    for (int Y = 0; Y < HEIGHT; ++Y)
    {
        for (int X = 0; X < WIDTH; ++X)
        {
            int iX = Y * WIDTH + X;
            if (!std::isnan(fld[iX]) && fld[iX] * fldvalsign <= 0) // test for sign flip only
            {
                fldvalsign = -fldvalsign;
            }
            if (std::isnan(fld[iX]))
            {
                fld[iX] = fldvalsign;
            }
        }
        fldvalsign = 1.0; // sqrt(2);
    }
    display(WIDTH, HEIGHT, fld, L"signed-areas-filled.png");

    int* distance = new int[WIDTH * HEIGHT];

    int DIST_MAX = WIDTH * WIDTH + HEIGHT * HEIGHT;


    for (int Y = 0; Y < HEIGHT; ++Y)
    {
        for (int X = 0; X < WIDTH; ++X)
        {
            distance[Y * WIDTH + X] = DIST_MAX;
        }
    }
    //distance[HEIGHT / 2 * WIDTH + WIDTH / 2] = 0;
    for (int Y = 0; Y < HEIGHT; ++Y)
    {
        for (int X = 0; X < WIDTH; ++X)
        {
            if (fld[Y * WIDTH + X] > 0) distance[Y * WIDTH + X] = DIST_MAX; else distance[Y * WIDTH + X] = 0;
            /*if ((X - WIDTH / 4) * (X - WIDTH / 4) + (Y - 3 * HEIGHT / 4) * (Y - 3 * HEIGHT / 4) < WIDTH * WIDTH / 64) distance[Y * WIDTH + X] = 0;
            if ((X - 3 * WIDTH / 4) * (X - 3 * WIDTH / 4) + (Y - HEIGHT / 4) * (Y - HEIGHT / 4) < WIDTH * WIDTH / 64) distance[Y * WIDTH + X] = 0;
            ////if ((X - WIDTH / 2) * (X - WIDTH / 2) + (Y - HEIGHT / 2) * (Y - HEIGHT / 2) < WIDTH * WIDTH / 64) distance[Y * WIDTH + X] = 0;
            //if (X > WIDTH / 8 && X < 7 * WIDTH / 8 && Y == HEIGHT / 2) distance[Y * WIDTH + X] = 0;*/
        }
    }

    signed_areas(WIDTH, HEIGHT, distance);
    for (int ix = 0; ix < WIDTH * HEIGHT; ++ix) fld[ix] = sqrt(sqrt(distance[ix]));
    double maxfld = 0;
    for (int ix = 0; ix < WIDTH * HEIGHT; ++ix) maxfld = (fld[ix] > maxfld) ? fld[ix] : maxfld; 
    //for (int ix = 0; ix < width; ++ix) fld[ix] = 0;
    //for (int ix = 0; ix < width; ++ix) fld[height * width + ix] = 0;
    for (int ix = 0; ix < WIDTH * HEIGHT; ++ix) { if (fld[ix] != 0) fld[ix] = maxfld - fld[ix]; else fld[ix] = -maxfld / 2.0; }/**/

    display(WIDTH, HEIGHT, fld, L"signed-areas-distance.png");

}

