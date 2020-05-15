#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#ifdef __linux__
	#define ANSI_COLOR_RED     "\x1b[31m"
	#define ANSI_COLOR_GREEN   "\x1b[32m"
	#define ANSI_COLOR_YELLOW  "\x1b[33m"
	#define ANSI_COLOR_BLUE    "\x1b[1;34m"
	#define ANSI_COLOR_MAGENTA "\x1b[35m"
	#define ANSI_COLOR_CYAN    "\x1b[36m"
	#define ANSI_COLOR_RESET   "\x1b[0m"
#else
	#define ANSI_COLOR_RED     ""
	#define ANSI_COLOR_GREEN   ""
	#define ANSI_COLOR_YELLOW  ""
	#define ANSI_COLOR_BLUE    ""
	#define ANSI_COLOR_MAGENTA ""
	#define ANSI_COLOR_CYAN    ""
	#define ANSI_COLOR_RESET   ""
#endif

int B_WIDTH = 8;
int B_HEIGHT = 8;
int BOMB_COUNT = 0;
int BOMBS_FOUND = 0;
char temp;

void initBoard(char board[B_WIDTH][B_HEIGHT], char realBoard[B_WIDTH][B_HEIGHT]);
void drawBoard(char board[B_WIDTH][B_HEIGHT]);
void revealBombs(char board[B_WIDTH][B_HEIGHT], char realBoard[B_WIDTH][B_HEIGHT]);
void markSquare(char board[B_WIDTH][B_HEIGHT], int row, int column);
void fillBoard(char board[B_WIDTH][B_HEIGHT], char fill);
int countTouching(char realBoard[B_WIDTH][B_HEIGHT], int row, int column, char c);
bool openSquare(char board[B_WIDTH][B_HEIGHT], char realBoard[B_WIDTH][B_HEIGHT], int row, int column);
bool checkWon(char board[B_WIDTH][B_HEIGHT], char realBoard[B_WIDTH][B_HEIGHT]);
bool isValid(int row, int column);
bool isFirstMove(char board[B_WIDTH][B_HEIGHT]);
void printHelp();
void refresh();

int main(int argc, char** argv) {
	if (argc > 1 && strcmp(argv[1], "?") == 0) {
		refresh();
		printHelp();
		return 0;
	}

	refresh();
	if (argc > 1 && sscanf(argv[1], "%dx%d", &B_WIDTH, &B_HEIGHT) == 2) {
		sscanf(argv[1], "%dx%d", &B_WIDTH, &B_HEIGHT);
	}
	else {
		printf("Select Board Size (8x8): ");
		scanf("%d x %d", &B_WIDTH, &B_HEIGHT);
		scanf("%c", &temp);
	}

	BOMB_COUNT = (B_WIDTH * B_HEIGHT) / 6;

	srand(time(NULL));
	bool isTest = (argc > 1 && strcmp(argv[1], "-dv") == 0) || (argc > 2 && strcmp(argv[2], "-dv") == 0);
	bool isPlaying = true;
	char Board[B_WIDTH][B_HEIGHT];
	char RealBoard[B_WIDTH][B_HEIGHT];
	fillBoard(Board, '-');
	fillBoard(RealBoard, ' ');
	time_t begin = time(NULL);

	while (isPlaying) {
		refresh();
		drawBoard(Board);

		if (isTest)
			drawBoard(RealBoard);

		char moveType;
		int row, column;
		if (!isFirstMove(RealBoard)) {
			printf("TYPE: ");
			scanf("%c", &moveType);
		}

		printf("ROW: ");
		scanf("%d", &row);
		printf("COLUMN: ");
		scanf("%d", &column);
		scanf("%c", &temp);

		if (moveType == 'm') {
			markSquare(Board, row, column);
			if (checkWon(Board, RealBoard)) {
				refresh();
				revealBombs(Board, RealBoard);

				printf("%sYOU WON! ", ANSI_COLOR_GREEN);
				isPlaying = false;
			}

			continue;
		}

		bool lost = openSquare(Board, RealBoard, row, column);
		if (lost) {
			refresh();
			revealBombs(Board, RealBoard);

			printf("%sYOU LOST! %s%d/%d BOMBS FOUND ", ANSI_COLOR_RED, ANSI_COLOR_MAGENTA, BOMBS_FOUND, BOMB_COUNT);
			isPlaying = false;
		}
	}

	time_t end = time(NULL);
	printf("%sTime: %lds", ANSI_COLOR_YELLOW, (end - begin));

	printf("\n\n%sPress enter to exit...", ANSI_COLOR_RED);
	getchar();
	return 0;
}

