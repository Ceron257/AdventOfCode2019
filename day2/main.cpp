#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

class TuringMachine
{
public:
  TuringMachine(ifstream& input)
  {
    int mass;
    while (true) 
    {
      input >> mass;
      mBand.emplace_back(mass);
      input.ignore(1);
      if (input.peek() == -1)
        break;
    }
  }

  void execute()
  {
    while (readValue(mCurrentPos) != 99)
    {
      auto op1 = readValue(readValue(mCurrentPos + 1));
      auto op2 = readValue(readValue(mCurrentPos + 2));
      auto op3 = readValue(mCurrentPos + 3);
      switch(mBand[mCurrentPos])
      {
        case 1:
          writeValue(op3, op1 + op2);
          break;
        case 2:
          writeValue(op3, op1 * op2);
          break;
        case 99:
          break;
      }
      mCurrentPos += 4;
    }
  }

  int readValue(int position)
  {
    if (position > mBand.size() - 1)
    {
      mBand.resize(position + 1);
    }
    return mBand[position];
  }

  void writeValue(int position, int value)
  {
    if (position > mBand.size() - 1)
    {
      mBand.resize(position + 1);
    }
    mBand[position] = value;
  }

private:
  vector<int> mBand;
  int mCurrentPos = 0;
};

int main ()
{
  const char* inputFile = INPUTS_PATH "/day2.txt";

  ifstream input(inputFile);
  TuringMachine machine(input);

  machine.writeValue(1, 12);
  machine.writeValue(2,2);

  machine.execute();

  cout << "value at position 0: " << machine.readValue(0) << endl;

  for(size_t verb = 0; verb < 100; verb++)
  {
    for(size_t noun = 0; noun < 100; noun++)
    {
      input.clear();
      input.seekg(0, ios_base::beg);
      TuringMachine tryMachine(input);
      tryMachine.writeValue(1, verb);
      tryMachine.writeValue(2, noun);

      tryMachine.execute();

      auto result = tryMachine.readValue(0);
      if (result == 19690720)
      {
        cout << "verb = " << verb << " noun = " << noun << endl;
        break;
      }
    }
  }
  return 0;
}