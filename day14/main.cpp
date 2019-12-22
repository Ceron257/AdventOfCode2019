#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <map>
#include <vector>

using namespace std;

class Reactor
{
public:
  struct Ingredient
  {
    Ingredient (size_t amount, string const&& elem)
      : mAmount (amount)
      , mElement (move(elem))
    {}
    bool operator < (Ingredient const& other) const
    {
      if (mAmount < other.mAmount) return true;
      if (mAmount == other.mAmount) return mElement < other.mElement;
      return false;
    }
    size_t mAmount;
    string mElement;
  };
  using Ingredients = vector<Ingredient>;

  Reactor (ifstream& input)
  {
    char x = 0;
    int amount = 0;
    char element[6] = {0, 0, 0, 0, 0, 0};
    unsigned short elementIndex = 0;

    auto resetAndPrintElement = [&element, &elementIndex] () -> string
    {
        elementIndex = 0;
        string elem (element);
        element[0] = 0;
        element[1] = 0;
        element[2] = 0;
        element[3] = 0;
        element[4] = 0;
        element[5] = 0;
        return elem;
    };
    Ingredients ingredients;
    
    while ((x = input.get()) != ifstream::traits_type::eof())
    {
      if (x >= 'A' && x <= 'Z')
      {
        element[elementIndex] = x;
        elementIndex++;
      }
      else if (x == ',')
      {
        auto elem = resetAndPrintElement();
        ingredients.emplace_back (amount, move(elem));
      }
      else if (x >= '0' && x <= '9')
      {
        resetAndPrintElement();
        input.unget();
        input >> amount;
      } else if (x == '=')
      {
        input >> x;
        if (x != '>')
        {
          cout << "ERROR: unexpected character: " << x << endl;
          break;
        }
        auto elem = resetAndPrintElement();
        ingredients.emplace_back(amount, move(elem));
      }
      else if (x == '\n')
      {
        auto elem = resetAndPrintElement();
        mReactions.insert(make_pair(Ingredient (amount, move(elem)), move (ingredients)));
      }
    }
  }

  size_t oreNeededToProduce (size_t amount, string const& element)
  {
    if (element == "ORE")
    {
      return amount;
    }

    auto reaction = find_if (mReactions.begin(), mReactions.end(),
      [&element] (auto& reaction)
      {
        return reaction.first.mElement == element;
      });
    if (reaction == mReactions.end())
    {
      cout << "ERROR: couldn't find reaction for " << element;
      return 0;
    }
    if (mSurplusses[element] >= amount)
    {
      mSurplusses[element] -= amount; // use surplus
      return 0;
    }
    auto reactionsNeeded = size_t (ceil(double(amount)/ reaction->first.mAmount));
    auto reactionsNeededSurplus = size_t (ceil((double(amount)-mSurplusses[element])/ reaction->first.mAmount));
    if (reactionsNeededSurplus < reactionsNeeded)
    {
      //we can take surplusses into account
      amount -= mSurplusses[element];
      mSurplusses[element] = 0;
      reactionsNeeded = reactionsNeededSurplus;
    }
    mSurplusses[element] += reaction->first.mAmount * reactionsNeeded - amount;
    return accumulate<Ingredients::const_iterator, size_t>(reaction->second.begin(), reaction->second.end(), 0,
      [reactionsNeeded, this] (size_t amountNeeded, auto const& ingredient)
      {
        return amountNeeded + oreNeededToProduce(reactionsNeeded * ingredient.mAmount, ingredient.mElement);
      });
  }

  void resetSurplusses ()
  {
    mSurplusses.clear();
  }

private:
  map<Ingredient, Ingredients> mReactions;
  map<string, size_t> mSurplusses;
};

int main ()
{
  const char* inputFile = INPUTS_PATH "/day14.txt";

  ifstream input(inputFile);
  Reactor reactor(input);

  string const fuel = "FUEL";
  
  auto oreNeeded = reactor.oreNeededToProduce(1, fuel);
  cout << oreNeeded << endl;

  size_t fuelProducable = 1;

  while (oreNeeded < 1e12)
  {
    fuelProducable += 5000; // coarse search
    oreNeeded = reactor.oreNeededToProduce(fuelProducable, fuel);
    reactor.resetSurplusses();
  }
  
  while (oreNeeded >= 1e12)
  {
    fuelProducable -= 500; // finer search
    oreNeeded = reactor.oreNeededToProduce(fuelProducable, fuel);
    reactor.resetSurplusses();
  }

  while (oreNeeded < 1e12)
  {
    fuelProducable += 1; // finest search
    oreNeeded = reactor.oreNeededToProduce(fuelProducable, fuel);
    reactor.resetSurplusses();
  }
  cout << fuelProducable - 1 << endl;
  return 0;
}