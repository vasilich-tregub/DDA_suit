#pragma once
#include <vector>

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
			col = (int)width_ - 1; row = (int)height_ - 1;
		}
		return buffer_[col + row * width_];
	};
	std::vector<T> buffer() { return buffer_; };
private:
	std::size_t width_;
	std::size_t height_;
	std::vector<T> buffer_;
};

void drawEdge(double x1, double y1, double x2, double y2, std::vector<std::pair<std::pair<int, int>, double>>& va, array2D<double>& fld);
