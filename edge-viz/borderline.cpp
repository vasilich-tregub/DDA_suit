#include <vector>
#include <cmath>

template <typename T>
struct V2D
{
	V2D(T x, T y) : x(x), y(y) {}
	T x;
	T y;
	T len2() { return x * x + y * y; }
	T len() { return sqrt(x * x + y * y); };
	V2D operator/(T divisor) { return V2D(x / divisor, y / divisor); };
	V2D operator*(T factor) { return V2D(x * factor, y * factor); };
	T cross(V2D other) { return x * other.y - y * other.x; };
	V2D tricross(V2D vec2, V2D vec3) 
	{
		return V2D(
			y * (vec2.x * vec3.y - vec2.y * vec3.x),
			x * (vec2.y * vec3.x - vec2.x * vec3.y)
		);
	}
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

void drawLine(double x1, double y1, double x2, double y2, std::vector<std::pair<std::pair<int, int>, double>>& va)
{
	double dx = x2 - x1;
	double dy = y2 - y1;

	bool swapped = false;
	bool backward = false;

	double yisect, cosgrad;
	double gradient = dy / dx;

	int ix1, iy1, ix2;

	if (std::abs(dx) < std::abs(dy))
	{
		std::swap(x1, y1);
		std::swap(x2, y2);
		std::swap(dx, dy);
		swapped = true;
	}
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


	// Add all the points between the endpoints /// SET PXL VALUE SIGNS!!!
	if (backward)
	{
		for (int x = ix1; x >= ix2; --x)
		{
			if (swapped)
			{
				setpixel(va, static_cast<int>(intery), x, -fpart(intery) * cosgrad);
				setpixel(va, static_cast<int>(intery) + 1, x, rfpart1_0(intery) * cosgrad);
			}
			else
			{
				setpixel(va, x, static_cast<int>(intery), fpart(intery) * cosgrad);
				setpixel(va, x, static_cast<int>(intery) + 1, -rfpart1_0(intery) * cosgrad);
			}
			intery += gradient;
		}
	}
	else
	{
		for (int x = ix1; x <= ix2; ++x)
		{
			if (swapped)
			{
				setpixel(va, static_cast<int>(intery), x, -fpart(intery) * cosgrad);
				setpixel(va, static_cast<int>(intery) + 1, x, rfpart1_0(intery) * cosgrad);
			}
			else
			{
				setpixel(va, x, static_cast<int>(intery), fpart(intery) * cosgrad);
				setpixel(va, x, static_cast<int>(intery) + 1, -rfpart1_0(intery) * cosgrad);
			}
			intery += gradient;
		}
	}

}
