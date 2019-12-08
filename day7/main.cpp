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

static queue<int> inQueue;
static queue<int> outQueue;

class TuringMachine
{
public:
  TuringMachine(TuringMachine const& other)
    : mBand (other.mBand)
    , mCurrentPos (0)
  {}

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
        case 1: // op3 = op1 + op2
          writeValue(op3, op1 + op2);
          mCurrentPos += 4;
          break;
        case 2: // op3 = op1 * op2
          writeValue(op3, op1 * op2);
          mCurrentPos += 4;
          break;
        case 3: // op1 = user-input
          if (inQueue.empty ()) return;
          input = inQueue.front();
          inQueue.pop ();
          //cout << "in:" << input << endl;
          op1 = getOperand(mCurrentPos + 1, 1);
          writeValue (op1, input);
          mCurrentPos += 2;
          break;
        case 4: // output = op1
          //cout << "out:" << op1 << endl;
          outQueue.push (op1);
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
        case 99:
          break;
      }
    }
  }

  bool halted () const
  {
    return readOpCode(mCurrentPos) == 99;
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
      default:
        return readValue (position);
    }
  }

  int readOpCode (int position) const
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
  const char* inputFile = INPUTS_PATH "/day7.txt";

  ifstream input(inputFile);
  TuringMachine machine(input);

  vector<TuringMachine> amplifiers (5, machine);

  {
    Digits phaseSequence = {0, 1, 2, 3, 4};
    auto maximumSequence = phaseSequence;
    int maximumOutput = 0;

    do 
    {
      auto localAmplifiers (amplifiers);
      inQueue.push (phaseSequence[0]); // phase of first amplifier
      inQueue.push (0); //input for first amplifier
      for (size_t i = 0; i < 5; i++)
      {
        if (i < 4) inQueue.push (phaseSequence[i+1]);
        localAmplifiers[i].execute ();
        inQueue.push (outQueue.front ());
        outQueue.pop ();
      }
      if (inQueue.front () > maximumOutput)
      {
        maximumOutput = inQueue.front ();
        maximumSequence = phaseSequence;
      }
      inQueue.pop ();
      while (!inQueue.empty ())
      {
        assert (false);
        cout << "+out: " << inQueue.front () << endl;
        inQueue.pop();
      }
    }
    while (next_permutation (phaseSequence.begin (), phaseSequence.end ()));

    cout << "Maximum output: " << maximumOutput << " with sequence";
    for (int seq : maximumSequence)
    {
      cout << " " << seq;
    }
    cout << endl;
  }
  cout << "Part 2:" << endl;
  {
    Digits phaseSequence = {5, 6, 7, 8, 9};
    auto maximumSequence = phaseSequence;
    int maximumOutput = 0;

    do 
    {
      auto localAmplifiers (amplifiers);
      bool initialRun = true;
      while (any_of (localAmplifiers.begin (), localAmplifiers.end (), [] (auto const& amplifier)
                                                                          { return !amplifier.halted ();}))
      {
        if (initialRun)
        {
          inQueue.push (phaseSequence[0]); // phase of first amplifier
          inQueue.push (0); //input for first amplifier
        }
        else
        {
          //enqueue input again
          inQueue.push(inQueue.front());
          inQueue.pop();
        }
        for (size_t i = 0; i < 5; i++)
        {
          localAmplifiers[i].execute ();
          if (initialRun && i < 4) inQueue.push (phaseSequence[i+1]);
          inQueue.push (outQueue.front ()); // re-wire output to next machine
          outQueue.pop ();
        }
        if (inQueue.back () > maximumOutput)
        {
          maximumOutput = inQueue.back ();
          maximumSequence = phaseSequence;
        }
        initialRun = false;
      }
      inQueue.pop ();
      while (!inQueue.empty ())
      {
        assert (false);
        cout << "+out: " << inQueue.front () << endl;
        inQueue.pop();
      }
    }
    while (next_permutation (phaseSequence.begin (), phaseSequence.end ()));

    cout << "Maximum output: " << maximumOutput << " with sequence";
    for (int seq : maximumSequence)
    {
      cout << " " << seq;
    }
    cout << endl;
  }
  return 0;
}