#include <cctype>
#include <utility>
#include "BattleshipGameAlgo.h"


IBattleshipGameAlgo* GetAlgorithm()
{
	return  new BattleshipGameAlgo();
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

// _markIrrelevant Has optional arg for board, if not sent updates this->playerBoard
void BattleshipGameAlgo::_markIrrelevant(int depth, int row, int col)
{
	if (row >= 0 && row < this->playerBoard.rows &&
		col >= 0 && col < this->playerBoard.cols &&
		depth >= 0 && depth < this->playerBoard.depth)
	{
		this->playerBoard.board[depth][row][col] = irrelevnatCell;
	}
}

void BattleshipGameAlgo::_placeShips(vector<vector<vector<int>>>& scoreBoard) const
{
	bool goodZ, goodI, goodJ;
	char ship = irrelevnatCell;

	for (auto const& shipCounter : this->playerBoard.hostileShips)
	{
		ship = shipCounter.first;
		for (int s = 0; s < shipCounter.second; s++)
		{
			for (int z = 0; z < this->playerBoard.depth; z++)
			{
				for (int i = 0; i < this->playerBoard.rows; i++)
				{
					for (int j = 0; j < this->playerBoard.cols; j++)
					{
						if (!_canAttack(z, i, j)) continue;
		
						goodZ = true;
						goodI = true;
						goodJ = true;
		
						// Verify there is room for the whole ship in each axis
						for (int l = 1; l < getShipSize(ship); l++)
						{
							if (!_canAttack(z + l, i, j)) goodZ = false;
							if (!_canAttack(z, i + l, j)) goodI = false;
							if (!_canAttack(z, i, j + l)) goodJ = false;
						}
		
						for (int l = 0; l < getShipSize(ship); l++)
						{
							if (goodZ) scoreBoard[z + l][i][j] += getShipScore(ship);
							if (goodI) scoreBoard[z][i + l][j] += getShipScore(ship);
							if (goodJ) scoreBoard[z][i][j + l] += getShipScore(ship);
						}
					}
				}
			}
		}
	}
}

Coordinate BattleshipGameAlgo::_getBestGuess()
{
	vector<vector<vector<int>>> scoreBoard(this->playerBoard.depth, vector<vector<int>>(this->playerBoard.rows, vector<int>(this->playerBoard.cols)));
	Coordinate bestCell = Coordinate(-1, -1, -1);
	int bestScore(-1), remainingShips(0), relevantCells(0);
	
	for (auto const& shipCounter : this->playerBoard.hostileShips) remainingShips += shipCounter.second;
	for (int z = 0; z < this->playerBoard.depth; z++)
	{
		for (int i = 0; i < this->playerBoard.rows; i++)
		{
			for (int j = 0; j < this->playerBoard.cols; j++)
			{
				if (_canAttack(z, i, j)) relevantCells++;
			}
		}
	}

	// Calculate score for each cell on the board according to how many ships
	// could be placed there (and how much points those ships are worth). 
	// Positioning ships under to the assumption that the opponent has the exact same ships as this player.
	_placeShips(scoreBoard);

	// Find highest scored cell
	for (int z = 0; z < this->playerBoard.depth; z++)
	{
		for (int i = 0; i < this->playerBoard.rows; i++)
		{
			for (int j = 0; j < this->playerBoard.cols; j++)
			{
				if (scoreBoard[z][i][j] > bestScore)
				{
					bestScore = scoreBoard[z][i][j];
					bestCell = _Coordinate(i, j ,z);
				}
			}
		}
	}

	// safety
	if (_canAttack(bestCell.depth - 1, bestCell.row - 1, bestCell.col - 1)) return bestCell;

	// attack the next relevant cell
	for (int z = 0; z < this->playerBoard.depth; z++)
		for (int i = 0; i < this->playerBoard.rows; i++)
			for (int j = 0; j < this->playerBoard.cols; j++)
				if (_canAttack(z, i, j)) return _Coordinate(i, j, z);

	// No more available moves
	return Coordinate(-1, -1, -1);
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
				if (current != ' ') this->playerBoard.hostileShips[tolower(current)]++;
			}
		}
	}

	for (char c : shipsBySize) this->playerBoard.hostileShips[tolower(c)] = this->playerBoard.hostileShips[tolower(c)] / getShipSize(c);

	set<tuple<int, int, int>> irrelevantCells;
	for (int z = 0; z < this->playerBoard.depth; z++)
	{
		for (int i = 0; i < this->playerBoard.rows; i++)
		{
			for (int j = 0; j < this->playerBoard.cols; j++)
			{
				if (this->playerBoard.board[z][i][j] != ' ')
				{
					if (i + 1 < this->playerBoard.rows && this->playerBoard.board[z][i + 1][j] == ' ') irrelevantCells.insert(make_tuple(z, i + 1, j));
					if (i > 0 && this->playerBoard.board[z][i - 1][j] == ' ') irrelevantCells.insert(make_tuple(z, i - 1, j));
					if (j + 1 < this->playerBoard.cols && this->playerBoard.board[z][i][j + 1] == ' ') irrelevantCells.insert(make_tuple(z, i, j + 1));
					if (j > 0 && this->playerBoard.board[z][i][j - 1] == ' ') irrelevantCells.insert(make_tuple(z, i, j - 1));
					if (z + 1 < this->playerBoard.depth && this->playerBoard.board[z + 1][i][j] == ' ') irrelevantCells.insert(make_tuple(z + 1, i, j));
					if (z > 0 && this->playerBoard.board[z - 1][i][j] == ' ') irrelevantCells.insert(make_tuple(z - 1, i, j));
				}
			}
		}
	}
	for (auto const& cell : irrelevantCells)
		_markIrrelevant(get<0>(cell), get<1>(cell), get<2>(cell));
}

