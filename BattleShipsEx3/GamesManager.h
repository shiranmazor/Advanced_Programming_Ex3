﻿#pragma once
#include "IBattleshipGameAlgo.h"
#include <iostream>
#include <string>
#include <direct.h>
#include <vector>
#include <windows.h>
#include <stdio.h>
#include "time.h"
#include "Common.h"
#include <algorithm>
#include <tuple>
#include <windows.h>
#include "BattleBoard.h"

using namespace std;

#define GREEN 10
#define RED 12
#define BLUE 9
#define PURPLE  13
#define YELLOW 14
#define WHITE 15
#define HitMarkA '*'
#define HitMarkB '#'
#define isPlayerA(y)  (isupper(y) || y == HitMarkA)

// define function of the type we expect
typedef IBattleshipGameAlgo *(*GetAlgorithmFuncType)();

class GameResult
{
public:
	string playerA;
	string playerB;
	int playerAScore;
	int playerBScore;
	string winPlayer;
	GameResult(string playerA, string playerB):playerAScore(0),playerBScore(0)
	{}

};
class PlayerBalance
{
public:
	string playerName;
	int wins;
	int losses;
	float winsRate;
	int gamesCounter;
	int pointsGained;
	int pointsAgainst;

	PlayerBalance(string name) :wins(0), losses(0), winsRate(0), gamesCounter(0), pointsGained(0), pointsAgainst(0)
	{
		this->playerName = name;
	}
	~PlayerBalance() = default;
};


/*
* check if the files below exist in the given path:
* one sboard file and 2 dll files
* if not return false
*/
bool CheckExistingGameFiles(vector<string> dllFiles, vector<string> sboardFiles, string path, vector<string>& error_messages);
void closeDLLs(vector<tuple<string, HINSTANCE, GetAlgorithmFuncType>> & dll_vec);
void getGameFiles(string folder, vector<string> & sboardFiles, vector<string> & dllFiles);

/*
load algorithms and board file for one game
*/
bool loadAlgoDllsCheckBoards(vector<string> dllfiles, vector<string> sboardfiles,
	vector<HINSTANCE>& dllLoaded, vector<GetAlgorithmFuncType>& algorithmFuncs, vector<shared_ptr<BattleBoard>>& boards);
void CalcCompetitionGames();

IBattleshipGameAlgo* swapPlayer(IBattleshipGameAlgo* current, IBattleshipGameAlgo* pA,
	IBattleshipGameAlgo* pB, int currentName);

/*
call load dll and init game 
create players
create boardgame instace and check validity of the board
*/
unique_ptr<GameResult> playSingleGame(pair<string, string> dllNames, GetAlgorithmFuncType algorithmFuncs1, GetAlgorithmFuncType algorithmFuncs2,
	shared_ptr<BattleBoard> board);
int manageGames(vector<string> dllFiles, vector<string> sboardFiles, int threads);