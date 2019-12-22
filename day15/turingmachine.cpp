#pragma once

#include "turingmachine.h"

using namespace std;

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

queue<long long int> TuringMachine::inQueue;
queue<long long int> TuringMachine::outQueue;

TuringMachine::TuringMachine(TuringMachine const& other)
  : mBand (other.mBand)
  , mCurrentPos (0)
{}

TuringMachine::TuringMachine(ifstream& input)
{
  long long int data;
  while (true) 
  {
    input >> data;
    mBand.emplace_back(data);
    input.ignore(1);
    if (input.peek() == -1)
      break;
  }
}

void TuringMachine::execute()
{
  long long int opCode = 0;
  while ((opCode = readOpCode(mCurrentPos)) != 99)
  {
    auto opDigits = getDigits (mBand[mCurrentPos]);
    ensureMemory(mCurrentPos + 1, opDigits[2]);
    ensureMemory(mCurrentPos + 2, opDigits[1]);
    bool useOP3 = false;
    if (opCode == 1 ||
        opCode == 2 ||
        opCode == 7 ||
        opCode == 8)
    {
      ensureMemory(mCurrentPos + 3, opDigits[0]);
      useOP3 = true;
    }
    long long int& op1 = access (mCurrentPos + 1, opDigits[2]);
    long long int& op2 = access (mCurrentPos + 2, opDigits[1]);
    long long int& op3 = useOP3 == true ? access (mCurrentPos + 3, opDigits[0]) : opCode;
    long long int input = 0;
    switch(opCode)
    {
      case 1: // op3 = op1 + op2
        op3 = op1 + op2;
        mCurrentPos += 4;
        break;
      case 2: // op3 = op1 * op2
        op3 = op1 * op2;
        mCurrentPos += 4;
        break;
      case 3: // op1 = user-input
        if (inQueue.empty ()) return;
        input = inQueue.front();
        inQueue.pop ();
        //cout << "in:" << input << endl;
        op1 = input;
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
        op3 = (op1 < op2);
        mCurrentPos += 4;
        break;
      case 8: // op3 = op1 == op2
        op3 = (op1 == op2);
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

bool TuringMachine::halted () const
{
  return readOpCode(mCurrentPos) == 99;
}

long long int& TuringMachine::access (long long int position, long long int mode)
{
  switch (mode)
  {
    case 0: // posiion mode
      return mBand [mBand[position]];
    case 2: // relative mode
      return mBand [mBand[position] +  mBaseAdress];
    case 1: // immediate mode
    default:
      return mBand [position];
  }
}

void TuringMachine::ensureMemory (long long int position, long long int mode)
{
  long long int targetIndex = position;
  if (targetIndex >= 0 && static_cast<size_t> (targetIndex) > mBand.size() - 1)
  {
    mBand.resize(targetIndex + 1);
  }
  switch (mode)
  {
    case 0:
      targetIndex = mBand[position];
      break;
    case 2:
      targetIndex = mBand[position] + mBaseAdress;
      break;
  }
  if (targetIndex >= 0 && static_cast<size_t> (targetIndex) > mBand.size() - 1)
  {
    mBand.resize(targetIndex + 1);
  }
}

long long int TuringMachine::readOpCode (long long int position) const
{
   auto opDigits = getDigits (mBand[position]);
   return *(opDigits.end()-2) * 10 + *(opDigits.end()-1);
}