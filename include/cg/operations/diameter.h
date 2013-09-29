#pragma once

#include <cg/primitives/point.h>
#include <cg/primitives/segment.h>
#include <cg/convex_hull/andrew.h>
#include <cg/common/range.h>
#include <cg/common/common.h>

#include <cg/operations/distance.h>

namespace cg
{
	template<class BidIter>
	std::pair<BidIter, BidIter> diameter(BidIter begin, BidIter end)
	{
		using namespace common;
		typedef typename BidIter::value_type::value_type Scalar;

		auto maxDistanceEdgePoint = [](const point_2t<Scalar> & p, const segment_2t<Scalar> & s)
		{
			return distance2(s[0], p, s[1], p);
		};

		end = andrew_hull(begin, end);

		if(begin == end || std::next(begin) == end)
			return std::make_pair(begin, begin);
		else if(std::next(begin, 2) == end)
			return std::make_pair(begin, std::next(begin));

		segment_2t<Scalar> edge(*begin, *std::next(begin));

		auto pointToEdgeCmp = [&](const point_2t<Scalar> & a, const point_2t<Scalar> & b) 
			{
				size_t ind_a = maxDistanceEdgePoint(a, edge) == CG_FIRST ? 0 : 1;
				size_t ind_b = maxDistanceEdgePoint(b, edge) == CG_FIRST ? 0 : 1;
				return distance2(a, edge[ind_a], b, edge[ind_b]) == CG_FIRST; 
			};

		BidIter tmp = std::max_element(begin, end, pointToEdgeCmp);

		BidIter ans1, ans2 = tmp;
		ans1 = distance2(*tmp, edge[0], *tmp, edge[1]) == CG_FIRST ? begin : std::next(begin);

		range_circulator<BidIter> cl(begin, end, tmp);

		for(auto it = range_circulator<BidIter>(begin, end, begin + 1); it.iter() != begin; ++it)
		{
			edge[0] = *it, edge[1] = *(it + 1);

			while(pointToEdgeCmp(*(cl + 1), *cl))
				++cl;
			size_t ind = maxDistanceEdgePoint(*cl, edge) == CG_FIRST ? 0 : 1;
			if(distance2(*cl, edge[ind], *ans1, *ans2) == CG_FIRST)
			{
				ans1 = (it + ind).iter();
				ans2 = cl.iter();
			}
		}

		return make_pair(ans1, ans2);
	}
}
