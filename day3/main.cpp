#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <set>
#include <tuple>
#include <cassert>

using namespace std;

using Position = pair<int, int>;
using CableSegment = pair<Position, Position>;
struct cableLess {
  bool operator () (CableSegment const& lhs, CableSegment const& rhs) const
  {
    auto lhsMax = max(abs(lhs.first.first), (lhs.first.second));
    auto rhsMax = max(abs(rhs.first.first), (rhs.first.second));
    return lhsMax < rhsMax;
  }
};
using Cable = set<CableSegment>;//, cableLess>;

int manhattenDist (Position const& segment)
{
  if (segment.first == numeric_limits<int>::max() ||
      segment.second == numeric_limits<int>::max())
      {
        return numeric_limits<int>::max();
      }
  return abs(segment.first) + abs(segment.second);
}

int dot (Position const& lhs, Position const& rhs)
{
  return lhs.first * rhs.first + lhs.second * rhs.second;
}

tuple<Position, bool> intersect (CableSegment const& lhs, CableSegment const& rhs)
{
  auto lhsStart = lhs.first;
  auto lhsDir = lhs.second;
  auto rhsStart = rhs.first;
  auto rhsDir = rhs.second;
  if (dot(lhsDir, rhsDir) != 0)
  {
    return make_tuple(make_pair(0, 0), false);
  }
  int x;
  int y;
  if (lhsDir.first == 0)
  {
    x = lhsStart.first;
    y = rhsStart.second;
    auto lowerXBound = min(rhsStart.first, rhsStart.first + rhsDir.first);
    auto upperXBound = max(rhsStart.first, rhsStart.first + rhsDir.first);
    auto lowerYBound = min(lhsStart.second, lhsStart.second + lhsDir.second);
    auto upperYBound = max(lhsStart.second, lhsStart.second + lhsDir.second);
    return make_tuple(make_pair(x, y), x >= lowerXBound &&
                                       x <= upperXBound &&
                                       y >= lowerYBound &&
                                       y <= upperYBound);
  }
  else
  {
    x = rhsStart.first;
    y = lhsStart.second;
    auto lowerXBound = min(lhsStart.first, lhsStart.first + lhsDir.first);
    auto upperXBound = max(lhsStart.first, lhsStart.first + lhsDir.first);
    auto lowerYBound = min(rhsStart.second, rhsStart.second + rhsDir.second);
    auto upperYBound = max(rhsStart.second, rhsStart.second + rhsDir.second);
    return make_tuple(make_pair(x, y), x >= lowerXBound &&
                                       x <= upperXBound &&
                                       y >= lowerYBound &&
                                       y <= upperYBound);
  }
}

Position createDir (char D, int manhattenDist)
{
  Position p;
  switch (D)
  {
  case 'L':
    p = make_pair(-manhattenDist, 0);
    break;  
  case 'R':
    p = make_pair(manhattenDist, 0);
    break;
  case 'U':
    p = make_pair(0, manhattenDist);
    break;
  case 'D':
    p = make_pair(0, -manhattenDist);
    break;
  default:
    assert(false);
    break;
  }
  return p;
}

Position operator + (Position const& lhs, Position const& rhs)
{
  return make_pair(lhs.first + rhs.first, lhs.second + rhs.second);
}

int main ()
{
  const char* inputFile = INPUTS_PATH "/day3.txt";

  ifstream input(inputFile);

  Cable firstCable;
  Cable secondCable;

  Cable* cablePtr = &firstCable;

  char direction;
  int segmentmanhattenDist;
  Position start = make_pair(0 ,0);
  while (input >> direction && input >> segmentmanhattenDist)
  {
    char x = input.get();
    if (x != ',')
    {
      input.unget();
    }    
    auto dir = createDir (direction, segmentmanhattenDist);
    auto [elem, inserted] = cablePtr->emplace(start, dir);

    start = start + dir;
    if (x == '\n')
    {
      cablePtr = &secondCable;
      start = make_pair(0 ,0);
      continue;
    }
  }

  Position intersection = make_pair (numeric_limits<int>::max(), numeric_limits<int>::max());

  for (auto const& firstSeg : firstCable)
  {
    for (auto const& secondSeg : secondCable)
    {
      auto [position, foundIntersection] = intersect (firstSeg, secondSeg);
      if (foundIntersection && (position.first != 0 || position.second != 0) && manhattenDist(position) < manhattenDist(intersection))
      {
        intersection = position;
      }
    }
  }
  cout << "Distance: " << manhattenDist(intersection) << endl;
  return 0;
}