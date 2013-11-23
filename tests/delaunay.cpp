#include <gtest/gtest.h>
#include <cg/triangulation/delaunay_triangulation.h>
#include "random_utils.h"

#include <iostream>
#include <cg/io/triangle.h>

using namespace std;
using namespace cg;

bool is_delaunay_triang(const vector<triangle_2> & faces, const vector<point_2> & pts)
{
   for(triangle_2 t : faces)
      for(point_2 s : pts)
         if(t[0] != s && t[1] != s && t[2] != s && contains(t[0], t[1], t[2], s))
         {
            cout << t << " contains " << s << endl;
            return false;
         }
   return true;
}

TEST(delaunay, uniform)
{
   for(size_t i = 0; i < 10; ++i)
   {
      vector<point_2> pts = uniform_points(70);
      delaunay_triangulation_2t<double> tr;
      for(auto pt : pts)
      {
         tr.add_vertex(pt);
         vector<triangle_2> faces = tr.get_triangulation();
         vector<point_2> points = tr.get_points();
         EXPECT_TRUE(is_delaunay_triang(faces, points));
      }
      for(auto pt : pts)
      {
         tr.del_vertex(pt);
         vector<triangle_2> faces = tr.get_triangulation();
         vector<point_2> points = tr.get_points();
         EXPECT_TRUE(is_delaunay_triang(faces, points));
      }
   }
}
