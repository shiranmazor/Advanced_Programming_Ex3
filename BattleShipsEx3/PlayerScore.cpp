#include "PlayerScore.h"

PlayerScore::PlayerScore(const string name, int id):wins(0), losses(0), winsRate(0),lossRate(0),
gamesCounter(0), pointsFor(0), pointsAgainst(0), playerId(id)
{
	this->playerName = name;
}

void PlayerScore::UpdateScore(bool isWon, int pointsFor, int pointsAgainst)
{
	this->gamesCounter++;

	if (isWon)
		this->wins++;
	else
		this->losses++;

	//update points and rates
	this->pointsAgainst += pointsAgainst;
	this->pointsFor += pointsFor;
	//calc rate:
	double total = this->wins + this->losses;
	this->winsRate = static_cast<double>(this->wins) / total;
	this->lossRate = static_cast<double>(this->losses) / total;
}


