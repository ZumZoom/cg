#include <gtest/gtest.h>

#include <boost/assign/list_of.hpp>

#include <cg/primitives/point.h>
#include <cg/Visibility/minkowski_sum.h>

#include <vector>
#include <iterator>

#include <iostream>
#include <cg/io/point.h>

using namespace std;

TEST(minkowski, test1)
{
   std::vector<cg::point_2> pts;
   pts.push_back(cg::point_2(0, 0));
   pts.push_back(cg::point_2(2, 0));
   pts.push_back(cg::point_2(1, 1));

   std::vector<cg::point_2> pts2;
   pts2.push_back(cg::point_2(-2, -2));
   pts2.push_back(cg::point_2(2, -2));
   pts2.push_back(cg::point_2(2, 2));
   pts2.push_back(cg::point_2(-2, 2));

   auto ans = cg::minkowski_sum(cg::contour_2(pts2), cg::contour_2(pts));

   cout << "Size: " << ans.size() << endl;
   for(auto pt : ans)
      cout << pt << endl;
}

TEST(minkowski, test2)
{
   std::vector<cg::point_2> pts;
   pts.push_back(cg::point_2(0, 0));
   pts.push_back(cg::point_2(4, 0));
   pts.push_back(cg::point_2(4, 4));
   pts.push_back(cg::point_2(0, 4));

   std::vector<cg::point_2> pts2;
   pts2.push_back(cg::point_2(-2, -2));
   pts2.push_back(cg::point_2(2, -2));
   pts2.push_back(cg::point_2(2, 2));
   pts2.push_back(cg::point_2(-2, 2));

   auto ans = cg::minkowski_sum(cg::contour_2(pts2), cg::contour_2(pts));

   cout << "Size: " << ans.size() << endl;
   for(auto pt : ans)
      cout << pt << endl;
}
