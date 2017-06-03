#include "BattleBoard.h"

/*
* 	Board validation assisting function, returns:
*	0 - no direction (ship size 1 or adjecent ships error)
*	1 - going right
*	2 - going down
*	3 - going in (deep)
*/
int BattleBoard::_getShipDirection(int z, int i, int j)
{
	if (j != this->cols() - 1 && this->board[z][i][j + 1] == this->board[z][i][j])
		return 1;
	
	if (i != this->rows() - 1 && this->board[z][i + 1][j] == this->board[z][i][j])
		return 2;
	
	if (z != this->depth() - 1 && this->board[z + 1][i][j] == this->board[z][i][j])
		return 3;

	return 0;
}

/*
* Recursively collect all adjacent cells containing the same symbol
*/
void BattleBoard::_collect_ship(int z, int i, int j, set<Coordinate>* s)
{
	char curr = this->board[z][i][j];
	s->insert(Coordinate(i, j , z));

	if (i + 1 < this->rows() && this->board[z][i + 1][j] == curr && s->find(Coordinate(i + 1, j, z)) == s->end()) _collect_ship(z, i + 1, j, s);
	if (i > 0 && this->board[z][i - 1][j] == curr && s->find(Coordinate(i - 1, j, z)) == s->end()) _collect_ship(z, i - 1, j, s);
	
	if (j + 1 < this->cols() && this->board[z][i][j + 1] == curr && s->find(Coordinate(i, j + 1, z)) == s->end()) _collect_ship(z, i, j + 1, s);
	if (j > 0 && this->board[z][i][j - 1] == curr && s->find(Coordinate(i, j - 1, z)) == s->end()) _collect_ship(z, i, j - 1, s);
	
	if (z + 1 < this->depth() && this->board[z + 1][i][j] == curr && s->find(Coordinate(i, j, z + 1)) == s->end()) _collect_ship(z + 1, i, j, s);
	if (z > 0 && this->board[z - 1][i][j] == curr && s->find(Coordinate(i, j, z - 1)) == s->end()) _collect_ship(z - 1, i, j, s);

	return;
}




/*
*  Check if the board is valid and print relevant messages if it isn't,
*  while doing so, also initialize 'ships' Map containing board cells
*  leading to Vessel object representing the different Vessels on the board.
*/
bool BattleBoard::isBoardValid(vector<string>& error_messages)
{
	int countA = 0;
	int countB = 0;
	int totalShape = 0;
	set<Coordinate> checkedCells, temp;
	bool allI, allJ, allZ;
	bool badShape[8] = {false};
	bool tooClose = false;
	shared_ptr<Vessel> currShip;

	for (int z = 0; z < this->depth(); z++)
	{
		for (int i = 0; i < this->rows(); i++)
		{
			for (int j = 0; j < this->cols(); j++)
			{
				// ignore empty or already checked boxes (in case of full ship scan)
				if (this->board[z][i][j] == ' ' || checkedCells.find(Coordinate(i, j, z)) != checkedCells.end()) continue;

				// collect all adjacent cells contaning the same symbol
				_collect_ship(z, i, j, &temp);
				allI = true;
				allJ = true;
				allZ = true;
				for (auto const& element : temp)
				{
					allI = element.row == i && allI;
					allJ = element.col == j && allJ;
					allZ = element.depth == z && allZ;
				}

				// determine ship shape and size
				if ((!allJ && !allI && !allZ) || temp.size() != getShipSize(this->board[z][i][j]))
				{
					badShape[ship2idx.at(this->board[z][i][j])] = true;
					for (auto const& element : temp)
						checkedCells.insert(element);
				}
				else // ship shape and size are correct
				{
					if (isPlayerChar(A, this->board[z][i][j])) countA++;
					else countB++;

					// add ship to the game board 'ships' Map and check for adjacent ships
					currShip = make_shared<Vessel>(Vessel(this->board[z][i][j]));
					for (auto const& element : temp)
					{
						checkedCells.insert(element);
						this->ships[makeKey(element)] = currShip;
						if ((element.row > 0 && this->board[element.depth][element.row - 1][element.col] != ' ' && this->board[element.depth][element.row - 1][element.col] != this->board[z][i][j]) ||
							(element.col > 0 && this->board[element.depth][element.row][element.col - 1] != ' ' && this->board[element.depth][element.row][element.col - 1] != this->board[z][i][j]) ||
							(element.depth > 0 && this->board[element.depth - 1][element.row][element.col] != ' ' && this->board[element.depth - 1][element.row][element.col] != this->board[z][i][j]) ||
							(element.row < this->rows() - 1 && this->board[element.depth][element.row + 1][element.col] != ' ' && this->board[element.depth][element.row + 1][element.col] != this->board[z][i][j]) ||
							(element.col < this->cols() - 1 && this->board[element.depth][element.row][element.col + 1] != ' ' && this->board[element.depth][element.row][element.col + 1] != this->board[z][i][j]) ||
							(element.depth < this->depth() - 1 && this->board[element.depth + 1][element.row][element.col] != ' ' && this->board[element.depth + 1][element.row][element.col] != this->board[z][i][j]))
						{
							tooClose = true;
						}
					}
				}
				temp.clear();
			}
		}
	}

	// print out relevant errors
	for (int i = 0; i < 8; i++)
	{
		if (badShape[i])
		{
			string error = "Wrong size or shape for ship ";
			error.append(&(idx2ship[i]));
			error.append(" for player ");
			error.append(((i < 4) ? "A" : "B"));
			error_messages.push_back(error);
		}

		totalShape += badShape[i];
	}

	//if (countA > this->playerToolsNum) error_messages.push_back("Too many ships for player A");
	//if (countA < this->playerToolsNum) error_messages.push_back("Too few ships for player A");
	//if (countB > this->playerToolsNum) error_messages.push_back("Too many ships for player B");
	//if (countB < this->playerToolsNum) error_messages.push_back("Too few ships for player B");
	
	// Assuming both players have same number of ships
	//if (countA != countB) error_messages.push_back("Players have different number of ships on board");
	this->playerToolsNum = countA;

	if (tooClose) error_messages.push_back("Adjacent Ships on Board");

	//return (countA == this->playerToolsNum && countB == this->playerToolsNum && !tooClose && totalShape == 0);
	return (!tooClose && totalShape == 0);
}

