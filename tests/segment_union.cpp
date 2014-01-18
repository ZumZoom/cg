//#include <gtest/gtest.h>

//#include <cg/operations/segment_union.h>
//#include <cg/io/segment.h>

//template<class Scalar>
//std::vector< segment_2t<Scalar> > segment_union_simple(std::vector< segment_2t<Scalar> > vec)
//{
//   bool change = true;
//   while(change)
//   {
//      change = false;
//      for(size_t i = 0; i < vec.size(); ++i)
//      {
//         for(size_t j = 0; j < vec.size(); ++j)
//         {
//            if(i != j)
//            {
//               if(cg::has_intersection(vec[i], vec[j]))
//               {

//               }
//            }
//         }
//      }
//   }

//   std::set< segment_2t <Scalar> > S(vec.begin(), vec.end());
//   for(typename std::set< segment_2t <Scalar> >::iterator it = S.begin(); std::next(it) != S.end();)
//   {
//      segment_2t <Scalar> cur = *it, next = *std::next(it);
//      if(has_intersection(cur, next))
//      {
//         it = S.erase(it);
//         it = S.erase(it);
//         if(cur[0] == max(cur))
//            cur[0] = std::max(max(next), cur[0]);
//         else
//            cur[1] = std::max(max(next), cur[1]);
//         it = S.insert(cur).first;
//      }
//      else
//         ++it;
//   }
//   return std::vector< segment_2t<Scalar> >(S.begin(), S.end());
//}

//TEST(segment_union, simple)
//{
//   using cg::point_2;
//   using cg::segment_2;

//   std::vector<segment_2> s =
//   {{
//       segment_2(point_2(0, 0), point_2(0, 2)),
//       segment_2(point_2(0, 4), point_2(0, 6)),
//       segment_2(point_2(0, 6), point_2(0, 8)),
//       segment_2(point_2(0, -1), point_2(0, -1))
//    }};

//   std::vector<segment_2> v = cg::segment_union(s);
//   for(auto i : v)
//      std::cout << i << std::endl;
//}

//TEST(segment_union, random)
//{
//   uniform_random_real<double, std::mt19937> distr(-(1LL << 53), (1LL << 53));

//   std::vector<cg::point_2> pts = uniform_points(100);
//   for (size_t l = 0, ln = 1; ln < pts.size(); l = ln++)
//   {
//      cg::point_2 a = pts[l];
//      cg::point_2 b = pts[ln];

//      std::vector<cg::point_2> line_pts;
//      for (size_t k = 0; k != 300; ++k)
//      {
//         double t = distr();
//         cg::point_2 c = a + t * (b - a);
//         line_pts.push_back(c);
//      }

//      std::vector<cg::segment_2> segments;
//      for(size_t i = 0; i + 1 < line_pts.size(); ++i)
//         segments.push_back(cg::segment_2(line_pts[i], line_pts[i+1]));

//      std::vector<cg::segment_2> un = cg::segment_union(segments);
//   }
//}
