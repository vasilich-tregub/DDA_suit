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

	if (std::abs(dx) < std::abs(dy))
	{
		std::swap(x1, y1);
		std::swap(x2, y2);
		std::swap(dx, dy);
		swapped = true;
	}
	if (x2 < x1)
	{
		//std::swap(x1, x2); // NEGATE 'gradient;
		//std::swap(y1, y2); // and swap EP1, EP2
		backward = true;
	}
	double gradient = dy / dx;
	double cosgrad = 1.0 / sqrt(1.0 + gradient * gradient);
	double xgap = rfpart(x1) * cosgrad; ///// compute the projection point and decide between the normal to line and distance to ep

	int ix1 = static_cast<int>(x1);
	double yisect = y1 - gradient * fpart(x1); // y axis intersect by line @ix1
	int iy1 = static_cast<int>(yisect);
	/// DO PROCESS 2nd ENDPOINT
	int ix2 = static_cast<int>(x2) + 1;
	double yisect_ep2 = y2 + gradient * fpart(x1); // y axis intersect by line @ix2
	int iy2 = static_cast<int>(yisect_ep2);
	/// for now, it is valid only when backward,swap == false
	V2D<double> linevec(dx, dy);
	auto seglen2 = linevec.len2();
	double seglen = sqrt(seglen2);
	V2D<double> nv1(x1 - ix1, y1 - iy1); // vector to node, cf. nv1.len() is a distance to node
	// cross product, the (signed) area and the distance to the line (linevec/seglen is unit vector)
	auto dist1 = linevec.cross(nv1) / seglen;
	auto foot = linevec.tricross(linevec, nv1) / seglen2;
	if ((!backward && (foot.x < nv1.x)) || (backward && (foot.x > nv1.x)))
	{
		dist1 = copysign(nv1.len(), dist1);
	}
	
	V2D<double> nv1a(x1 - ix1, y1 - (iy1 + 1));
	auto dist1a = linevec.cross(nv1a) / seglen;
	foot = linevec.tricross(linevec, nv1a) / seglen2;
	if ((!backward && (foot.x < nv1a.x)) || (backward && (foot.x > nv1a.x)))
	{
		dist1a = copysign(nv1a.len(), dist1a);
	}

	// Add the first endpoint
	if (swapped)
	{		
		setpixel(va, iy1, ix1, dist1); // or setpixel(va, iy1, ix1, d2l);?
		setpixel(va, iy1 + 1, ix1, dist1a); // etc..
	}
	else
	{
		setpixel(va, ix1, iy1, dist1); // etc..
		setpixel(va, ix1, iy1 + 1, dist1a);
	}

	double intery = yisect + gradient;


	// Add all the points between the endpoints /// SET PXL VALUE SIGNS!!!
	if (backward)
	{
		for (int x = ix1 - 1; x >= ix2 + 1; --x)
		{
			if (swapped)
			{
				setpixel(va, static_cast<int>(intery), x, fpart(intery) * cosgrad);
				setpixel(va, static_cast<int>(intery) + 1, x, -rfpart(intery) * cosgrad);
			}
			else
			{
				setpixel(va, x, static_cast<int>(intery), fpart(intery) * cosgrad);
				setpixel(va, x, static_cast<int>(intery) + 1, -rfpart(intery) * cosgrad);
			}
			intery += gradient;
		}
	}
	else
	{
		for (int x = ix1 + 1; x <= ix2 - 1; ++x)
		{
			if (swapped)
			{
				setpixel(va, static_cast<int>(intery), x, fpart(intery) * cosgrad);
				setpixel(va, static_cast<int>(intery) + 1, x, -rfpart(intery * cosgrad));
			}
			else
			{
				setpixel(va, x, static_cast<int>(intery), fpart(intery) * cosgrad);
				setpixel(va, x, static_cast<int>(intery) + 1, -rfpart(intery) * cosgrad);
			}
			intery += gradient;
		}
	}

	// Add the second endpoint // the calling procedure computes the signs of ep pxls and adjusts the values(?)
	V2D<double> nv2(x2 - ix2, y2 - iy2); // vector to node, cf. nv1.len() is a distance to node
	// cross product, the (signed) area and the distance to the line (linevec/seglen is unit vector)
	auto dist2 = linevec.cross(nv2) / seglen;
	foot = linevec.tricross(linevec, nv2) / seglen2;
	if ((!backward && (foot.x > nv2.x)) || (backward && (foot.x < nv2.x)))
	{
		dist2 = copysign(nv2.len(), dist2);
	}

	V2D<double> nv2a(x2 - ix2, y2 - (iy2 + 1));
	auto dist2a = linevec.cross(nv2a) / seglen;
	foot = linevec.tricross(linevec, nv2a) / seglen2;
	if ((!backward && (foot.x > nv2a.x)) || (backward && (foot.x < nv2a.x)))
	{
		dist2a = copysign(nv2a.len(), dist2a);
	}

	if (swapped)
	{
		setpixel(va, iy2, ix2, dist2); // or setpixel(va, iy1, ix1, d2l);?
		setpixel(va, iy2 + 1, ix2, dist2a); // etc..
	}
	else
	{
		setpixel(va, ix2, iy2, dist2);
		setpixel(va, ix2, iy2 + 1, dist2a);
	}
}
