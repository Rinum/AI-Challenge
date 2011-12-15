#include "State.h"

using namespace std;

//constructor
State::State()
{
    gameover = 0;
    turn = 0;
    bug.open("./debug.txt");
};

//deconstructor
State::~State()
{
    bug.close();
};

//sets the state up
void State::setup()
{
    grid = vector<vector<Square> >(rows, vector<Square>(cols, Square()));

    double dist[rows][cols][rows][cols];
};

//resets all non-water squares to land and clears the bots ant vector
void State::reset()
{
    myAnts.clear();
    enemyAnts.clear();
    myHills.clear();
    enemyHills.clear();
    food.clear();
    for(int row=0; row<rows; row++)
        for(int col=0; col<cols; col++)
            if(!grid[row][col].isWater)
                grid[row][col].reset();
};


//outputs move information to the engine
void State::makeMove(Location source)
{
  int r = source.row;
  int c = source.col;

  vector<DistLoc> ant_diff; //store adjacent tiles w/ diffusion values

  /* Actions: 0 = Gather Food, 1 = Explore, 2 = Attack Hill, 3 = Run Away!,
     4 = Attack Ant */
  int action;

  //check current location to determine action
  double n = grid[r][c].n;
  double f = grid[r][c].f;
  double h = grid[r][c].h;

  double d = grid[r][c].d;
  double o = grid[r][c].o - 10;

  //bug << d << endl;

  if( d != 0 )
    action = 1; // run away!
  else
    action = 0;

  //check north
  Location north( (((r-1)%rows)+rows)%rows, c );
  if( passable( north ) && !occupied( north ) ){
    double dv;
    double o = grid[north.row][north.col].o - 5;
    if( action == 0 ){
      dv = grid[north.row][north.col].f;
      dv += grid[north.row][north.col].n;
      dv += grid[north.row][north.col].h;
    } else
      if( grid[north.row][north.col].d > o )
	dv = -1 * grid[north.row][north.col].d;
      else
	dv = grid[north.row][north.col].d + o;
    DistLoc el(dv,source,north);
    ant_diff.push_back(el);
  }
    
  //check south
  Location south( (((r+1)%rows)+rows)%rows, c );
  if( passable( south ) && !occupied( south ) ){
    double dv;
    double o = grid[south.row][south.col].o - 5;
    if( action == 0 ){
      dv = grid[south.row][south.col].f;
      dv += grid[south.row][south.col].n;
      dv += grid[south.row][south.col].h;
    } else
      if( grid[south.row][south.col].d > o )
	dv = -1 * grid[south.row][south.col].d;
      else
	dv = grid[south.row][south.col].d + o;
    DistLoc el(dv,source,south);
    ant_diff.push_back(el);
  }

  //check east
  Location east(r,(((c+1)%cols)+cols)%cols);
  if( passable( east ) && !occupied( east ) ){
    double dv;
    double o = grid[east.row][east.col].o - 5;
    if( action == 0 ){
      dv = grid[east.row][east.col].f;
      dv += grid[east.row][east.col].n;
      dv += grid[east.row][east.col].h;
    } else
      if( grid[east.row][east.col].d > o )
	dv = -1 * grid[east.row][east.col].d;
      else
	dv = grid[east.row][east.col].d + o;
    DistLoc el(dv,source,east);
    ant_diff.push_back(el);
  }

  //check west
  Location west(r,(((c-1)%cols)+cols)%cols);
  if( passable( west ) && !occupied( west ) ){
    double dv;
    double o = grid[west.row][west.col].o - 5;
    if( action == 0 ){
      dv = grid[west.row][west.col].f;
      dv += grid[west.row][west.col].n;
      dv += grid[west.row][west.col].h;
    } else
      if( grid[west.row][west.col].d > o )
	dv = -1 * grid[west.row][west.col].d;
      else
	dv = grid[west.row][west.col].d + o;
    DistLoc el(dv,source,west);
    ant_diff.push_back(el);
  }
 
  //check current
    double dv;
    if( action == 0 ){
      dv = f;
      dv += n;
      dv += h;
    } else 
      if( d > o )
	dv = -1 * d;
      else
	dv = d + o;
  DistLoc el(dv,source,source);
  ant_diff.push_back(el);


  sort(ant_diff.begin(),ant_diff.end());

  //go in the direction of diff gradient
  if( ant_diff.size() ){

    Location source = (ant_diff.back()).source;
    Location target = (ant_diff.back()).target;

    //kill scent at target
    //grid[target.row][target.col].dv = 0;

    int drow = target.row - source.row;
    int dcol = target.col - source.col;

    if( drow < -1 )
      drow = 1;

    if( drow > 1 )
      drow = -1;

    if( dcol < -1 )
      dcol = 1;
    
    if( dcol > 1 )
      dcol = -1;

    char d;

    if( drow > 0 ){ d = 'S'; }
    else if( dcol > 0 ){ d = 'E'; }
    else if( dcol < 0 ){ d = 'W'; }
    else if( drow < 0 ){ d = 'N'; }
    else
      return;

    cout << "o " << source.row << " " << source.col << " " << d << endl;
    
    if( source != target ){
      grid[target.row][target.col].ant = grid[source.row][source.col].ant;
      grid[source.row][source.col].ant = -1;
    }
  }
};

