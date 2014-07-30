#include <test_extended.h>
#include <arbitrary.h>
using namespace std;

//------------------------------------------------------------------------------
DECLARE_TEST(bool, Arbitrary)
{
  testpp::Arbitrary<bool> a;
  bool v = a.generate(0,0);
  a.shrink(v);
  return true;
}

//------------------------------------------------------------------------------
DECLARE_TEST(char, Arbitrary)
{
  testpp::Arbitrary<char> a;
  char v = a.generate(0,0);
  a.shrink(v);
  return true;
}

//------------------------------------------------------------------------------
DECLARE_TEST(signedchar, Arbitrary)
{
  testpp::Arbitrary<signed char> a;
  signed char v = a.generate(0,0);
  a.shrink(v);
  return true;
}

//------------------------------------------------------------------------------
DECLARE_TEST(unsignedchar, Arbitrary)
{
  testpp::Arbitrary<unsigned char> a;
  unsigned char v = a.generate(0,0);
  a.shrink(v);
  return true;
}

//------------------------------------------------------------------------------
DECLARE_TEST(wchar_t, Arbitrary)
{
  testpp::Arbitrary<wchar_t> a;
  wchar_t v = a.generate(0,0);
  a.shrink(v);
  return true;
}

//------------------------------------------------------------------------------
DECLARE_TEST(char16_t, Arbitrary)
{
  testpp::Arbitrary<char16_t> a;
  char16_t v = a.generate(0,0);
  a.shrink(v);
  return true;
}

//------------------------------------------------------------------------------
DECLARE_TEST(char32_t, Arbitrary)
{
  testpp::Arbitrary<char32_t> a;
  char32_t v = a.generate(0,0);
  a.shrink(v);
  return true;
}

//------------------------------------------------------------------------------
DECLARE_TEST(short, Arbitrary)
{
  testpp::Arbitrary<short> a;
  short v = a.generate(0,0);
  a.shrink(v);
  return true;
}

//------------------------------------------------------------------------------
DECLARE_TEST(unsignedshort, Arbitrary)
{
  testpp::Arbitrary<unsigned short> a;
  unsigned short v = a.generate(0,0);
  a.shrink(v);
  return true;
}

//------------------------------------------------------------------------------
DECLARE_TEST(int, Arbitrary)
{
  testpp::Arbitrary<int> a;
  int v = a.generate(0,0);
  a.shrink(v);
  return true;
}

//------------------------------------------------------------------------------
DECLARE_TEST(unsignedint, Arbitrary)
{
  testpp::Arbitrary<unsigned int> a;
  unsigned int v = a.generate(0,0);
  a.shrink(v);
  return true;
}

//------------------------------------------------------------------------------
DECLARE_TEST(long, Arbitrary)
{
  testpp::Arbitrary<long> a;
  long v = a.generate(0,0);
  a.shrink(v);
  return true;
}

//------------------------------------------------------------------------------
DECLARE_TEST(unsignedlong, Arbitrary)
{
  testpp::Arbitrary<unsigned long> a;
  unsigned long v = a.generate(0,0);
  a.shrink(v);
  return true;
}

//------------------------------------------------------------------------------
DECLARE_TEST(longlong, Arbitrary)
{
  testpp::Arbitrary<long long> a;
  long long v = a.generate(0,0);
  a.shrink(v);
  return true;
}

//------------------------------------------------------------------------------
DECLARE_TEST(unsignedlonglong, Arbitrary)
{
  testpp::Arbitrary<unsigned long long> a;
  unsigned long long v = a.generate(0,0);
  a.shrink(v);
  return true;
}

//------------------------------------------------------------------------------
DECLARE_TEST(float, Arbitrary)
{
  testpp::Arbitrary<float> a;
  float v = a.generate(0,0);
  a.shrink(v);
  return true;
}

//------------------------------------------------------------------------------
DECLARE_TEST(double, Arbitrary)
{
  testpp::Arbitrary<double> a;
  double v = a.generate(0,0);
  a.shrink(v);
  return true;
}

//------------------------------------------------------------------------------
DECLARE_TEST(longdouble, Arbitrary)
{
  testpp::Arbitrary<long double> a;
  long double v = a.generate(0,0);
  a.shrink(v);
  return true;
}

