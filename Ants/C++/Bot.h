#ifndef BOT_H_
#define BOT_H_

#include "State.h"

/*
    This struct represents your bot in the game of Ants
*/
struct Bot
{
  State state;

  Bot();

  void playGame();    //plays a single game of Ants
  
  void makeMoves();   //makes moves for a single turn
  void endTurn();     //indicates to the engine that it has made its moves

  void updateFood(); //update food stuffs
  void adjustFood(); //checks if food still exists...adjusts grid
  void addFood(Location f); //adds food to food vector

  void updateHills(); //update hill stuffs
  void addHill(Location h);
  void adjustHills();

  void computeGradient(); //diffusion gradient stuffs
  void diffuse(std::deque<DiffLoc> &scents);//, std::vector<std::vector<Location> > &parent);

  std::vector<Location> food, hills;
};

#endif //BOT_H_
