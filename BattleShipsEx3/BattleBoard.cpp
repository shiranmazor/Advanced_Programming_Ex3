#include "BattleBoard.h"

/*
* Recursively collect all adjacent cells containing the same symbol
*/
void BattleBoard::_collect_ship(int z, int i, int j, set<tuple<int, int, int>>* s)
{
	char curr = this->board[z][i][j];
	s->insert(make_tuple(i, j , z));

	if (i + 1 < this->rows() && this->board[z][i + 1][j] == curr && s->find(make_tuple(i + 1, j, z)) == s->end()) _collect_ship(z, i + 1, j, s);
	if (i > 0 && this->board[z][i - 1][j] == curr && s->find(make_tuple(i - 1, j, z)) == s->end()) _collect_ship(z, i - 1, j, s);
	
	if (j + 1 < this->cols() && this->board[z][i][j + 1] == curr && s->find(make_tuple(i, j + 1, z)) == s->end()) _collect_ship(z, i, j + 1, s);
	if (j > 0 && this->board[z][i][j - 1] == curr && s->find(make_tuple(i, j - 1, z)) == s->end()) _collect_ship(z, i, j - 1, s);
	
	if (z + 1 < this->depth() && this->board[z + 1][i][j] == curr && s->find(make_tuple(i, j, z + 1)) == s->end()) _collect_ship(z + 1, i, j, s);
	if (z > 0 && this->board[z - 1][i][j] == curr && s->find(make_tuple(i, j, z - 1)) == s->end()) _collect_ship(z - 1, i, j, s);

	return;
}

/*
*  Check if the board is valid and print relevant messages if it isn't,
*  while doing so, also initialize 'ships' Map containing board cells
*  leading to Vessel object representing the different Vessels on the board.
*/
bool BattleBoard::isBoardValid(vector<string>& error_messages)
{
	//int countA = 0;
	//int countB = 0;
	int totalShape = 0;
	set<tuple<int, int, int>> checkedCells, temp;
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
				if (this->board[z][i][j] == ' ' || checkedCells.find(make_tuple(i, j, z)) != checkedCells.end()) continue;

				// collect all adjacent cells contaning the same symbol
				_collect_ship(z, i, j, &temp);
				allI = true;
				allJ = true;
				allZ = true;
				for (auto const& element : temp)
				{
					allI = get<0>(element) == i && allI;
					allJ = get<1>(element) == j && allJ;
					allZ = get<2>(element) == z && allZ;
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
					//if (isPlayerChar(A, this->board[z][i][j])) countA++;
					//else countB++;

					// add ship to the game board 'ships' Map and check for adjacent ships
					currShip = make_shared<Vessel>(Vessel(this->board[z][i][j]));
					for (auto const& element : temp)
					{
						checkedCells.insert(element);
						Coordinate cell(get<0>(element), get<1>(element), get<2>(element));
						this->ships[makeKey(cell)] = currShip;
						if ((cell.row > 0 && this->board[cell.depth][cell.row - 1][cell.col] != ' ' && this->board[cell.depth][cell.row - 1][cell.col] != this->board[z][i][j]) ||
							(cell.col > 0 && this->board[cell.depth][cell.row][cell.col - 1] != ' ' && this->board[cell.depth][cell.row][cell.col - 1] != this->board[z][i][j]) ||
							(cell.depth > 0 && this->board[cell.depth - 1][cell.row][cell.col] != ' ' && this->board[cell.depth - 1][cell.row][cell.col] != this->board[z][i][j]) ||
							(cell.row < this->rows() - 1 && this->board[cell.depth][cell.row + 1][cell.col] != ' ' && this->board[cell.depth][cell.row + 1][cell.col] != this->board[z][i][j]) ||
							(cell.col < this->cols() - 1 && this->board[cell.depth][cell.row][cell.col + 1] != ' ' && this->board[cell.depth][cell.row][cell.col + 1] != this->board[z][i][j]) ||
							(cell.depth < this->depth() - 1 && this->board[cell.depth + 1][cell.row][cell.col] != ' ' && this->board[cell.depth + 1][cell.row][cell.col] != this->board[z][i][j]))
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

	// Assuming both players have same number of ships
	//if (countA != countB) error_messages.push_back("Players have different number of ships on board");

	if (tooClose) error_messages.push_back("Adjacent Ships on Board");

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
	int countA(0), countB(0), sinkA(0), sinkB(0);
	set<shared_ptr<Vessel>> seenVessels;

	for (auto const& element : this->ships)
		if (seenVessels.find(element.second) == seenVessels.end())
		{
			seenVessels.insert(element.second);
			if (element.second->player == A) countA++;
			else countB++;
			if (element.second->size <= element.second->hitNum)
			{
				if (element.second->player == A) sinkA++;
				else sinkB++;
			}
		}

	if (countA == sinkA) winner = B;
	else if (countB == sinkB) winner = A;

	return winner;
}

char BattleBoard::charAt(Coordinate c) const
{
	if (c.depth > 0 && c.row > 0 && c.col)
		return this->board[c.depth - 1][c.row - 1][c.col - 1];
	else
		return this->board[c.depth][c.row][c.col];
}

BattleBoard BattleBoard::getPlayerBoard(Player player) const
{
	BattleBoard pBoard(*this);
	vector<string> keys;

	// Remove opponent's vessels from player board
	for (int z = 0; z < pBoard.depth(); z++)
		for (int i = 0; i < pBoard.rows(); i++)
			for (int j = 0; j < pBoard.cols(); j++)
				if (!isPlayerChar(player, pBoard.board[z][i][j])) pBoard.board[z][i][j] = ' ';

	// Remove opponent's vessels from player board's ships map
	for (auto const& element : this->ships)
		if (element.second->player != player)
			keys.push_back(element.first);

	for (string key : keys) pBoard.ships.erase(key);

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

void BattleBoard::debug_print_board()
{
	for (int z = 0; z < this->depth(); z++)
	{
		cout << "\ndepth " << z << "\n\n";
		for (int i = 0; i < this->rows(); i++)
		{
			for (int j = 0; j < this->cols(); j++)
				cout << this->board[z][i][j] << "|";
			cout << endl;
		}
	}
}