#pragma once
#include <cg/primitives/point.h>

namespace cg {

   template<class Scalar>
   double distance(point_2t<Scalar> const& a, point_2t<Scalar> const& b)
   {
      return (a - b).length();
   }

}
