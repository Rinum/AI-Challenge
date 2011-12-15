#ifndef LOCATION_H_
#define LOCATION_H_

#include <cmath>

/*
    struct for representing locations in the grid.
*/
struct Location
{
  int row, col;
  int dummy;

  Location()
  {
    row = col = 0;
    dummy = 1;//is a non-used location
  };
  
  Location(int r, int c)
  {
    row = r;
    col = c;
    dummy = 0;
  };

  void operator = (const Location &rhs){
    row = rhs.row;
    col = rhs.col;
    dummy = 0;
  }

  bool operator == (const Location &rhs) const
  {
    return (row == rhs.row && col == rhs.col);
  }

  bool operator != (const Location &rhs) const
  {
    return !(*this == rhs);
  }

};

#endif //LOCATION_H_
