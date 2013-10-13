#pragma once

#include <cg/primitives/contour.h>
#include <cg/operations/orientation.h>

#include <iostream>
#include <cg/io/point.h>

namespace cg
{
	template<typename T>
	contour_2t<T> minkowski_sum(contour_2t<T> const & a, contour_2t<T> const & b)
	{
		contour_2t<T> ans;

		auto a_it = std::min_element(a.begin(), a.end()),
		     b_it = std::min_element(b.begin(), b.end()),
		     a_next = a_it + 1, b_next = b_it + 1;
		if(a_next == a.end())
			a_next = a.begin();
		if(b_next == b.end())
			b_next = b.begin();

		while(ans.size() < a.size() + b.size())
    	{
			ans.add_point(*a_it + vector_2t<T>(b_it->x, b_it->y));
			if(orientation(*a_it, *a_next, *b_it, *b_next) == CG_RIGHT)
			{
				b_it = b_next++;
				if(b_next == b.end())
					b_next = b.begin();
			}
			else
			{
				a_it = a_next++;
				if(a_next == a.end())
					a_next = a.begin();
			}
		}

		return ans;
	}
}
