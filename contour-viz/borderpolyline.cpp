#include <vector>
#include <array>
#include <cmath>
#include <stdexcept>

template <typename T>
struct array2D
{
	array2D(std::vector<T>& buffer, std::size_t W, std::size_t H)
		: buffer_(buffer), width_(W), height_(H)
	{ 
		if (buffer.size() < W * H)
		{
			throw std::invalid_argument("buffer too short");
		}
	}
	T& operator() (int col, int row) 
	{ 
		if (col < 0 || row < 0 || col >= width_ || row >= height_)
		{
			throw std::invalid_argument("invalid index");
			col = width_ - 1; row = height_ - 1;
		}
		return buffer_[col + row * width_]; 
	};
private:
	std::size_t width_;
	std::size_t height_;
	std::vector<T> buffer_;
};

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

static void setpixel(std::vector<std::pair<std::pair<int, int>, double>>& va, array2D<double>& fld, int x, int y, double c)
{
	va.emplace_back(std::make_pair(std::make_pair(x, y), c));
	fld(x, y) = c;
}

void drawLine(double x1, double y1, double x2, double y2, std::vector<std::pair<std::pair<int, int>, double>>& va, array2D<double>& fld)
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

	// EP1
	if (backward)
	{
		if (swapped)
		{
			setpixel(va, fld, static_cast<int>(intery), ix1, -fpart(intery) * cosgrad);
			setpixel(va, fld, static_cast<int>(intery) + 1, ix1, rfpart1_0(intery) * cosgrad);
		}
		else
		{
			setpixel(va, fld, ix1, static_cast<int>(intery), fpart(intery) * cosgrad);
			setpixel(va, fld, ix1, static_cast<int>(intery) + 1, -rfpart1_0(intery) * cosgrad);
		}
		intery += gradient;
	}
	else
	{
		/*V2D<double> linevec(x2 - x1, y2 - y1), eppxl(ix1 - x1, iy1 - y1);
		auto proj = linevec * eppxl;
		setpixel(va, fld, -ix1, -static_cast<int>(intery), proj);
		eppxl.y += 1.0;
		proj = linevec * eppxl;
		setpixel(va, fld, -ix1, -static_cast<int>(intery) - 1, proj);*/
		if (swapped)
		{
			setpixel(va, fld, static_cast<int>(intery), ix1, -fpart(intery) * cosgrad);
			setpixel(va, fld, static_cast<int>(intery) + 1, ix1, rfpart1_0(intery) * cosgrad);
		}
		else
		{
			setpixel(va, fld, ix1, static_cast<int>(intery), fpart(intery) * cosgrad);
			setpixel(va, fld, ix1, static_cast<int>(intery) + 1, -rfpart1_0(intery) * cosgrad);
		}
		intery += gradient;
	}

	// Add all the points between the endpoints /// SET PXL VALUE SIGNS!!!
	if (backward)
	{
		for (int x = ix1 - 1; x >= ix2 + 1; --x)
		{
			if (swapped)
			{
				setpixel(va, fld, static_cast<int>(intery), x, -fpart(intery) * cosgrad);
				setpixel(va, fld, static_cast<int>(intery) + 1, x, rfpart1_0(intery) * cosgrad);
			}
			else
			{
				setpixel(va, fld, x, static_cast<int>(intery), fpart(intery) * cosgrad);
				setpixel(va, fld, x, static_cast<int>(intery) + 1, -rfpart1_0(intery) * cosgrad);
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
				setpixel(va, fld, static_cast<int>(intery), x, -fpart(intery) * cosgrad);
				setpixel(va, fld, static_cast<int>(intery) + 1, x, rfpart1_0(intery) * cosgrad);
			}
			else
			{
				setpixel(va, fld, x, static_cast<int>(intery), fpart(intery) * cosgrad);
				setpixel(va, fld, x, static_cast<int>(intery) + 1, -rfpart1_0(intery) * cosgrad);
			}
			intery += gradient;
		}
	}

	// EP2
	if (backward)
	{
		if (swapped)
		{
			setpixel(va, fld, static_cast<int>(intery), ix2, -fpart(intery) * cosgrad);
			setpixel(va, fld, static_cast<int>(intery) + 1, ix2, rfpart1_0(intery) * cosgrad);
		}
		else
		{
			setpixel(va, fld, ix2, static_cast<int>(intery), fpart(intery) * cosgrad);
			setpixel(va, fld, ix2, static_cast<int>(intery) + 1, -rfpart1_0(intery) * cosgrad);
		}
		intery += gradient;
	}
	else
	{
		if (swapped)
		{
			setpixel(va, fld, static_cast<int>(intery), ix2, -fpart(intery) * cosgrad);
			setpixel(va, fld, static_cast<int>(intery) + 1, ix2, rfpart1_0(intery) * cosgrad);
		}
		else
		{
			setpixel(va, fld, ix2, static_cast<int>(intery), fpart(intery) * cosgrad);
			setpixel(va, fld, ix2, static_cast<int>(intery) + 1, -rfpart1_0(intery) * cosgrad);
		}
		intery += gradient;
	}
}
