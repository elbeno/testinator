// Copyright (c) 2014, 2015 Ben Deane
// This code is distributed under the MIT license. See LICENSE for details.

#include <test.h>
#include <arbitrary.h>
using namespace std;

//------------------------------------------------------------------------------
DEF_TEST(bool, Arbitrary)
{
  testinator::Arbitrary<bool> a;
  bool v = a.generate(0,0);
  a.shrink(v);
  return true;
}

//------------------------------------------------------------------------------
DEF_TEST(char, Arbitrary)
{
  testinator::Arbitrary<char> a;
  char v = a.generate(0,0);
  a.shrink(v);
  return true;
}

//------------------------------------------------------------------------------
DEF_TEST(signedchar, Arbitrary)
{
  testinator::Arbitrary<signed char> a;
  signed char v = a.generate(0,0);
  a.shrink(v);
  return true;
}

//------------------------------------------------------------------------------
DEF_TEST(unsignedchar, Arbitrary)
{
  testinator::Arbitrary<unsigned char> a;
  unsigned char v = a.generate(0,0);
  a.shrink(v);
  return true;
}

//------------------------------------------------------------------------------
DEF_TEST(wchar_t, Arbitrary)
{
  testinator::Arbitrary<wchar_t> a;
  wchar_t v = a.generate(0,0);
  a.shrink(v);
  return true;
}

//------------------------------------------------------------------------------
DEF_TEST(char16_t, Arbitrary)
{
  testinator::Arbitrary<char16_t> a;
  char16_t v = a.generate(0,0);
  a.shrink(v);
  return true;
}

//------------------------------------------------------------------------------
DEF_TEST(char32_t, Arbitrary)
{
  testinator::Arbitrary<char32_t> a;
  char32_t v = a.generate(0,0);
  a.shrink(v);
  return true;
}

//------------------------------------------------------------------------------
DEF_TEST(short, Arbitrary)
{
  testinator::Arbitrary<short> a;
  short v = a.generate(0,0);
  a.shrink(v);
  return true;
}

//------------------------------------------------------------------------------
DEF_TEST(unsignedshort, Arbitrary)
{
  testinator::Arbitrary<unsigned short> a;
  unsigned short v = a.generate(0,0);
  a.shrink(v);
  return true;
}

//------------------------------------------------------------------------------
DEF_TEST(int, Arbitrary)
{
  testinator::Arbitrary<int> a;
  int v = a.generate(0,0);
  a.shrink(v);
  return true;
}

//------------------------------------------------------------------------------
DEF_TEST(unsignedint, Arbitrary)
{
  testinator::Arbitrary<unsigned int> a;
  unsigned int v = a.generate(0,0);
  a.shrink(v);
  return true;
}

//------------------------------------------------------------------------------
DEF_TEST(long, Arbitrary)
{
  testinator::Arbitrary<long> a;
  long v = a.generate(0,0);
  a.shrink(v);
  return true;
}

//------------------------------------------------------------------------------
DEF_TEST(unsignedlong, Arbitrary)
{
  testinator::Arbitrary<unsigned long> a;
  unsigned long v = a.generate(0,0);
  a.shrink(v);
  return true;
}

//------------------------------------------------------------------------------
DEF_TEST(longlong, Arbitrary)
{
  testinator::Arbitrary<long long> a;
  long long v = a.generate(0,0);
  a.shrink(v);
  return true;
}

//------------------------------------------------------------------------------
DEF_TEST(unsignedlonglong, Arbitrary)
{
  testinator::Arbitrary<unsigned long long> a;
  unsigned long long v = a.generate(0,0);
  a.shrink(v);
  return true;
}

//------------------------------------------------------------------------------
DEF_TEST(float, Arbitrary)
{
  testinator::Arbitrary<float> a;
  float v = a.generate(0,0);
  a.shrink(v);
  return true;
}

//------------------------------------------------------------------------------
DEF_TEST(double, Arbitrary)
{
  testinator::Arbitrary<double> a;
  double v = a.generate(0,0);
  a.shrink(v);
  return true;
}

//------------------------------------------------------------------------------
DEF_TEST(longdouble, Arbitrary)
{
  testinator::Arbitrary<long double> a;
  long double v = a.generate(0,0);
  a.shrink(v);
  return true;
}

//------------------------------------------------------------------------------
DEF_TEST(vector, Arbitrary)
{
  testinator::Arbitrary<vector<int>> a;
  vector<int> v = a.generate(1,0);
  vector<vector<int>> vv = a.shrink(v);
  return vv.size() == 2
    && vv[0].size() == v.size()/2
    && vv[1].size() == v.size()/2;
}

