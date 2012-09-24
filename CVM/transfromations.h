#pragma once

#include <functional>
#include <cv.h>

namespace videoMosaic {

namespace transformations
{
	typedef cv::Point2d Point;
	typedef double Precision;
	struct Shift : public std::unary_function<Point, Point> {
		double dx_, dy_;
		Shift(Precision dx, Precision dy) : dx_(dx),
												dy_(dy) {}
		Point operator()(const Point& in)
		{
			return Point(in.x + dx_, in.y + dy_);
		}
	};

	struct Scale : public std::unary_function<Point, Point> {
		Precision scale_;
		Scale(Precision scale) : scale_(scale) {}
		Point operator()(const Point& in)
		{
			return Point(in.x*scale_, in.y*scale_);
		}
	};

	struct Rotate : public std::unary_function<Point, Point> {
		Precision ang_;
		Rotate(Precision ang) : ang_(ang) {}
		Point operator()(const Point& in)
		{
			Precision rx = cos(ang_) * in.x - sin(ang_) * in.y;
			Precision ry = sin(ang_) * in.x + cos(ang_) * in.y;
			return Point(rx, ry);
		}
	};

	template <typename F1, typename F2>
	struct Compose : public std::unary_function<Point, Point>
	{
		F1 f1_;
		F2 f2_;
		Compose(const F1& f1, const F2& f2) : f1_(f1), f2_(f2) {}
		Point operator()(const Point& in)
		{
			return f1_(f2_(in));
		}
	};

	template <typename F1, typename F2>
	Compose<F1,F2> operator*(const F1& f1, const F2& f2)
	{
		return Compose<F1,F2>(f1,f2);
	}
} }
