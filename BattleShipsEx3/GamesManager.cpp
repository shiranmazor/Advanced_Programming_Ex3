#include "GamesManager.h"
#include <memory>
#include "BattleBoard.h"
#include <thread>
#include <iomanip>

#define COLUMN_SPACE 8
#define TEAM_NAME 45

queue<Game> g_games;
map<int, pair<GetAlgorithmFuncType, string>> g_playersAlgo;
vector<PlayerScore> g_pScores;
int g_gamesCounter;
int g_total_games_size;
int g_each_player_games_num;

mutex g_gameCounter_mutex;
mutex g_playersAlgo_mutex;
mutex g_gamesQueue_mutex;
mutex g_playerScore_mutex;
std::condition_variable g_cond;

void closeDLLs(vector<HINSTANCE> dlls)
{
	// close all the dynamic libs we opened
	for (int i = 0; i< dlls.size(); i++)
		FreeLibrary(dlls[i]);
}

bool CheckExistingGameFiles(vector<string> dllFiles, vector<string> sboardFiles, string path, vector<string>& error_messages)
{
	if (sboardFiles.size() < 1)
		error_messages.push_back("No board files (*.sboard) looking in path: "+path);
	if (dllFiles.size() < 2)
		error_messages.push_back("Missing algorithm (dll) files looking in path:" + path + " (needs at least two)");
	if (error_messages.size() > 0)
		return false;
	return true;
}

/*
game files are:
.sboard files (can be multiple)
*.dll files (can be multiple)
*/
void getGameFiles(string folder, vector<string> & sboardFiles, vector<string> & dllFiles, vector<string>& dllNames)
{
	WIN32_FIND_DATAA search_data;
	HANDLE handle;
	memset(&search_data, 0, sizeof(WIN32_FIND_DATAA));

	//start with sboard files -iterate over *.sboard files in folder
	string sboardPath = folder + "\\*.sboard";
	handle = FindFirstFileA(sboardPath.c_str(), &search_data);

	if (handle != INVALID_HANDLE_VALUE)
	{
		// test each file suffix and set variables as needed
		do
		{
			//check file extension
			string filename(search_data.cFileName);
			if (filename.find("sboard") != std::string::npos)
			{
				//found sboard file
				string fullPath = folder + "\\" + filename;
				sboardFiles.push_back(fullPath);
			}
		} while (FindNextFileA(handle, &search_data));
		
	}

	// iterate over *.dll files in folder
	string dllPath = folder + "\\*.dll";
	handle = FindFirstFileA(dllPath.c_str(), &search_data);
	if (handle != INVALID_HANDLE_VALUE)
	{
		// test each file suffix and set variables as needed
		do
		{
			//check file extension
			string filename(search_data.cFileName);
			if (filename.find("dll") != std::string::npos)
			{
				//found sboard file
				string fullPath = folder + "\\" + filename;
				dllFiles.push_back(fullPath);
				dllNames.push_back(filename);
			}
		} while (FindNextFileA(handle, &search_data));

	}
}

bool loadAlgoDllsCheckBoards(vector<string> dllfiles, vector<string> sboardfiles,
	vector<HINSTANCE>& dllLoaded, vector<GetAlgorithmFuncType>& algorithmFuncs, 
	vector<shared_ptr<BattleBoard>>& boards)
{
	vector<string>  errors;
	vector<string>  boardErrors;
	//load dll's
	HINSTANCE hDll;
	for (auto dll : dllfiles)
	{
		hDll = LoadLibraryA(dll.c_str());
		if (hDll)
		{
			dllLoaded.push_back(hDll);
			GetAlgorithmFuncType algoF = GetAlgorithmFuncType(GetProcAddress(hDll, "GetAlgorithm"));
			if (algoF)
				algorithmFuncs.push_back(algoF);

		}
	}
	if (algorithmFuncs.size() == 0)
		errors.push_back("Error: Faild to load all DLL's! no dll for Game Manager, Exising!");

	//load files, create battleBoard instance and check validity
	for(auto boardF: sboardfiles)
	{
		shared_ptr<BattleBoard> board = make_shared<BattleBoard>(boardF);
		if (board->isBoardValid(boardErrors))
			boards.push_back(board);
	}
	if (boards.size() == 0)
		errors.push_back("Error: all sboard files are Invalid. no board! exising.");

	if (errors.size() > 0)
	{
		for (auto e : errors)
			cout << e << endl;
		return false;
	}

	return true;
}

