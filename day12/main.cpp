#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>
#include <numeric>
#include <future>

using namespace std;

void applyGravityToComponent (int const& posA, int const& posB, int& velA, int& velB)
{
    if (posA < posB)
    {
      velA++;
    }
    else if (posA > posB)
    {
      velA--;
    }
}

class Position
{
public:
  Position (int x, int y, int z)
    : x(x), y(y), z(z)
  {}

  void applyGravity (Position& other)
  {
    applyGravityToComponent(x, other.x, vX, other.vX);
    applyGravityToComponent(y, other.y, vY, other.vY);
    applyGravityToComponent(z, other.z, vZ, other.vZ);
  }

  void applyVelocity ()
  {
    x += vX;
    y += vY;
    z += vZ;
  }

  int kineticEnergy () const
  {
    return abs(vX) + abs (vY) + abs(vZ);
  }

  int potentialEnergy () const
  {
    return abs(x) + abs (y) + abs(z);
  }

  int x;
  int y;
  int z;

  int vX = 0;
  int vY = 0;
  int vZ = 0;

  bool operator == (Position const& other) const
  {
    return x == other.x &&
           y == other.y &&
           z == other.z &&
           vX == other.vX &&
           vY == other.vY &&
           vZ == other.vZ;
  }
};

int main ()
{
  const char* inputFile = INPUTS_PATH "/day12.txt";

  ifstream input(inputFile);

  vector<Position> positions;

  int x = 0;
  int y = 0;
  int z = 0;
  char ignore;
  while (input >> ignore >> ignore >> ignore >> x >> ignore >> ignore >> ignore >> y >> ignore >> ignore >> ignore >> z >> ignore)
  {
    positions.emplace_back(x, y, z);
  }

  auto const initial = positions;
  
  for (size_t timeStep = 0; timeStep < 1e3; timeStep ++)
  {
    for (auto& first : positions)
    {
      for (auto& second : positions)
      {
        if (&first == &second) continue;
        first.applyGravity (second);
      }
    }
    for (auto& moon : positions)
    {
      moon.applyVelocity ();
    }
    bool equal = true;
    for (size_t i = 0; i < positions.size (); i++)
    {
      equal &= positions[i] == initial[i];
    }
  }

  auto energy = accumulate (positions.begin(), positions.end (), 0, [] (int const& A, Position const& B)
    {
      return B.kineticEnergy () * B.potentialEnergy () + A;
    });

  cout << "Energy: " << energy << endl;

  auto positions2 = initial;

  size_t xSteps = numeric_limits<size_t>::max();
  size_t ySteps = numeric_limits<size_t>::max();
  size_t zSteps = numeric_limits<size_t>::max();

  for (size_t timeStep = 1;timeStep < 1e32; timeStep++)
  {
    for (auto& first : positions2)
    {
      for (auto& second : positions2)
      {
        if (&first == &second) continue;
        first.applyGravity (second);
      }
    }
    for (auto& moon : positions2)
    {
      moon.applyVelocity ();
    }
    bool equal[3] = {true, true, true};
    for (size_t i = 0; i < positions2.size (); i++)
    {
      equal[0] &= positions2[i].x == initial[i].x && positions2[i].vX == initial[i].vX;
      equal[1] &= positions2[i].y == initial[i].y && positions2[i].vY == initial[i].vY;
      equal[2] &= positions2[i].z == initial[i].z && positions2[i].vZ == initial[i].vZ;
    }
    if (equal[0] && xSteps == numeric_limits<size_t>::max())
    {
      xSteps = timeStep;
      cout << "Steps for x:" << xSteps << endl;
    }
    if (equal[1] && ySteps == numeric_limits<size_t>::max())
    {
      ySteps = timeStep;
      cout << "Steps for y:" << ySteps << endl;
    }
    if (equal[2] && zSteps == numeric_limits<size_t>::max())
    {
      zSteps = timeStep;
      cout << "Steps for z:" << zSteps << endl;
    }

    if (xSteps < numeric_limits<size_t>::max() &&
        ySteps < numeric_limits<size_t>::max() &&
        zSteps < numeric_limits<size_t>::max())
    {
      break;
    }
  }
  cout << "Steps per cycle: " << lcm(lcm (xSteps, ySteps),zSteps) << endl;

  return 0;
}