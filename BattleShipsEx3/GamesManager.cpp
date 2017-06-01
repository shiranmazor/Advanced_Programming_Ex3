#include "GamesManager.h"
#include <memory>
#include "BattleBoard.h"


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
void getGameFiles(string folder, vector<string> & sboardFiles, vector<string> & dllFiles)
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
			}
		} while (FindNextFileA(handle, &search_data));

	}
}

bool loadAlgoDllsCheckBoards(vector<string> dllfiles, vector<string> sboardfiles,
	 vector<HINSTANCE>& dllLoaded, vector<GetAlgorithmFuncType>& algorithmFuncs, vector<shared_ptr<BattleBoard>>& boards)
{
	vector<string>  errors;
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
		if (board->isBoardValid(errors))
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

int manageGames(vector<string> dllFiles, vector<string> sboardFiles, int threads)
{;
	vector<HINSTANCE> dllLoaded;
	//load dll's
	vector<GetAlgorithmFuncType> algorithmFuncs;
	vector<shared_ptr<BattleBoard>> boards;

	if (!loadAlgoDllsCheckBoards(dllFiles, sboardFiles, dllLoaded, algorithmFuncs, boards))
		return -1;

	//create game combinations
	//first create a map of player unique number to algo name
	map<int, GetAlgorithmFuncType> playersAlgo;
	int i = 0;
	for(auto algo : algorithmFuncs)
	{
		playersAlgo.insert(make_pair(i, algo));
		i++;
	}
		

	//finish manage games release dlls
	closeDLLs(dllLoaded);

}

void getGameCombinations(map<int, GetAlgorithmFuncType> playersAlgo, vector<shared_ptr<BattleBoard>> boards)
{
	
}
/*
 *algorithmFuncs c
 */
unique_ptr<GameResult> playSingleGame(pair<string,string> dllNames,GetAlgorithmFuncType algorithmFuncs1, GetAlgorithmFuncType algorithmFuncs2,
	shared_ptr<BattleBoard> board)
{
	//create players instance
	unique_ptr<IBattleshipGameAlgo> playerA(algorithmFuncs1());
	unique_ptr<IBattleshipGameAlgo> playerB(algorithmFuncs2());
	unique_ptr<GameResult> result = make_unique<GameResult>(get<0>(dllNames), get<1>(dllNames));

	playerA->setPlayer(A);
	playerB->setPlayer(B);
	
	//Todo: check for any potential memory leak in this lines
	playerA->setBoard(board->getPlayerBoard(A));
	playerA->setBoard(board->getPlayerBoard(B));

	//we starts with player A
	Player currentPlayer = A;
	int onePlayerName = -1;
	bool onePlayerGame = false; 
	bool victory = false; int winPlayer = 2;
	Coordinate attackMove(0, 0, 0);

	while (!victory)
	{
		//set current player board
		if (currentPlayer == A)
			attackMove = playerA->attack();
		else
			attackMove = playerA->attack();
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
		/*
		 *else if //check if attack move coordinate is bad
		{
				
		}
		 */
		AttackResult moveRes = board->performGameMove(currentPlayer, attackMove);
		//notify both players on the moveAttak results
		playerA->notifyOnAttackResult(currentPlayer, attackMove, moveRes);
		playerB->notifyOnAttackResult(currentPlayer, attackMove, moveRes);
		//check victory:
		winPlayer = board->CheckVictory();
		if (winPlayer == A || winPlayer == B)
		{
			victory = true;
			break;
		}

		// if Miss or self hit next turn is of the other player.

		if (moveRes == AttackResult::Miss || (moveRes != AttackResult::Miss &&
			isSelfHit(currentPlayer, board->charAt(attackMove)))
		{
			currentPlayer = currentPlayer == A ? B : A;//swap
		}
	}
	//outside game loop
	pair<int, int> gameScore = board->CalcScore();
	//check victory:
	if (victory)
	{
		result->winPlayer = winPlayer;
	}
	result->playerAScore = get<0>(gameScore);
	result->playerBScore = get<1>(gameScore);
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
	vector<tuple<string, HINSTANCE, GetAlgorithmFuncType>>  dll_vec;
	char the_path[256];

	//setup defult parameters
	_getcwd(the_path, 255);
	path = std::string(the_path);
	map<string, string> configMap = getConfigParams(path);
	threads = stoi(configMap["threads"]);

	//reading params from command line
	int i = 1; //first item is the name of the program
	while (i < argc)
	{
		if (i == 1)//path is apearing only in the first item
			path = argv[i];
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
	if (!dirExists(path))
	{
		std::cout << "Wrong path:" + path << endl;
		return -1;
	}
	//done reading params, get game files
	getGameFiles(path, sboardFiles, dllFiles);
	if (!CheckExistingGameFiles(dllFiles, sboardFiles, path, error_messages))
	{
		//output error messages
		for (int i = 0; i < error_messages.size(); i++)
			cout << error_messages[i] << endl;
		return -1;
	}

}