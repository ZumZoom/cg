#pragma once

#include <vector>
#include <set>

#include <cg/primitives/point.h>
#include <cg/primitives/segment.h>
#include <cg/operations/has_intersection/segment_segment.h>

namespace cg {

template<class Scalar>
bool operator < (const segment_2t<Scalar> & a, const segment_2t<Scalar> & b)
{
   return (min(a).x < min(b).x) || (min(a).x == min(b).x && min(a).y < min(b).y);
}

template<class Scalar>
std::vector< segment_2t<Scalar> > segment_union(std::vector< segment_2t<Scalar> > const & vec)
{
   std::set< segment_2t <Scalar> > S(vec.begin(), vec.end());
   for(typename std::set< segment_2t <Scalar> >::iterator it = S.begin(); std::next(it) != S.end();)
   {
      segment_2t <Scalar> cur = *it, next = *std::next(it);
      if(has_intersection(cur, next))
      {
         it = S.erase(it);
         it = S.erase(it);
         if(cur[0] == max(cur))
            cur[0] = std::max(max(next), cur[0]);
         else
            cur[1] = std::max(max(next), cur[1]);
         it = S.insert(cur).first;
      }
      else
         ++it;
   }
   return std::vector< segment_2t<Scalar> >(S.begin(), S.end());
}

}
