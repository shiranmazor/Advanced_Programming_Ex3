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

#define isPlayerChar(x, y) (x==A && (isupper(y) || isspace(y)) || (x==B && (islower(y) || isspace(y))))
#define isSelfHit(x, y) (x==A && y == HitMarkA) || (x==B && y == HitMarkB))
#define isOppChar(x, y) ((x==A && islower(y)) || (x==B && isupper(y)))
#define HitMarkA '*'
#define HitMarkB '#'
#define makeKey(x) (std::to_string(x.first) + '_' + std::to_string(x.second))
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
	int playerToolsNum;
	unordered_map<string, Vessel*> ships;

	// Blocking Copy and Assignment
	BattleBoard(const BattleBoard&) = delete;
	BattleBoard& operator = (const BattleBoard&) = delete;

	// constructor
	BattleBoard(string boardFilePath)
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
					if (!isCharValid(this->board[i][j][z])) this->board[i][j][z] = ' ';
	}


	BattleBoard(const char** initBoard, int R = 10, int C = 10) :playerToolsNum(5)
	{
		this->R = R;
		this->C = C;

		// copy the init board to  new board member
		this->board = new char*[this->R];
		for (int i = 0; i < this->R; i++) this->board[i] = new char[this->C];

		for (int i = 0; i < this->R; i++)
		{
			this->board[i] = _strdup(initBoard[i]);
		}
	}

	// destructor
	~BattleBoard()
	{
		if (this->board != NULL)
		{
			set<Vessel*> vessles;
			for (auto const& element : this->ships)
				vessles.insert(element.second);
			for (auto const& element : vessles)
				delete[] element;
		}
	}

	bool isBoardValid(vector<string>& error_messages);
	pair<int, int> CalcScore();
	int CheckVictory();
	void getPlayerBoard(Player player, char** &pBoard) const;
	AttackResult performGameMove(int p, pair<int, int> move);
};
