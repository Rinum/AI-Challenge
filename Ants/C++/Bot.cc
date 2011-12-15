#include "Bot.h"

using namespace std;

//constructor
Bot::Bot()
{

};

//plays a single game of Ants.
void Bot::playGame()
{
    //reads the game parameters and sets up
    cin >> state;
    state.setup();
    endTurn();

    //continues making moves while the game is not over
    while(cin >> state)
    {
        state.updateVisionInformation();
        makeMoves();
        endTurn();
    }
};

void Bot::updateFood()
{  
    //Loop though food locations...add to food database
    for(int f=0; f<(int)state.food.size();f++){
      Location loc1 = state.food[f];
      addFood(loc1);
    }

    adjustFood();
}

void Bot::updateHills()
{
    // Loop through hills, add hills to vector...
    for(int h=0; h<(int)state.enemyHills.size(); h++){
      Location hill = state.enemyHills[h];
      addHill(hill);
    }

    adjustHills();
}

void Bot::computeGradient()
{
  int i,j;
  int m,n;
  int k = 0;

  deque<DiffLoc> scents;//queue
  //vector<vector<Location> > parent 
  //= vector<vector<Location> >(state.rows, vector<Location>(state.cols, Location()));

  for(i = 0; i < state.rows; i++){
    for(j = 0; j < state.cols; j++){
      
      if( state.passable(Location(i,j)) ){

	/* diffuse food */
	if(state.grid[i][j].isFood){
	  DiffLoc el = DiffLoc(0,Location(i,j),25,1,0);
	  scents.push_back(el);
	} else if(state.grid[i][j].hillPlayer > 0){
	  if( state.myAnts.size() > state.enemyAnts.size() ){
	    DiffLoc el = DiffLoc(0,Location(i,j),50*state.myAnts.size(),2,0);
	    scents.push_back(el);
	  } else {
	    DiffLoc el = DiffLoc(0,Location(i,j),50,2,0);
	    scents.push_back(el);
	  }
	} else if(state.grid[i][j].ant > 0){
	  DiffLoc el = DiffLoc(0,Location(i,j),10,3,state.attackradius+3);
	  scents.push_back(el);
	} else if(state.grid[i][j].ant == 0){
	  DiffLoc el = DiffLoc(0,Location(i,j),10,4,state.viewradius);
	  scents.push_back(el);
	} else if(!state.grid[i][j].isKnown){
	  DiffLoc el = DiffLoc(0,Location(i,j),25,0,0);
	  scents.push_back(el);
	} else if(!state.grid[i][j].isVisible){
	  DiffLoc el = DiffLoc(0,Location(i,j),12.5,0,0);
	  scents.push_back(el);
	}

      }

    }
  }

  diffuse(scents);//,parent);

}

void Bot::diffuse(deque<DiffLoc> &scents)//, vector<vector<Location> > &parent)
{
  //path.push_back(DistLoc(0,source,Location()));
  //parent[source.row][source.col] = source;

  while( scents.size() ){
    DiffLoc curr = scents.front();scents.pop_front();//actual current struct
    Location current = curr.source;
    int r = current.row;
    int c = current.col;

    double dist = curr.dist;
    double str = curr.str;
    int type = curr.type;
    int radius = curr.radius;

    //diff value
    double diff = str/(pow(2,dist));

    if( (type == 0 && state.grid[r][c].n < diff)
	|| (type == 1 && state.grid[r][c].f < diff)
	|| (type == 2 && state.grid[r][c].h < diff)
	|| (type == 3 && dist <= radius && state.grid[r][c].d <= diff)
	|| (type == 4 && dist <= radius && state.grid[r][c].o <= diff) ){

      switch(type){
      case 0:
	state.grid[r][c].n = diff;
	break;
      case 1:
	state.grid[r][c].f = diff;
	break;
      case 2:
	state.grid[r][c].h = diff;
	break;
      case 3:
	state.grid[r][c].d += diff;
	break;
      case 4:
	state.grid[r][c].o += diff;
	break;
      }

      //check north
      Location north( (((r-1)%state.rows)+state.rows)%state.rows, c );
      if( state.passable( north ) ){
	scents.push_back(DiffLoc(dist+1,north,str,type,radius));
      }
      
      //check south
      Location south( (((r+1)%state.rows)+state.rows)%state.rows, c );
      if( state.passable( south ) ){
	scents.push_back(DiffLoc(dist+1,south,str,type,radius));
      }
      
      //check east
      Location east(r,(((c+1)%state.cols)+state.cols)%state.cols);
      if( state.passable( east ) ){
	scents.push_back(DiffLoc(dist+1,east,str,type,radius));
      }
      
      //check west
      Location west(r,(((c-1)%state.cols)+state.cols)%state.cols);
      if( state.passable( west ) ){
	scents.push_back(DiffLoc(dist+1,west,str,type,radius));
      }
      
    }
  }

}

//makes the bots moves for the turn
void Bot::makeMoves()
{
  state.bug << "turn: " << state.turn << endl;
  //if(state.turn > 1)
  //state.bug << "Turn" << endl;

    updateFood();
    updateHills();

    computeGradient();

    //state.bug << state << endl;
    
    /* Loop through ants */
    for(int ant=0; ant<(int)state.myAnts.size();ant++){
      Location loc = state.myAnts[ant];
      state.makeMove(loc);
    }

   state.bug << "time: " << state.timer.getTime() << "ms" << endl << endl;
};

void Bot::adjustFood()
{
  vector<Location>::iterator i = food.begin();
  Location loc;
  while(i!=food.end()){
    loc = *i;
    if( !state.grid[loc.row][loc.col].isFood 
	&& state.grid[loc.row][loc.col].isVisible )
      food.erase(i);
    else {
      state.grid[loc.row][loc.col].isFood = 1;
      i++;
    }
  }
}

void Bot::addFood(Location f)
{
  for(int i=0; i<food.size(); i++){
    if( food[i] == f )
      return;
  }
  food.push_back(f);
}


void Bot::adjustHills()
{
  vector<Location>::iterator i = hills.begin();
  Location loc;
  while(i!=hills.end()){
    loc = *i;
    if( !state.grid[loc.row][loc.col].isHill 
	&& state.grid[loc.row][loc.col].isVisible )
      hills.erase(i);
    else {
      state.grid[loc.row][loc.col].isHill = 1;
      state.grid[loc.row][loc.col].hillPlayer = 5;
      i++;
    }
  }
}

void Bot::addHill(Location h)
{
  for(int i=0; i<hills.size(); i++){
    if( hills[i] == h )
      return;
  }
  hills.push_back(h);
}

//finishes the turn
void Bot::endTurn()
{
    if(state.turn > 0)
        state.reset();
    state.turn++;

    cout << "go" << endl;
};
