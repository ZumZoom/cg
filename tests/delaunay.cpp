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

TEST(delaunay, circle)
{
   const double PI = acos(-1);
   const size_t n = 360;
   const double r = 100;

   delaunay_triangulation_2t<double> tr;

   for(size_t i = 0; i != n; ++i)
   {
      double angle = PI * 2 * i / n;
      double x = cos(angle) * r;
      double y = sin(angle) * r;
      tr.add_vertex({x, y});
      if(i % 60 == 0)
      {
         vector<triangle_2> faces = tr.get_triangulation();
         vector<point_2> points = tr.get_points();
         EXPECT_TRUE(is_delaunay_triang(faces, points));
      }
   }

   srand(time(0));

   vector<point_2> points = tr.get_points();
   std::random_shuffle(points.begin(), points.end());

   for(size_t i = 0; i != n; ++i)
   {
      tr.del_vertex(points[i]);
      if(i % 60 == 0)
      {
         vector<triangle_2> faces = tr.get_triangulation();
         vector<point_2> points = tr.get_points();
         EXPECT_TRUE(is_delaunay_triang(faces, points));
      }
   }
}

TEST(delaunay, ordered_horisontal_line)
{
   const size_t n = 100;
   delaunay_triangulation_2t<double> tr;

   for(size_t i = 0; i < n; ++i)
   {
      tr.add_vertex({0, i});
      vector<triangle_2> faces = tr.get_triangulation();
      vector<point_2> points = tr.get_points();
      EXPECT_TRUE(is_delaunay_triang(faces, points));
   }

   vector<point_2> points = tr.get_points();

   for(size_t i = 0; i < n; ++i)
   {
      tr.del_vertex(points[i]);
      vector<triangle_2> faces = tr.get_triangulation();
      vector<point_2> points = tr.get_points();
      EXPECT_TRUE(is_delaunay_triang(faces, points));
   }

   for(size_t i = 0; i < n; ++i)
   {
      tr.add_vertex({0, -i});
      vector<triangle_2> faces = tr.get_triangulation();
      vector<point_2> points = tr.get_points();
      EXPECT_TRUE(is_delaunay_triang(faces, points));
   }

   points = tr.get_points();

   for(size_t i = 0; i < n; ++i)
   {
      tr.del_vertex(points[i]);
      vector<triangle_2> faces = tr.get_triangulation();
      vector<point_2> points = tr.get_points();
      EXPECT_TRUE(is_delaunay_triang(faces, points));
   }
}

TEST(delaunay, ordered_vertical_line)
{
   const size_t n = 100;
   delaunay_triangulation_2t<double> tr;

   for(size_t i = 0; i < n; ++i)
   {
      tr.add_vertex({i, 0});
      vector<triangle_2> faces = tr.get_triangulation();
      vector<point_2> points = tr.get_points();
      EXPECT_TRUE(is_delaunay_triang(faces, points));
   }

   vector<point_2> points = tr.get_points();

   for(size_t i = 0; i < n; ++i)
   {
      tr.del_vertex(points[i]);
      vector<triangle_2> faces = tr.get_triangulation();
      vector<point_2> points = tr.get_points();
      EXPECT_TRUE(is_delaunay_triang(faces, points));
   }

   for(size_t i = 0; i < n; ++i)
   {
      tr.add_vertex({-i, 0});
      vector<triangle_2> faces = tr.get_triangulation();
      vector<point_2> points = tr.get_points();
      EXPECT_TRUE(is_delaunay_triang(faces, points));
   }

   points = tr.get_points();

   for(size_t i = 0; i < n; ++i)
   {
      tr.del_vertex(points[i]);
      vector<triangle_2> faces = tr.get_triangulation();
      vector<point_2> points = tr.get_points();
      EXPECT_TRUE(is_delaunay_triang(faces, points));
   }
}

TEST(delaunay, random_vertical_line)
{
   const size_t n = 100;
   delaunay_triangulation_2t<double> tr;

   srand(time(0));

   for(size_t i = 0; i < n; ++i)
   {
      tr.add_vertex({rand(), 0});
      vector<triangle_2> faces = tr.get_triangulation();
      vector<point_2> points = tr.get_points();
      EXPECT_TRUE(is_delaunay_triang(faces, points));
   }

   vector<point_2> pts = tr.get_points();

   for(auto pt : pts)
   {
      tr.del_vertex(pt);
      vector<triangle_2> faces = tr.get_triangulation();
      vector<point_2> points = tr.get_points();
      EXPECT_TRUE(is_delaunay_triang(faces, points));
   }
}
