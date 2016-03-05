// Copyright (c) 2014, 2015 Ben Deane
// This code is distributed under the MIT license. See LICENSE for details.

#include <complexity.h>

#include <algorithm>
#include <string>
using namespace std;

DEF_COMPLEXITY_PROPERTY(O_1, Complexity, ORDER_1, const string&, int)
{
}

char g_c;

DEF_COMPLEXITY_PROPERTY(O_LOG_N, Complexity, ORDER_LOG_N, const string& s)
{
  for (size_t i = 1, lim = s.size(); i < lim; i <<= 1)
  {
    g_c = s[i];
  }
}

DEF_COMPLEXITY_PROPERTY(O_N, Complexity, ORDER_N, const string& s)
{
  max_element(s.begin(), s.end());
}

DEF_COMPLEXITY_PROPERTY(O_N_LOG_N, Complexity, ORDER_N_LOG_N, const string& s)
{
  for (size_t i = 1, lim = s.size(); i < lim; i <<= 1)
  {
    max_element(s.begin(), s.end());
  }
}

DEF_COMPLEXITY_PROPERTY(PairCoverage, Complexity, ORDER_N2, const pair<int, int>&)
{
}

DEF_COMPLEXITY_PROPERTY(VectorCoverage, Complexity, ORDER_N2, const vector<int>&)
{
}

DEF_COMPLEXITY_PROPERTY(DequeCoverage, Complexity, ORDER_N2, const deque<int>&)
{
}

DEF_COMPLEXITY_PROPERTY(ListCoverage, Complexity, ORDER_N2, const list<int>&)
{
}

DEF_COMPLEXITY_PROPERTY(ForwardListCoverage, Complexity, ORDER_N2, const forward_list<int>&)
{
}

DEF_COMPLEXITY_PROPERTY(ArrayCoverage, Complexity, ORDER_N2, const array<int, 10>&)
{
}

struct MyUnspecializedType {};

DEF_COMPLEXITY_PROPERTY(ArbitraryCoverage, Complexity, ORDER_N2, const MyUnspecializedType&)
{
}
