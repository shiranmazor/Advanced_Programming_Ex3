#include <cctype>
#include <utility>
#include "BattleshipGameAlgo.h"


IBattleshipGameAlgo* GetAlgorithm()
{
	return new BattleshipGameAlgo();
}

void BattleshipGameAlgo::setPlayer(int player)
{
	this->playerNum = player;
}

bool BattleshipGameAlgo::_canAttack(int z, int i, int j) const
{
	return (i >= 0 && i < this->playerBoard.rows && 
			j >= 0 && j < this->playerBoard.cols && 
			z >= 0 && z < this->playerBoard.depth && 
			this->playerBoard.board[z][i][j] == ' ');
}

void BattleshipGameAlgo::_markIrrelevant(int row, int col, int depth)
{
	if (row >= 0 && row < this->playerBoard.rows &&
		col >= 0 && col < this->playerBoard.cols &&
		depth >= 0 && depth < this->playerBoard.depth)
	{
		this->playerBoard.board[row][col][depth] = irrelevnatCell;
	}
}



void BattleshipGameAlgo::setBoard(const BoardData& board)
{
	char current;
	this->playerBoard.rows = board.rows();
	this->playerBoard.cols = board.cols();
	this->playerBoard.depth = board.depth();

	// Copy board data to playerBoard
	for (int z = 0; z < this->playerBoard.depth; z++)
	{
		this->playerBoard.board.push_back(vector<vector<char>>());
		for (int i = 0; i < this->playerBoard.rows; i++)
		{
			this->playerBoard.board[z].push_back(vector<char>());
			for (int j = 0; j < this->playerBoard.cols; j++)
			{
				current = board.charAt(Coordinate(i + 1, j + 1, z + 1));
				this->playerBoard.board[z][i].push_back(current);
				if (current != ' ') this->playerBoard.hostileShips[current]++;
			}
		}
	}

	for (char c : shipsBySize) this->playerBoard.hostileShips[c] = this->playerBoard.hostileShips[c] / getShipSize(c);

	set<Coordinate> irrelevantCells;
	for (int z = 0; z < this->playerBoard.depth; z++)
	{
		for (int i = 0; i < this->playerBoard.rows; i++)
		{
			for (int j = 0; j < this->playerBoard.cols; j++)
			{
				if (this->playerBoard.board[z][i][j] != ' ')
				{
					if (i + 1 < this->playerBoard.rows && this->playerBoard.board[z][i + 1][j] == ' ') irrelevantCells.insert(Coordinate(z, i + 1, j));
					if (i > 0 && this->playerBoard.board[z][i - 1][j] == ' ') irrelevantCells.insert(Coordinate(z, i - 1, j));
					if (j + 1 < this->playerBoard.cols && this->playerBoard.board[z][i][j + 1] == ' ') irrelevantCells.insert(Coordinate(z, i, j + 1));
					if (j > 0 && this->playerBoard.board[z][i][j - 1] == ' ') irrelevantCells.insert(Coordinate(z, i, j - 1));
					if (z + 1 < this->playerBoard.depth && this->playerBoard.board[z + 1][i][j] == ' ') irrelevantCells.insert(Coordinate(z + 1, i, j));
					if (z > 0 && this->playerBoard.board[z - 1][i][j] == ' ') irrelevantCells.insert(Coordinate(z - 1, i, j));
				}
			}
		}
	}
	for (auto const& cell : irrelevantCells)
		this->_markIrrelevant(cell.row, cell.col, cell.depth);
}

// TODO - new _getBestGuess using hostileShips dict (try to fit all these exact ships on the board)