//------------------------------------------------------------------------------
DEF_TEST(string, Arbitrary)
{
  testinator::Arbitrary<string> a;
  string v = a.generate(1,0);
  vector<string> vv = a.shrink(v);
  return vv.size() == 2
    && vv[0].size() == v.size()/2
    && vv[1].size() == v.size()/2;
}

//------------------------------------------------------------------------------
DEF_TEST(deque, Arbitrary)
{
  testinator::Arbitrary<deque<int>> a;
  deque<int> v = a.generate(1,0);
  vector<deque<int>> vv = a.shrink(v);
  return vv.size() == 2
    && vv[0].size() == v.size()/2
    && vv[1].size() == v.size()/2;
}

//------------------------------------------------------------------------------
DEF_TEST(list, Arbitrary)
{
  testinator::Arbitrary<list<int>> a;
  list<int> v = a.generate(1,0);
  vector<list<int>> vv = a.shrink(v);
  return vv.size() == 2
    && vv[0].size() == v.size()/2
    && vv[1].size() == v.size()/2;
}

//------------------------------------------------------------------------------
DEF_TEST(forward_list, Arbitrary)
{
  testinator::Arbitrary<forward_list<int>> a;
  forward_list<int> v = a.generate(1,0);
  vector<forward_list<int>> vv = a.shrink(v);

  std::size_t vlen = 0;
  for (auto it = v.begin(); it != v.end(); ++it, ++vlen);

  std::size_t vv0len = 0;
  std::size_t vv1len = 0;
  if (vv.size() > 0)
    for (auto it = vv[0].begin(); it != vv[0].end(); ++it, ++vv0len);
  if (vv.size() > 1)
    for (auto it = vv[1].begin(); it != vv[1].end(); ++it, ++vv1len);

  return vv.size() == 2
    && vv0len == vlen/2
    && vv1len == vlen/2;
}

//------------------------------------------------------------------------------
DEF_TEST(set, Arbitrary)
{
  testinator::Arbitrary<set<int>> a;
  set<int> v = a.generate(1,0);
  vector<set<int>> vv = a.shrink(v);
  return vv.size() == 2
    && vv[0].size() == v.size()/2
    && vv[1].size() == v.size()/2;
}

//------------------------------------------------------------------------------
DEF_TEST(multiset, Arbitrary)
{
  testinator::Arbitrary<multiset<int>> a;
  multiset<int> v = a.generate(1,0);
  vector<multiset<int>> vv = a.shrink(v);
  return vv.size() == 2
    && vv[0].size() == v.size()/2
    && vv[1].size() == v.size()/2;
}

//------------------------------------------------------------------------------
DEF_TEST(unordered_set, Arbitrary)
{
  testinator::Arbitrary<unordered_set<int>> a;
  unordered_set<int> v = a.generate(1,0);
  vector<unordered_set<int>> vv = a.shrink(v);
  return vv.size() == 2
    && vv[0].size() == v.size()/2
    && vv[1].size() == v.size()/2;
}

//------------------------------------------------------------------------------
DEF_TEST(pair, Arbitrary)
{
  testinator::Arbitrary<pair<int, float>> a;
  pair<int, float> v = a.generate(0,0);
  a.shrink(v);
  return true;
}

//------------------------------------------------------------------------------
DEF_TEST(tuple, Arbitrary)
{
  testinator::Arbitrary<tuple<int, float>> a;
  tuple<int, float> v = a.generate(0,0);
  a.shrink(v);
  return true;
}

//------------------------------------------------------------------------------
DEF_TEST(map, Arbitrary)
{
  testinator::Arbitrary<map<int, float>> a;
  map<int, float> v = a.generate(1,0);
  vector<map<int, float>> vv = a.shrink(v);
  return vv.size() == 2
    && vv[0].size() == v.size()/2
    && vv[1].size() == v.size()/2;
}

//------------------------------------------------------------------------------
DEF_TEST(multimap, Arbitrary)
{
  testinator::Arbitrary<multimap<int, float>> a;
  multimap<int, float> v = a.generate(1,0);
  vector<multimap<int, float>> vv = a.shrink(v);
  return vv.size() == 2
    && vv[0].size() == v.size()/2
    && vv[1].size() == v.size()/2;
}

//------------------------------------------------------------------------------
DEF_TEST(unordered_map, Arbitrary)
{
  testinator::Arbitrary<unordered_map<int, float>> a;
  unordered_map<int, float> v = a.generate(1,0);
  vector<unordered_map<int, float>> vv = a.shrink(v);
  return vv.size() == 2
    && vv[0].size() == v.size()/2
    && vv[1].size() == v.size()/2;
}
