#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>
#include <chrono>

using namespace std;

using Digits = vector<char>;
Digits getDigits (int number)
{
   Digits digits (6, 0);
   string numberStr = to_string(number);
   for (size_t i = 0; i < 6; i++)
   {
      digits[i] = stoi (numberStr.substr (i, 1));
   }
   return digits;
}

bool hasAdjacentDigits (Digits const& digits)
{
  for (size_t digitIndex = 0; digitIndex < 5; digitIndex++)
  {
    if (digits[digitIndex] == digits[digitIndex + 1])
    {
      return true;
    }
  }
  return false;
}

bool hasAdjacentDigitsStrict (Digits const& digits)
{
  auto lastDigit = digits[0];
  size_t timesSeenLastDigit = 1;
  bool foundAdjacentDigits = false;
  for (size_t digitIndex = 1; digitIndex < 6; digitIndex++)
  {
    if (digits[digitIndex] == lastDigit)
    {
      timesSeenLastDigit++;
    }
    else
    {
      if (timesSeenLastDigit == 2)
      {
        foundAdjacentDigits = true;
      }
      lastDigit = digits[digitIndex];
      timesSeenLastDigit = 1;
    }
  }
  return foundAdjacentDigits || timesSeenLastDigit == 2;
}

bool hasIncreasingDigits (Digits const& digits)
{
  Digits::value_type maximumDigit = digits[0];
  for (size_t digitIndex = 1; digitIndex < 6; digitIndex++)
  {
    if (digits[digitIndex] >= maximumDigit)
    {
      maximumDigit = digits[digitIndex];
    }
    else
    {
      return false;
    }
  }
  return true;
}

int main ()
{
  auto start = chrono::high_resolution_clock::now();
  int startNumber = 245182;
  int stopNumber = 790572;
  auto isPasswordValid = [] (int password)
  {
    auto digits = getDigits (password);
    return hasAdjacentDigits (digits) && hasIncreasingDigits (digits);
  };
  
  vector<int> possiblePasswords (stopNumber - startNumber + 1, 0);
  iota (possiblePasswords.begin (), possiblePasswords.end (), startNumber);

  auto validPasswords = count_if (possiblePasswords.begin (), possiblePasswords.end (), isPasswordValid);

  auto isPasswordValidStrict = [] (int password)
  {
    auto digits = getDigits (password);
    return hasAdjacentDigitsStrict (digits) && hasIncreasingDigits (digits);
  };

  auto validPasswordsStrict = count_if (possiblePasswords.begin (), possiblePasswords.end (), isPasswordValidStrict);

  auto end = chrono::high_resolution_clock::now();

  chrono::duration<double, milli> duration = end - start;

  cout << "runtime " << duration.count() << endl;

  cout << "Found " << validPasswords << " valid passwords!" << endl;
  cout << "Found " << validPasswordsStrict << " valid strict passwords!" << endl;
  return 0;
}