Coordinate BattleshipGameAlgo::_getBestGuess() const
{
	/*vector<vector<int>> scoreBoard(this->playerBoard->R, vector<int>(this->playerBoard->C));
	bool goodI, goodJ;
	pair<int, int> bestCell;
	int bestScore = -1;

	for (int i = 0; i < this->playerBoard->R; i++)
		for (int j = 0; j < this->playerBoard->C; j++) scoreBoard[i][j] = 0;

	// Calculate score for each cell on the board according to how many ships
	// could be placed there (and how much points those ships are worth)
	for (char ship : shipsBySize)
	{
		for (int i = 0; i < this->playerBoard->R; i++)
		{
			for (int j = 0; j < this->playerBoard->C; j++)
			{
				if (this->_canAttack(i, j))
				{
					goodI = true;
					goodJ = true;
					for (int l = 1; l < getShipSize(ship); l++) // Verify there is room for the whole ship
					{
						if (!this->_canAttack(i + l, j)) goodI = false;
						if (!this->_canAttack(i, j + l)) goodJ = false;
					}
					for (int l = 0; l < getShipSize(ship); l++) // Add ship's score to all relevant cells
					{
						if (goodI) scoreBoard[i + l][j] += getShipScore(ship);
						if (goodJ && getShipSize(ship) > 1) scoreBoard[i][j + l] += getShipScore(ship); // Prevent double scoring for 1 cell ships
					}
				}
			}
		}
	}

	// Find highest scored cell
	for (int i = 0; i < this->playerBoard->R; i++)
	{
		for (int j = 0; j < this->playerBoard->C; j++)
		{
			if (scoreBoard[i][j] > bestScore)
			{
				bestScore = scoreBoard[i][j];
				bestCell = _make_pair(i, j);
			}
		}
	}

	if (_canAttack(bestCell.first - 1, bestCell.second - 1)) return bestCell;

	// No more available moves (safety)
	return make_pair(-1, -1);*/
	return _Coordinate(0, 0, 0);
}



