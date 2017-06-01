#include "GamesManager.h"


IBattleshipGameAlgo* swapPlayer(IBattleshipGameAlgo* current, IBattleshipGameAlgo* pA,
	IBattleshipGameAlgo* pB, int currentName)
{
	return currentName == A ? pB : pA;
}

bool dirExists(const std::string& dirName_in)
{
	DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;  //something is wrong with your path!

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;   // this is a directory!

	return false;    // this is not a directory!
}

void closeDLLs(vector<HINSTANCE> dlls)
{
	// close all the dynamic libs we opened
	for (int i = 0; i< dlls.size(); i++)
		FreeLibrary(dlls[i]);
}

bool CheckExistingDlls(vector<string> dllFiles, string path, vector<string>& error_messages)
{
	int dllCounter = 0;

	for (int i = 0; i < dllFiles.size(); i++)
	{
		if (dllFiles[i].find("dll") != std::string::npos)
			dllCounter++;
	}
	if (dllCounter < 2)//need at least 2 dlls!!
	{
		error_messages.push_back("Missing an algorithm (dll) file looking in path:" + path);
		return false;
	}
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

bool loadAlgoDllAndInitGame(string folder, vector<string> dllfiles, vector<string> sboardfiles, BoardData* mainBoard,
	tuple<IBattleshipGameAlgo*, IBattleshipGameAlgo*>& players, vector<HINSTANCE>& dllLoaded)
{
	return true;
}
/*
arguments: path [optional] -threads[optional]
in main we get arguments, get dll and sboard files, calc number of games and then runs the gamesManager
*/
int main(int argc, char **argv)
{
	string path; int threads;
	vector<string> error_messages;
	vector<tuple<string, HINSTANCE, GetAlgorithmFuncType>>  dll_vec;
	char the_path[256];

	//setup defult parameters
	_getcwd(the_path, 255);
	path = std::string(the_path);
}