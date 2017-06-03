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

/*
bool BattleshipGameAlgo::_canAttack(int i, int j) const
{
return (i >= 0 && i < this->playerBoard->R && j >= 0 && j < this->playerBoard->C && this->playerBoard->board[i][j] == ' ');
}

void BattleshipGameAlgo::_markIrrelevant(int i, int j) const
{
if (i >= 0 && i < this->playerBoard->R && j >= 0 && j < this->playerBoard->C)
this->playerBoard->board[i][j] = irrelevnatCell;
}
*/


void BattleshipGameAlgo::setBoard(const BoardData& board)
{
	/*
	this->playerNum = player;
	delete this->playerBoard; //avoid memory leak
	this->playerBoard = new BattleBoard(board, numRows, numCols);
	this->hostileShipsNum = this->playerBoard->playerToolsNum;
	*/
	
}

/*
bool BattleshipGameAlgo::init(const std::string& path)
{
set<pair<int, int>> irrelevantCells;
for (int i = 0; i < this->playerBoard->R; i++)
{
for (int j = 0; j < this->playerBoard->C; j++)
{

if (this->playerBoard->board[i][j] != ' ') {
if (i + 1 < this->playerBoard->R && this->playerBoard->board[i + 1][j] == ' ') irrelevantCells.insert(make_pair(i + 1, j));
if (i > 0 && this->playerBoard->board[i - 1][j] == ' ') irrelevantCells.insert(make_pair(i - 1, j));
if (j + 1 < this->playerBoard->C && this->playerBoard->board[i][j + 1] == ' ') irrelevantCells.insert(make_pair(i, j + 1));
if (j > 0 && this->playerBoard->board[i][j - 1] == ' ') irrelevantCells.insert(make_pair(i, j - 1));
}
}
}
for (auto const& cell : irrelevantCells)
this->_markIrrelevant(cell.first, cell.second);
return true;
}

pair<int, int> BattleshipGameAlgo::_getBestGuess() const
{
vector<vector<int>> scoreBoard(this->playerBoard->R, vector<int>(this->playerBoard->C));
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
return make_pair(-1, -1);
}

*/



Coordinate  BattleshipGameAlgo::attack()
{
	return Coordinate(0, 0, 0);
	/*
	// search mode - trying to find opponent ship
	if (this->target == nullptr)
	return _getBestGuess();

	// target mode - targeting a specific ship
	if (this->target->direction == -1) // don't know direction yet
	{
	if (this->_canAttack(this->target->edges[0].first - 1, this->target->edges[0].second)) return _make_pair(this->target->edges[0].first - 1, this->target->edges[0].second);
	if (this->_canAttack(this->target->edges[0].first + 1, this->target->edges[0].second)) return _make_pair(this->target->edges[0].first + 1, this->target->edges[0].second);
	if (this->_canAttack(this->target->edges[0].first, this->target->edges[0].second + 1)) return _make_pair(this->target->edges[0].first, this->target->edges[0].second + 1);
	if (this->_canAttack(this->target->edges[0].first, this->target->edges[0].second - 1)) return _make_pair(this->target->edges[0].first, this->target->edges[0].second - 1);
	}
	if (this->target->direction == 0) // horizontal
	{
	if (this->target->edgeReached != 0) // didn't reach the end of target vessel with edge[0]
	{
	if (this->target->edges[0].second > this->target->edges[1].second && this->_canAttack(this->target->edges[0].first, this->target->edges[0].second + 1))
	return _make_pair(this->target->edges[0].first, this->target->edges[0].second + 1);
	if (this->target->edges[0].second < this->target->edges[1].second && this->_canAttack(this->target->edges[0].first, this->target->edges[0].second - 1))
	return _make_pair(this->target->edges[0].first, this->target->edges[0].second - 1);
	// in the case that an attack cannot be made from this edge, mark it as reached
	this->target->edgeReached = 0;
	}
	if (this->target->edgeReached != 1) // didn't reach the end of target vessel with edge[1]
	{
	if (this->target->edges[1].second > this->target->edges[0].second && this->_canAttack(this->target->edges[1].first, this->target->edges[1].second + 1))
	return _make_pair(this->target->edges[1].first, this->target->edges[1].second + 1);
	if (this->target->edges[1].second < this->target->edges[0].second && this->_canAttack(this->target->edges[1].first, this->target->edges[1].second - 1))
	return _make_pair(this->target->edges[1].first, this->target->edges[1].second - 1);
	// in the case that an attack cannot be made from this edge, mark it as reached
	this->target->edgeReached = 1;
	}
	}
	else // this->target->direction == 1, vertical
	{
	if (this->target->edgeReached != 0) // didn't reach the end of target vessel with edge[0]
	{
	if (this->target->edges[0].first > this->target->edges[1].first && this->_canAttack(this->target->edges[0].first + 1, this->target->edges[0].second))
	return _make_pair(this->target->edges[0].first + 1, this->target->edges[0].second);
	if (this->target->edges[0].first < this->target->edges[1].first && this->_canAttack(this->target->edges[0].first - 1, this->target->edges[0].second))
	return _make_pair(this->target->edges[0].first - 1, this->target->edges[0].second - 1);
	// in the case that an attack cannot be made from this edge, mark it as reached
	this->target->edgeReached = 0;
	}
	if (this->target->edgeReached != 1) // didn't reach the end of target vessel with edge[1]
	{
	if (this->target->edges[1].first > this->target->edges[0].first && this->_canAttack(this->target->edges[1].first + 1, this->target->edges[1].second))
	return _make_pair(this->target->edges[1].first + 1, this->target->edges[1].second);
	if (this->target->edges[1].first < this->target->edges[0].first && this->_canAttack(this->target->edges[1].first - 1, this->target->edges[1].second))
	return _make_pair(this->target->edges[1].first - 1, this->target->edges[1].second);
	// in the case that an attack cannot be made from this edge, mark it as reached
	this->target->edgeReached = 1;
	}
	}

	// safety, should never get here
	cout << "BattleshipGameAlgoSmart.attack: Something went wrong, can't find next cell to attack (Player " << this->playerNum << ", currently in " << (this->target == nullptr ? "HUNT" : "TARGET") << " mode)" << endl;
	return make_pair(-1, -1);
	*/
	
}