Coordinate BattleshipGameAlgo::attack()
{
	// search mode - trying to find opponent ship
	if (this->target == nullptr)
		return _getBestGuess();

	// target mode - targeting a specific ship
	if (this->target->direction < 1) // don't know direction or no direction
	{
		if (this->_canAttack(this->target->newEdges[0].depth, this->target->newEdges[0].row + 1, this->target->newEdges[0].col)) return _Coordinate(this->target->newEdges[0].depth, this->target->newEdges[0].row + 1, this->target->newEdges[0].col);
		if (this->_canAttack(this->target->newEdges[0].depth, this->target->newEdges[0].row - 1, this->target->newEdges[0].col)) return _Coordinate(this->target->newEdges[0].depth, this->target->newEdges[0].row - 1, this->target->newEdges[0].col);
		if (this->_canAttack(this->target->newEdges[0].depth, this->target->newEdges[0].row, this->target->newEdges[0].col + 1)) return _Coordinate(this->target->newEdges[0].depth, this->target->newEdges[0].row, this->target->newEdges[0].col + 1);
		if (this->_canAttack(this->target->newEdges[0].depth, this->target->newEdges[0].row, this->target->newEdges[0].col - 1)) return _Coordinate(this->target->newEdges[0].depth, this->target->newEdges[0].row, this->target->newEdges[0].col - 1);
		if (this->_canAttack(this->target->newEdges[0].depth + 1, this->target->newEdges[0].row, this->target->newEdges[0].col)) return _Coordinate(this->target->newEdges[0].depth + 1, this->target->newEdges[0].row, this->target->newEdges[0].col);
		if (this->_canAttack(this->target->newEdges[0].depth - 1, this->target->newEdges[0].row, this->target->newEdges[0].col)) return _Coordinate(this->target->newEdges[0].depth - 1, this->target->newEdges[0].row, this->target->newEdges[0].col);
	}
	if (this->target->direction == 1) // horizontal
	{
		if (this->target->edgeReached != 0) // didn't reach the end of target vessel with edge[0]
		{
			if (this->target->newEdges[0].col > this->target->newEdges[1].col && this->_canAttack(this->target->newEdges[0].depth, this->target->newEdges[0].row, this->target->newEdges[0].col + 1))
				return _Coordinate(this->target->newEdges[0].depth, this->target->newEdges[0].row, this->target->newEdges[0].col + 1);
			if (this->target->newEdges[0].col < this->target->newEdges[1].col && this->_canAttack(this->target->newEdges[0].depth, this->target->newEdges[0].row, this->target->newEdges[0].col - 1))
				return _Coordinate(this->target->newEdges[0].depth, this->target->newEdges[0].row, this->target->newEdges[0].col - 1);
			// in the case that an attack cannot be made from this edge, mark it as reached
			this->target->edgeReached = 0;
		}
		if (this->target->edgeReached != 1) // didn't reach the end of target vessel with edge[1]
		{
			if (this->target->newEdges[1].col > this->target->newEdges[0].col && this->_canAttack(this->target->newEdges[0].depth, this->target->newEdges[1].row, this->target->newEdges[1].col + 1))
				return _Coordinate(this->target->newEdges[0].depth, this->target->newEdges[1].row, this->target->newEdges[1].col + 1);
			if (this->target->newEdges[1].col < this->target->newEdges[0].col && this->_canAttack(this->target->newEdges[0].depth, this->target->newEdges[1].row, this->target->newEdges[1].col - 1))
				return _Coordinate(this->target->newEdges[0].depth, this->target->newEdges[1].row, this->target->newEdges[1].col - 1);
			// in the case that an attack cannot be made from this edge, mark it as reached
			this->target->edgeReached = 1;
		}
	}
	else if (this->target->direction == 2) // vertical
	{
		if (this->target->edgeReached != 0) // didn't reach the end of target vessel with edge[0]
		{
			if (this->target->newEdges[0].row > this->target->newEdges[1].row && this->_canAttack(this->target->newEdges[0].depth, this->target->newEdges[0].row + 1, this->target->newEdges[0].col))
				return _Coordinate(this->target->newEdges[0].depth, this->target->newEdges[0].row + 1, this->target->newEdges[0].col);
			if (this->target->newEdges[0].row < this->target->newEdges[1].row && this->_canAttack(this->target->newEdges[0].depth, this->target->newEdges[0].row - 1, this->target->newEdges[0].col))
				return _Coordinate(this->target->newEdges[0].depth, this->target->newEdges[0].row - 1, this->target->newEdges[0].col - 1);
			// in the case that an attack cannot be made from this edge, mark it as reached
			this->target->edgeReached = 0;
		}
		if (this->target->edgeReached != 1) // didn't reach the end of target vessel with edge[1]
		{
			if (this->target->newEdges[1].row > this->target->newEdges[0].row && this->_canAttack(this->target->newEdges[0].depth, this->target->newEdges[1].row + 1, this->target->newEdges[1].col))
				return _Coordinate(this->target->newEdges[0].depth, this->target->newEdges[1].row + 1, this->target->newEdges[1].col);
			if (this->target->newEdges[1].row < this->target->newEdges[0].row && this->_canAttack(this->target->newEdges[0].depth, this->target->newEdges[1].row - 1, this->target->newEdges[1].col))
				return _Coordinate(this->target->newEdges[0].depth, this->target->newEdges[1].row - 1, this->target->newEdges[1].col);
			// in the case that an attack cannot be made from this edge, mark it as reached
			this->target->edgeReached = 1;
		}
	}
	else // this->target->direction == 3 => z/depth axis
	{
		if (this->target->edgeReached != 0) // didn't reach the end of target vessel with edge[0]
		{
			if (this->target->newEdges[0].depth > this->target->newEdges[1].depth && this->_canAttack(this->target->newEdges[0].depth + 1, this->target->newEdges[0].row, this->target->newEdges[0].col))
				return _Coordinate(this->target->newEdges[0].depth + 1, this->target->newEdges[0].row, this->target->newEdges[0].col);
			if (this->target->newEdges[0].depth < this->target->newEdges[1].depth && this->_canAttack(this->target->newEdges[0].depth - 1, this->target->newEdges[0].row, this->target->newEdges[0].col))
				return _Coordinate(this->target->newEdges[0].depth - 1, this->target->newEdges[0].row - 1, this->target->newEdges[0].col);
			// in the case that an attack cannot be made from this edge, mark it as reached
			this->target->edgeReached = 0;
		}
		if (this->target->edgeReached != 1) // didn't reach the end of target vessel with edge[1]
		{
			if (this->target->newEdges[1].depth > this->target->newEdges[0].depth && this->_canAttack(this->target->newEdges[1].depth + 1, this->target->newEdges[1].row, this->target->newEdges[0].col))
				return _Coordinate(this->target->newEdges[1].depth + 1, this->target->newEdges[1].row, this->target->newEdges[0].col);
			if (this->target->newEdges[1].depth < this->target->newEdges[0].depth && this->_canAttack(this->target->newEdges[1].depth - 1, this->target->newEdges[1].row, this->target->newEdges[0].col))
				return _Coordinate(this->target->newEdges[1].depth - 1, this->target->newEdges[1].row, this->target->newEdges[0].col);
			// in the case that an attack cannot be made from this edge, mark it as reached
			this->target->edgeReached = 1;
		}
	}

	// safety, should never get here
	cout << "BattleshipGameAlgoSmart.attack: Something went wrong, can't find next cell to attack (Player " << this->playerNum << ", currently in " << (this->target == nullptr ? "HUNT" : "TARGET") << " mode)" << endl;
	return Coordinate(-1, -1, -1);
}