void refresh() {
	#ifdef _WIN32
		system("cls");
	#else
		system("clear");
	#endif

	printf("%s##################### Mysweeper ####################\n%s#################### BY BILLVOG ####################%s\n\n", ANSI_COLOR_YELLOW, ANSI_COLOR_CYAN, ANSI_COLOR_RESET);
}

void printHelp() {
	printf("How to play:\nTYPE — Mark box that you think it has a bomb | (m)\nOPEN — Open box. (o)\n");
	printf("ROW & COLUMN  — Type the coordinates of the box you want to open.\n");
	printf("\n%sPress enter to exit...", ANSI_COLOR_RED);
	getchar();
}

void fillBoard(char board[B_WIDTH][B_HEIGHT], char fill) {
	for (size_t row = 0; row < B_WIDTH; row++)
		for (size_t column = 0; column < B_HEIGHT; column++) {
			board[row][column] = fill;
		}
}

void initBoard(char board[B_WIDTH][B_HEIGHT], char realBoard[B_WIDTH][B_HEIGHT]) {
	// Generate bombs
	for (int i = 0; i < BOMB_COUNT; i++) {
		int _w = rand() % B_WIDTH;
		int _h = rand() % B_HEIGHT;

		while (realBoard[_w][_h] == '*' || realBoard[_w][_h] == 'i' || countTouching(realBoard, _w, _h, 'i') != 0) {
			_w = rand() % B_WIDTH;
			_h = rand() % B_HEIGHT;
		}

		realBoard[_w][_h] = '*';
	}

	for (int width = 0; width < B_WIDTH; width++)
		for (int height = 0; height < B_HEIGHT; height++) {
			board[width][height] = '-';

			if (realBoard[width][height] != '*') {
				realBoard[width][height] = countTouching(realBoard, width, height, '*') + '0';
			}
		}
}

int countTouching(char realBoard[B_WIDTH][B_HEIGHT], int row, int column, char c) {
	int count = 0;

	int mW = 1, mH = 0;
	for (int i = 0; i < 8; i++) {
		switch (i) {
			case 0:
				mW = 1; mH = 0;
				break;
			case 1:
				mW = -1; mH = 0;
				break;
			case 2:
				mW = 0; mH = 1;
				break;
			case 3:
				mW = 0; mH = -1;
				break;
			case 4:
				mW = -1; mH = -1;
				break;
			case 5:
				mW = -1; mH = +1;
				break;
			case 6:
				mW = +1; mH = -1;
				break;
			case 7:
				mW = +1; mH = +1;
				break;
		}

		if (realBoard[row + mW][column + mH] == c && isValid(row + mW, column + mH)) {
			count++;
		}
	}

	return count;
}

bool isValid(int row, int column) {
	row++;
	column++;

	return (row <= B_WIDTH && column <= B_HEIGHT) && (row > 0 && column > 0);
}

bool isFirstMove(char board[B_WIDTH][B_HEIGHT]) {
	for (int row = 0; row < B_WIDTH; row++)
		for (int column = 0; column < B_HEIGHT; column++) {
			if (board[row][column] != ' ')
				return false;
		}

	return true;
}

