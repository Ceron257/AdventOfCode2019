#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <set>
#include <tuple>
#include <cassert>
#include <vector>
#include <numeric>

#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

class Vec
{
public:

  Vec (int x, int y)
    : x (x)
    , y (y)
    {}

  Vec operator - (Vec const& other) const
  {
    return Vec (x - other.x, y - other.y);
  }

  bool operator== (Vec const& other) const
  {
    return x == other.x && y == other.y;
  }

  bool operator== (Vec& other)
  {
    return x == other.x && y == other.y;
  }

  Vec& normalize ()
  {
    auto divisor = gcd (x,y);
    x /= divisor;
    y /= divisor;
    return *this;
  }

  double angleToUp () const
  {
    auto up = Vec (0,-1);
    auto left = Vec (-1,0);

    auto upDot = dot(up);
    auto leftDot = dot(left);

    auto angle = acos (upDot/euclidLength());
    angle *= 180.0/M_PI;
    if (leftDot > 0)
    {
      angle = 360.0 - angle;
    }
    return angle;
  }

  bool operator < (Vec const& other) const
  {
    auto myAngle = Vec(*this).normalize().angleToUp();
    auto otherangle = Vec(other).normalize().angleToUp();
    if (myAngle < otherangle)
    {
      return true;
    }
    else if (myAngle == otherangle)
    {
      return length () < other.length();
    }
    return false;
  }

  double dot (Vec const& other) const
  {
    return double (x) * other.x + y * other.y;
  }

  double euclidLength () const
  {
    return sqrt(pow(x,2) + pow(y,2));
  }

  int length () const
  {
    return abs(x) + abs(y);
  }

  int x;
  int y;
  size_t numSightBlockers = 0;
  Vec* ref = nullptr;
};

struct MiningOrder
{
  bool operator ()(Vec const& lhs, Vec const& rhs) const
  {
    if (lhs.numSightBlockers < rhs.numSightBlockers)
    {
      return true;
    }
    else if (lhs.numSightBlockers == rhs.numSightBlockers)
    {
      return Vec(lhs).normalize().angleToUp() < Vec(rhs).normalize().angleToUp();
    }
    return false;
  }
};

int main ()
{
  const char* inputFile = INPUTS_PATH "/day10.txt";

  ifstream input(inputFile);

  vector<Vec> positions;

  char character = 0;
  int x = 0;
  int y = 0;
  while ((character = input.get ()) != ifstream::traits_type::eof())
  {
    if (character == '#')
    {
      positions.emplace_back(x,y);
    }
    x++;
    if (character == '\n')
    {
      x = 0;
      y++;
    }
  }

  vector<Vec> knownDiretions;
  size_t maxKnownDirections = 0;
  Vec bestPos (numeric_limits<int>::max(), numeric_limits<int>::max());

  for (auto const& refPos : positions)
  {
    for (auto& otherPos : positions)
    {
      if (&refPos == &otherPos)
      {
        continue;
      }
      auto possibleDirection = (otherPos - refPos).normalize();
      if (find(knownDiretions.begin(), knownDiretions.end(), possibleDirection) == knownDiretions.end())
      {
        knownDiretions.push_back(possibleDirection);
        knownDiretions.back().ref = &otherPos;
      }
    }
    if (knownDiretions.size () >= maxKnownDirections)
    {
      maxKnownDirections = knownDiretions.size();
      bestPos = refPos;
    }
    knownDiretions.clear();
  }
  cout << "maximum visible asteroids " << maxKnownDirections << " at (" << bestPos.x << ", " << bestPos.y << ")" << endl;

  vector<Vec> asteroidsToVaporize;
  for (auto& refPos : positions)
  {
    if (refPos == bestPos)
    {
      continue;
    }
    asteroidsToVaporize.push_back(refPos - bestPos);
    asteroidsToVaporize.back().ref = &refPos;
  }
  
  sort(asteroidsToVaporize.begin(), asteroidsToVaporize.end());
  size_t sightBlockers = 0;
  Vec currentDir = Vec(asteroidsToVaporize[0]).normalize ();
  for (auto& asteroid : asteroidsToVaporize)
  {
    if (!(Vec(asteroid).normalize() == currentDir))
    {
      currentDir = Vec(asteroid).normalize();
      asteroid.numSightBlockers = 0;
      sightBlockers = 1;
    }
    else
    {
      asteroid.numSightBlockers = sightBlockers;
      sightBlockers++;
    }
  }
  sort (asteroidsToVaporize.begin(), asteroidsToVaporize.end(), MiningOrder ());
  size_t index = 0;

  cout << "200th x*100+y = " << asteroidsToVaporize[199].ref->x * 100 + asteroidsToVaporize[199].ref->y << endl;
  return 0;
}