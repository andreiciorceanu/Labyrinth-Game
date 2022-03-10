#pragma once
#include <string>
#include <random>
#include <iostream>
#include <fstream>
#include <vector>
#include <utility>


class Maze {
public:
	int h, w, exits;
	Maze();
	Maze(int h, int w, int exits);
	~Maze();
	void generate();
	void set(int r, int c, int val);
	int IsNearToExitorExit(int r, int c);
	std::vector<std::pair<int, int>> Maze::FindNeigh(int r, int c, int isWall);
	int H, W;
	int visited;
	std::vector<std::vector<int>> maze;
	std::vector<std::pair<int, int>> exitVec;
	std::default_random_engine rng = std::default_random_engine{};
};