void drawBoard(char board[B_WIDTH][B_HEIGHT]) {
	for (int height = 0; height < B_HEIGHT; height++) {
		printf("%s%c%d ", ANSI_COLOR_YELLOW, (height + 1) >= 10 ? '\0' : ' ', height + 1);
	}
	printf("\n");

	for (int height = 0; height < B_HEIGHT; height++) printf("%s | %s", ANSI_COLOR_YELLOW, ANSI_COLOR_RESET);
	printf("\n");

	for (int width = 0; width < B_WIDTH; width++) {
		for (int height = 0; height < B_HEIGHT; height++) {
			if (board[width][height] == '1')
				printf(" %s%c%s ", ANSI_COLOR_CYAN, board[width][height], ANSI_COLOR_RESET);
			else if (board[width][height] == '2')
				printf(" %s%c%s ", ANSI_COLOR_GREEN, board[width][height], ANSI_COLOR_RESET);
			else if (board[width][height] == '3')
				printf(" %s%c%s ", ANSI_COLOR_YELLOW, board[width][height], ANSI_COLOR_RESET);
			else if (board[width][height] == '4' || board[width][height] == '5' || board[width][height] == '6' || board[width][height] == '7' || board[width][height] == '8')
				printf(" %s%c%s ", ANSI_COLOR_RED, board[width][height], ANSI_COLOR_RESET);
			else if (board[width][height] == '*')
				printf(" %s%c%s ", ANSI_COLOR_MAGENTA, board[width][height], ANSI_COLOR_RESET);
			else if (board[width][height] == '+')
				printf(" %s%c%s ", ANSI_COLOR_BLUE, board[width][height], ANSI_COLOR_RESET);
			else
				printf(" %c ", board[width][height]);
		}

		printf("%s— %d%s", ANSI_COLOR_YELLOW, width + 1, ANSI_COLOR_RESET);

		printf("\n");
	}

	printf("\n");
}

bool openSquare(char board[B_WIDTH][B_HEIGHT], char realBoard[B_WIDTH][B_HEIGHT], int row, int column) {
	--row;
	--column;

	if (!isValid(row, column)) {
		return false;
	}
	else if (board[row][column] != '-' && board[row][column] != 'i') {
		return false;
	}
	else if (realBoard[row][column] == '*') {
		return true;
	}
	else if (isFirstMove(realBoard)) {
		board[row][column] = realBoard[row][column] = 'i';
		initBoard(board, realBoard);
		openSquare(board, realBoard, ++row, ++column);

		return false;
	}
	else {
		board[row][column] = realBoard[row][column];

		if (realBoard[row][column] == '0') {
			int mW = 1, mH = 0;
			for (int i = 0; i < 8; i++) {
				switch (i) {
					case 0:
						mW = 1; mH = 0;
						break;
					case 1:
						mW = -1; mH = 0;
						break;
					case 2:
						mW = 0; mH = 1;
						break;
					case 3:
						mW = 0; mH = -1;
						break;
					case 4:
						mW = -1; mH = -1;
						break;
					case 5:
						mW = -1; mH = +1;
						break;
					case 6:
						mW = +1; mH = -1;
						break;
					case 7:
						mW = +1; mH = +1;
						break;
				}

				for (int y = 0; y < 3; y++) {
					int newWidth = row + mW + y;
					int newHeight = column + mH + y;
					if (realBoard[newWidth][newHeight] != '*' && board[newWidth][newHeight] != '+' && isValid(newWidth, newHeight)) {
						board[newWidth][newHeight] = realBoard[newWidth][newHeight];

						if (board[newWidth][newHeight] != '0')
							break;
					}
				}
			}
		}

		return false;
	}
}

void revealBombs(char board[B_WIDTH][B_HEIGHT], char realBoard[B_WIDTH][B_HEIGHT]) {
	for (int width = 0; width < B_WIDTH; width++)
		for (int height = 0; height < B_HEIGHT; height++) {
			if (realBoard[width][height] == '*') {
				if (board[width][height] == '+') {
					BOMBS_FOUND++;
				}

				board[width][height] = '*';
			}
		}

	drawBoard(board);
}

void markSquare(char board[B_WIDTH][B_HEIGHT], int row, int column) {
	--row;
	--column;

	if (board[row][column] == '-') {
		board[row][column] = '+';
	}
	else if (board[row][column] == '+') {
		board[row][column] = '-';
	}
}

bool checkWon(char board[B_WIDTH][B_HEIGHT], char realBoard[B_WIDTH][B_HEIGHT]) {
	int bombsFound = 0;
	for (int width = 0; width < B_WIDTH; width++)
		for (int height = 0; height < B_HEIGHT; height++) {
			if (board[width][height] == '+' && realBoard[width][height] == '*') {
				bombsFound++;
			}
		}

	return (bombsFound == BOMB_COUNT);
}
