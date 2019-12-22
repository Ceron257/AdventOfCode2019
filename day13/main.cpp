#include "turingmachine.h"
#include <map>
#include <iostream>
#include <thread>

using namespace std;

struct Position
{
public:
  int x;
  int y;

  bool operator < (Position const& other) const
  {
    if (x < other.x)
    {
      return true;
    }
    else if (x == other.x)
    {
      return y < other.y;
    }
    return false;
  }
};

enum class Type
{
  Empty,
  Wall,
  Block,
  HorizontalPaddle,
  Ball
};

Type typeFromInt (int i)
{
  switch (i)
  {
  case 0:
    return Type::Empty;
    break;
  case 1:
    return Type::Wall;
    break;
  case 2:
    return Type::Block;
    break;
  case 3:
    return Type::HorizontalPaddle;
    break;
  case 4:
    return Type::Ball;
    break;
  }
  return Type::Empty;
}

int main ()
{
  const char* inputFile = INPUTS_PATH "/day13.txt";

  ifstream input(inputFile);
  TuringMachine machine(input);

  auto hackedMaching = machine;
  
  map<Position, Type> world;

  machine.execute();
  while (!TuringMachine::outQueue.empty ())
  {
    auto x = TuringMachine::outQueue.front();
    TuringMachine::outQueue.pop();
    auto y = TuringMachine::outQueue.front();
    TuringMachine::outQueue.pop();
    auto type = TuringMachine::outQueue.front();
    TuringMachine::outQueue.pop();
    Position pos {static_cast<int> (x),static_cast<int> (y)};
    world[pos] = typeFromInt (static_cast<int>(type));
  }
  cout << count_if (
      world.begin (),
      world.end(),
      [] (auto const& tile) { return tile.second == Type::Block; }) << endl;

  hackedMaching.access(0, 1) = 2; // set money to 2
  world.clear();

  long long int score = 0;
  while (!hackedMaching.halted ())
  {
    hackedMaching.execute ();
    while (!TuringMachine::outQueue.empty ())
    {
      auto x = TuringMachine::outQueue.front();
      TuringMachine::outQueue.pop();
      auto y = TuringMachine::outQueue.front();
      TuringMachine::outQueue.pop();
      auto type = TuringMachine::outQueue.front();
      TuringMachine::outQueue.pop();
      Position pos {static_cast<int> (x),static_cast<int> (y)};
      if (pos.x == -1 && pos.y == 0)
      {
        score = type;
        continue;
      }
      world[pos] = typeFromInt (static_cast<int>(type));
    }
    int currentY = 0;
    int ballX = 0;
    int paddleX = 0;
    for (int y = 0; y < 23; y++)
    {
      for (int x = 0; x < 43; x++)
      {
        auto pos = Position {x, y};
        auto type = world[pos];
        switch (type)
        {
        case Type::Empty:
          break;
        case Type::Wall:
          break;
        case Type::Block:
          break;
        case Type::HorizontalPaddle:
          paddleX = x;
          break;
        case Type::Ball:
          ballX = x;
          break;
        }
      }
    }
    int userInput = 0;
    userInput = clamp (ballX - paddleX, -1, 1);
    TuringMachine::inQueue.push (userInput);
  }
  cout << "Score: " << score << endl;
  for (int y = 0; y < 23; y++)
  {
    for (int x = 0; x < 43; x++)
    {
      auto pos = Position {x, y};
      auto type = world[pos];
      switch (type)
      {
      case Type::Empty:
        printf(" ");
        break;
      case Type::Wall:
        printf("X");
        break;
      case Type::Block:
        printf("#");
        break;
      case Type::HorizontalPaddle:
        printf("-");
        break;
      case Type::Ball:
        printf("o");
        break;
      }
    }
      printf("\n");
  }
  
  return 0;
}