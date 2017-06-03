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
	int direction = -1;
	int edgeReached = -1;
	//pair<int, int> edges[2];
	Coordinate newEdges[2] = { Coordinate(-1, -1, -1), Coordinate(-1, -1, -1) };
};

class BattleshipGameAlgo : public IBattleshipGameAlgo
{
public:
	int playerNum;
	unique_ptr<BattleBoard> playerBoard = nullptr;
	targetVessel* target = nullptr;
	int hostileShipsNum = -1;
	unordered_map<char, int> myShips = { {'d', 0}, {'m', 0}, {'p', 0}, {'b', 0} };

	// Blocking Copy and Assignment
	BattleshipGameAlgo(const BattleshipGameAlgo&) = delete;
	BattleshipGameAlgo& operator = (const BattleshipGameAlgo&) = delete;

	//constructor
	BattleshipGameAlgo()
	{
	}

	~BattleshipGameAlgo()
	{
		//delete playerBoard;
		delete target;
	}

	virtual void setPlayer(int player) override;
	virtual void setBoard(const BoardData& board) override;
	//virtual bool init(const std::string& path) override;
	virtual  Coordinate attack() override;
	virtual void notifyOnAttackResult(int player, Coordinate move, AttackResult result)  override;

private:
	void _markIrrelevant(int row, int col, int depth) const;
	bool _canAttack(int z, int i, int j) const;
	pair<int, int> _getBestGuess() const;
};