#ifndef SQUARE_H_
#define SQUARE_H_

#include <vector>

/*
    struct for representing a square in the grid.
*/
struct Square
{
  bool isVisible, isWater, isHill, isFood, isKnown;
  int ant, hillPlayer;
  double n;//explore layer
  double f;//food layer
  double h;//enemy hills layer
  double d;//defensive layer
  double o;//offensive layer

  std::vector<int> deadAnts;
  
  Square()
  {
    isVisible = isWater = isHill = isFood = isKnown = n = f = h = d = o = 0;
    ant = hillPlayer = -1;
  };
  
  //resets the information for the square except water information
  void reset()
  {
    n = 0;//0
    f = 0;//1
    h = 0;//2
    d = 0;//3
    o = 0;//4

    isVisible = 0;
    isHill = 0;
    isFood = 0;
    ant = hillPlayer = -1;
    deadAnts.clear();
  };
};

#endif //SQUARE_H_
