#include "GamesManager.h"
#include <memory>


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

bool loadAlgoDllsCheckBoards(string folder, vector<string> dllfiles, vector<string> sboardfiles,
	, vector<HINSTANCE>& dllLoaded)
{
	return true;
}

int manageGames(vector<string> dllFiles, vector<string> sboardFiles, int threads)
{
	vector<HINSTANCE> dllLoaded;
	vector<IBattleshipGameAlgo> playersInstances;
	//load dll's
	vector<GetAlgorithmFuncType> algorithmFuncs;

	HINSTANCE hDll;
	for (auto dll : dllFiles)
	{
		hDll = LoadLibraryA(dll.c_str());
		if (hDll)
		{
			dllLoaded.push_back(hDll);
			GetAlgorithmFuncType algoF = GetAlgorithmFuncType(GetProcAddress(hDll, "GetAlgorithm"));
			if (algoF)
				algorithmFuncs.push_back(algoF);
			//make unique ptr ;
			unique_ptr<IBattleshipGameAlgo> = make_unique<IBattleshipGameAlgo>(algorithmFuncs[0]());
			
		}
	}
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