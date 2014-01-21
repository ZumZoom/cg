#pragma once

#include <vector>
#include <cmath>

#include "cg/primitives/triangle.h"


namespace cg
{
   template<class Scalar>
   Scalar get_x(point_2t<Scalar> const & p1, point_2t<Scalar> const & p2, double y)
   {
      return ((p2.x * p1.y - p2.y * p1.x) - y * p2.x + y * p1.x) / (p1.y - p2.y);
   }

   template<class Scalar>
   std::vector< std::pair< point_2i, point_2i > > triangle_raster(triangle_2t<Scalar> const & tr)
   {
      std::vector< point_2t<Scalar> > pts({{tr[0], tr[1], tr[2]}});
      std::sort(pts.begin(), pts.end(),
                [](point_2t<Scalar> const & p1, point_2t<Scalar> const & p2)
                { return p1.y < p2.y; }
               );

      std::vector< std::pair< point_2i, point_2i > > ans;

      int cur_y = static_cast<int>(ceil(pts[0].y));

      for(; cur_y - 1 < pts[1].y; ++cur_y)
      {
         std::vector<Scalar> x = {{ get_x(pts[0], pts[1], std::min(static_cast<double>(cur_y), pts[1].y)),
                                    get_x(pts[0], pts[2], std::min(static_cast<double>(cur_y), pts[1].y)),
                                    get_x(pts[0], pts[1], std::max(static_cast<double>(cur_y - 1), pts[0].y)),
                                    get_x(pts[0], pts[2], std::max(static_cast<double>(cur_y - 1), pts[0].y))
                                  }};
         std::sort(x.begin(), x.end());
         ans.push_back(std::make_pair(
                          point_2i(static_cast<int>(floor(x[0])), cur_y),
                          point_2i(static_cast<int>(ceil(x[3])), cur_y)));
      }

      --cur_y;

      for(; cur_y - 1 < pts[2].y; ++cur_y)
      {
         std::vector<Scalar> x = {{ get_x(pts[1], pts[2], std::min(static_cast<double>(cur_y), pts[2].y)),
                                    get_x(pts[0], pts[2], std::min(static_cast<double>(cur_y), pts[2].y)),
                                    get_x(pts[1], pts[2], std::max(static_cast<double>(cur_y - 1), pts[1].y)),
                                    get_x(pts[0], pts[2], std::max(static_cast<double>(cur_y - 1), pts[1].y))
                                  }};
         std::sort(x.begin(), x.end());
         ans.push_back(std::make_pair(
                          point_2i(static_cast<int>(floor(x[0])), cur_y),
                          point_2i(static_cast<int>(ceil(x[3])), cur_y)));
      }

      return ans;
   }
}
