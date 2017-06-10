#pragma once
#pragma warning(disable:4996)

#include <string>
#include <fstream>
#include <unordered_map>
#include "Common.h"
#include "IBattleshipGameAlgo.h"
#include <string.h>
#include <set>
#include <iostream>
#include <ppltasks.h>
#include <regex>

using namespace std;

#define	isPlayerChar(x, y) (x==A && (isupper(y) || isspace(y)) || (x==B && (islower(y) || isspace(y))))
#define isSelfHit(x, y) (x==A && y == HitMarkA) || (x==B && y == HitMarkB))
#define isOppChar(x, y) ((x==A && islower(y)) || (x==B && isupper(y)))
#define HitMarkA '*'
#define HitMarkB '#'
#define makeKey(x) (to_string(x.row) + ' ' + to_string(x.col) + ' ' + to_string(x.depth))
#define isAlreadyHit(x) (x == '*' || x == '#')

const char idx2ship[8] = { 'B', 'P', 'M', 'D', 'b', 'p', 'm', 'd' };
const unordered_map<char, int> ship2idx = {
	{ 'B', 0 },{ 'P', 1 },{ 'M', 2 },{ 'D', 3 },{ 'b', 4 },{ 'p', 5 },{ 'm', 6 },{ 'd', 7 },
};

class Vessel
{
public:
	char type;
	int size;
	int hitNum;
	Player player;

	Vessel(char type) :hitNum(0)
	{
		this->type = type;
		this->size = getShipSize(type);
		if (isupper(type))
			this->player = A;
		else
			this->player = B;
	}
};

class BattleBoard : public BoardData
{
public:
	vector<vector<vector<char>>> board;
	//int playerToolsNum;
	unordered_map<string, shared_ptr<Vessel>> ships;

	// Blocking Assignment
	//BattleBoard(const BattleBoard&) = delete;
	BattleBoard& operator = (const BattleBoard&) = delete;

	// constructor
	BattleBoard(string boardFilePath) //: playerToolsNum(0)
	{
		ifstream boardFile(boardFilePath); //here assuming board file exist!
		string line;

		getline(boardFile, line);
		if (regex_match(line, regex("\d+x\d+x\d+")))
		{
			stringstream s(regex_replace(line, regex("x"), " "));
			s >> this->_cols >> this->_rows >> this->_depth;
		}
		else
		{
			// bad board, skip it
			boardFile.close();
			return;
		}
		getline(boardFile, line);

		for (int z = 0; z < this->_depth; z++)
		{
			for (int i = 0; i < this->_rows; i++)
			{
				getline(boardFile, line);
				if (int(line.length()) >= this->_cols)
					this->board[z][i] = vector<char>(line.begin(), line.begin() + this->_cols);
				else
				{
					this->board[z][i] = vector<char>(line.begin(), line.end());
					for (int j = int(line.length()); j < this->_cols; j++) this->board[z][i][j] = ' ';
				}
			}
			getline(boardFile, line);
			if (line != "\r")
				// bad board
				return;
		}

		// convert invalid chars to spaces
		for (int z = 0; z < this->_depth; z++)
			for (int i = 0; i < this->_rows; i++)
				for (int j = 0; j < this->_cols; j++)
					if (!isCharValid(this->board[z][i][j])) this->board[z][i][j] = ' ';
	}

	// Copy ctor
	//BattleBoard(const BattleBoard& b) : board(b.board), playerToolsNum(b.playerToolsNum), ships(b.ships)
	BattleBoard(const BattleBoard& b) : board(b.board), ships(b.ships)
	{
		this->_cols = b.cols();
		this->_rows = b.rows();
		this->_depth = b.depth();
	}

	// destructor
	~BattleBoard()
	{
		for (auto element : this->ships)
			element.second.reset();
	}

	bool isBoardValid(vector<string>& error_messages);
	pair<int, int> CalcScore();
	int CheckVictory();
	BattleBoard BattleBoard::getPlayerBoard(Player player) const;
	AttackResult performGameMove(int p, Coordinate move);
	virtual char charAt(Coordinate c) const override;

private:
	//int _getShipDirection(int z, int i, int j);
	void _collect_ship(int z, int i, int j, set<tuple<int, int, int>>* s);
};
