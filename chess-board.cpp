#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <algorithm>

using namespace std;

// NOTES / THOUGHT PROCESS:
// I don't think I can come up with an algorithm that only ever looks at one square at a time
// in order to determine the water level of a single square, while only being able to look at
// the water level of neighbouring squares.
// There are still some cases where these algorithms fail, because they fail to update according
// to the water level of squares on the complete opposite side of the board!

// NEW IDEA! 💡
// What if I WAS the water?? 🌊
// Instead of trying to determine the water level for a single square, what if I put a single drop
// of water on a given square, and followed it as it flowed down the board?

// I came up with a simple flowchart describing my algorithm:
// https://imgur.com/SUQM0Lo

// It will be possible for water to get stuck in certain places it shouldn't be...
// Because I don't allow water to flow back to squares it has already visited in that iteration,
// it will be possible to essentially back itself into a corner, and get stuck there.
// But I don't think it will ever be possible to get stuck in a place where it's meant to flow out,
// so it should be possible to clean up the board in another pass.

// 2nd pass algorithm:
// https://imgur.com/QduFdzB

// ----------------------------------------------------------------------------

// It actually seems to be working! 🎉
// It would be nice to create some test cases to pass it with, but manually testing the random boards I haven't been able to break it yet...
// This was a really fun challenge, and I deeply enjoyed the process of coming up with the algorithm and implementing it 😃
// It was also a great opportunity to learn C++ for the first time! 🧠

// ASSUMPTIONS
// - The board is a rectangle
// - The board squares are true squares (i.e. all sides are the same length)
// - The board size is a standard chess board: 8x8 (This can be customized during board creation)
// - The square size is a a simple 1x1 inch (This can be customized during board creation)
// - Theoretically the square heights could be infinite,
//   but I keep them capped at 10 for simplicity/readability (capped at 100 for complex random boards)


#define ASSERT_EQUAL(expected, actual)                                                       \
	if ((expected) != (actual))                                                              \
	{                                                                                        \
		std::cerr << "❌  FAIL: Expected " << (expected) << ", but got " << (actual) << endl; \
	}                                                                                        \
	else                                                                                     \
	{                                                                                        \
		cout << "✅  PASS" << endl;                                                           \
	}

/**
 * Class to represent a single square on the board
 */
class Square
{
public:
	int row;
	int col;
	float height;
	bool isTouched;
	bool isEdge;
	float waterLevel;

	/**
	 * Width of the square in inches
	 */
	float width = 1;

	Square(float height, int row, int col, bool isEdge, float width = 1)
		: row(row), col(col), height(height), isEdge(isEdge), width(width)
	{
		waterLevel = 0;
		isTouched = false;
	}

	float totalHeight() const
	{
		return height + waterLevel;
	}
};

/**
 * Class to represent the board
 */
class Board
{
public:
	int rows;
	int cols;
	vector<vector<Square>> grid;

	/**
	 * Create a new board with random heights
	 * @param rows number of columns
	 * @param cols number of rows
	 * @return Board * new board
	 */
	Board(int rows, int cols, bool useFloat = false, float width = 1) : rows(rows), cols(cols)
	{
		for (int i = 0; i < rows; i++)
		{
			vector<Square> row;
			for (int j = 0; j < cols; j++)
			{
				bool isEdge = (i == 0 || i == rows - 1 || j == 0 || j == cols - 1);
				if (useFloat)
				{
					float randomNum = (float)rand() / (float)RAND_MAX * 100;
					row.push_back(Square(randomNum, i, j, isEdge, width));
				}
				else
				{
					row.push_back(Square(rand() % 10, i, j, isEdge, width));
				}
			}
			grid.push_back(row);
		}
	}

	/**
	 * Create a new board with given heights
	 * @param heights 2D array of heights
	 * @return Board * new board
	 */
	Board(const vector<vector<float>> &heights)
	{
		rows = heights.size();
		cols = heights[0].size();

		for (int i = 0; i < rows; i++)
		{
			vector<Square> row;
			for (int j = 0; j < cols; j++)
			{
				bool isEdge = (i == 0 || i == rows - 1 || j == 0 || j == cols - 1);
				row.push_back(Square(heights[i][j], i, j, isEdge));
			}
			grid.push_back(row);
		}
	}

