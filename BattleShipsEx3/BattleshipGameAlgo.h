#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "BattleBoard.h"
#include "Common.h"

#define irrelevnatCell '-'

using namespace std;

const char shipsBySize[4] = { 'd', 'm', 'p', 'b' };

class targetVessel
{
public:
	bool exists = false;
	int direction = -1;
	/*0 - no direction(ship size 1)
	* 1 - going right
	* 2 - going down
	* 3 - going in(deep) */
	int edgeReached = -1;
	Coordinate edges[2] = { Coordinate(-1, -1, -1), Coordinate(-1, -1, -1) };
	int hitCount = 0;
};

class PlayerBoard
{
public:
	unordered_map<char, int> hostileShips = { { 'd', 0 },{ 'm', 0 },{ 'p', 0 },{ 'b', 0 } };
	vector<vector<vector<char>>> board;
	int rows = -1;
	int cols = -1;
	int depth = -1;
};

class BattleshipGameAlgo : public IBattleshipGameAlgo
{
public:
	int playerNum;
	PlayerBoard playerBoard;
	targetVessel target;
	
	// Blocking Copy and Assignment
	BattleshipGameAlgo(const BattleshipGameAlgo&) = delete;
	BattleshipGameAlgo& operator = (const BattleshipGameAlgo&) = delete;

	//constructor
	BattleshipGameAlgo()
	{
	}

	~BattleshipGameAlgo()
	{
	}

	virtual void setPlayer(int player) override;
	virtual void setBoard(const BoardData& board) override;
	virtual  Coordinate attack() override;
	virtual void notifyOnAttackResult(int player, Coordinate move, AttackResult result)  override;

private:
	void _markIrrelevant(int depth, int row, int col);
	bool _canAttack(int z, int i, int j) const;
	Coordinate _getBestGuess();
	void BattleshipGameAlgo::_placeShips(vector<vector<vector<int>>>& scoreBoard) const;
};