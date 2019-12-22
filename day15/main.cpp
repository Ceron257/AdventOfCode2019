#include "turingmachine.h"
#include <array>
#include <map>
#include <optional>
#include <queue>
#include <stack>
#include <thread>

using namespace std;

class RepairDroid
{
public:
  RepairDroid (TuringMachine& machine)
    : mBrain (machine)
    {
      mWorld[mPosition] = make_pair(TileType::Empty, false);
    }

  void run ()
  {
    while (!mBrain.halted())
    {
      auto nextMovement = getNextMovement ();
      if(!nextMovement)
      {
        nextMovement = backTrack();
      }
      TuringMachine::inQueue.push(static_cast<long long int>(*nextMovement));
      mBrain.execute();
      if (TuringMachine::outQueue.empty()) break;
      processResponse(static_cast<Movement>(*nextMovement));
      //printImage();
      //cout << "----------" << endl;
      //this_thread::sleep_for(25ms);
    }
    printImage();
    cout << stepsToOxygenSystem << " steps to reach oxygen system." << endl;
    auto oxygenSystemPositionIt = find_if(mWorld.begin(), mWorld.end(), 
      [] (auto const it)
      {
        return it.second.first == TileType::OxygenSystem;
      });
    if (oxygenSystemPositionIt != mWorld.end())
    {
      releaseOxygen(oxygenSystemPositionIt->first);
    }
  }

  void printImage()
  {
    Position minPosition {numeric_limits<int>::max(), numeric_limits<int>::max()};
    Position maxPosition {numeric_limits<int>::min(), numeric_limits<int>::min()};

    for (auto const& position : mWorld)
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
        Position pos = {x,y};
        if (pos == mPosition)
        {
          cout << "D";
          continue;
        }
        auto typeIt = mWorld.find (pos);
        if (typeIt != mWorld.end())
        {
          switch (typeIt->second.first)
          {
            case TileType::Wall:
              cout << "#";
              break;
            case TileType::Empty:
              if(typeIt->second.second)
              {
                cout << "O";
              }
              else
              {
                cout << ".";
              }
              break;
            case TileType::OxygenSystem:
              cout << "X";
              break;
          }
        }
        else
        {
          cout << " ";
        }
      }
      cout << endl;
    }
  }

private:
  
  enum class Movement
  {
    North = 1,
    South = 2,
    West = 3,
    East = 4
  };

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

    bool operator == (Position const& other) const
    {
      return x == other.x && y == other.y;
    }
  };

private:

  void processResponse (Movement movementCommand, bool backTracking = false)
  {
    auto output = TuringMachine::outQueue.front();
    TuringMachine::outQueue.pop();

    auto nextPosition = getNextPosition(movementCommand, mPosition);

    if (output == 0)
    {
      mWorld[nextPosition] = make_pair(TileType::Wall, true);
    }
    else if (output == 1)
    {
      mPosition = nextPosition;
      if (!backTracking)
        mMovementStack.push(movementCommand);
      mWorld[mPosition] = make_pair(TileType::Empty, false);
    }
    else if (output == 2)
    {
      mPosition = nextPosition;
      if (!backTracking)
        mMovementStack.push(movementCommand);
      mWorld[mPosition] = make_pair(TileType::OxygenSystem, false);
      stepsToOxygenSystem = mMovementStack.size();
    }
  }

  optional<Movement> getNextMovement () const
  {
    auto north = getNextPosition(Movement::North, mPosition);
    auto south = getNextPosition(Movement::South, mPosition);
    auto east = getNextPosition(Movement::East, mPosition);
    auto west = getNextPosition(Movement::West, mPosition);

    if      (mWorld.count(north) == 0) {return Movement::North;}
    else if (mWorld.count(south) == 0) {return Movement::South;}
    else if (mWorld.count(east)  == 0) {return Movement::East;}
    else if (mWorld.count(west)  == 0) {return Movement::West;}
    return nullopt;
  }

  optional<Movement> backTrack ()
  {
    optional<Movement> nextMovement;
    while (!(nextMovement = getNextMovement()))
    {
      if (mMovementStack.empty ()) break;
      auto backMovement = invertMovement(mMovementStack.top());
      mMovementStack.pop();
      TuringMachine::inQueue.push(static_cast<long long int>(backMovement));
      mBrain.execute();
      processResponse(backMovement, true);
    }
    return nextMovement;
  }

  Movement invertMovement (Movement movement) const
  {
    switch (movement)
    {
      case Movement::North:
        return Movement::South;
        break;
      case Movement::South:
        return Movement::North;
        break;
      case Movement::East:
        return Movement::West;
        break;
      case Movement::West:
        return Movement::East;
        break;
    }
    return Movement::North;
  }

  Position getNextPosition (Movement movementCommand, Position const& reference) const
  {
    Position newPos = reference;
    switch (movementCommand)
    {
      case Movement::North:
        newPos.y += 1;
        break;
      case Movement::South:
        newPos.y -= 1;
        break;
      case Movement::West:
        newPos.x -= 1;
        break;
      case Movement::East:
        newPos.x += 1;
        break;
    }
    return newPos;
  }

  vector<Position> getAdjacentPositions (Position const& position)
  {
    array<Movement, 4> movements = {Movement::North, Movement::South, Movement::West, Movement::East};
    vector<Position> positions;
    for (auto movement : movements)
    {
      auto positionCandidate = getNextPosition(movement, position);
      if (mWorld[positionCandidate].first == TileType::Empty &&
          !mWorld[positionCandidate].second)
      {
        positions.emplace_back(positionCandidate);
      }
    }
    return positions;
  }

  void releaseOxygen (Position const& oxygenSystemPosition)
  {
    mWorld[oxygenSystemPosition].second = true;
    mOxygenLessTiles.push(oxygenSystemPosition);
    size_t timePassed = 0;
    while(!mOxygenLessTiles.empty())
    {
      queue<Position> nextTileQueue;
      while (!mOxygenLessTiles.empty())
      {
        auto currentTile = mOxygenLessTiles.front(); mOxygenLessTiles.pop();
        auto nextTiles = getAdjacentPositions (currentTile);
        for (auto tile : nextTiles)
        {
          nextTileQueue.push(tile);
          mWorld[tile].second = true;
        }
      }
      mOxygenLessTiles.swap(nextTileQueue);
      timePassed++;
      //this_thread::sleep_for(1000ms);
      //printImage();
    }
    cout << "Area filled with oxygen after " << timePassed - 1 << " minutes." << endl;
  }

private:
  TuringMachine& mBrain;

  Position mPosition = {0, 0};
  
  enum class TileType
  {
    Wall = 0,
    Empty = 1,
    OxygenSystem = 2
  };

  queue<Position> mOxygenLessTiles;
  stack<Movement> mMovementStack;
  size_t stepsToOxygenSystem = 0;
  map<Position, pair<TileType, bool>> mWorld;
};

int main ()
{
  const char* inputFile = INPUTS_PATH "/day15.txt";

  ifstream input(inputFile);
  TuringMachine machine(input);

  RepairDroid repairDroid (machine);
  repairDroid.run();
  return 0;
}