	/**
	 * Return array of non-edge squares
	 * @return vector<Square *> non-edge squares
	 */
	vector<Square *> &getNonEdgeSquares()
	{
		vector<Square *> &nonEdgeSquares = *new vector<Square *>;
		for (auto &row : grid)
		{
			for (auto &square : row)
			{
				if (!square.isEdge)
				{
					nonEdgeSquares.push_back(&square);
				}
			}
		}
		return nonEdgeSquares;
	}

	/**
	 * Return array of squares that are neighbours of the given square
	 * @param square square to find all neighbours of
	 * @return vector<Square *> neighbours of square (up, down, left, right)
	 */
	vector<Square *> &getNeighbours(Square square)
	{
		vector<Square *> &neighbours = *new vector<Square *>;
		// Check if square is on the top row
		if (square.row > 0)
		{
			vector<Square> &gridRow = grid[square.row - 1];
			neighbours.push_back(&gridRow[square.col]);
		}
		// Check if square is on the bottom row
		if (square.row < rows - 1)
		{
			vector<Square> &gridRow = grid[square.row + 1];
			neighbours.push_back(&gridRow[square.col]);
		}
		// Check if square is on the leftmost column
		if (square.col > 0)
		{
			vector<Square> &gridRow = grid[square.row];
			neighbours.push_back(&gridRow[square.col - 1]);
		}
		// Check if square is on the rightmost column
		if (square.col < cols - 1)
		{
			vector<Square> &gridRow = grid[square.row];
			neighbours.push_back(&gridRow[square.col + 1]);
		}
		return neighbours;
	}

	/**
	 * Get all squares with waterLevel > 0
	 * @return vector<Square *> squares with waterLevel > 0
	 */
	vector<Square *> &getWaterSquares()
	{
		vector<Square *> &waterSquares = *new vector<Square *>;
		for (auto &row : grid)
		{
			for (auto &square : row)
			{
				if (square.waterLevel > 0)
				{
					waterSquares.push_back(&square);
				}
			}
		}
		return waterSquares;
	}

	/**
	 * Return the lowest neighbour of the given square
	 * @param square square to find lowest neighbour of
	 * @param isNotTouched if true, only return neighbours that have not been touched
	 * @return Square * lowest neighbour of square
	 */
	Square *getLowestNeighbour(Square square, bool isNotTouched = false)
	{
		vector<Square *> &neighbours = getNeighbours(square);
		Square *lowestNeighbour = nullptr;
		for (auto &neighbour : neighbours)
		{
			if (isNotTouched && neighbour->isTouched)
			{
				continue;
			}
			if (lowestNeighbour == nullptr || neighbour->totalHeight() < lowestNeighbour->totalHeight())
			{
				lowestNeighbour = neighbour;
			}
		}
		return lowestNeighbour;
	}

	/**
	 * Flood the board with water.
	 * Water will flow from the highest square to the lowest square.
	 * Water will pool on squares that are lower than their neighbours, as long as they don't fall off the edge.
	 *
	 * Multiple passes will be made starting from each non-edge square, until no more water can be placed without falling off.
	 */
	void flood()
	{
		// We don't need to test edge squares because regardless
		// of their height, water will always flow out
		vector<Square *> &nonEdgeSquares = getNonEdgeSquares();

		// Drop one or more water on each non-edge square to populate waterLevel
		for (Square *square : nonEdgeSquares)
		{
			Square *currentSquare = square;
			bool isPooling = true;

			// Set max attempt as failsafe
			// We should not ever exceed the maximum distance possible for water to travel across the board
			int max = cols * rows;
			int cur = 0;

			// isPooling will remain true if water is able to flow to a neighbouring square
			// Even if the water settles on a square, currentSquare will be reset to the same
			// square from this iteration through the nonEdgeSquares loop,
			// and water pooling will continue inside this while loop
			while (isPooling && cur++ < max)
			{
				vector<Square *> &neighbours = getNeighbours(*currentSquare);

				// Filter out squares previously touched in this iteration
				auto remove_iter = remove_if(neighbours.begin(), neighbours.end(), [](Square *s)
											 { return s->isTouched; });
				neighbours.erase(remove_iter, neighbours.end());

				// If any neighbours are edge pieces and water can fall out
				for (auto &neighbour : neighbours)
				{
					if (neighbour->isEdge && neighbour->height <= currentSquare->totalHeight())
					{
						// The only way to break out of the current pooling iteration is for water to fall off the edge!
						isPooling = false;
					}
				}
				if (!isPooling)
					continue;

				// Find the lowest height neighbour
				Square *lowestNeighbourNotTouched = getLowestNeighbour(*currentSquare, true);
				Square *lowestNeighbourTouched = getLowestNeighbour(*currentSquare, false);
				Square *lowestNeighbour = lowestNeighbourNotTouched ? lowestNeighbourNotTouched : lowestNeighbourTouched;

				// If water can travel to neighouring square, move to that square
				if (lowestNeighbour != nullptr && lowestNeighbour->totalHeight() <= currentSquare->totalHeight())
				{
					currentSquare->isTouched = true;
					currentSquare = lowestNeighbour;
				}
				// If water cannot travel to neighbouring square, settle here at the height of the lowest neighbour
				else
				{
					currentSquare->waterLevel = lowestNeighbour->height + lowestNeighbour->waterLevel - currentSquare->height;

					// Restart dropping water from the original square to fill up any remaining pool space
					currentSquare = square;
					cur = 0;
					resetTouched();
				}
			}
		}
	}

