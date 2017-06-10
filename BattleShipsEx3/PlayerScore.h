#pragma once
#include <string>

using namespace std;

class PlayerScore
{
public:
	string playerName;//dllname
	int playerId;
	unsigned int wins;
	unsigned int losses;
	double winsRate;
	double lossRate;
	unsigned int gamesCounter;
	int pointsFor;
	int pointsAgainst;

	PlayerScore(const string name, int id);
	void UpdateScore(bool isWon, int pointsFor, int pointsAgainst);
	~PlayerScore() = default;
};
