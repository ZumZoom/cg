#pragma once

#include <cg/primitives/point.h>
#include <boost/numeric/interval.hpp>
#include <gmpxx.h>

#include <boost/optional.hpp>

namespace cg
{
   enum distance_t
   {
      CG_FIRST = -1,
      CG_EQUAL = 0,
      CG_SECOND = 1
   };

   struct distance2_d
   {
      boost::optional<distance_t> operator() (point_2 const & a, point_2 const & b, point_2 const & c, point_2 const & d) const
      {

         double l = (b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y);
         double r = (d.x - c.x) * (d.x - c.x) + (d.y - c.y) * (d.y - c.y);
         double res = l - r;
         double eps = (fabs(l) + fabs(r)) * 16 * std::numeric_limits<double>::epsilon();

         if (res > eps)
            return CG_FIRST;

         if (res < -eps)
            return CG_SECOND;

         return boost::none;
      }
   };

   struct distance2_i
   {
      boost::optional<distance_t> operator() (point_2 const & a, point_2 const & b, point_2 const & c, point_2 const & d) const
      {
         typedef boost::numeric::interval_lib::unprotect<boost::numeric::interval<double> >::type interval;

         boost::numeric::interval<double>::traits_type::rounding _;
         interval res =   (interval(b.x) - a.x) * (interval(b.x) - a.x)
                        + (interval(b.y) - a.y) * (interval(b.y) - a.y)
                        - (interval(d.x) - c.x) * (interval(d.x) - c.x)
                        - (interval(d.y) - c.y) * (interval(d.y) - c.y);

         if (res.lower() > 0)
            return CG_FIRST;

         if (res.upper() < 0)
            return CG_SECOND;

         return boost::none;
      }
   };

   struct distance2_r
   {
      boost::optional<distance_t> operator() (point_2 const & a, point_2 const & b, point_2 const & c, point_2 const & d) const
      {
         mpq_class res =  (mpq_class(b.x) - a.x) * (mpq_class(b.x) - a.x)
                        + (mpq_class(b.y) - a.y) * (mpq_class(b.y) - a.y)
                        - (mpq_class(d.x) - c.x) * (mpq_class(d.x) - c.x)
                        - (mpq_class(d.y) - c.y) * (mpq_class(d.y) - c.y);

         int cres = cmp(res, 0);

         if (cres > 0)
            return CG_FIRST;

         if (cres < 0)
            return CG_SECOND;

         return CG_EQUAL;
      }
   };

   inline distance_t distance2(point_2 const & a, point_2 const & b, point_2 const & c, point_2 const & d)
   {
      if (boost::optional<distance_t> v = distance2_d()(a, b, c, d))
         return *v;

      if (boost::optional<distance_t> v = distance2_i()(a, b, c, d))
         return *v;

      return *distance2_r()(a, b, c, d);
   }
}
