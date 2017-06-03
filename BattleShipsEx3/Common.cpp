#include "Common.h"
#include <algorithm>
#include <ctype.h>
#include <numeric>


std::vector<std::string> splitString(std::string str, char c)
{
	std::stringstream test(str);
	std::string segment;
	std::vector<std::string> seglist;

	while (std::getline(test, segment, c))
	{
		seglist.push_back(segment);
	}
	return seglist;
}

void replaceAll(string &str, const string &search, const string &replace)
{
	for (size_t pos = 0; ; pos += replace.length())
	{
		// Locate the substring to replace
		pos = str.find(search, pos);
		if (pos == string::npos) break;
		// Replace by erasing and inserting
		str.erase(pos, search.length());
		str.insert(pos, replace);
	}
}

string removeSpaces(string input)
{
	input.erase(remove_if(input.begin(), input.end(), isspace), input.end());
	return input;
}

int getShipSize(char C)
{
	if (tolower(C) == 'b') return 1;
	if (tolower(C) == 'p') return 2;
	if (tolower(C) == 'm') return 3;
	if (tolower(C) == 'd') return 4;
	return 0;//default case
}

int getShipScore(char C)
{
	if (tolower(C) == 'b') return 2;
	if (tolower(C) == 'p') return 3;
	if (tolower(C) == 'm') return 7;
	if (tolower(C) == 'd') return 8;
	return 0;//default case
}

bool isCharValid(char C)
{
	return tolower(C) == 'b' || tolower(C) == 'p' || tolower(C) == 'm' || tolower(C) == 'd';
}

Coordinate fitCoordinate(Coordinate coor)
{
	return Coordinate(coor.row - 1, coor.col - 1, coor.depth - 1);
}

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

/*
find the first *.config file in the working folder
extract defualt params
config file format:
param=value
*/
map<string, string> getConfigParams(string path)
{
	map<string, string> result;
	WIN32_FIND_DATAA search_data;
	HANDLE handle;
	string fullconfigPath;
	memset(&search_data, 0, sizeof(WIN32_FIND_DATAA));
	
	string configPath = path + "\\*.config";
	handle = FindFirstFileA(configPath.c_str(), &search_data);

	if (handle != INVALID_HANDLE_VALUE)
	{
		//check file extension
		string filename(search_data.cFileName);
		if (filename.find("config") != std::string::npos)
		{
			//found attack file
			fullconfigPath = path + "\\" + filename;
		}

		//open the file and read params:
		ifstream configFile;
		configFile.open(fullconfigPath);
		if (!configFile.fail())
		{
			string line;
			while (getline(configFile, line))
			{
				line = removeSpaces(line);
				//output the config params
				vector<string> splitLine = splitString(line, '=');
				result.insert(make_pair(splitLine[0], splitLine[1]));
			}
			
			configFile.close();
		}
	}

	return result;
}

int Factorial(int n)
{
	int result = 1;
	while (n>1) {
		result *= n--;
	}
	return result;
}

/*
 *chooses n from 2
 */
vector<pair<int,int>> PairesPermGenerator(int n)
{
	vector<pair<int, int>> permutations;
	std::vector<int> d(n);
	std::iota(d.begin(), d.end(), 1);
	
	int repeat = Factorial(n - 2);
	do
	{
		permutations.push_back(make_pair(d[0], d[1]));
		
		
		for (int i = 1; i != repeat; ++i)
		{
			next_permutation(d.begin(), d.end());
		}
	} while (next_permutation(d.begin(), d.end()));


	return permutations;
}