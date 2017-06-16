#pragma once
#include <vector>
#include <sstream>
#include <map>
#include <tuple>
#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include "IBattleshipGameAlgo.h"

#define MyHitMark '*'
#define OpHitMark '#'
#define MyMissMark '~'
#define OpMissMark '^'
#define MySinkMark '!'
#define OpSinkMark '?'

#define _Coordinate(x, y, z) Coordinate(x + 1, y + 1, z + 1)

enum Player { A, B };
using namespace std;

std::vector<std::string> splitString(std::string str, char c);
void replaceAll(string &str, const string &search, const string &replace);
string removeSpaces(string input);
bool dirExists(const std::string& dirName_in);
map<string, string> getConfigParams(string path);

int getShipSize(char C);
int getShipScore(char C);
char getShipBySize(int size);
bool isCharValid(char C);
Coordinate fitCoordinate(Coordinate coor);
int Factorial(int n);
vector<pair<int, int>> PairesPermGenerator(int n);

