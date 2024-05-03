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

static void setpixel(std::vector<std::pair<std::pair<int, int>, double>>& va, int x, int y, double c)
{
	va.emplace_back(std::make_pair(std::make_pair(x, y), c));
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
		setpixel(va, -ix1, -static_cast<int>(intery), proj);
		eppxl.y += 1.0;
		proj = linevec * eppxl;
		setpixel(va, -ix1, -static_cast<int>(intery) - 1, proj);
		int iintery = static_cast<int>(intery);
		if (swapped)
		{
			V2D<double> linevec(y2 - y1, x2 - x1), eppxl(iy1 - y1, ix1 - x1);
			auto proj = linevec * eppxl;
			if (proj >= 0)
			{
				setpixel(va, iintery, ix1, -fpart(intery) * cosgrad);
				fld(iintery, ix1) = -fpart(intery) * cosgrad;
			}
			eppxl.y += 1.0;
			proj = linevec * eppxl;
			if (proj >= 0)
			{
				setpixel(va, iintery + 1, ix1, rfpart1_0(intery) * cosgrad);
				fld(iintery + 1, ix1) = rfpart(intery) * cosgrad;
			}
		}
		else
		{
			V2D<double> linevec(x2 - x1, y2 - y1), eppxl(ix1 - x1, iy1 - y1);
			auto proj = linevec * eppxl;
			if (proj >= 0)
			{
				setpixel(va, ix1, iintery, fpart(intery) * cosgrad);
				fld(ix1, iintery) = fpart(intery) * cosgrad;
			}
			eppxl.y += 1.0;
			proj = linevec * eppxl;
			if (proj >= 0)
			{
				setpixel(va, ix1, iintery + 1, -rfpart1_0(intery) * cosgrad);
				fld(ix1, iintery + 1) = -rfpart1_0(intery) * cosgrad;
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
				setpixel(va, iintery, x, -fpart(intery) * cosgrad);
				fld(iintery, x) = -fpart(intery) * cosgrad;
				setpixel(va, iintery + 1, x, rfpart1_0(intery) * cosgrad);
				fld(iintery + 1, x) = rfpart1_0(intery) * cosgrad;
			}
			else
			{
				setpixel(va, x, iintery, fpart(intery) * cosgrad);
				fld(x, iintery) = fpart(intery) * cosgrad;
				setpixel(va, x, iintery + 1, -rfpart1_0(intery) * cosgrad);
				fld(x, iintery + 1) = -rfpart1_0(intery) * cosgrad;
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
				setpixel(va, iintery, x, -fpart(intery) * cosgrad);
				fld(iintery, x) = -fpart(intery) * cosgrad;
				setpixel(va, iintery + 1, x, rfpart1_0(intery) * cosgrad);
				fld(iintery + 1, x) = rfpart1_0(intery) * cosgrad;
			}
			else
			{
				setpixel(va, x, iintery, fpart(intery) * cosgrad);
				fld(x, iintery) = fpart(intery) * cosgrad;
				setpixel(va, x, iintery + 1, -rfpart1_0(intery) * cosgrad);
				fld(x, iintery + 1) = -rfpart1_0(intery) * cosgrad;
			}
			intery += gradient;
		}
	}

	// EP2
	iintery = static_cast<int>(intery);
	int iy2 = static_cast<int>(y2); ///???

	V2D<double> linevecc(x1 - x2, y1 - y2), eppxlc(ix2 - x2, iy2 - y2);
	proj = linevecc * eppxlc;
	setpixel(va, -ix2, -static_cast<int>(intery), proj);
	eppxlc.y += 1.0;
	proj = linevecc * eppxlc;
	setpixel(va, -ix2, -static_cast<int>(intery) - 1, proj);

	if (swapped)
	{
		V2D<double> linevec(y1 - y2, x1 - x2), eppxl(iy2 - y2, ix2 - x2);
		auto proj = linevec * eppxl;
		if (proj >= 0)
		{
			setpixel(va, iintery, ix2, -fpart(intery) * cosgrad);
			fld(iintery, ix2) = -fpart(intery) * cosgrad;
		}
		eppxl.y += 1.0;
		proj = linevec * eppxl;
		if (proj >= 0)
		{
			setpixel(va, iintery + 1, ix2, rfpart1_0(intery) * cosgrad);
			fld(iintery + 1, ix2) = -fpart(intery) * cosgrad;
		}
	}
	else
	{
		V2D<double> linevec(x1 - x2, y1 - y2), eppxl(ix2 - x2, iy2 - y2);
		auto proj = linevec * eppxl;
		if (proj >= 0)
		{
			setpixel(va, ix2, iintery, fpart(intery) * cosgrad);
			fld(ix2, iintery) = fpart(intery) * cosgrad;
		}
		eppxl.y += 1.0;
		proj = linevec * eppxl;
		if (proj >= 0)
		{
			setpixel(va, ix2, iintery + 1, -rfpart1_0(intery) * cosgrad);
			fld(ix2, iintery + 1) = -rfpart(intery) * cosgrad;
		}
	}
	intery += gradient;
}
