#ifndef STATE_H_
#define STATE_H_

#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <cmath>
#include <string>
#include <vector>
#include <queue>
#include <deque>
#include <stack>
#include <map>
#include <stdint.h>
#include <sstream>
#include <algorithm>

#include "Timer.h"
#include "Bug.h"
#include "Square.h"
#include "Location.h"

/*
    constants
*/
const int TDIRECTIONS = 4;
const char CDIRECTIONS[4] = {'N', 'E', 'S', 'W'};
const int DIRECTIONS[4][2] = { {-1, 0}, {0, 1}, {1, 0}, {0, -1} };      //{N, E, S, W}

/*
  This struct represents the calculated distances from a location
 */
struct DistLoc
{
  DistLoc(double d, Location s, Location t):dist(d),source(s),target(t){}

  double dist;

  //source row,col
  Location source;

  //target row,col
  Location target;

  bool operator < (const DistLoc &rhs) const
  {
    return (dist < rhs.dist);
  }

};


/*
  This struct represents diffusion stuffs
 */
struct DiffLoc
{
 DiffLoc(double d, Location s, double p, int t, int r)
 :dist(d),source(s),str(p),type(t),radius(r){}

  double dist;

  //source row,col
  Location source;

  //stength of scent
  double str;

  //type 0 = neutral, 1 = defensive, 2 = offensive
  int type;

  //maximum radius 0 = inf
  int radius;

  bool operator < (const DiffLoc &rhs) const
  {
    return (dist < rhs.dist);
  }

};

/*
    struct to store current state information
*/
struct State
{
  /*
    Variables
  */
  int rows, cols,
      turn, turns,
      noPlayers;
  double attackradius, spawnradius, viewradius;
  double loadtime, turntime;
  std::vector<double> scores;
  bool gameover;
  int64_t seed;
  
  std::vector<std::vector<Square> > grid;

  //std::vector<std::vector<std::vector<std::vector<double> > > > dist;

  double **** dist;

  std::vector<Location> myAnts, enemyAnts, myHills, enemyHills, food;

  Timer timer;
  Bug bug;
  
  /*
    Functions
  */
  State();
  ~State();
  
  void setup();
  void reset();
  
  void makeMove(Location source);
  
  double distance(const Location &loc1, const Location &loc2);
  Location getLocation(const Location &startLoc, int direction);

  bool passable(const Location &loc);
  bool occupied(const Location &loc);

  void updateVisionInformation();
};

std::ostream& operator<<(std::ostream &os, const State &state);
std::istream& operator>>(std::istream &is, State &state);

#endif //STATE_H_
