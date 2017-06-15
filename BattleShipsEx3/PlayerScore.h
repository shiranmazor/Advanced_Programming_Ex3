#pragma once
#include <string>
#include <vector>

using namespace std;

class PlayerRoundScore
{
public:
	unsigned int wins;
	unsigned int losses;
	int pointsFor;
	int pointsAgainst;
	double winRate;

	PlayerRoundScore():wins(0), losses(0), winRate(0),
		 pointsFor(0), pointsAgainst(0){}

	PlayerRoundScore(const PlayerRoundScore& p)
	{
		wins = p.wins;
		losses = p.losses;
		pointsFor = p.pointsFor;
		pointsAgainst = p.pointsAgainst;
		winRate = p.winRate;
		
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
	void UpdateScore(bool isWon, int pointsFor, int pointsAgainst, bool tie);
	~PlayerScore() = default;
};