void BattleshipGameAlgo::notifyOnAttackResult(int player, Coordinate move, AttackResult result)
{
	Coordinate fitMove = fitCoordinate(move);
	if (player == this->playerNum)
	{
		int row = fitMove.row;
		int col = fitMove.col;
		int depth = fitMove.depth;

		this->_markIrrelevant(row, col, depth);

		switch (result) {
		case AttackResult::Miss:
			if (this->target != nullptr) {
				if (this->target->direction == 1 && (col == this->target->newEdges[0].col + 1 || col == this->target->newEdges[0].col - 1)) this->target->edgeReached = 0;
				if (this->target->direction == 1 && (col == this->target->newEdges[1].col + 1 || col == this->target->newEdges[1].col - 1)) this->target->edgeReached = 1;
				if (this->target->direction == 2 && (row == this->target->newEdges[0].row + 1 || row == this->target->newEdges[0].row - 1)) this->target->edgeReached = 0;
				if (this->target->direction == 2 && (row == this->target->newEdges[1].row + 1 || row == this->target->newEdges[1].row - 1)) this->target->edgeReached = 1;
				if (this->target->direction == 3 && (depth == this->target->newEdges[0].depth + 1 || depth == this->target->newEdges[0].depth - 1)) this->target->edgeReached = 0;
				if (this->target->direction == 3 && (depth == this->target->newEdges[1].depth + 1 || depth == this->target->newEdges[1].depth - 1)) this->target->edgeReached = 1;
			}
			break;
		case AttackResult::Hit:
			if (this->target == nullptr)
			{
				this->target = new targetVessel();
				this->target->newEdges[0] = Coordinate(row, col, depth);
				//this->target->newEdges[1] = Coordinate(-1, -1, -1);
			}
			else
			{
				if (this->target->newEdges[1].row == -1 && 
					this->target->newEdges[1].col == -1 && 
					this->target->newEdges[1].depth == -1)
				{
					this->target->newEdges[1] = Coordinate(row, col, depth);
					if (this->target->newEdges[0].row == this->target->newEdges[1].row) {
						this->target->direction = 1;
						this->_markIrrelevant(this->target->newEdges[0].row + 1, this->target->newEdges[0].col, target->newEdges[0].depth - 1);
						this->_markIrrelevant(this->target->newEdges[0].row - 1, this->target->newEdges[0].col, target->newEdges[0].depth + 1);
						this->_markIrrelevant(this->target->newEdges[0].row + 1, this->target->newEdges[0].col, target->newEdges[0].depth + 1);
						this->_markIrrelevant(this->target->newEdges[0].row - 1, this->target->newEdges[0].col, target->newEdges[0].depth - 1);
					}
					else if (this->target->newEdges[0].col == this->target->newEdges[1].col)
					{
						this->target->direction = 2;
						this->_markIrrelevant(this->target->newEdges[0].row, this->target->newEdges[0].col + 1, target->newEdges[0].depth + 1);
						this->_markIrrelevant(this->target->newEdges[0].row, this->target->newEdges[0].col - 1, target->newEdges[0].depth + 1);
						this->_markIrrelevant(this->target->newEdges[0].row, this->target->newEdges[0].col + 1, target->newEdges[0].depth - 1);
						this->_markIrrelevant(this->target->newEdges[0].row, this->target->newEdges[0].col - 1, target->newEdges[0].depth - 1);
					}
					else {
						this->target->direction = 3;
						this->_markIrrelevant(this->target->newEdges[0].row + 1, this->target->newEdges[0].col + 1, target->newEdges[0].depth);
						this->_markIrrelevant(this->target->newEdges[0].row + 1, this->target->newEdges[0].col - 1, target->newEdges[0].depth);
						this->_markIrrelevant(this->target->newEdges[0].row - 1, this->target->newEdges[0].col + 1, target->newEdges[0].depth);
						this->_markIrrelevant(this->target->newEdges[0].row - 1, this->target->newEdges[0].col - 1, target->newEdges[0].depth);

					}
				}
				else
				{
					if (abs(this->target->newEdges[1].row - row) + abs(this->target->newEdges[1].col - col) + abs(this->target->newEdges[1].depth - depth) == 1) // one cell away from edge[1]
						this->target->newEdges[1] = Coordinate(row, col, depth);
					else
						this->target->newEdges[0] = Coordinate(row, col, depth);
				}

				// mark current cell's surrounding cells as irrelevant according to direction
				if (this->target->direction == 1)
				{
					this->_markIrrelevant(row + 1, col, depth + 1);
					this->_markIrrelevant(row - 1, col, depth + 1);
					this->_markIrrelevant(row + 1, col, depth - 1);
					this->_markIrrelevant(row - 1, col, depth - 1);
				}
				else if (this->target->direction == 2)
				{
					this->_markIrrelevant(row, col + 1, depth + 1);
					this->_markIrrelevant(row, col - 1, depth + 1);
					this->_markIrrelevant(row, col + 1, depth - 1);
					this->_markIrrelevant(row, col - 1, depth - 1);
				}
				else {
					this->_markIrrelevant(row + 1, col + 1, depth);
					this->_markIrrelevant(row + 1, col - 1, depth);
					this->_markIrrelevant(row - 1, col + 1, depth);
					this->_markIrrelevant(row - 1, col - 1, depth);
				}

				this->target->hitCount++;
			}
			break;
		case AttackResult::Sink:
			// mark surrounding cells as irrelevant (last hit, can add all directions)
			this->_markIrrelevant(row + 1, col, depth + 1);
			this->_markIrrelevant(row - 1, col, depth + 1);
			this->_markIrrelevant(row + 1, col, depth - 1);
			this->_markIrrelevant(row - 1, col, depth - 1);
			this->_markIrrelevant(row, col + 1, depth + 1);
			this->_markIrrelevant(row, col - 1, depth + 1);
			this->_markIrrelevant(row, col + 1, depth - 1);
			this->_markIrrelevant(row, col - 1, depth - 1);
			this->_markIrrelevant(row + 1, col + 1, depth);
			this->_markIrrelevant(row + 1, col - 1, depth);
			this->_markIrrelevant(row - 1, col + 1, depth);
			this->_markIrrelevant(row - 1, col - 1, depth);

			this->playerBoard.hostileShips[getShipBySize(this->target->hitCount + 1)]--;

			delete this->target;
			this->target = nullptr; // back to search mode
			//this->hostileShipsNum -= 1;
			break;
		default:
			// safety, should never get here
			cout << "BattleshipGameAlgo.notifyOnAttackResult: Something went wrong, got bad AttackResult" << endl;
			break;
		}
	}
}

