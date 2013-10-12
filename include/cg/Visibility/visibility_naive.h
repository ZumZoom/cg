#pragma once

#include <cg/primitives/contour.h>
#include <cg/operations/orientation.h>
#include <cg/primitives/segment.h>
#include <cg/operations/has_intersection/segment_segment.h>

#include <vector>
#include <iostream>
#include <cg/io/segment.h>

namespace cg 
{
	template<class T>
	bool has_intersection(const point_2t<T> & p1, const point_2t<T> & p2, const std::vector< contour_2t <T> > & g)
	{
		for(auto g3_it = g.begin(), g3_end = g.end(); g3_it != g3_end; ++g3_it)
		{
			for(auto cont3_it = g3_it->begin(), cont3_prev = g3_it->end() - 1; cont3_it != g3_it->end(); cont3_prev = cont3_it++)
			{
				segment_2t<T> a(p1, p2), b(*cont3_prev, *cont3_it);
				if(a[0] != b[0] && a[0] != b[1] && a[1] != b[0] && a[1] != b[1] && has_intersection(a, b))
					return true;
			}
		}
		return false;
	}

	template<class T>
	void get_all_visible(point_2t<T> const & pt, typename std::vector< contour_2t <T> >::const_iterator g2_it,
		std::vector< contour_2t <T> > const & g, std::back_insert_iterator< typename std::vector< segment_2t<T> > > out)
	{
		for(auto g_it = g.begin(), g_end = g.end(); g_it != g_end; ++g_it)
		{
			for(auto cont_it = g_it->begin(), cont_end = g_it->end(); cont_it != cont_end; ++cont_it)
			{
				if(*cont_it == pt)
					continue;

				point_2t<T> prev = (cont_it == g_it->begin() ? g_it->back() : *(cont_it - 1));
				point_2t<T> next = (cont_it == (g_it->end() - 1) ? g_it->front() : *(cont_it + 1));

				if(orientation(prev, *cont_it, pt) == CG_RIGHT && orientation(*cont_it, next, pt) == CG_RIGHT)
					continue;

				bool intersects = has_intersection(*cont_it, pt, g);

				if(g_it == g2_it)
				{
					if(orientation(prev, *cont_it, next) == CG_LEFT)
					{	
						if(orientation(prev, *cont_it, pt) == CG_LEFT && orientation(*cont_it, next, pt) == CG_LEFT)
							intersects = true;
					}
					else
					{
						if(orientation(prev, *cont_it, pt) == CG_LEFT || orientation(*cont_it, next, pt) == CG_LEFT)
							intersects = true;
					}
				}

				if(!intersects && (*cont_it < pt || g2_it == g.end()))
					*out++ = segment_2t<T>(*cont_it, pt);
			}
		}
	}

	template<class T>
	std::vector< segment_2t<T> > make_visibility_graph(point_2t<T> const & s, point_2t<T> const & f, std::vector< contour_2t <T> > const & g)
	{
		std::vector< segment_2t<T> > out;
		for(auto g1_it = g.begin(), g1_end = g.end(); g1_it != g1_end; ++g1_it)
		{
			for(auto cont1_it = g1_it->begin(), cont1_end = g1_it->end(); cont1_it != cont1_end; ++cont1_it)
			{
				size_t last = out.size();
				get_all_visible(*cont1_it, g1_it, g, std::back_inserter(out));

				point_2t<T> prev = (cont1_it == g1_it->begin() ? g1_it->back() : *(cont1_it - 1));
				point_2t<T> next = (cont1_it == (g1_it->end() - 1) ? g1_it->front() : *(cont1_it + 1));

				for(; last < out.size(); ++last)
					if(orientation(prev, *cont1_it, out[last][0]) == CG_RIGHT && orientation(*cont1_it, next, out[last][0]) == CG_RIGHT)
					{
						out.erase(out.begin() + last);
						--last;
					}
			}
		}

		get_all_visible(s, g.end(), g, std::back_inserter(out));
		get_all_visible(f, g.end(), g, std::back_inserter(out));

		if(!has_intersection(s, f, g))
			out.push_back(segment_2t<T>(s, f));

		return out;
	}

}