int manageGames(vector<string> dllFiles,vector<string> dllNames, vector<string> sboardFiles, int threadsNum)
{
	vector<unique_ptr<thread>> threads(threadsNum);
	vector<HINSTANCE> dllLoaded;
	//load dll's
	vector<GetAlgorithmFuncType> algorithmFuncs;
	vector<shared_ptr<BattleBoard>> boards;
	g_gamesCounter = 0;

	if (!loadAlgoDllsCheckBoards(dllFiles, sboardFiles, dllLoaded, algorithmFuncs, boards))
		return -1;

	//print number of legal players and number of legal boards
	cout << "Number of legal players: " << algorithmFuncs.size() << endl;
	cout << "Number of legal boards: " << boards.size()  << endl;

	//first create a map of player unique number to algo name
	int i = 0;
	for (auto algo : algorithmFuncs)
	{
		g_playersAlgo[i+1] = make_pair(algo, dllNames[i]);
		//create players score  for each player
		PlayerScore p(dllNames[i], i);
		g_pScores.push_back(p);
		i++;
	}
	
	//create game combinations  in g_Games queue
	calcGameCombinations(int(g_playersAlgo.size()), int(boards.size()));

	//adjust threads number to the number of games
	if (g_games.size() < threadsNum)
		threadsNum = static_cast<int>(g_games.size());

	//run threads
	for (auto& thread_ptr : threads)
	{
		//create the threads and run them
		thread_ptr = make_unique<thread>(GameThread, boards); // create and run the thread
	}
	//here call function that print the mid results for each round
	ReportResults();
	
	// ===> join all the threads to finish nicely 
	for (auto& thread_ptr : threads) {
		thread_ptr->join();
	}
	//finish manage games release dlls
	closeDLLs(dllLoaded);
	return 0;

}


void ReportResults()
{
	int current_round = 1;

	while(current_round <= g_each_player_games_num)//as long as the Tournament continue we want to check mid results and print them
	{
		//check if all players has done current round
		if (checkRound(current_round))
		{
			cout << "Current Round " << current_round << endl;
			//print game counter
			g_gameCounter_mutex.lock();
			
			std::cout << endl;
			std::cout << "---- Games Played until now (these results): " << g_gamesCounter << "/" << g_total_games_size << endl;
			std::cout << endl;
			//create vector<string,playerRoundScore>
			vector<pair<string, PlayerRoundScore>> currentScores;
			//create middle results object for all players
			g_playerScore_mutex.lock();
			for(auto ps: g_pScores)
			{
				currentScores.push_back(make_pair(ps.playerName, ps.rounds[current_round - 1]));
				
			}
			g_playerScore_mutex.unlock();
			g_gameCounter_mutex.unlock();
			//sort rounds by player win rate
			sort(currentScores.begin(), currentScores.end(), sortPlayersScoreByWinRate());
			
			//print round results:
			//first line
			std::cout << std::setprecision(2) << std::fixed;
			cout << setw(COLUMN_SPACE) << left << "#";
			cout << setw(TEAM_NAME) << left << "Team Name";
			cout << setw(COLUMN_SPACE) << left << "Wins";
			cout << setw(COLUMN_SPACE) << left << "Losses";
			cout << setw(COLUMN_SPACE) << left << "%";
			cout << setw(COLUMN_SPACE) << left << "Pts For";
			cout << setw(COLUMN_SPACE) << left << "Pts Againts" << endl;
			int num = 1;
			for (auto currScore: currentScores)
			{
				string curr_num = std::to_string(num) + ".";
				cout << setw(COLUMN_SPACE) << left << curr_num;
				cout << setw(TEAM_NAME) << left << currScore.first;
				cout << setw(COLUMN_SPACE) << left << currScore.second.wins;
				cout << setw(COLUMN_SPACE) << left << currScore.second.losses;
				cout << setw(COLUMN_SPACE) << left << currScore.second.winRate;
				cout << setw(COLUMN_SPACE) << left << currScore.second.pointsFor;
				cout << setw(COLUMN_SPACE) << left << currScore.second.pointsAgainst << endl;
				num++;
			}
			//only if round happend continue to next round
			current_round++;
			
		}
		//end if
		
	}
}

