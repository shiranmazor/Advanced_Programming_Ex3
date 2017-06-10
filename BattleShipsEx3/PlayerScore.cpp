#include "PlayerScore.h"

PlayerScore::PlayerScore(const string name, int id): playerId(id), gamesCounter(0)
{
	this->playerName = name;

}

void PlayerScore::UpdateScore(bool isWon, int pointsFor, int pointsAgainst)
{
	if (rounds.size() == 0)//first round
	{
		//create first round score objects
		rounds.push_back(PlayerRoundScore());
		if (isWon)
			rounds[this->gamesCounter].wins++;
		else
			rounds[this->gamesCounter].losses++;
		//update points and rates
		rounds[this->gamesCounter].pointsAgainst += pointsAgainst;
		rounds[this->gamesCounter].pointsFor += pointsFor;
		//calc rate:
		double total = rounds[this->gamesCounter].wins + rounds[this->gamesCounter].losses;
		rounds[this->gamesCounter].winsRate = static_cast<double>(rounds[this->gamesCounter].wins) / total;
		rounds[this->gamesCounter].lossRate = static_cast<double>(rounds[this->gamesCounter].losses) / total;

	}
	else
	{
		//create new round from last round and increase
		int last_index = static_cast<int>(rounds.size()) - 1;
		PlayerRoundScore newRound = PlayerRoundScore(rounds[last_index]);
		if (isWon)
			newRound.wins++;
		else
			newRound.losses++;
		//update points and rates
		newRound.pointsAgainst += pointsAgainst;
		newRound.pointsFor += pointsFor;
		//calc rate:
		double total = newRound.wins + newRound.losses;
		newRound.winsRate = static_cast<double>(newRound.wins) / total;
		newRound.lossRate = static_cast<double>(newRound.losses) / total;
		rounds.push_back(newRound);

	}


	this->gamesCounter++;
}


