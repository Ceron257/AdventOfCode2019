#include "turingmachine.h"
#include <map>

using namespace std;

class PaintingBot
{
public:
  PaintingBot (TuringMachine& machine)
    : mBrain (machine) {}

  void run ()
  {
    while (!mBrain.halted())
    {
      auto color = readColor();
      TuringMachine::inQueue.push(color);
      mBrain.execute();
      paintPosition();
      turn();
      move();
    }

    cout << "Painted " << mPaintedPositions.size () << endl;
  }

  void paintPosition ()
  {
    auto color = TuringMachine::outQueue.front();
    TuringMachine::outQueue.pop();

    auto knownPosition = mPaintedPositions.find(mPosition);
    if (knownPosition != mPaintedPositions.end())
    {
      if (color != knownPosition->second.color)
      {
        knownPosition->second.numPaints++;
        knownPosition->second.color = color;
      }
    }
    else
    {
      if (color != 0)
        mPaintedPositions.insert ({mPosition, {1, color}});
    }
  }

  void turn ()
  {
    auto direction = TuringMachine::outQueue.front();
    TuringMachine::outQueue.pop();
    if (direction == 0)
    {
      turnLeft();
    }
    else
    {
      turnRight();
    }
  }

  void move ()
  {
    switch (mOrientation)
    {
    case Orientation::Up:
      mPosition.y += 1;
      break;
    
    case Orientation::Down:
      mPosition.y -= 1;
      break;
    
    case Orientation::Left:
      mPosition.x -= 1;
      break;
    
    case Orientation::Right:
      mPosition.x += 1;
      break;
    }
  }

  void printImage()
  {
    Position minPosition {numeric_limits<int>::max(), numeric_limits<int>::max()};
    Position maxPosition {numeric_limits<int>::min(), numeric_limits<int>::min()};

    for (auto const& position : mPaintedPositions)
    {
      if (position.first.x < minPosition.x) minPosition.x = position.first.x;
      if (position.first.y < minPosition.y) minPosition.y = position.first.y;
      
      if (position.first.x > maxPosition.x) maxPosition.x = position.first.x;
      if (position.first.y > maxPosition.y) maxPosition.y = position.first.y;
    }

    for (int y = maxPosition.y + 1; y >= minPosition.y; y--)
    {
      for (int x = minPosition.x; x <= maxPosition.x; x++)
      {
        auto colorIt = mPaintedPositions.find ({x,y});
        if (colorIt != mPaintedPositions.end())
        {
          switch (colorIt->second.color)
          {
            case 0:
              cout << ".";
              break;
            case 1:
              cout << "#";
              break;
          }
        }
        else
        {
          cout << ".";
        }
      }
      cout << endl;
    }
  }

private:
  void turnLeft()
  {
    switch (mOrientation)
    {
    case Orientation::Up:
      mOrientation = Orientation::Left;
      break;
    
    case Orientation::Down:
      mOrientation = Orientation::Right;
      break;
    
    case Orientation::Left:
      mOrientation = Orientation::Down;
      break;
    
    case Orientation::Right:
      mOrientation = Orientation::Up;
      break;
    }
  }

  void turnRight()
  {
    switch (mOrientation)
    {
    case Orientation::Up:
      mOrientation = Orientation::Right;
      break;
    
    case Orientation::Down:
      mOrientation = Orientation::Left;
      break;
    
    case Orientation::Left:
      mOrientation = Orientation::Up;
      break;
    
    case Orientation::Right:
      mOrientation = Orientation::Down;
      break;
    }
  }

  long long int readColor()
  {
    auto paintedTile = mPaintedPositions.find(mPosition);

    if (paintedTile != mPaintedPositions.end())
    {
      return paintedTile->second.color;
    }
    return 0;
  }

private:
  TuringMachine& mBrain;

  enum class Orientation
  {
    Up, Down, Left, Right
  };
  Orientation mOrientation = Orientation::Up;
  struct Position
  {
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
  Position mPosition {0,0};
  
  struct PaintInfo
  {
    int numPaints = 0;
    long long int color = 0;
  };
  map<Position, PaintInfo> mPaintedPositions;
};

int main ()
{
  const char* inputFile = INPUTS_PATH "/day11.txt";

  ifstream input(inputFile);
  TuringMachine machine(input);
  TuringMachine machine2(machine);
  
  PaintingBot bot(machine);
  bot.run();

  PaintingBot bot2(machine2);
  TuringMachine::outQueue.push(1);
  bot2.paintPosition();
  bot2.run();
  bot2.printImage();
  return 0;
}