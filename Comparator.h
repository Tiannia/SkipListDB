#pragma once
#include <string>
#include <utility>

class IntComparator
{
public:
  inline int operator()(const int lhs, const int rhs) const
  {
    if (lhs < rhs)
    {
      return -1;
    }
    if (rhs < lhs)
    {
      return 1;
    }
    return 0;
  }
};

class StringComparator
{
public:
  inline int operator()(const std::string lhs, const std::string rhs) const
  {
    if (lhs < rhs)
    {
      return -1;
    }
    if (rhs < lhs)
    {
      return 1;
    }
    return 0;
  }
};

template <typename F, typename S>
class PairComparator
{
public:
  inline int operator()(const std::pair<F, S> lhs, const std::pair<F, S> rhs) const
  {
    return 0;
  }
};