pair<int, int> BattleBoard::CalcScore()
{
	pair<int, int> scores(0, 0);
	set<shared_ptr<Vessel>> seenVessels;

	for (auto const& element : this->ships)
		if (element.second->size <= element.second->hitNum && seenVessels.find(element.second) == seenVessels.end())
		{
			//ship has sinked!
			seenVessels.insert(element.second);
			if (element.second->player == B)
				scores.first += getShipScore(element.second->type);
			else
				scores.second += getShipScore(element.second->type);
		}
	
	return scores;
}

int BattleBoard::CheckVictory()
{
	int winner(-1);
	int countA(0), countB(0);
	set<shared_ptr<Vessel>> seenVessels;

	for (auto const& element : this->ships)
		if (element.second->size <= element.second->hitNum && seenVessels.find(element.second) == seenVessels.end())
		{
			seenVessels.insert(element.second);
			if (element.second->player == A) countA++;
			else countB++;
		}

	if (countA == this->playerToolsNum) winner = B;
	else if (countB == this->playerToolsNum) winner = A;

	return winner;
}

char BattleBoard::charAt(Coordinate c) const
{
	return this->board[c.depth - 1][c.row - 1][c.col - 1];
}

BattleBoard BattleBoard::getPlayerBoard(Player player) const
{
	BattleBoard pBoard(*this);
	pBoard.ships = {};

	for (int z = 0; z < pBoard.depth(); z++)
		for (int i = 0; i < pBoard.rows(); i++)
			for (int j = 0; j < pBoard.cols(); j++)
				if (!isPlayerChar(player, pBoard.board[z][i][j])) pBoard.board[z][i][j] = ' ';

	return pBoard;
}

AttackResult BattleBoard::performGameMove(int p, Coordinate move)
{
	char c = this->charAt(move);
	move = fitCoordinate(move);
	if (!isspace(c)) 
	{
		if (isAlreadyHit(c)) 
		{
			return (this->ships[makeKey(move)]->hitNum == this->ships[makeKey(move)]->size) ? AttackResult::Miss : AttackResult::Hit;
		}
		if (isupper(c) || islower(c)) 
		{
			this->board[move.depth][move.row][move.col] = isupper(c) ? HitMarkA : HitMarkB;
			this->ships[makeKey(move)]->hitNum++;
			return (this->ships[makeKey(move)]->hitNum == this->ships[makeKey(move)]->size) ? AttackResult::Sink : AttackResult::Hit;
		}
	}
	return AttackResult::Miss;
}