	/**
	 * Level water across the board.
	 *
	 * The first `flood()` algorithm is a little bit naive, and will leave water pools that are not level.
	 * This function will level out the water pools, and ensure water
	 */
	void levelWater()
	{
		int changesMade = 0;

		// Set max attempt as failsafe
		// We should not ever exceed the maximum distance possible for water to travel across the board
		int max = cols * rows;
		int cur = 0;
		do
		{
			changesMade = 0;

			// Get all squares with water
			vector<Square *> &waterSquares = getWaterSquares();

			// Sort by lowest water level first
			sort(waterSquares.begin(), waterSquares.end(), [](Square *a, Square *b)
				 { return a->waterLevel < b->waterLevel; });

			// For each water square, get all neighbours with water
			for (auto &waterSquare : waterSquares)
			{
				vector<Square *> &waterNeighbours = getNeighbours(*waterSquare);
				auto removeWaterless_iter = remove_if(waterNeighbours.begin(), waterNeighbours.end(), [](Square *s)
													  { return s->waterLevel == 0; });
				waterNeighbours.erase(removeWaterless_iter, waterNeighbours.end());

				for (auto &neighbour : waterNeighbours)
				{
					// If neighbour has higher water level, level their water with self
					if (neighbour->totalHeight() > waterSquare->totalHeight())
					{
						// Level water
						neighbour->waterLevel = std::max(waterSquare->totalHeight() - neighbour->height, float(0));
						changesMade++;
					}
				}

				// For each water square, get all neighbours without water
				vector<Square *> &waterlessNeighbours = getNeighbours(*waterSquare);
				auto removeWater_iter = remove_if(waterlessNeighbours.begin(), waterlessNeighbours.end(), [](Square *s)
												  { return s->waterLevel > 0; });
				waterlessNeighbours.erase(removeWater_iter, waterlessNeighbours.end());
				for (auto &neighbour : waterlessNeighbours)
				{
					// If neighbour has lower totalHeight than self, level self with neighbour
					if (neighbour->totalHeight() < waterSquare->totalHeight())
					{
						// Level water
						waterSquare->waterLevel = std::max(neighbour->totalHeight() - waterSquare->height, float(0));
						changesMade++;
					}
				}
			}
		} while (changesMade > 0 && cur++ < max);
	}

	/**
	 * Get the total volume of water on the board
	 * For each square, the water volume is calculated by multiplying the water level (height) by the area of the square's base (width * width)
	 */
	float getWaterVolume()
	{
		float volume = 0;
		for (const auto &row : grid)
		{
			for (const auto &square : row)
			{
				volume += square.waterLevel * square.width * square.width;
			}
		}
		return volume;
	}

	/**
	 * Get the total volume of water on the board
	 */
	void resetTouched()
	{
		for (auto &row : grid)
		{
			for (auto &square : row)
			{
				square.isTouched = false;
			}
		}
	}