//checks if location is free (for ant to move on)
bool State::passable(const Location &loc)
{
  //bug << "Checking if passable " << loc.row << "," << loc.col << endl;
  return !grid[loc.row][loc.col].isWater;
}

bool State::occupied(const Location &loc)
{
  //bug << "Checking if occupied " << loc.row << "," << loc.col << endl;
  return grid[loc.row][loc.col].ant != -1;
}

//returns the euclidean distance between two locations with the edges wrapped
double State::distance(const Location &loc1, const Location &loc2)
{

  int d1 = abs(loc1.row-loc2.row),
    d2 = abs(loc1.col-loc2.col),
    dr = min(d1, rows-d1),
    dc = min(d2, cols-d2);
  return dr*dr + dc*dc;
  
};

//returns the new location from moving in a given direction with the edges wrapped
Location State::getLocation(const Location &loc, int direction)
{
    return Location( (loc.row + DIRECTIONS[direction][0] + rows) % rows,
                     (loc.col + DIRECTIONS[direction][1] + cols) % cols );
};

/*
    This function will update update the lastSeen value for any squares currently
    visible by one of your live ants.

    BE VERY CAREFUL IF YOU ARE GOING TO TRY AND MAKE THIS FUNCTION MORE EFFICIENT,
    THE OBVIOUS WAY OF TRYING TO IMPROVE IT BREAKS USING THE EUCLIDEAN METRIC, FOR
    A CORRECT MORE EFFICIENT IMPLEMENTATION, TAKE A LOOK AT THE GET_VISION FUNCTION
    IN ANTS.PY ON THE CONTESTS GITHUB PAGE.
*/
void State::updateVisionInformation()
{
  std::queue<Location> locQueue;
  Location sLoc, cLoc, nLoc;
  
  for(int a=0; a<(int) myAnts.size(); a++)
  {
      sLoc = myAnts[a];
      locQueue.push(sLoc);
      
      std::vector<std::vector<bool> > visited(rows, std::vector<bool>(cols, 0));
      grid[sLoc.row][sLoc.col].isVisible = 1;
      grid[sLoc.row][sLoc.col].isKnown = 1;
      visited[sLoc.row][sLoc.col] = 1;
          
      while(!locQueue.empty())
      {
	  cLoc = locQueue.front();
	  locQueue.pop();
	  
	  for(int d=0; d<TDIRECTIONS; d++)
          {
	      nLoc = getLocation(cLoc, d);
	      
	      if(!visited[nLoc.row][nLoc.col] && distance(sLoc, nLoc) <= viewradius)
              {
		grid[nLoc.row][nLoc.col].isVisible = 1;
		grid[nLoc.row][nLoc.col].isKnown = 1;
		locQueue.push(nLoc);
	      }
	      visited[nLoc.row][nLoc.col] = 1;	
	  }
      }
  }
};

/*
    This is the output function for a state. It will add a char map
    representation of the state to the output stream passed to it.

    For example, you might call "cout << state << endl;"
*/

