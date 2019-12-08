#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

class Image
{
public:
  Image (vector<unsigned short>&& pixels, size_t width, size_t height)
    : mPixels (move (pixels))
    , mWidth (width)
    , mHeight (height)
    , mLayers (mPixels.size () / (width * height))
    {}

  unsigned short getPixel (size_t layer, size_t x, size_t y) const
  {
    size_t index = (mWidth * mHeight) * layer + y * (mWidth) + x;
    return mPixels[index];
  }

  unsigned short getPixelColor (size_t x, size_t y) const
  {
    for (size_t layer = 0; layer < mLayers; layer++)
    {
      auto pixel = getPixel (layer, x, y);
      if (pixel != 2)
        return pixel;
    }
    return numeric_limits<unsigned short>::max ();
  }

  size_t width () const { return mWidth; }
  size_t height () const { return mHeight; }
  size_t layers () const { return mLayers; }

private:
  vector<unsigned short> mPixels;
  size_t mWidth = 0;
  size_t mHeight = 0;
  size_t mLayers = 0;
};

int main ()
{
  const char* inputFile = INPUTS_PATH "/day8.txt";

  ifstream input(inputFile);

  vector<unsigned short> pixels;

  char digit;
  while (input >> digit)
  {
    pixels.push_back (digit - '0');
  }

  Image screenshot (move (pixels), 25, 6);

  size_t zeroLayer = 0;
  size_t minNumZeros = numeric_limits<size_t>::max();
  size_t numZeros = 0;

  for (size_t layer = 0; layer < screenshot.layers(); layer++)
  {
    numZeros = 0;
    for (size_t y = 0; y < screenshot.height(); y++)
    {
      for (size_t x = 0; x < screenshot.width(); x++)
      {
        auto pixel = screenshot.getPixel (layer, x, y);
        if (pixel == 0)
        {
          numZeros++;
        }
      }
    }
    if (numZeros <  minNumZeros)
    {
      zeroLayer = layer;
      minNumZeros = numZeros;
    }
  }

  {
    size_t ones = 0;
    size_t twos = 0;
    for (size_t y = 0; y < screenshot.height(); y++)
    {
      for (size_t x = 0; x < screenshot.width(); x++)
      {
        auto pixel = screenshot.getPixel (zeroLayer, x, y);
        if (pixel == 1)
        {
          ones++;
        }
        else if (pixel == 2)
        {
          twos++;
        }
        cout << pixel;
      }
      cout << endl;
    }
    cout << endl;
    cout << "ones = " << ones << " twos = " << twos << endl;
    cout << "Result: " << ones * twos << endl;
  }

  cout << "Composed image:" << endl;
  for (size_t y = 0; y < screenshot.height(); y++)
  {
    for (size_t x = 0; x < screenshot.width(); x++)
    {
      auto pixel = screenshot.getPixelColor (x, y);
      if (pixel == 0)
      {
        cout << " ";
      }
      else
      {
        cout << "#";
      }
      
    }
    cout << endl;
  }
  
  return 0;
}