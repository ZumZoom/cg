#pragma once

#include <cg/primitives/point.h>
#include <cg/primitives/segment.h>
#include <cg/convex_hull/andrew.h>
#include <cg/common/range.h>
#include <cg/common/common.h>

namespace cg
{
	template<class BidIter>
	std::pair<BidIter, BidIter> diameter(BidIter begin, BidIter end)
	{
		using namespace common;
		typedef typename BidIter::value_type::value_type Scalar;

		auto maxDistanceEdgePoint = [](const point_2t<Scalar> & p, const segment_2t<Scalar> & s)
		{
			return std::max(distance(s[0], p), distance(s[1], p));
		};

		end = andrew_hull(begin, end);

		if(begin == end || std::next(begin) == end)
			return std::make_pair(begin, begin);
		else if(std::next(begin, 2) == end)
			return std::make_pair(begin, std::next(begin));

		segment_2t<Scalar> edge(*begin, *std::next(begin));

		BidIter tmp = std::max_element(begin, end,
			[&](const point_2t<Scalar> & a, const point_2t<Scalar> & b) 
			{ return maxDistanceEdgePoint(a, edge) < maxDistanceEdgePoint(b, edge); });

		Scalar d = 0;
		BidIter ans1, ans2 = tmp;
		for(size_t i = 0; i < 2; ++i)
			if(make_max(d, distance(edge[i], *tmp)))
				ans1 = begin + i;

		range_circulator<BidIter> cl(begin, end, tmp);

		for(auto it = range_circulator<BidIter>(begin, end, begin + 1); it.iter() != begin; ++it)
		{
			edge = segment_2t<Scalar>(*it, *(it + 1));

			while(maxDistanceEdgePoint(*cl, edge) < maxDistanceEdgePoint(*(cl + 1), edge))
				++cl;

			for(size_t i = 0; i < 2; ++i)
			{
				if(make_max(d, distance(edge[i], *cl)))
				{
					ans1 = (it + i).iter();
					ans2 = cl.iter();
				}
			}
		}

		return make_pair(ans1, ans2);
	}
}