void BattleshipGameAlgo::notifyOnAttackResult(int player, Coordinate move, AttackResult result)
{
	/*
	row--;
	col--;
	if (player == this->playerNum)
	{
	this->_markIrrelevant(row, col);
	switch (result) {
	case AttackResult::Miss:
	if (this->target != nullptr)
	{
	if (this->target->direction == 0 && (col == this->target->edges[0].second + 1 || col == this->target->edges[0].second - 1)) this->target->edgeReached = 0;
	if (this->target->direction == 0 && (col == this->target->edges[1].second + 1 || col == this->target->edges[1].second - 1)) this->target->edgeReached = 1;
	if (this->target->direction == 1 && (col == this->target->edges[0].first + 1 || col == this->target->edges[0].first - 1)) this->target->edgeReached = 0;
	if (this->target->direction == 1 && (col == this->target->edges[1].first + 1 || col == this->target->edges[1].first - 1)) this->target->edgeReached = 1;
	}
	break;
	case AttackResult::Hit:
	if (this->target == nullptr)
	{
	this->target = new targetVessel();
	this->target->edges[0] = make_pair(row, col);
	this->target->edges[1] = make_pair(-1, -1);
	}
	else
	{
	if (this->target->edges[1] == make_pair(-1, -1))
	{
	this->target->edges[1] = make_pair(row, col);
	this->target->direction = this->target->edges[0].second == this->target->edges[1].second;

	// mark edge[0]'s surrounding cells as irrelevant according to direction
	if (this->target->direction == 0)
	{
	this->_markIrrelevant(this->target->edges[0].first + 1, this->target->edges[0].second);
	this->_markIrrelevant(this->target->edges[0].first - 1, this->target->edges[0].second);
	}
	else
	{
	this->_markIrrelevant(this->target->edges[0].first, this->target->edges[0].second + 1);
	this->_markIrrelevant(this->target->edges[0].first, this->target->edges[0].second - 1);
	}

	}
	else
	{
	if (abs(this->target->edges[1].first - row) + abs(this->target->edges[1].second - col) == 1) // one cell away from edge[1]
	this->target->edges[1] = make_pair(row, col);
	else
	this->target->edges[0] = make_pair(row, col);
	}

	// mark current cell's surrounding cells as irrelevant according to direction
	if (this->target->direction == 0)
	{
	this->_markIrrelevant(row + 1, col);
	this->_markIrrelevant(row - 1, col);
	}
	else
	{
	this->_markIrrelevant(row, col + 1);
	this->_markIrrelevant(row, col - 1);
	}
	}
	break;
	case AttackResult::Sink:
	// mark surrounding cells as irrelevant (last hit, can add all directions)
	this->_markIrrelevant(row + 1, col);
	this->_markIrrelevant(row - 1, col);
	this->_markIrrelevant(row, col + 1);
	this->_markIrrelevant(row, col - 1);

	delete this->target;
	this->target = nullptr; // back to search mode
	this->hostileShipsNum -= 1;
	break;
	default:
	// safety, should never get here
	cout << "BattleshipGameAlgo.notifyOnAttackResult: Something went wrong, got bad AttackResult" << endl;
	break;
	}
	}
	*/
	
}

