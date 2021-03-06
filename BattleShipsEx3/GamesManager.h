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
#include <queue>
#include "PlayerScore.h"

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
	int winPlayer;
	GameResult(string playerAS, string playerBS):playerA(playerAS),
	playerB(playerBS),playerAScore(0),playerBScore(0), winPlayer(-1)
	{}

};
struct Game
{
	 int gameNumber;
	 int boardNumber;
	 int playerANumber;
	 int playerBNumber;
	 Game(int gameNum, int boardNum, int playerANum, int playerBNum) :gameNumber(gameNum), boardNumber(boardNum),
		 playerANumber(playerANum), playerBNumber(playerBNum){}

};

struct sortPlayersScoreByWinRate
{
	bool operator()(const std::pair<string, PlayerRoundScore> &left, const std::pair<string, PlayerRoundScore> &right) const
	{
		return  left.second.winRate > right.second.winRate;
	}
};


/*
* check if the files below exist in the given path:
* one sboard file and 2 dll files
* if not return false
*/
bool CheckExistingGameFiles(vector<string> dllFiles, vector<string> sboardFiles, string path,
	vector<string>& error_messages);
void closeDLLs(vector<HINSTANCE> dlls);
void getGameFiles(string folder, vector<string> & sboardFiles, vector<string> & dllFiles,
	vector<string>& dllNames);

/*
load algorithms and board file for one game
*/
bool loadAlgoDllsCheckBoards(vector<string> dllfiles, vector<string> sboardfiles,
	vector<HINSTANCE>& dllLoaded, vector<GetAlgorithmFuncType>& algorithmFuncs);
void  calcGameCombinations(int playersNum, int boardsNumber);

GameResult playSingleGame(pair<GetAlgorithmFuncType, string> playerAPair, 
	pair<GetAlgorithmFuncType, string> playerBPair ,int curentBoardNum);

int manageGames(vector<string> dllFiles, vector<string> dllNames, vector<string> sboardFiles, int threads);

void GameThread();
void updateGameResult(GameResult result);
bool isTournamentDone();
void IncreaseGameCounter();
void ReportResults();
bool checkRound(int roundNumber);