//------------------------------------------------------------------------------
DECLARE_TEST(vector, Arbitrary)
{
  testpp::Arbitrary<vector<int>> a;
  vector<int> v = a.generate(0,0);
  vector<vector<int>> vv = a.shrink(v);
  return vv.size() == 2
    && vv[0].size() == v.size()/2
    && vv[1].size() == v.size()/2;
}

//------------------------------------------------------------------------------
DECLARE_TEST(string, Arbitrary)
{
  testpp::Arbitrary<string> a;
  string v = a.generate(0,0);
  vector<string> vv = a.shrink(v);
  return vv.size() == 2
    && vv[0].size() == v.size()/2
    && vv[1].size() == v.size()/2;
}

//------------------------------------------------------------------------------
DECLARE_TEST(deque, Arbitrary)
{
  testpp::Arbitrary<deque<int>> a;
  deque<int> v = a.generate(0,0);
  vector<deque<int>> vv = a.shrink(v);
  return vv.size() == 2
    && vv[0].size() == v.size()/2
    && vv[1].size() == v.size()/2;
}

//------------------------------------------------------------------------------
DECLARE_TEST(list, Arbitrary)
{
  testpp::Arbitrary<list<int>> a;
  list<int> v = a.generate(0,0);
  vector<list<int>> vv = a.shrink(v);
  return vv.size() == 2
    && vv[0].size() == v.size()/2
    && vv[1].size() == v.size()/2;
}

//------------------------------------------------------------------------------
DECLARE_TEST(forward_list, Arbitrary)
{
  testpp::Arbitrary<forward_list<int>> a;
  forward_list<int> v = a.generate(0,0);
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
DECLARE_TEST(set, Arbitrary)
{
  testpp::Arbitrary<set<int>> a;
  set<int> v = a.generate(0,0);
  vector<set<int>> vv = a.shrink(v);
  return vv.size() == 2
    && vv[0].size() == v.size()/2
    && vv[1].size() == v.size()/2;
}

//------------------------------------------------------------------------------
DECLARE_TEST(multiset, Arbitrary)
{
  testpp::Arbitrary<multiset<int>> a;
  multiset<int> v = a.generate(0,0);
  vector<multiset<int>> vv = a.shrink(v);
  return vv.size() == 2
    && vv[0].size() == v.size()/2
    && vv[1].size() == v.size()/2;
}

//------------------------------------------------------------------------------
DECLARE_TEST(unordered_set, Arbitrary)
{
  testpp::Arbitrary<unordered_set<int>> a;
  unordered_set<int> v = a.generate(0,0);
  vector<unordered_set<int>> vv = a.shrink(v);
  return vv.size() == 2
    && vv[0].size() == v.size()/2
    && vv[1].size() == v.size()/2;
}

//------------------------------------------------------------------------------
DECLARE_TEST(pair, Arbitrary)
{
  testpp::Arbitrary<pair<int, float>> a;
  pair<int, float> v = a.generate(0,0);
  a.shrink(v);
  return true;
}

//------------------------------------------------------------------------------
DECLARE_TEST(map, Arbitrary)
{
  testpp::Arbitrary<map<int, float>> a;
  map<int, float> v = a.generate(0,0);
  vector<map<int, float>> vv = a.shrink(v);
  return vv.size() == 2
    && vv[0].size() == v.size()/2
    && vv[1].size() == v.size()/2;
}

//------------------------------------------------------------------------------
DECLARE_TEST(multimap, Arbitrary)
{
  testpp::Arbitrary<multimap<int, float>> a;
  multimap<int, float> v = a.generate(0,0);
  vector<multimap<int, float>> vv = a.shrink(v);
  return vv.size() == 2
    && vv[0].size() == v.size()/2
    && vv[1].size() == v.size()/2;
}

//------------------------------------------------------------------------------
DECLARE_TEST(unordered_map, Arbitrary)
{
  testpp::Arbitrary<unordered_map<int, float>> a;
  unordered_map<int, float> v = a.generate(0,0);
  vector<unordered_map<int, float>> vv = a.shrink(v);
  return vv.size() == 2
    && vv[0].size() == v.size()/2
    && vv[1].size() == v.size()/2;
}
