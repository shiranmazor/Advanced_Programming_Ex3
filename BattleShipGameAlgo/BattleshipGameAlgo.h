#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "IBattleshipGameAlgo.h"
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
	pair<int, int> edges[2];
};

class BattleshipGameAlgo : public IBattleshipGameAlgo
{
public:
	int playerNum;
	BattleBoard* playerBoard = nullptr;
	targetVessel* target = nullptr;
	int hostileShipsNum = -1;

	// Blocking Copy and Assignment
	BattleshipGameAlgo(const BattleshipGameAlgo&) = delete;
	BattleshipGameAlgo& operator = (const BattleshipGameAlgo&) = delete;

	//constructor
	BattleshipGameAlgo()
	{
	}

	~BattleshipGameAlgo()
	{
		delete playerBoard;
		delete target;
	}

	virtual void setBoard(int player, const char** board, int numRows, int numCols) override;
	virtual bool init(const std::string& path) override;
	virtual pair<int, int> attack() override;
	virtual void notifyOnAttackResult(int player, int row, int col, AttackResult result) override;

private:
	void _markIrrelevant(int i, int j) const;
	bool _canAttack(int i, int j) const;
	pair<int, int> _getBestGuess() const;
};