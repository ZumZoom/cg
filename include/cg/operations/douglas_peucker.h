#pragma once

#include <cg/primitives/point.h>
#include <cg/primitives/segment.h>
#include <cg/primitives/vector.h>
#include <math.h>

#include <iostream>
#include <cg/io/segment.h>
#include <cg/io/point.h>

namespace cg
{
	template <class Scalar>
	double dist(point_2t<Scalar> pt, segment_2t<Scalar> seg)
	{
		vector_2t<Scalar> a(seg[1].x - seg[0].x, seg[1].y - seg[0].y), b(pt.x - seg[0].x, pt.y - seg[0].y);
		double t = (a * b) / (a.len() * a.len());
		std::cout << pt << " " << seg[0] << " " << seg[1] << " : " << t << std::endl;
		if(t > 1)
			return vector_2t<Scalar>(pt.x - seg[1].x, pt.y - seg[1].y).len();
		else if(t < 0)
			return vector_2t<Scalar>(pt.x - seg[0].x, pt.y - seg[0].y).len();
		else
			return fabs((a ^ b) / a.len());
	}

	template <class BidIter, class OutIter>
	OutIter douglas_peucker(BidIter b, BidIter e, double eps, OutIter o, bool first = true)
	{
		BidIter ind = b;
		double maxdist = 0;
		segment_2 line(*b, *(e-1));
		for(BidIter it = b + 1; it != e-1; ++it)
		{
			double d = dist(*it, line);
			std::cout << d << std::endl;
			std::cout << *it << " " << line << " " << d << std::endl;
			if(d > maxdist)
			{
				ind = it;
				maxdist = d;
			}
		}
		if(maxdist > eps)
		{
			std::cout << "splitting :(" << std::endl;
			o = douglas_peucker(b, ind + 1, eps, o, false);
			o = douglas_peucker(ind, e, eps, o, false);
		}
		else
		{
			std::cout << "ok " << maxdist << std::endl;
			*o++ = *b;
		}
		if(first)
			*o++ = *(e-1);
		return o;
	}

}
