#pragma once

#include <cg/primitives/point.h>
#include <cmath>

#include <iostream>

const double de2ra = acos(-1) / 180.0;
const double ra2de = 180.0 / acos(-1);

template<typename Scalar>
struct heodesic_point;

template <typename Scalar>
struct point_3t
{
   Scalar x, y, z;

   point_3t(Scalar x, Scalar y, Scalar z)
      : x(x)
      , y(y)
      , z(z)
   {}

   template <class UScalar>
   point_3t(point_3t<UScalar> const & o)
      : x(o.x)
      , y(o.y)
      , z(o.z)
   {}

   template <class UScalar>
   point_3t(heodesic_point<UScalar> const & o)
      : x(cos(o.get_phi()) * cos(o.get_psi()))
      , y(cos(o.get_phi()) * sin(o.get_psi()))
      , z(sin(o.get_phi()))
   {}

   point_3t()
      : x(0)
      , y(0)
      , z(0)
   {}

   double len() const
   {
      return sqrt(x * x + y * y + z * z);
   }

   void norm()
   {
      double len;
      len = this->len();
      x = x / len;
      y = y / len;
      z = z / len;
   }
};

template<typename Scalar>
point_3t<Scalar> const operator^ (point_3t<Scalar> const & a, point_3t<Scalar> const & b)
{
   return point_3t<Scalar>(a.y * b.z - a.z * b.y,
                           a.z * b.x - a.x * b.z,
                           a.x * b.y - a.y * b.x);
}

template<typename Scalar>
point_3t<Scalar> const operator - (point_3t<Scalar> const & v)
{
   return point_3t<Scalar>(-v.x, -v.y, -v.z);
}

template <typename Scalar>
struct heodesic_point
{
   Scalar lat, lon;

   heodesic_point(Scalar lat, Scalar lon)
      : lat(lat)
      , lon(lon)
   {}

   template <class UScalar>
   heodesic_point(point_3t<UScalar> const & o)
      : lat(atan(o.z / sqrt(o.x * o.x + o.y * o.y)) * ra2de)
      , lon(atan(o.y / o.x) * ra2de)
   {}

   template <class UScalar>
   heodesic_point(heodesic_point<UScalar> const & o)
      : lat(o.lat)
      , lon(o.lon)
   {}

   heodesic_point()
      : lat(0)
      , lon(0)
   {}

   double get_phi() const
   {
      return lat * de2ra;
   }

   double get_psi() const
   {
      return lon * de2ra;
   }
};

template <class Scalar>
double arc_len(heodesic_point<Scalar> pt1, heodesic_point<Scalar> pt2)
{
   return acos(sin(pt1.get_phi()) * sin(pt2.get_phi()) +
               cos(pt1.get_phi()) * cos(pt2.get_phi()) * cos(pt2.get_psi() - pt1.get_psi()));
}

bool equal(double a, double b)
{
   const double eps = 1e-10;
   return fabs(a - b) < eps;
}

template<class Scalar>
heodesic_point<Scalar> intersection(heodesic_point<Scalar> pt1, heodesic_point<Scalar> pt2, heodesic_point<Scalar> pt3, heodesic_point<Scalar> pt4)
{
//   std::cout << pt1.lat << " " << pt1.lon << std::endl;
//   std::cout << pt2.lat << " " << pt2.lon << std::endl;
//   std::cout << pt3.lat << " " << pt3.lon << std::endl;
//   std::cout << pt4.lat << " " << pt4.lon << std::endl;

   point_3t<Scalar> p1(pt1), p2(pt2), p3(pt3), p4(pt4);
   p1.norm(); p2.norm(); p3.norm(); p4.norm();

   std::cout << "p1: " << p1.x << " " << p1.y << " " << p1.z << std::endl;
   std::cout << "p2: " << p2.x << " " << p2.y << " " << p2.z << std::endl;
   std::cout << "p3: " << p3.x << " " << p3.y << " " << p3.z << std::endl;
   std::cout << "p4: " << p4.x << " " << p4.y << " " << p4.z << std::endl;

   point_3t<Scalar> n1(p1 ^ p2), n2(p3 ^ p4);
   n1.norm(); n2.norm();

   point_3t<Scalar> v1(n1 ^ n2);
   v1.norm();

   point_3t<Scalar> v2(-v1);
   v2.norm();

   std::cout << "n1: " << n1.x << " " << n1.y << " " << n1.z << std::endl;
   std::cout << "n2: " << n2.x << " " << n2.y << " " << n2.z << std::endl;
   std::cout << "v1: " << v1.x << " " << v1.y << " " << v1.z << std::endl;
   std::cout << "v2: " << v2.x << " " << v2.y << " " << v2.z << std::endl;

//   std::cout << arc_len(pt1, heodesic_point<Scalar>(v2)) << std::endl;
//   std::cout << arc_len(heodesic_point<Scalar>(v2), pt2) << std::endl;
//   std::cout << arc_len(pt1, pt2) << std::endl;

//   heodesic_point<Scalar> pv2(v1);
//   std::cout << pv2.lat << " " << pv2.lon << std::endl;

   if(equal(arc_len(pt1, heodesic_point<Scalar>(v1)) + arc_len(heodesic_point<Scalar>(v1), pt2), arc_len(pt1, pt2))
      && equal(arc_len(pt3, heodesic_point<Scalar>(v1)) + arc_len(heodesic_point<Scalar>(v1), pt4), arc_len(pt3, pt4)))
      return v1;

   if(equal(arc_len(pt1, heodesic_point<Scalar>(v2)) + arc_len(heodesic_point<Scalar>(v2), pt2), arc_len(pt1, pt2))
      && equal(arc_len(pt3, heodesic_point<Scalar>(v2)) + arc_len(heodesic_point<Scalar>(v2), pt4), arc_len(pt3, pt4)))
      return v2;

   return heodesic_point<Scalar>();
}