	/**
	 * Print the board to the console
	 */
	void printBoard()
	{
		// Column Headers 1 - 26
		cout << "    ";
		for (int i = 0; i < cols; i++)
		{
			cout << " " << (i + 1) << " ";
		}
		cout << endl;

		cout << "-----------------------------" << endl;
		for (const auto &row : grid)
		{
			// Row Headers A - Z
			cout << " " << (char)('A' + row[0].row) << " ";

			cout << "|";
			for (const auto &square : row)
			{
				if (square.waterLevel > 0)
				{
					cout << "[" << square.totalHeight() << "]";
				}
				else
				{
					cout << " " << square.height << " ";
				}
			}
			cout << "|";
			cout << endl;
		}
		cout << "-----------------------------" << endl;
	}
};

/**
 * Flood the board with water and provide sample output and stats
 *
 * @param board The board to flood
 */
void floodBoard(Board &board)
{
	cout << "\n-- START --------------------\n"
		 << endl;
	cout << "Before flooding:" << endl;
	board.printBoard();

	// Calculate time it takes to flood the board
	auto start = chrono::high_resolution_clock::now();

	board.flood();
	board.levelWater();

	auto end = chrono::high_resolution_clock::now();
	auto duration = chrono::duration_cast<chrono::microseconds>(end - start);

	cout << "After flooding:" << endl;
	board.printBoard();

	cout << "Volume: " << board.getWaterVolume() << " inches cubed" << endl;
	cout << "Calculation time: " << duration.count() / 1000.0 << " ms" << endl;
}

// ---------------------------- MENU ----------------------------

/**
 * A simple struct to store sample boards and their expected volume
 */
struct
{
	Board board;
	float expectedVolume;
} typedef SampleBoard;

/**
 * Sample boards to test with
 */