bool checkRound(int roundNumber)
{
	lock_guard<std::mutex> lock(g_playerScore_mutex);
	for (auto pScor: g_pScores)
	{
		if (pScor.rounds.size() < roundNumber)
			return false;
	}
	return true;
}

/*
 *calculate the game rounds and attach players number to each game
 *return games map < gameNumber(int), value = tuple(board num, playerA number, player B number)>
 */
void  calcGameCombinations(int playersNum, int boardsNumber)
{
	g_each_player_games_num = 2 * boardsNumber* (playersNum - 1);
	int gamesCounter = 1;
	//calc all players permutations
	vector<pair<int, int>> playersPermutations = PairesPermGenerator(playersNum);
	//scan all boards, attach players to each board
	
	for(int boardNum = 1; boardNum <= boardsNumber; boardNum++)
	{
		
		for(auto permutation: playersPermutations)
		{
			Game gameObj(gamesCounter, boardNum, permutation.first, permutation.second);
			g_games.push(gameObj);
			gamesCounter++;
		}

	}
	g_total_games_size = gamesCounter-1;
}
/*
 *playSingleGame : dllNames include playerA dll name and playerB dll name
 */

Game getCurrentGame()
{
	Game g(-1, -1, -1, -1);
	lock_guard<std::mutex> lock(g_gamesQueue_mutex);
	if (g_games.empty())
		return g;

	Game currGame = g_games.front();
	g_games.pop();
	return currGame;	
		
}
void IncreaseGameCounter()
{
	lock_guard<std::mutex> lock(g_gameCounter_mutex);
	g_gamesCounter++;
}

bool isTournamentDone()
{
	lock_guard<std::mutex> lock(g_gameCounter_mutex);
	return g_gamesCounter == g_total_games_size;
}

/*
 * run in single thread and call playSingleGame
 * update gameResults
 * 
 */
void GameThread(vector<shared_ptr<BattleBoard>> boards)
{
	//get unique next current Game obj from queue
	while(!isTournamentDone())
	{
		Game currentGame = getCurrentGame();

		//check if games is over
		if (currentGame.gameNumber == -1)
			return;
		//build new game objects
		g_playersAlgo_mutex.lock();
		pair<GetAlgorithmFuncType, string> playerA = g_playersAlgo.at(currentGame.playerANumber);
		pair<GetAlgorithmFuncType, string> playerB = g_playersAlgo.at(currentGame.playerBNumber);
		g_playersAlgo_mutex.unlock();


		GameResult result = playSingleGame(playerA, playerB, boards, currentGame.boardNumber);
		//update players scores with updateGameResult
		updateGameResult(result);
		IncreaseGameCounter();
		
	}

}

void updateGameResult(GameResult result)
{	
	//find relevet playerA, playerB name from g_pScores, update his data
	int i = 0; 
	int playerBIndex = 0; 
	int playerAIndex = 0;
	//let's update scores
	lock_guard<std::mutex> lock(g_playerScore_mutex);
	while (i < g_pScores.size())
	{
		if (g_pScores[i].playerName == result.playerA)
			playerAIndex = i;
		if (g_pScores[i].playerName == result.playerB)
			playerBIndex = i;
		i++;
	}
	
	//A:
	g_pScores[playerAIndex].UpdateScore(result.winPlayer == 0, result.playerAScore, result.playerBScore, result.winPlayer == -1);
	//B:
	g_pScores[playerBIndex].UpdateScore(result.winPlayer == 1, result.playerBScore, result.playerAScore, result.winPlayer == -1);
	
}


