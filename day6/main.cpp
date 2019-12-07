#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <cassert>
#include <stack>

using namespace std;

class OrbitalMapEntry
{
public:
  OrbitalMapEntry (string const& data)
  {
    auto delimiterPosition = data.find (')');
    assert (delimiterPosition != string::npos);
    mID = data.substr (delimiterPosition + 1);
    mParentID = data.substr (0, delimiterPosition);
  }

  size_t countOrbits () const
  {
    size_t orbits = 1;
    OrbitalMapEntry const* entry = this;
    while (entry->mParentPtr != nullptr)
    {
      orbits++;
      entry = entry->mParentPtr;
    }
    return orbits;
  }

  OrbitalMapEntry* mParentPtr = nullptr;
  string mID;
  string mParentID;
};

class OrbitalMap
{
public:
  void addEntry (OrbitalMapEntry&& newEntry)
  {
    auto [insertedEntry, inserted] = mEntries.insert ({newEntry.mID, std::move (newEntry)});
    assert (inserted);

    auto parentEntry = mEntries.find (insertedEntry->second.mParentID);
    if (parentEntry != mEntries.end ())
    {
      insertedEntry->second.mParentPtr = &parentEntry->second;
    }

    for (auto& entry : mEntries)
    {
      if (&entry.second == &insertedEntry->second) continue;

      if (entry.second.mParentID == insertedEntry->second.mID)
      {
        entry.second.mParentPtr = &insertedEntry->second;
      }
    }

    if (mCenterOfMass == nullptr && insertedEntry->second.mParentID == "COM")
    {
      mCenterOfMass = &insertedEntry->second;
    }
  }

  size_t countOrbits () const
  {
    size_t orbits = accumulate (mEntries.begin (), mEntries.end (), static_cast<size_t> (0), [] (size_t count, auto const& entry)
    {
      return count + entry.second.countOrbits ();
    });
    return orbits;
  }

  size_t calculateTransforms (string const& from, string const& to) const
  {
    auto fromEntry = mEntries.find (from)->second;
    auto toEntry = mEntries.find (to)->second;
    auto root = getCommonRoot (fromEntry, toEntry);

    return (fromEntry.countOrbits () + toEntry.countOrbits ()) - 2 * root.countOrbits () - 2;
  }

  OrbitalMapEntry const& getCommonRoot (OrbitalMapEntry const& left, OrbitalMapEntry const& right) const
  {
    auto leftPath = getEntryPath (left);
    auto rightPath = getEntryPath (right);
    auto root = leftPath.top ();
    while (leftPath.top () == rightPath.top ())
    {
      root = leftPath.top ();
      leftPath.pop ();
      rightPath.pop ();
    }
    return mEntries.find (root)->second;
  }

  stack<string> getEntryPath (OrbitalMapEntry const& entry) const
  {
    stack<string> path;
    OrbitalMapEntry const* entryPtr = &entry;
    while (entryPtr->mParentPtr != nullptr)
    {
      path.push (entryPtr->mID);
      entryPtr = entryPtr->mParentPtr;
    }
    return path;
  } 

private:
  OrbitalMapEntry* mCenterOfMass = nullptr;
  map<string, OrbitalMapEntry> mEntries;
};

int main ()
{
  const char* inputFile = INPUTS_PATH "/day6.txt";

  ifstream input(inputFile);

  OrbitalMap map;
  string line;
  while (input >> line)
  {
    OrbitalMapEntry entry (line);
    map.addEntry (std::move (entry));
  }
  cout << "Orbits: " << map.countOrbits() << endl;
  cout << "Needed transforms: " << map.calculateTransforms("YOU", "SAN") << endl;

  return 0;
}