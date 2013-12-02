#pragma once

#include <vector>

#include "point.h"
#include "cg/common/range.h"

namespace cg
{
template <class Scalar>
struct contour_2t;

typedef contour_2t<float> contour_2f;
typedef contour_2t<double> contour_2;
typedef contour_2t<int>   contour_2i;

template <class Scalar>
struct contour_2t : public std::vector< point_2t <Scalar> >
{
   contour_2t() {}
   contour_2t(std::vector<point_2t<Scalar> > const& pts) : std::vector< point_2t < Scalar> >(pts)
   {}

   typedef typename common::range_circulator<contour_2t<Scalar> > circulator_t;

   circulator_t circulator() const
   {
      return common::range_circulator<contour_2t<Scalar> >(*this);
   }

   circulator_t circulator(typename std::vector< point_2t <Scalar> >::const_iterator itr) const
   {
      return common::range_circulator<contour_2t<Scalar> >(*this, itr);
   }

   size_t vertices_num() const
   {
      return this->size();
   }

   void add_point(point_2t<Scalar> const& point)
   {
      this->push_back(point);
   }

private:
   friend struct contour_builder_type;
};

typedef common::range_circulator<contour_2f> contour_circulator;
}
