#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

int calculateFuelNeed (int mass, bool recursive = true)
{
  auto requiredFuel = (mass / 3) - 2;
  if (recursive && requiredFuel > 0)
  {
    return requiredFuel + calculateFuelNeed (requiredFuel);
  }
  return requiredFuel < 0 ? 0 : requiredFuel;
}

int main ()
{
  const char* inputFile = INPUTS_PATH "/day1.txt";

  ifstream input(inputFile);

  cout << "100756 -> " << calculateFuelNeed (100756) << endl;

  int neededFuel = 0;
  int neededFuelRecursive = 0;
  int mass;
  while (input >> mass)
  {
    neededFuel += calculateFuelNeed(mass, false);
    neededFuelRecursive += calculateFuelNeed(mass);
  }
  cout << "Part 1 fuel needs: " << neededFuel << endl;
  cout << "Part 2 fuel needs: " << neededFuelRecursive << endl;
  return 0;
}