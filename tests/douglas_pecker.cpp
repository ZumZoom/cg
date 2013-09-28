#include <gtest/gtest.h>

#include <cg/operations/douglas_peucker.h>

#include <iostream>
#include <cg/io/point.h>

using namespace std;
using namespace cg;

TEST(douglas_peucker, simple1)
{
	vector_2 v(3, 4);
	cout << v.len() << endl;
	vector<point_2> chain( {point_2(0, 0), point_2(1, -1), point_2(2, -1), point_2(3, 0)} ), ans;
	douglas_peucker(chain.begin(), chain.end(), 0.5, back_inserter(ans));
	for(int i = 0; i < ans.size(); ++i)
		cout << ans[i] << endl;
}
