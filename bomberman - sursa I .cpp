#include "Player.h"
#include <iostream>
#include <unordered_map>
using namespace std;

// main.cpp is the only file you will be submitting for the contest
// HINT: before submitting this file, make sure to test compiling on Release
//       and deactivating Debug mode from the Match Maker
// HINT: read the complete rules for submitting this file on the contest website

class PairHash{
public:
    long operator()(const pair<int, int> &k) const{
        return k.first * 100 + k.second;
    }
};

unordered_map< pair<int, int>, int, PairHash > bombs;
unordered_map< pair<int, int>, int, PairHash >::iterator it;;
const int maxN = 160, maxM = 160, inf = 999;
int n, m, x, y;
int walls[maxN][maxM], bomb_damage[maxN][maxM];

const int dx[] = {-1, 1, 0, 0};
const int dy[] = {0, 0, -1, 1};


void PlayerInit(int playerId, IGame* game)
{

	n = game->SizeX();
	m = game->SizeY();
	for(int i=0; i<n; ++i)
		for(int j=0; j<m; ++j)
			walls[i][j] = game->Wall(i, j);
}


Move moves[] = {moveUp, moveDown, moveLeft, moveRight, moveNone};

void GetBombs(IGame* game){
	bombs.clear();
	for(int i=0; i<n; ++i)
		for(int j=0; j<m; ++j)
			if(game->Bomb(i, j))
				bombs[make_pair(i, j)] = game->Bomb(i, j);
}

void initBombDamage(){
	for(int i=0; i<n; ++i)
		for(int j=0; j<m; ++j)
			bomb_damage[i][j] = inf;
}

void getBombDamage(IGame* game){

	int x1, y1, value;

	for(it = bombs.begin(); it != bombs.end(); ++it){
			x1 = it->first.first;
			y1 = it->first.second;
			value = it->second;
			for(int i = x1; i < n; ++i)
				if(walls[i][y1]) break;
				else bomb_damage[i][y1] = min(bomb_damage[i][y1], value);
			for(int i = x1; i >= 0; --i)
				if(walls[i][y1]) break;
				else bomb_damage[i][y1] = min(bomb_damage[i][y1], value);
			for(int j = y1; j < m; ++j)
				if(walls[x1][j]) break;
				else bomb_damage[x1][j] = min(bomb_damage[x1][j], value);
			for(int j = y1; j >= 0; --j)
				if(walls[x1][j]) break;
				else bomb_damage[x1][j] = min(bomb_damage[x1][j], value);
		}

		for(int i=0; i<n; ++i)
			for(int j=0; j<m; ++j)
				if(bomb_damage[i][j] == inf)
					bomb_damage[i][j] = 0;
}

int nextmove = inf;

void PlayerMove(int playerId, IGame* game, Move& move, bool& bomb)
{
	PlayerInit(playerId, game);
	simulation->BeginTurn(game);
	x = game->PlayerPosX(playerId);
	y = game->PlayerPosY(playerId);
	GetBombs(game);
	initBombDamage();
	getBombDamage(game);

	int x1, y1, x2, y2;
	int ok_move[] = {0, 0, 0, 0}, next_move[] = {0, 0, 0, 0};

	if(nextmove != inf){
		move = moves[nextmove]; nextmove = inf;
		/*cout<<"*** misc: "<<move<<'\n';*/
		bomb = false;
		return;
	}

	for(int i = 0; i < 4; ++i){
		x1 = x + dx[i];
		y1 = y + dy[i];
		if(bombs.find(make_pair(x1, y1)) == bombs.end() && walls[x1][y1] == 0 && (bomb_damage[x1][y1] > 2 || bomb_damage[x1][y1] == 0) ){
			for (int j = 0; j < 4; j++)
				{
					if(j + i == 1 || i == j || j+i == 5) continue;
					x2 = x1 + dx[j],
					y2 = y1 + dy[j];
					if(bombs.find(make_pair(x2, y2)) == bombs.end() && (bomb_damage[x2][y2] == 0) && walls[x2][y2] == 0){
						ok_move[i]++; next_move[i] = j;
					}
				}
			}
		}

		int run = 4, run_val = inf;
		for(int i = 0; i < 4; ++i){
			x1 = ::x + dx[i],
			y1 = ::y + dy[i];
			if(bombs.find(make_pair(x1, y1)) == bombs.end() && walls[x1][y1] == 0 && (bomb_damage[x1][y1] == 0 || bomb_damage[x1][y1] > 1) ){
				if(bomb_damage[x1][y1] <= run_val)
					run_val = bomb_damage[x1][y1],
					run = i;
			}
		}
		if(run_val != 0 && bomb_damage[x][y] == 0 ){
			move = moveNone;
			return;
		}
		move = moves[run];
		if(ok_move[run] >= 1)
			bomb = true, nextmove = next_move[run];
		else
			nextmove = inf;
}
