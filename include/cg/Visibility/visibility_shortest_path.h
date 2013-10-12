#pragma once

#include "visibility_shortest_path.h"
#include <math.h>
#include <map>
#include <vector>
#include <queue>
#include <algorithm>

#include <iostream>
#include <cg/io/segment.h>

namespace cg
{
	template<class T>
	double length(const vector_2t<T> & v)
	{
		return sqrt(v * v);
	}

	template<class T>
	std::vector< point_2t <T> > shortest_path(const point_2t<T> & s, const point_2t<T> & f, const std::vector< segment_2t<T> > & edges)
	{
		std::map<point_2t <T>, int> indexes;
		std::vector< point_2t<T> > points;
		for(auto e : edges)
		{
			if(indexes.count(e[0]) == 0)
			{
				indexes.insert(std::make_pair(e[0], indexes.size()));
				points.push_back(e[0]);
			}
			if(indexes.count(e[1]) == 0)
			{
				indexes.insert(std::make_pair(e[1], indexes.size()));
				points.push_back(e[1]);
			}
		}
		if(indexes.count(s) == 0)
		{
			indexes.insert(std::make_pair(s, indexes.size()));
			points.push_back(s);
		}
		if(indexes.count(f) == 0)
		{
			indexes.insert(std::make_pair(f, indexes.size()));
			points.push_back(f);
		}

		// std::cout << "Indexes" << std::endl;
		// for(auto p : indexes)
		// 	std::cout << p.first << " " << p.second << std::endl;

		// std::cout << "Points" << std::endl;
		// for(int i = 0; i < points.size(); ++i)
		// 	std::cout << i << " " << points[i] << std::endl;


		const int N = indexes.size();
		std::vector< std::vector < std::pair<int, double> > > G(N);
		// std::cout << "Running" << std::endl;
		for(auto e : edges)
		{
			double dist = length(e[0] - e[1]);
			// std::cout << e << std::endl;
			G[indexes[e[0]]].push_back(std::make_pair(indexes[e[1]], dist));
			G[indexes[e[1]]].push_back(std::make_pair(indexes[e[0]], dist));
		}

		std::vector<double> d(N, INFINITY);
		std::vector<int> p(N, -1);
		std::priority_queue< std::pair <double, int> > Q;
		d[indexes[s]] = 0;
		Q.push(std::make_pair(-0, indexes[s]));

		while(!Q.empty())
		{
			int v = Q.top().second;
			double cur_d = -Q.top().first;

			// std::cout << "current: " << v << ", distance: " << cur_d << ", array dist: " << d[v] << std::endl;
			Q.pop();
			if(cur_d > d[v]) continue;

			for(auto e : G[v])
			{
				// std::cout << "edge to " << e.first << " of length " << e.second << std::endl;
				// std::cout << d[v] + e.second << "<" << d[e.first] << std::endl;
				if(d[v] + e.second < d[e.first])
				{
					// std::cout << "relaxing: " << e.first << ", prev distance: " << d[e.first] << ", new distance: " << d[v] + e.second << std::endl;
					d[e.first] = d[v] + e.second;
					p[e.first] = v;
					Q.push(std::make_pair(-d[e.first], e.first));
				}
			}
		}

		std::vector< point_2t <T> > ans;
		int v = indexes[f];
		while(p[v] != -1)
		{
			ans.push_back(points[v]);
			v = p[v];
		}
		ans.push_back(points[v]);

		std::reverse(ans.begin(), ans.end());

		// std::cout << "Done" <<std::endl;
		return ans;
	}
}