SampleBoard sampleBoards[] = {
	{
		Board({
			{5, 5, 5, 5, 5, 5, 5, 5},
			{5, 0, 0, 0, 8, 8, 8, 5},
			{5, 0, 0, 0, 8, 4, 6, 5},
			{5, 0, 0, 0, 8, 8, 8, 5},
			{5, 0, 0, 0, 2, 0, 0, 5},
			{5, 0, 0, 0, 2, 0, 1, 5},
			{9, 1, 2, 3, 2, 0, 0, 5},
			{9, 9, 5, 5, 5, 1, 5, 5},
		}),
		38.0f,
	},
	{
		Board({
			{5, 5, 5, 5, 5, 5, 5, 5},
			{0, 0, 0, 0, 1, 8, 8, 5},
			{5, 2, 2, 2, 8, 6, 6, 5},
			{5, 2, 2, 2, 8, 8, 8, 5},
			{5, 3, 2, 2, 2, 2, 2, 5},
			{5, 3, 3, 2, 2, 1, 2, 5},
			{9, 3, 3, 3, 2, 1, 2, 5},
			{9, 9, 5, 5, 5, 1, 5, 5},
		}),
		0.0f,
	},
	{
		// Basin
		Board({
			{9, 9, 9, 9, 9, 9, 9, 9},
			{9, 0, 0, 0, 0, 0, 0, 9},
			{9, 0, 0, 0, 0, 0, 0, 9},
			{9, 0, 0, 0, 0, 0, 0, 9},
			{9, 0, 0, 0, 0, 0, 0, 9},
			{9, 0, 0, 0, 0, 0, 0, 9},
			{9, 0, 0, 0, 0, 0, 0, 9},
			{9, 9, 9, 9, 9, 9, 9, 9},
		}),
		324.0f,
	},
	{
		// Basin hole
		Board({
			{9, 9, 9, 9, 9, 9, 9, 9},
			{9, 0, 0, 0, 0, 0, 0, 9},
			{9, 0, 0, 0, 0, 0, 0, 9},
			{9, 0, 0, 0, 0, 0, 0, 9},
			{9, 0, 0, 0, 0, 0, 0, 9},
			{9, 0, 0, 0, 0, 0, 0, 9},
			{9, 0, 0, 0, 0, 0, 0, 9},
			{9, 9, 9, 9, 0, 9, 9, 9},
		}),
		0.0f,
	},
	{
		// Basin 2 holes
		Board({
			{9, 9, 9, 1, 9, 9, 9, 9},
			{9, 0, 0, 0, 0, 0, 0, 9},
			{9, 0, 0, 0, 0, 0, 0, 9},
			{9, 0, 0, 0, 0, 0, 0, 9},
			{9, 0, 0, 0, 0, 0, 0, 9},
			{9, 0, 0, 0, 0, 0, 0, 9},
			{9, 0, 0, 0, 0, 0, 0, 9},
			{9, 9, 9, 9, 0, 9, 9, 9},
		}),
		0.0f,
	},
	{
		// Basin 2 holes + small wall
		Board({
			{9, 9, 9, 1, 9, 9, 9, 9},
			{9, 0, 0, 1, 0, 0, 0, 9},
			{9, 0, 0, 1, 0, 0, 0, 9},
			{9, 0, 0, 1, 0, 0, 0, 9},
			{9, 0, 0, 1, 0, 0, 0, 9},
			{9, 0, 0, 1, 0, 0, 0, 9},
			{9, 0, 0, 1, 0, 0, 0, 9},
			{9, 9, 9, 9, 0, 9, 9, 9},
		}),
		12.0f,
	},
	{
		// Pyramid
		Board({
			{0, 0, 0, 0, 0, 0, 0, 0},
			{0, 1, 1, 1, 1, 1, 1, 0},
			{0, 1, 2, 2, 2, 2, 1, 0},
			{0, 1, 2, 3, 3, 2, 1, 0},
			{0, 1, 2, 3, 3, 2, 1, 0},
			{0, 1, 2, 2, 2, 2, 1, 0},
			{0, 1, 1, 1, 1, 1, 1, 0},
			{0, 0, 0, 0, 0, 0, 0, 0},
		}),
		0.0f,
	},
	{
		// Pyramid lines
		Board({
			{0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 1, 1, 1, 1, 0, 0},
			{0, 1, 0, 2, 2, 0, 1, 0},
			{0, 1, 2, 3, 3, 2, 1, 0},
			{0, 1, 2, 3, 3, 2, 1, 0},
			{0, 1, 0, 2, 2, 0, 1, 0},
			{0, 0, 1, 1, 1, 1, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0},
		}),
		4.0f,
	},
	{
		// Tiered pools
		Board({
			{9, 9, 9, 9, 7, 7, 7, 7},
			{9, 0, 0, 9, 7, 0, 0, 7},
			{9, 0, 0, 9, 7, 0, 0, 7},
			{9, 9, 9, 9, 7, 7, 7, 7},
			{3, 3, 3, 3, 5, 5, 5, 5},
			{3, 0, 0, 3, 5, 0, 0, 5},
			{3, 0, 0, 3, 5, 0, 0, 5},
			{3, 3, 3, 3, 5, 5, 5, 5},
		}),
		96.0f,
	},
	{
		// Waterfall
		Board({
			{9, 9, 9, 9, 7, 7, 7, 7},
			{9, 0, 0, 8, 7, 0, 0, 7},
			{9, 0, 0, 8, 7, 0, 0, 7},
			{9, 9, 9, 9, 7, 6, 6, 7},
			{3, 3, 3, 3, 5, 5, 5, 5},
			{3, 0, 0, 3, 4, 0, 0, 5},
			{3, 0, 0, 3, 4, 0, 0, 5},
			{3, 2, 2, 3, 5, 5, 5, 5},
		}),
		80.0f,
	},
	{
		// Smile
		Board({
			{1, 1, 1, 1, 1, 1, 1, 1},
			{1, 0, 2, 2, 2, 2, 0, 1},
			{1, 2, 0, 3, 3, 0, 2, 1},
			{1, 2, 3, 4, 4, 3, 2, 1},
			{1, 2, 3, 4, 4, 3, 2, 1},
			{1, 0, 3, 3, 3, 3, 0, 1},
			{1, 2, 0, 0, 0, 0, 2, 1},
			{1, 1, 1, 1, 1, 1, 1, 1},
		}),
		12.0f,
	},
};

/**
 * Runs unit tests for the Board's volume calculation.
 */