GameResult playSingleGame(pair<GetAlgorithmFuncType, string> playerAPair, pair<GetAlgorithmFuncType, string> playerBPair,
	vector<shared_ptr<BattleBoard>> const boards, int curentBoardNum)
{
	//create players instance
	unique_ptr<IBattleshipGameAlgo> playerA(playerAPair.first());
	unique_ptr<IBattleshipGameAlgo> playerB(playerBPair.first());
	BattleBoard board(*(boards[curentBoardNum - 1].get()));
	
	playerA->setPlayer(A);
	playerB->setPlayer(B);
	
	playerA->setBoard(board.getPlayerBoard(A));
	playerB->setBoard(board.getPlayerBoard(B));

	//we starts with player A
	Coordinate attackMove(1, 1, 1);
	Player currentPlayer = A;
	int onePlayerName = -1;
	bool onePlayerGame = false; 
	bool victory = false; int winPlayer = -1;
	

	while (!victory)
	{
		if (currentPlayer == B)
		{
			attackMove = playerB->attack();
		}
		else
		{
			attackMove = playerA->attack();
		}
			
		//check if attack is over:
		if (attackMove.col == -1 && attackMove.depth == -1 && attackMove.row == -1)
		{
			if (onePlayerGame && onePlayerName == currentPlayer)
			{
				//exit while loop
				break;
			}
			else if (!onePlayerGame)
			{
				onePlayerGame = true;
				currentPlayer = currentPlayer == A ? B : A;//swap
				onePlayerName = currentPlayer;
			}
			else
			{
				currentPlayer = currentPlayer == A ? B : A;//swap
			}
			continue;
		}

		AttackResult moveRes = board.performGameMove(currentPlayer, attackMove);
		//notify both players on the moveAttak results
		playerA->notifyOnAttackResult(currentPlayer, attackMove, moveRes);
		playerB->notifyOnAttackResult(currentPlayer, attackMove, moveRes);
		//check victory:
		winPlayer = board.CheckVictory();
		if (winPlayer == A || winPlayer == B)
		{
			victory = true;
			break;
		}

		// if Miss or self hit next turn is of the other player. in case there are 2  players
		if (moveRes == AttackResult::Miss || (isSelfHit(currentPlayer, board.charAt(attackMove)) && !onePlayerGame)
		{
			currentPlayer = currentPlayer == A ? B : A;//swap
		}
	}
	//outside game loop
	pair<int, int> gameScore = board.CalcScore();

	GameResult result(playerAPair.second, playerBPair.second);
	result.winPlayer = winPlayer;
	result.playerAScore = gameScore.first;
	result.playerBScore = gameScore.second;
	return result;
}

/*
arguments: path [optional] -threads <num> [optional]
in main we get arguments, get dll and sboard files, calc number of games and then runs the gamesManager
*/
int main(int argc, char **argv)
{
	string path; int threads;
	vector<string> error_messages;
	vector<string> sboardFiles;
	vector<string> dllFiles;
	vector<string> dllNames;
	vector<tuple<string, HINSTANCE, GetAlgorithmFuncType>>  dll_vec;
	char the_path[256];

	//setup defult parameters
	_getcwd(the_path, 1024);
	path = std::string(the_path);
	map<string, string> configMap;
	
	//reading params from command line
	int i = 1; //first item is the name of the program
	while (i < argc)
	{
		if (i == 1)//path is apearing only in the first item
		{
			path = argv[i];
			if (!dirExists(path))
			{
				std::cout << "Wrong path:" + path << endl;
				return -1;
			}
			configMap = getConfigParams(path);
			threads = stoi(configMap["threads"]);//set default threade number
		}
		else
		{
			if (strcmp(argv[i], "-threads") == 0)
			{
				threads = atoi(argv[i + 1]);
				i++;
			}
		}
		i++;
	}
	//done reading params, get game files
	getGameFiles(path, sboardFiles, dllFiles, dllNames);

	if (!CheckExistingGameFiles(dllFiles, sboardFiles, path, error_messages))
	{
		//output error messages
		for (int i = 0; i < error_messages.size(); i++)
			cout << error_messages[i] << endl;
		return -1;
	}
	//call manageGames
	manageGames(dllFiles, dllNames, sboardFiles, threads);
	return 0;

}