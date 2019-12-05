#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

using Digits = vector<char>;
Digits getDigits (int number)
{
  string numberStr = to_string(number);
  Digits digits (numberStr.length (), 0);
  for (size_t i = 0; i < numberStr.length (); i++)
  {
     digits[i] = stoi (numberStr.substr (i, 1));
  }
  auto oldSize = digits.size ();
  for (size_t i = 0; i < 4 - oldSize; i++)
  {
    digits.insert(digits.begin(), 0);
  }
  return digits;
}

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
    int opCode = 0;
    while ((opCode = readOpCode(mCurrentPos)) != 99)
    {
      auto opDigits = getDigits (mBand[mCurrentPos]);
      
      auto op1 = getOperand (mCurrentPos + 1, opDigits[1]);
      auto op2 = getOperand (mCurrentPos + 2, opDigits[0]);
      auto op3 = readValue (mCurrentPos + 3);
      int input = 0;
      switch(opCode)
      {
        case 1:
          writeValue(op3, op1 + op2);
          mCurrentPos += 4;
          break;
        case 2:
          writeValue(op3, op1 * op2);
          mCurrentPos += 4;
          break;
        case 3:
          cout << "in:";
          cin >> input;
          op1 = getOperand(mCurrentPos + 1, 1);
          writeValue (op1, input);
          mCurrentPos += 2;
          break;
        case 4:
          op1 = getOperand(mCurrentPos + 1, 1);
          cout << "out:" << readValue (op1) << endl;
          mCurrentPos += 2;
          break;
        case 99:
          break;
      }
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
  int getOperand (int position, int mode)
  {
    switch (mode)
    {
      case 0:
        return readValue (readValue (position));
      case 1:
        return readValue (position);
    }
  }

  int readOpCode (int position)
  {
     auto opDigits = getDigits (mBand[position]);
     return *(opDigits.end()-2) * 10 + *(opDigits.end()-1);
  }

private:
  vector<int> mBand;
  int mCurrentPos = 0;
};

int main ()
{
  const char* inputFile = INPUTS_PATH "/day5.txt";

  ifstream input(inputFile);
  TuringMachine machine(input);

  machine.execute();
  return 0;
}