#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <cassert>

using namespace std;

using Digits = vector<char>;

Digits getDigits (long long int number)
{
  string numberStr = to_string(number);
  Digits digits (numberStr.length (), 0);
  for (size_t i = 0; i < numberStr.length (); i++)
  {
     digits[i] = stoi (numberStr.substr (i, 1));
  }
  auto oldSize = digits.size ();
  for (size_t i = 0; i < 5 - oldSize; i++)
  {
    digits.insert(digits.begin(), 0);
  }
  return digits;
}

class TuringMachine
{
public:
  TuringMachine(TuringMachine const& other)
    : mBand (other.mBand)
    , mCurrentPos (0)
  {}

  TuringMachine(ifstream& input)
  {
    long long int mass;
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
    long long int opCode = 0;
    while ((opCode = readOpCode(mCurrentPos)) != 99)
    {
      auto opDigits = getDigits (mBand[mCurrentPos]);
      
      auto op1 = getOperand (mCurrentPos + 1, opDigits[2]);
      auto op2 = getOperand (mCurrentPos + 2, opDigits[1]);
      auto op3 = readValue (mCurrentPos + 3) + (opDigits[0] == 2 ? mBaseAdress : 0);
      long long int input = 0;
      switch(opCode)
      {
        case 1: // op3 = op1 + op2
          writeValue(op3, op1 + op2);
          mCurrentPos += 4;
          break;
        case 2: // op3 = op1 * op2
          writeValue(op3, op1 * op2);
          mCurrentPos += 4;
          break;
        case 3: // op1 = user-input
          cout << "in:";
          cin >> input;
          if (opDigits[2] == 2)
          {
            op1 = readValue (mCurrentPos + 1) + mBaseAdress;
          }
          writeValue (op1, input);
          mCurrentPos += 2;
          break;
        case 4: // output = op1
          cout << "out:" << op1 << endl;
          mCurrentPos += 2;
          break;
        case 5: // jump to op2 if op1 != 0
          if (op1 != 0)
          {
            mCurrentPos = op2;
            break;
          }
          mCurrentPos += 3;
          break;
        case 6: // jump to op2 if op1 == 0
          if (op1 == 0)
          {
            mCurrentPos = op2;
            break;
          }
          mCurrentPos += 3;
          break;
        case 7: // op3 = op1 < op2
          writeValue (op3, op1 < op2);
          mCurrentPos += 4;
          break;
        case 8: // op3 = op1 == op2
          writeValue (op3, op1 == op2);
          mCurrentPos += 4;
          break;
        case 9: // mBaseAdress = op1
          mBaseAdress += op1;
          mCurrentPos += 2;
          break;
        case 99:
          break;
      }
    }
  }

  bool halted () const
  {
    return readOpCode(mCurrentPos) == 99;
  }

  long long int readValue(long long int position)
  {
    if (static_cast<size_t> (position) > mBand.size() - 1)
    {
      mBand.resize(position + 1);
    }
    return mBand[position];
  }

  void writeValue(long long int position, long long int value)
  {
    if (static_cast<size_t> (position) > mBand.size() - 1)
    {
      mBand.resize(position + 1);
    }
    mBand[position] = value;
  }

private:
  long long int getOperand (long long int position, long long int mode)
  {
    switch (mode)
    {
      case 0: // posiion mode
        return readValue (readValue (position));
      case 2: // relative mode
        return readValue (readValue (position) + mBaseAdress);
      case 1: // immediate mode
      default:
        return readValue (position);
    }
  }

  long long int readOpCode (long long int position) const
  {
     auto opDigits = getDigits (mBand[position]);
     return *(opDigits.end()-2) * 10 + *(opDigits.end()-1);
  }

private:
  vector<long long int> mBand;
  long long int mCurrentPos = 0;
  long long int mBaseAdress = 0;
};

int main ()
{
  const char* inputFile = INPUTS_PATH "/day9.txt";

  ifstream input(inputFile);
  TuringMachine machine(input);
  machine.execute ();
  return 0;
}