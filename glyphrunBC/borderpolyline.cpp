#include <vector>
#include <array>
#include <cmath>
#include <stdexcept>

#include "borderpolyline.h"

template <typename T>
struct V2D
{
	V2D(T x, T y) : x(x), y(y) {}
	T x;
	T y;
	T len2() { return x * x + y * y; }
	T len() { return sqrt(x * x + y * y); };
	T operator*(V2D other) { return x * other.x + y * other.y; };
};

template <typename T>
V2D<T> operator*(T factor, const V2D<T>& vec)
{
	return vec * factor;
}

static double fpart(double x)
{
	return x - std::floor(x);
}

static double rfpart(double x)
{
	return std::ceil(x) - x;
}

static double rfpart1_0(double x)
{
	return 1.0 - x + std::floor(x);
}

static void setpixel(int x, int y, double c, std::vector<std::pair<std::pair<int, int>, double>>& va, array2D<double>& fld)
{
	va.emplace_back(std::make_pair(std::make_pair(x, y), c));
	auto f = fld(x, y);
	if (std::isnan(f) || (f >= 0 && c >= 0 && f < c) || (f <= 0 && c <= 0 && f > c))
		fld(x, y) = c;
}

void drawEdge(double x1, double y1, double x2, double y2, std::vector<std::pair<std::pair<int, int>, double>>& va, array2D<double>& fld)
{

	bool swapped = false;
	bool backward = false;

	double yisect, cosgrad;

	int ix1, iy1, ix2;

	double dx = x2 - x1;
	double dy = y2 - y1;
	if (dx == 0 && dy == 0)
	{
		return;
	}
	if (std::abs(dx) < std::abs(dy))
	{
		std::swap(x1, y1);
		std::swap(x2, y2);
		std::swap(dx, dy);
		swapped = true;
	}
	double gradient = gradient = dy / dx;

	if (x2 < x1)
	{
		backward = true;
		gradient = -gradient;
		ix1 = static_cast<int>(std::floor(x1));
		ix2 = static_cast<int>(std::ceil(x2));
		yisect = y1 + gradient * fpart(x1); // y axis intersect by line @ix1
		cosgrad = -1.0 / sqrt(1.0 + gradient * gradient);
	}
	else
	{
		ix1 = static_cast<int>(std::ceil(x1));
		ix2 = static_cast<int>(std::floor(x2));
		yisect = y1 + gradient * rfpart(x1); // y axis intersect by line @ix1
		cosgrad = 1.0 / sqrt(1.0 + gradient * gradient);
	}

	iy1 = static_cast<int>(yisect);

	double intery = yisect;

	// EP1
	V2D<double> linevec(x2 - x1, y2 - y1), eppxl(ix1 - x1, iy1 - y1);
	auto proj = linevec * eppxl;
	int iintery = static_cast<int>(intery);
	if (swapped)
	{
		V2D<double> linevec(y2 - y1, x2 - x1), eppxl(iy1 - y1, ix1 - x1);
		auto proj = linevec * eppxl;
		if (proj >= 0)
		{
			setpixel(iintery, ix1, -fpart(intery) * cosgrad, va, fld);
		}
		else
		{
			auto dist = sqrt(iintery - y1) * (iintery - y1) + (ix1 - x1) * (ix1 - x1);
			dist = copysign(dist, -fpart(intery) * cosgrad);
			setpixel(iintery, ix1, dist, va, fld);
		}
		eppxl.y += 1.0;
		proj = linevec * eppxl;
		if (proj >= 0)
		{
			setpixel(iintery + 1, ix1, rfpart1_0(intery) * cosgrad, va, fld);
		}
		else
		{
			auto dist = sqrt(iintery + 1 - y1) * (iintery + 1 - y1) + (ix1 - x1) * (ix1 - x1);
			dist = copysign(dist, rfpart1_0(intery) * cosgrad);
		}
	}
	else
	{
		V2D<double> linevec(x2 - x1, y2 - y1), eppxl(ix1 - x1, iy1 - y1);
		auto proj = linevec * eppxl;
		if (proj >= 0)
		{
			setpixel(ix1, iintery, fpart(intery) * cosgrad, va, fld);
		}
		else
		{
			auto dist = sqrt(iintery - y1) * (iintery - y1) + (ix1 - x1) * (ix1 - x1);
			dist = copysign(dist, fpart(intery) * cosgrad);
			setpixel(ix1, iintery, dist, va, fld);
		}
		eppxl.y += 1.0;
		proj = linevec * eppxl;
		if (proj >= 0)
		{
			setpixel(ix1, iintery + 1, -rfpart1_0(intery) * cosgrad, va, fld);
		}
		else
		{
			auto dist = sqrt(iintery + 1 - y1) * (iintery + 1 - y1) + (ix1 - x1) * (ix1 - x1);
			dist = copysign(dist, -rfpart1_0(intery) * cosgrad);
			setpixel(ix1, iintery, dist, va, fld);
		}
	}
	intery += gradient;

	// Add all the points between the endpoints /// SET PXL VALUE SIGNS!!!
	if (backward)
	{
		for (int x = ix1 - 1; x >= ix2 + 1; --x)
		{
			int iintery = static_cast<int>(intery);
			if (swapped)
			{
				setpixel(iintery, x, -fpart(intery) * cosgrad, va, fld);
				setpixel(iintery + 1, x, rfpart1_0(intery) * cosgrad, va, fld);
			}
			else
			{
				setpixel(x, iintery, fpart(intery) * cosgrad, va, fld);
				setpixel(x, iintery + 1, -rfpart1_0(intery) * cosgrad, va, fld);
			}
			intery += gradient;
		}
	}
	else
	{
		for (int x = ix1 + 1; x <= ix2 - 1; ++x)
		{
			int iintery = static_cast<int>(intery);
			if (swapped)
			{
				setpixel(iintery, x, -fpart(intery) * cosgrad, va, fld);
				setpixel(iintery + 1, x, rfpart1_0(intery) * cosgrad, va, fld);
			}
			else
			{
				setpixel(x, iintery, fpart(intery) * cosgrad, va, fld);
				setpixel(x, iintery + 1, -rfpart1_0(intery) * cosgrad, va, fld);
			}
			intery += gradient;
		}
	}

	// EP2
	iintery = static_cast<int>(intery);
	int iy2 = static_cast<int>(y2); ///???

	V2D<double> linevecc(x1 - x2, y1 - y2), eppxlc(ix2 - x2, iy2 - y2);
	proj = linevecc * eppxlc;

	if (swapped)
	{
		V2D<double> linevec(y1 - y2, x1 - x2), eppxl(iy2 - y2, ix2 - x2);
		auto proj = linevec * eppxl;
		if (proj >= 0)
		{
			setpixel(iintery, ix2, -fpart(intery) * cosgrad, va, fld);
		}
		else
		{
			auto dist = sqrt(iintery - y2) * (iintery - y2) + (ix2 - x2) * (ix2 - x2);
			dist = copysign(dist, -fpart(intery) * cosgrad);
			setpixel(iintery, ix2, dist, va, fld);
		}
		eppxl.y += 1.0;
		proj = linevec * eppxl;
		if (proj >= 0)
		{
			setpixel(iintery + 1, ix2, rfpart1_0(intery) * cosgrad, va, fld);
		}
		else
		{
			auto dist = sqrt(iintery + 1 - y2) * (iintery + 1 - y2) + (ix2 - x2) * (ix2 - x2);
			dist = copysign(dist, rfpart1_0(intery) * cosgrad);
			setpixel(iintery + 1, ix2, dist, va, fld);
		}
	}
	else
	{
		V2D<double> linevec(x1 - x2, y1 - y2), eppxl(ix2 - x2, iy2 - y2);
		auto proj = linevec * eppxl;
		if (proj >= 0)
		{
			setpixel(ix2, iintery, fpart(intery) * cosgrad, va, fld);
		}
		else
		{
			auto dist = sqrt(iintery - y2) * (iintery - y2) + (ix2 - x2) * (ix2 - x2);
			dist = copysign(dist, fpart(intery) * cosgrad);
			setpixel(ix2, iintery, dist, va, fld);
		}
		eppxl.y += 1.0;
		proj = linevec * eppxl;
		if (proj >= 0)
		{
			setpixel(ix2, iintery + 1, -rfpart1_0(intery) * cosgrad, va, fld);
		}
		else
		{
			auto dist = sqrt(iintery + 1 - y2) * (iintery + 1 - y2) + (ix2 - x2) * (ix2 - x2);
			dist = copysign(dist, -rfpart1_0(intery) * cosgrad);
			setpixel(ix2, iintery + 1, dist, va, fld);
		}
	}
	intery += gradient;
}

void closureBorderline(std::vector<std::pair<double, double>>& vertices, array2D<double>& fld)
{
	for (auto vertex : vertices)
	{
		double x = vertex.first, y = vertex.second;
		int ix = static_cast<int>(x), iy = static_cast<int>(y);
		if (std::isnan(fld(ix, iy)))
			fld(ix, iy) = -sqrt((ix - x) * (ix - x) + (iy - y) * (iy - y));
		if (std::isnan(fld(ix + 1, iy)))
			fld(ix + 1, iy) = -sqrt((ix + 1 - x) * (ix + 1 - x) + (iy - y) * (iy - y));
		if (std::isnan(fld(ix, iy + 1)))
			fld(ix, iy + 1) = -sqrt((ix - x) * (ix - x) + (iy + 1 - y) * (iy + 1 - y));
		if (std::isnan(fld(ix + 1, iy + 1)))
			fld(ix + 1, iy + 1) = -sqrt((ix + 1 - x) * (ix + 1 - x) + (iy + 1 - y) * (iy + 1 - y));
	}
}