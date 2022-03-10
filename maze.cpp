#include "maze.h"

// Algoritmul Aldous-Broder pentru generarea labirintului
using namespace std;

Maze::Maze() {};

Maze::Maze(int h, int w, int exits) {
	this->h = h;
	this->w = w;
	this->exits = exits;
	H = 2 * h + 1;
	W = 2 * h + 1;

	visited = 0;
	maze = vector<vector<int>>(H, vector<int>(W, 1));
}

void Maze::set(int r, int c, int val) {
	maze[r][c] = val;
}

int Maze::IsNearToExitorExit(int r, int c) {
	for (auto& e : exitVec) {
		if (e.first == r && e.second == c) {
			return 1;
		}
		else {
			return 2;
		}

		if (abs(e.first - r) <= 1 || abs(e.second - c) <= 1) {
			return 3;
		}
		else {
			return 4;
		}
	}
	return 0;
}

vector<pair<int, int>> Maze::FindNeigh(int r, int c, int isWall) {
	vector<pair<int, int>> ns;
	if (r > 1 && maze[r - 2][c] == isWall) {
		ns.push_back({ r - 2, c });
	}
	if (r < (H - 2) && maze[r + 2][c] == isWall) {
		ns.push_back({ r + 2, c });
	}
	if (c > 1 && maze[r][c - 2] == isWall) {
		ns.push_back({ r, c - 2 });
	}
	if (c < (W - 2) && maze[r][c + 2] == isWall) {
		ns.push_back({ r, c + 2 });
	}

	shuffle(begin(ns), end(ns), rng);
	return ns;

}

void Maze::generate() {

	srand((unsigned)time(NULL));
	int crow = 2 * (rand() % h) + 1;
	int ccol = 2 * (rand() % w) + 1;
	maze[crow][ccol] = 0;
	visited++;

	while (visited < h * w) {
		vector<pair<int, int>> neighbours = FindNeigh(crow, ccol, 1);

		if (neighbours.size() == 0) {
			vector<pair<int, int>> cells = FindNeigh(crow, ccol, 0);
			int i = rand() % cells.size();

			crow = cells[i].first;
			ccol = cells[i].second;
		}
		
		for (pair<int, int> pairs : neighbours) {
			int nrow = pairs.first;
			int ncol = pairs.second;

			if (maze[nrow][ncol] == 1) {
				maze[(nrow + crow) / 2][(ncol + ccol) / 2] = 0;
				maze[nrow][ncol] = 0;

				if (exits > 0) {
					if (nrow == 1 && !IsNearToExitorExit(0, ncol)) {
						maze[0][ncol] = 0;
						exitVec.push_back({ 0, ncol });
						exits--;
					}
					else if (nrow == H - 2 && !IsNearToExitorExit(H - 1, ncol)) {
						maze[H - 1][ncol] = 0;
						exitVec.push_back({ H - 1, ncol });
						exits--;
					}
					else if (ncol == 1 && !IsNearToExitorExit(nrow, 0)) {
						maze[nrow][0] = 0;
						exitVec.push_back({ nrow, 0 });
						exits--;
					}
					if (ncol == W - 2 && !IsNearToExitorExit(nrow, W - 1)) {
						maze[nrow][W - 1] = 0;
						exitVec.push_back({ nrow, W - 1 });
						exits--;
					}
				}
			}

			visited++;
			crow = nrow;
			ccol = ncol;
			break;
		}
	}
}

Maze::~Maze() {}