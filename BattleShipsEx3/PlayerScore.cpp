#include "PlayerScore.h"

PlayerScore::PlayerScore(const string name, int id): playerId(id), gamesCounter(0)
{
	this->playerName = name;

}

void PlayerScore::UpdateScore(bool isWon, int pointsFor, int pointsAgainst, bool tie)
{
	if (rounds.size() == 0)//first round
	{
		//create first round score objects
		PlayerRoundScore ps = PlayerRoundScore();
		if (isWon)
			ps.wins++;
		else if (!tie) //if tie wins and losses stay the same
			ps.losses++;
		//update points and rates
		ps.pointsAgainst += pointsAgainst;
		ps.pointsFor += pointsFor;
		double total = ps.wins + ps.losses;
		ps.winRate = static_cast<double>(ps.wins) / total;
		rounds.push_back(ps);
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
		double total = newRound.wins + newRound.losses;
		newRound.winRate = static_cast<double>(newRound.wins) / total;
		rounds.push_back(newRound);

	}


	this->gamesCounter++;
}


