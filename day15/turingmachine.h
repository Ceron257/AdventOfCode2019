#pragma once

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <cassert>

using namespace std;

using Digits = vector<char>;

Digits getDigits (long long int number);


class TuringMachine
{
public:
  TuringMachine(TuringMachine const& other);
  TuringMachine(ifstream& input);

  static queue<long long int> inQueue;
  static queue<long long int> outQueue;
  
  void execute();
  bool halted () const;

private:
  long long int& access (long long int position, long long int mode);
  void ensureMemory (long long int position, long long int mode);
  long long int getOperand (long long int position, long long int mode);
  long long int readOpCode (long long int position) const;

private:
  vector<long long int> mBand;
  long long int mCurrentPos = 0;
  long long int mBaseAdress = 0;
};