Coordinate BattleshipGameAlgo::attack()
{
	// search mode - trying to find opponent ship
	if (!this->target.exists)
		return _getBestGuess();

	// target mode - targeting a specific ship
	if (this->target.direction < 1) // don't know direction or no direction
	{
		if (_canAttack(this->target.edges[0].depth, this->target.edges[0].row + 1, this->target.edges[0].col)) return _Coordinate(this->target.edges[0].row + 1, this->target.edges[0].col, this->target.edges[0].depth);
		if (_canAttack(this->target.edges[0].depth, this->target.edges[0].row - 1, this->target.edges[0].col)) return _Coordinate(this->target.edges[0].row - 1, this->target.edges[0].col, this->target.edges[0].depth);
		if (_canAttack(this->target.edges[0].depth, this->target.edges[0].row, this->target.edges[0].col + 1)) return _Coordinate(this->target.edges[0].row, this->target.edges[0].col + 1, this->target.edges[0].depth);
		if (_canAttack(this->target.edges[0].depth, this->target.edges[0].row, this->target.edges[0].col - 1)) return _Coordinate(this->target.edges[0].row, this->target.edges[0].col - 1, this->target.edges[0].depth);
		if (_canAttack(this->target.edges[0].depth + 1, this->target.edges[0].row, this->target.edges[0].col)) return _Coordinate(this->target.edges[0].row, this->target.edges[0].col, this->target.edges[0].depth + 1);
		if (_canAttack(this->target.edges[0].depth - 1, this->target.edges[0].row, this->target.edges[0].col)) return _Coordinate(this->target.edges[0].row, this->target.edges[0].col, this->target.edges[0].depth - 1);
	}
	if (this->target.direction == 1) // horizontal
	{
		if (this->target.edgeReached != 0) // didn't reach the end of target vessel with edge[0]
		{
			if (this->target.edges[0].col > this->target.edges[1].col && _canAttack(this->target.edges[0].depth, this->target.edges[0].row, this->target.edges[0].col + 1))
				return _Coordinate(this->target.edges[0].row, this->target.edges[0].col + 1, this->target.edges[0].depth);
			if (this->target.edges[0].col < this->target.edges[1].col && _canAttack(this->target.edges[0].depth, this->target.edges[0].row, this->target.edges[0].col - 1))
				return _Coordinate(this->target.edges[0].row, this->target.edges[0].col - 1, this->target.edges[0].depth);
			// in the case that an attack cannot be made from this edge, mark it as reached
			this->target.edgeReached = 0;
		}
		if (this->target.edgeReached != 1) // didn't reach the end of target vessel with edge[1]
		{
			if (this->target.edges[1].col > this->target.edges[0].col && _canAttack(this->target.edges[0].depth, this->target.edges[1].row, this->target.edges[1].col + 1))
				return _Coordinate(this->target.edges[1].row, this->target.edges[1].col + 1, this->target.edges[0].depth);
			if (this->target.edges[1].col < this->target.edges[0].col && _canAttack(this->target.edges[0].depth, this->target.edges[1].row, this->target.edges[1].col - 1))
				return _Coordinate(this->target.edges[1].row, this->target.edges[1].col - 1, this->target.edges[0].depth);
			// in the case that an attack cannot be made from this edge, mark it as reached
			this->target.edgeReached = 1;
		}
	}
	else if (this->target.direction == 2) // vertical
	{
		if (this->target.edgeReached != 0) // didn't reach the end of target vessel with edge[0]
		{
			if (this->target.edges[0].row > this->target.edges[1].row && _canAttack(this->target.edges[0].depth, this->target.edges[0].row + 1, this->target.edges[0].col))
				return _Coordinate(this->target.edges[0].row + 1, this->target.edges[0].col, this->target.edges[0].depth);
			if (this->target.edges[0].row < this->target.edges[1].row && _canAttack(this->target.edges[0].depth, this->target.edges[0].row - 1, this->target.edges[0].col))
				return _Coordinate(this->target.edges[0].row - 1, this->target.edges[0].col, this->target.edges[0].depth);
			// in the case that an attack cannot be made from this edge, mark it as reached
			this->target.edgeReached = 0;
		}
		if (this->target.edgeReached != 1) // didn't reach the end of target vessel with edge[1]
		{
			if (this->target.edges[1].row > this->target.edges[0].row && _canAttack(this->target.edges[0].depth, this->target.edges[1].row + 1, this->target.edges[1].col))
				return _Coordinate(this->target.edges[1].row + 1, this->target.edges[1].col, this->target.edges[0].depth);
			if (this->target.edges[1].row < this->target.edges[0].row && _canAttack(this->target.edges[0].depth, this->target.edges[1].row - 1, this->target.edges[1].col))
				return _Coordinate(this->target.edges[1].row - 1, this->target.edges[1].col, this->target.edges[0].depth);
			// in the case that an attack cannot be made from this edge, mark it as reached
			this->target.edgeReached = 1;
		}
	}
	else // this->target.direction == 3 => z/depth axis
	{
		if (this->target.edgeReached != 0) // didn't reach the end of target vessel with edge[0]
		{
			if (this->target.edges[0].depth > this->target.edges[1].depth && _canAttack(this->target.edges[0].depth + 1, this->target.edges[0].row, this->target.edges[0].col))
				return _Coordinate(this->target.edges[0].row, this->target.edges[0].col, this->target.edges[0].depth + 1);
			if (this->target.edges[0].depth < this->target.edges[1].depth && _canAttack(this->target.edges[0].depth - 1, this->target.edges[0].row, this->target.edges[0].col))
				return _Coordinate(this->target.edges[0].row, this->target.edges[0].col, this->target.edges[0].depth - 1);
			// in the case that an attack cannot be made from this edge, mark it as reached
			this->target.edgeReached = 0;
		}
		if (this->target.edgeReached != 1) // didn't reach the end of target vessel with edge[1]
		{
			if (this->target.edges[1].depth > this->target.edges[0].depth && _canAttack(this->target.edges[1].depth + 1, this->target.edges[1].row, this->target.edges[0].col))
				return _Coordinate(this->target.edges[1].row, this->target.edges[0].col, this->target.edges[1].depth + 1);
			if (this->target.edges[1].depth < this->target.edges[0].depth && _canAttack(this->target.edges[1].depth - 1, this->target.edges[1].row, this->target.edges[0].col))
				return _Coordinate(this->target.edges[1].row, this->target.edges[0].col, this->target.edges[1].depth - 1);
			// in the case that an attack cannot be made from this edge, mark it as reached
			this->target.edgeReached = 1;
		}
	}

	// attack the next relevant cell
	for (int z = 0; z < this->playerBoard.depth; z++)
		for (int i = 0; i < this->playerBoard.rows; i++)
			for (int j = 0; j < this->playerBoard.cols; j++)
				if (_canAttack(z, i, j)) return _Coordinate(i, j, z);

	// safety, should never get here
	cout << "BattleshipGameAlgoSmart.attack: Something went wrong, can't find next cell to attack (Player " << this->playerNum << ", currently in " << (this->target.exists ? "TARGET" : "HUNT") << " mode)" << endl;
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

		_markIrrelevant(depth ,row, col);

		switch (result) {
		case AttackResult::Miss:
			if (this->target.exists) {
				if (this->target.direction == 1 && (col == this->target.edges[0].col + 1 || col == this->target.edges[0].col - 1)) this->target.edgeReached = 0;
				if (this->target.direction == 1 && (col == this->target.edges[1].col + 1 || col == this->target.edges[1].col - 1)) this->target.edgeReached = 1;
				if (this->target.direction == 2 && (row == this->target.edges[0].row + 1 || row == this->target.edges[0].row - 1)) this->target.edgeReached = 0;
				if (this->target.direction == 2 && (row == this->target.edges[1].row + 1 || row == this->target.edges[1].row - 1)) this->target.edgeReached = 1;
				if (this->target.direction == 3 && (depth == this->target.edges[0].depth + 1 || depth == this->target.edges[0].depth - 1)) this->target.edgeReached = 0;
				if (this->target.direction == 3 && (depth == this->target.edges[1].depth + 1 || depth == this->target.edges[1].depth - 1)) this->target.edgeReached = 1;
			}
			break;
		case AttackResult::Hit:
			if (!this->target.exists)
			{
				this->target.exists = true;
				this->target.edges[0] = Coordinate(row, col, depth);
			}
			else
			{
				if (this->target.edges[1].row == -1 && 
					this->target.edges[1].col == -1 && 
					this->target.edges[1].depth == -1)
				{
					this->target.edges[1] = Coordinate(row, col, depth);
					if (this->target.edges[0].col != this->target.edges[1].col) {
						this->target.direction = 1;
						_markIrrelevant(target.edges[0].depth - 1, this->target.edges[0].row + 1, this->target.edges[0].col);
						_markIrrelevant(target.edges[0].depth + 1, this->target.edges[0].row - 1, this->target.edges[0].col);
						_markIrrelevant(target.edges[0].depth + 1, this->target.edges[0].row + 1, this->target.edges[0].col);
						_markIrrelevant(target.edges[0].depth - 1, this->target.edges[0].row - 1, this->target.edges[0].col);
					}
					else if (this->target.edges[0].row != this->target.edges[1].row)
					{
						this->target.direction = 2;
						_markIrrelevant(target.edges[0].depth + 1, this->target.edges[0].row, this->target.edges[0].col + 1);
						_markIrrelevant(target.edges[0].depth + 1, this->target.edges[0].row, this->target.edges[0].col - 1);
						_markIrrelevant(target.edges[0].depth - 1, this->target.edges[0].row, this->target.edges[0].col + 1);
						_markIrrelevant(target.edges[0].depth - 1, this->target.edges[0].row, this->target.edges[0].col - 1);
					}
					else {
						this->target.direction = 3;
						_markIrrelevant(target.edges[0].depth, this->target.edges[0].row + 1, this->target.edges[0].col + 1);
						_markIrrelevant(target.edges[0].depth, this->target.edges[0].row + 1, this->target.edges[0].col - 1);
						_markIrrelevant(target.edges[0].depth, this->target.edges[0].row - 1, this->target.edges[0].col + 1);
						_markIrrelevant(target.edges[0].depth, this->target.edges[0].row - 1, this->target.edges[0].col - 1);

					}
				}
				else
				{
					if (abs(this->target.edges[1].row - row) + abs(this->target.edges[1].col - col) + abs(this->target.edges[1].depth - depth) == 1) // one cell away from edge[1]
						this->target.edges[1] = Coordinate(row, col, depth);
					else
						this->target.edges[0] = Coordinate(row, col, depth);
				}

				// mark current cell's surrounding cells as irrelevant according to direction
				if (this->target.direction == 1)
				{
					_markIrrelevant(depth + 1, row + 1, col);
					_markIrrelevant(depth + 1, row - 1, col);
					_markIrrelevant(depth - 1, row + 1, col);
					_markIrrelevant(depth - 1, row - 1, col);
				}
				else if (this->target.direction == 2)
				{
					_markIrrelevant(depth + 1, row, col + 1);
					_markIrrelevant(depth + 1, row, col - 1);
					_markIrrelevant(depth - 1, row, col + 1);
					_markIrrelevant(depth - 1, row, col - 1);
				}
				else {
					_markIrrelevant(depth, row + 1, col + 1);
					_markIrrelevant(depth, row + 1, col - 1);
					_markIrrelevant(depth, row - 1, col + 1);
					_markIrrelevant(depth, row - 1, col - 1);
				}

			}
			this->target.hitCount++;
			break;
		case AttackResult::Sink:
			// mark surrounding cells as irrelevant (last hit, can add all directions)
			_markIrrelevant(depth + 1, row + 1, col);
			_markIrrelevant(depth + 1, row - 1, col);
			_markIrrelevant(depth - 1, row + 1, col);
			_markIrrelevant(depth - 1, row - 1, col);
			_markIrrelevant(depth + 1, row, col + 1);
			_markIrrelevant(depth + 1, row, col - 1);
			_markIrrelevant(depth - 1, row, col + 1);
			_markIrrelevant(depth - 1, row, col - 1);
			_markIrrelevant(depth, row + 1, col + 1);
			_markIrrelevant(depth, row + 1, col - 1);
			_markIrrelevant(depth, row - 1, col + 1);
			_markIrrelevant(depth, row - 1, col - 1);

			if (this->target.exists) this->playerBoard.hostileShips[tolower(getShipBySize(this->target.hitCount + 1))]--;
			else this->playerBoard.hostileShips[tolower(getShipBySize(1))]--;

			// back to search mode
			this->target.exists = false; 
			this->target.direction = -1;
			this->target.edgeReached = -1;
			this->target.edges[0] = Coordinate(-1, -1, -1);
			this->target.edges[1] = Coordinate(-1, -1, -1);
			this->target.hitCount = 0;
			break;
		default:
			// safety, should never get here
			cout << "BattleshipGameAlgo.notifyOnAttackResult: Something went wrong, got bad AttackResult" << endl;
			break;
		}
	}
}

