#include <gtest/gtest.h>

#include "random_utils.h"

#include <boost/assign/list_of.hpp>

#include <cg/sphere/segment_intersection.h>

using namespace std;

TEST(sphere, simple)
{
   heodesic_point<double> a(0, 0), b(0, 90), c(45, 45), d(-45, 45);
   heodesic_point<double> res(intersection(a, b, c, d));
   cout << res.lat << " " << res.lon << endl;
}
