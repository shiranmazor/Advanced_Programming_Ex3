#pragma once
#include <string>
#include <vector>

using namespace std;

class PlayerRoundScore
{
public:
	unsigned int wins;
	unsigned int losses;
	double winsRate;
	double lossRate;
	int pointsFor;
	int pointsAgainst;

	PlayerRoundScore():wins(0), losses(0), winsRate(0), lossRate(0),
		 pointsFor(0), pointsAgainst(0){}

	PlayerRoundScore(const PlayerRoundScore& p):lossRate(0), winsRate(0)
	{
		wins = p.wins;
		losses = p.losses;
		pointsFor = p.pointsFor;
		pointsAgainst = p.pointsAgainst;
	}
	~PlayerRoundScore() = default;
};
class PlayerScore
{
public:
	string playerName;//dllname
	int playerId;
	unsigned int gamesCounter;
	vector<PlayerRoundScore> rounds;

	PlayerScore(const string name, int id);
	void UpdateScore(bool isWon, int pointsFor, int pointsAgainst);
	~PlayerScore() = default;
};