ostream& operator<<(ostream &os, const State &state)
{
    for(int row=0; row<state.rows; row++)
    {
        for(int col=0; col<state.cols; col++)
        {
            if(state.grid[row][col].isWater)
                os << '%';
            else if(state.grid[row][col].isFood)
                os << '*';
            else if(state.grid[row][col].isHill)
                os << (char)('A' + state.grid[row][col].hillPlayer);
            else if(state.grid[row][col].ant >= 0)
                os << (char)('a' + state.grid[row][col].ant);
            else if(state.grid[row][col].isVisible)
                os << '.';
            else
                os << '?';
        }
        os << endl;
    }

    return os;
};

 /*
ostream& operator<<(ostream &os, const State &state)
{
    for(int row=0; row<state.rows; row++)
    {
        for(int col=0; col<state.cols; col++)
        {
            if(state.grid[row][col].isWater)
                os << "% ";
            else if(state.grid[row][col].isFood)
                os << "* ";
            else if(state.grid[row][col].hillPlayer > 0)
                os << "H ";
	    else if(state.grid[row][col].hillPlayer == 0)
	        os << "M ";
            else if(state.grid[row][col].ant > 0)
                os << "e ";
	    else if(state.grid[row][col].ant == 0)
	        os << "a ";
            else {
                os << state.grid[row][col].dv;
		os << " ";
	    }
        }
        //os << endl;
    }

    return os;
};
 */

//input function
istream& operator>>(istream &is, State &state)
{
    int row, col, player;
    string inputType, junk;

    //finds out which turn it is
    while(is >> inputType)
    {
        if(inputType == "end")
        {
            state.gameover = 1;
            break;
        }
        else if(inputType == "turn")
        {
            is >> state.turn;
            break;
        }
        else //unknown line
            getline(is, junk);
    }

    if(state.turn == 0)
    {
        //reads game parameters
        while(is >> inputType)
        {
            if(inputType == "loadtime")
                is >> state.loadtime;
            else if(inputType == "turntime")
                is >> state.turntime;
            else if(inputType == "rows")
                is >> state.rows;
            else if(inputType == "cols")
                is >> state.cols;
            else if(inputType == "turns")
                is >> state.turns;
            else if(inputType == "player_seed")
                is >> state.seed;
            else if(inputType == "viewradius2")
            {
                is >> state.viewradius;
                state.viewradius = sqrt(state.viewradius);
            }
            else if(inputType == "attackradius2")
            {
                is >> state.attackradius;
                state.attackradius = sqrt(state.attackradius);
            }
            else if(inputType == "spawnradius2")
            {
                is >> state.spawnradius;
                state.spawnradius = sqrt(state.spawnradius);
            }
            else if(inputType == "ready") //end of parameter input
            {
                state.timer.start();
                break;
            }
            else    //unknown line
                getline(is, junk);
        }
    }
    else
    {
        //reads information about the current turn
        while(is >> inputType)
        {
            if(inputType == "w") //water square
            {
                is >> row >> col;
                state.grid[row][col].isWater = 1;
            }
            else if(inputType == "f") //food square
            {
                is >> row >> col;
                state.grid[row][col].isFood = 1;
                state.food.push_back(Location(row, col));
            }
            else if(inputType == "a") //live ant square
            {
                is >> row >> col >> player;
                state.grid[row][col].ant = player;
                if(player == 0)
                    state.myAnts.push_back(Location(row, col));
                else
                    state.enemyAnts.push_back(Location(row, col));
            }
            else if(inputType == "d") //dead ant square
            {
                is >> row >> col >> player;
                state.grid[row][col].deadAnts.push_back(player);
            }
            else if(inputType == "h")
            {
                is >> row >> col >> player;
                state.grid[row][col].isHill = 1;
                state.grid[row][col].hillPlayer = player;
                if(player == 0)
                    state.myHills.push_back(Location(row, col));
                else
                    state.enemyHills.push_back(Location(row, col));

            }
            else if(inputType == "players") //player information
                is >> state.noPlayers;
            else if(inputType == "scores") //score information
            {
                state.scores = vector<double>(state.noPlayers, 0.0);
                for(int p=0; p<state.noPlayers; p++)
                    is >> state.scores[p];
            }
            else if(inputType == "go") //end of turn input
            {
                if(state.gameover)
                    is.setstate(std::ios::failbit);
                else
                    state.timer.start();
                break;
            }
            else //unknown line
                getline(is, junk);
        }
    }

    return is;
};