void runUnitTests()
{
	for (size_t i = 0; i < sizeof(sampleBoards) / sizeof(SampleBoard); i++)
	{
		Board board = sampleBoards[i].board;
		board.flood();
		board.levelWater();
		ASSERT_EQUAL(sampleBoards[i].expectedVolume, board.getWaterVolume());
	}
}

/**
 * Runs the demo for the predefined boards.
 */
void runPredefinedBoardsDemo()
{
	for (size_t i = 0; i < sizeof(sampleBoards) / sizeof(SampleBoard); i++)
	{
		Board board = sampleBoards[i].board;
		floodBoard(board);
	}
}

/**
 * Runs the demo for random boards.
 */
void runRandomBoardsDemo(bool isComplex)
{
	if (isComplex)
	{
		Board randComplex1(8, 8, true);
		floodBoard(randComplex1);
		Board randComplex2(8, 8, true);
		floodBoard(randComplex2);
		Board randComplex3(8, 8, true);
		floodBoard(randComplex3);
	}
	else
	{
		Board rand1(8, 8);
		floodBoard(rand1);
		Board rand2(8, 8);
		floodBoard(rand2);
		Board rand3(8, 8);
		floodBoard(rand3);
	}
}

/**
 * Display the demo submenu.
 */
void demoMenu()
{
	int choice;

	while (choice != 4)
	{
		cout << "\n"
			 << endl;
		cout << "Please make a selection:\n"
			 << endl;
		cout << "  1. Predefined Boards" << endl;
		cout << "  2. Random Boards (Simple)" << endl;
		cout << "  3. Random Boards (Complex)" << endl;
		cout << "  4. Back" << endl;
		cout << "\n"
			 << endl;

		std::cin >> choice;

		switch (choice)
		{
		case 1:
			runPredefinedBoardsDemo();
			break;
		case 2:
			runRandomBoardsDemo(false);
			break;
		case 3:
			runRandomBoardsDemo(true);
			break;
		case 4:
			cout << "Going back..." << endl;
		default:
			cout << "Invalid choice. Please try again." << endl;
		}
	}
}

/**
 * Display the main menu.
 */
void displayMenu()
{
	cout << "_____________________________________________~~~^~^~^~~~_____________\n"
		 << "                                             |   |     |\n"
		 << "  ``~ Welcome to the chess board water       | |   |  ||\n"
		 << "          volume calculator!  ~~~``          |  |  |   |\n"
		 << "                                             |'. .' .`.|\n"
		 << "_____________________________________________|0oOO0oO0o|_____________\n"
		 << "                                            /  '  '. ` `\\\n"
		 << "  -- Question --                           /  .  '  . `  \\\n"
		 << "Imagine a chess board where each          /  .  '  .  `  `\\\n"
		 << "square has a height, forming a topology. /  '  `  .  '  `  \\\n"
		 << "Water is poured over the entire board   /  .  '  `  .  '  ` \\\n"
		 << "and collects in \"valleys\" or flows     /  `     `  |  '  `   \\\n"
		 << "over the edges. What volume of water  /  .  |  `  .  |  |  `  \\\n"
		 << "does a given board hold?              |  |[]|  |[]|  |[]|  |[]|\n"
		 << "                                      |[]|  |[]|  |[]|  |[]|  |\n"
		 << "                                      |  |[]|  |[]|  |[]|  |[]|\n"
		 << "Please make a selection:              |[]|  |[]|  |[]|  |[]|  |\n"
		 << "                                      |  |[]|  |[]|  |[]|  |[]|\n"
		 << "  1. Run Unit Tests                   |[]|  |[]|  |[]|  |[]|  |\n"
		 << "  2. Board Flooding Demo              |  |[]|  |[]|  |[]|  |[]|\n"
		 << "  3. Exit                             |[]|  |[]|  |[]|  |[]|  |\n";
	cout << "\n"
		 << endl;
}

int main()
{
	srand(time(NULL)); // Seed the random number generator
	int choice;

	while (true)
	{
		displayMenu();
		std::cin >> choice;

		switch (choice)
		{
		case 1:
			runUnitTests();
			break;
		case 2:
			demoMenu();
			break;
		case 3:
			cout << "Exiting..." << endl;
			return 0;
		default:
			cout << "Invalid choice. Please try again." << endl;
		}
	}

	return 0;
}
