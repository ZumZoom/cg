#include <gtest/gtest.h>
#include <cg/primitives/contour.h>
#include <cg/primitives/point.h>
#include <cg/visibility/visibility_naive.h>
#include <cg/visibility/visibility_shortest_path.h>

#include <vector>
#include <boost/assign/list_of.hpp>

#include <iostream>
#include <cg/io/segment.h>

using namespace std;
using namespace cg;

TEST(visibility, sample)
{
	point_2 start(0, 0), finish(10, 10);
	vector<contour_2> poly;
	std::vector<point_2> pts = boost::assign::list_of
								(point_2(1, 1))
								(point_2(2, 1))
								(point_2(2, 2))
								(point_2(1, 2));

    poly.push_back(pts);

    auto ans = cg::make_visibility_graph(start, finish, poly);
    cout << "size: " << ans.size() << endl;
    for(auto seg : ans)
    	cout << seg << endl;

    auto path = cg::shortest_path(start, finish, ans);
    cout << "path size: " << path.size() << endl;
    for(auto pt : path)
    	cout << pt << endl;

	EXPECT_FALSE(false);
}

TEST(visibility, hard)
{

	point_2 start(0, 0), finish(10, 6);
	vector<contour_2> poly;
	std::vector<point_2> first = boost::assign::list_of
								(point_2(5, 1))
								(point_2(5, 5))
								(point_2(3, 3));

	poly.push_back(first);

	std::vector<point_2> second = boost::assign::list_of
								(point_2(5, -1))
								(point_2(8, -1))
								(point_2(7, 3));

	poly.push_back(second);

	std::vector<point_2> third = boost::assign::list_of
								(point_2(6, 4))
								(point_2(8, 4))
								(point_2(6, 5));

	poly.push_back(third);

	vector<segment_2> ans = cg::make_visibility_graph(start, finish, poly);
	cout << "size: " << ans.size() << endl;
	for (segment_2 seg : ans)
		cout << seg << endl;

	auto path = cg::shortest_path(start, finish, ans);
    cout << "path size: " << path.size() << endl;
    for(auto pt : path)
    	cout << pt << endl;

	EXPECT_FALSE(false);
}

TEST(visibility, unbelievable)
{
	point_2 start(0, 0), finish(4, 5);
	vector<contour_2> poly;
	std::vector<point_2> first = boost::assign::list_of
								(point_2(1, 1))
								(point_2(1, -2))
								(point_2(2, 0))
								(point_2(3, -2))
								(point_2(3, 1));

	poly.push_back(first);

	vector<segment_2> ans = cg::make_visibility_graph(start, finish, poly);
	cout << "size: " << ans.size() << endl;
	for (segment_2 seg : ans)
		cout << seg << endl;

	auto path = cg::shortest_path(start, finish, ans);
    cout << "path size: " << path.size() << endl;
    for(auto pt : path)
    	cout << pt << endl;

	EXPECT_FALSE(false);
}

TEST(visibility, gangsta)
{
	point_2 start(0, 0), finish(4, 5);
	vector<contour_2> poly;

	vector<segment_2> ans = cg::make_visibility_graph(start, finish, poly);
	cout << "size: " << ans.size() << endl;
	for (segment_2 seg : ans)
		cout << seg << endl;
	
	auto path = cg::shortest_path(start, finish, ans);
    cout << "path size: " << path.size() << endl;
    for(auto pt : path)
    	cout << pt << endl;

	EXPECT_FALSE(false);	
}
