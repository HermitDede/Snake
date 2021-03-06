// Snake.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <string>
#include <conio.h>

#define _WIN32_WINNT 0x0500
#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77

using namespace std;

#pragma region Variables and Interface
static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
static const HWND hDesktop = GetDesktopWindow();

const int rows = 28, columns = 56; // Max Rows = 28 (posY = 0-27), Max Columns = 118 (posX = 0-117)
const int maxSnakeSize = 300; // maximum snake size which can be reached
int snakeLength = 3;

const int title[7][26] = {
	{ 1,1,1,1,0,1,0,0,0,1,0,1,1,1,1,0,1,0,0,0,1,0,1,1,1,1 },
	{ 1,0,0,0,0,1,1,0,0,1,0,1,0,0,1,0,1,0,0,1,0,0,1,0,0,0 },
	{ 1,0,0,0,0,1,1,1,0,1,0,1,0,0,1,0,1,0,1,0,0,0,1,0,0,0 },
	{ 1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1,1,0,0,0,0,1,1,1,0 },
	{ 0,0,0,1,0,1,0,1,1,1,0,1,0,0,1,0,1,0,1,0,0,0,1,0,0,0 },
	{ 0,0,0,1,0,1,0,0,1,1,0,1,0,0,1,0,1,0,0,1,0,0,1,0,0,0 },
	{ 1,1,1,1,0,1,0,0,0,1,0,1,0,0,1,0,1,0,0,0,1,0,1,1,1,1 } };
char arr[rows][columns];
int highScores[10] = { 1000, 900, 800, 700, 600, 500, 400, 300, 200, 100 };
bool gameover = false;

// 'x' = frame, 's' = snake, ' ' = empty space, 'f' = food, 'b' = bad food
const char frame = 'x';
const char space = ' ';
const char snake = 's';
const char food = 'f';
const char badFood = 'b';

const int frameColor = 153;
const int spaceColor = 0;
const int snakeColorHead = 221;
const int snakeColorBody = 255;
const int foodColor = 170;
const int badFoodColor = 204;

void PrintArray(char arr[rows][columns]);
void FillArray(char arr[rows][columns]);
void SetCursorPosition(int x, int y);
int RandomInt(int min, int max);
void SetColor(int colorNum);
void PutRandomFood(bool isBad);
void MoveSnake(int directionY, int directionX, int snakeArr[maxSnakeSize][2]);
void BeginSnake(int snakeArr[maxSnakeSize][2]);
void PaintAndWrite(char type, int color, int x, int y);
int GameLogic(int directionY, int directionX, int snakeArr[maxSnakeSize][2]);
void StartGame();
void MainMenu();
void Sleep(int miliseconds);
void KeyValueFinder();
void ClearConsole();
void UpdateHighscore(int highScore);
void SetScore(int CurrentScore);
void WaitEnter();
COORD GetDesktopResolution();
#pragma endregion

int main() {


	CONSOLE_SCREEN_BUFFER_INFO currentBuffer;
	GetConsoleScreenBufferInfo(hOut, &currentBuffer);
	SetConsoleScreenBufferSize(hOut, currentBuffer.dwMaximumWindowSize);

	COORD desktopResolution;
	desktopResolution = GetDesktopResolution();

	HWND console = GetConsoleWindow();
	RECT r;
	GetWindowRect(console, &r);
	// Window frame horizontal = +16 pixel (real = 2), vertical = +39 pixel (real = 32)
	// each column = 8 pixels, each row = 16 pixels
	int gameWidthInCells = (columns + 4);
	int gameHeightInCells = ((rows + 1) + 2);
	int gameWidthInPixels = gameWidthInCells * 8 + 16;
	int gameHeightInPixels = gameHeightInCells * 16 + 39;
	MoveWindow(console, (desktopResolution.X - gameWidthInPixels) / 2, (desktopResolution.Y - gameHeightInPixels) / 2,
		gameWidthInPixels, gameHeightInPixels, TRUE);

	int sizeX = columns;
	int sizeY = rows + 1;

	COORD bufferSize;
	bufferSize.X = currentBuffer.dwMaximumWindowSize.X;
	bufferSize.Y = 0;
	cout << bufferSize.X << " " << bufferSize.Y << endl;

	while (1) {
		if (bufferSize.Y != gameHeightInCells)
			bufferSize.Y++;
		system("cls");
		cout << bufferSize.Y << endl;
		GetConsoleScreenBufferInfo(hOut, &currentBuffer);
		cout << currentBuffer.dwSize.X << " " << currentBuffer.dwSize.Y << endl;
		if (!SetConsoleScreenBufferSize(hOut, bufferSize)) {
			cout << "Loading (1/2)";
		}
		else {
			cout << "Step 1 out of 2 is finished!" << endl;
			break;
		}
	}

	while (1) {
		if (bufferSize.X < gameHeightInCells)
			bufferSize.X++;
		else
			bufferSize.X--;

		system("cls");
		if (!SetConsoleScreenBufferSize(hOut, bufferSize)) {
			cout << "Game starting...";
			break;
		}
		else {
			cout << "Loading (2/2)";
		}
	}

	while (1) {
		MainMenu();
	}

	cin.ignore();
	return 0;
}

COORD GetDesktopResolution()
{
	RECT desktop;
	COORD desktopCoord;
	// Get a handle to the desktop window
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	desktopCoord.X = desktop.right;
	desktopCoord.Y = desktop.bottom;

	return desktopCoord;
}

void Sleep(int miliseconds) {
	unsigned int mseconds = 100;
	clock_t goal = mseconds + clock();
	while (goal > clock());
}

void ClearConsole() {
	SetColor(7);
	system("cls");
}

void KeyValueFinder() {
	int ch;

	while ((ch = _getch()) != 27) /* 27 = Esc key */
	{
		printf("%d", ch);
		if (ch == 0 || ch == 224)
			printf(", %d", _getch());
		printf("\n");
	}
}

void MainMenu() {
	ClearConsole();

	SetCursorPosition((columns / 2) - 7, 15);
	cout << "[1] - New Game";
	SetCursorPosition((columns / 2) - 7, 16);
	cout << "[2] - High Scores";

	int defaultX = (columns / 2) - 24;
	int defaultY = 5;
	int titleColors[] = { 255,238,221,204,187,170,153 };
	int pressedKey = 0;
	while (1) {
		if (_kbhit()) {
			pressedKey = _getch();
			if (pressedKey == 49 || pressedKey == 50) {
				break;
			}
		}

		SetColor(titleColors[RandomInt(0, 6)]);
		for (int i = 0; i < 7; i++)
		{
			for (int k = 0; k < 26; k++)
			{
				if (title[i][k]) {
					SetCursorPosition(defaultX + 2 * k, defaultY + i);
					cout << "  ";
				}
			}
		}
		SetCursorPosition(0, 0);
		Sleep(1000);
	}

	if (pressedKey == 49) { // Key Code 49 = number 1
		ClearConsole();
		StartGame();
	}
	else if (pressedKey == 50) { // Key Code 50 = number 2
		ClearConsole();

		SetCursorPosition((columns / 2) - 7, 8);
		cout << "HIGH SCORES";
		SetCursorPosition((columns / 2) - 7, 9);
		cout << "===========";
		for (int i = 0; i < 10; i++)
		{
			SetCursorPosition((columns / 2) - 6, 11 + i);
			if (i + 1 != 10)
				cout << i + 1 << ".   " << highScores[i];
			else
				cout << i + 1 << ".  " << highScores[i];
		}
		SetCursorPosition((columns / 2) - 19, 22);
		cout << "Press 'Enter' Key to Return Main Menu!";
		SetColor(0);

		WaitEnter();
	}
}

void WaitEnter() {
	while (true)
	{
		if (_kbhit()) {
			if (_getch() == 13)
				break;
		}
	}
}

void StartGame() {
	int score = 0;
	gameover = false;
	srand(time(NULL)); // random seed for rand() func

	int snakeArr[maxSnakeSize][2] = { 0 };
	snakeLength = 3;

	FillArray(arr);
	PrintArray(arr);

	SetColor(255);
	BeginSnake(snakeArr);
	for (int i = 0; i < 5; i++)
	{
		PutRandomFood(false);
	}
	for (int i = 0; i < 3; i++)
	{
		PutRandomFood(true);
	}

	SetScore(score);

	int directionY = 0; // 1 = down, 0 = no Y direction, -1 = up
	int directionX = 2; // 2 = right, 0 = no X direction, -2 = left
	int key = 0;

	unsigned int mseconds = 100;
	clock_t goal = mseconds + clock();

	while (!gameover)
	{
		if (_kbhit())
			switch (key = _getch()) {
			case KEY_UP:
				if (directionY != 1) {
					directionX = 0;
					directionY = -1;
				}
				break;
			case KEY_DOWN:
				if (directionY != -1) {
					directionX = 0;
					directionY = 1;
				}
				break;
			case KEY_LEFT:
				if (directionX != 2) {
					directionX = -2;
					directionY = 0;
				}
				break;
			case KEY_RIGHT:
				if (directionX != -2) {
					directionX = 2;
					directionY = 0;
				}
				break;
			default:
				break;
			}

		if (goal < clock()) {
			score += GameLogic(directionY, directionX, snakeArr);
			SetScore(score);
			goal = mseconds + clock();
		}
	}

	SetColor(207);
	SetCursorPosition((columns / 2) - 4, 12);
	cout << "GAME OVER !"; // length 11
	SetCursorPosition((columns / 2) - 6, 13);
	cout << "Your Score: " << score;  // length 13-16
	SetCursorPosition((columns / 2) - 17, 14);
	cout << "Press 'Enter' Key to Return Main Menu!"; // length 38
	UpdateHighscore(score);
	WaitEnter();
}

void PrintArray(char arr[rows][columns]) {

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++)
		{
			if (arr[i][j] == frame)
				SetColor(frameColor);
			else
				SetColor(spaceColor);

			cout << arr[i][j];
		}
		cout << endl;
	}
}

void FillArray(char arr[rows][columns]) {
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++)
		{
			if (j < 2 || j > columns - 3 || i == 0 || i == rows - 1)
				arr[i][j] = frame;
			else
				arr[i][j] = space;
		}
	}
}

void SetCursorPosition(int x, int y)
{
	cout.flush();
	COORD coord = { (SHORT)x, (SHORT)y };
	SetConsoleCursorPosition(hOut, coord);
}

int RandomInt(int min, int max) {
	int result;
	result = min + rand() % max;
	return result;
}

void SetColor(int colorNum) {
	SetConsoleTextAttribute(hOut, colorNum);
}

void PutRandomFood(bool isBad) {
	bool posEmpty = false;
	int posX;
	int posY;
	while (posEmpty == false) {
		posX = (RandomInt(2, columns - 3) / 2) * 2;
		posY = (RandomInt(1, rows - 2) / 2) * 2;
		if (arr[posY][posX] == space) {
			posEmpty = true;
		}
	}

	if (isBad == false)
		PaintAndWrite(food, foodColor, posY, posX);
	else
		PaintAndWrite(badFood, badFoodColor, posY, posX);
}

void BeginSnake(int snakeArr[maxSnakeSize][2]) {
	int beginX = (columns / 4) * 2;
	int beginY = (rows / 4) * 2;

	for (int i = 0; i < snakeLength; i++)
	{
		snakeArr[i][0] = beginY;
		snakeArr[i][1] = beginX - 2 * i;
	}

	int color = 0;
	for (int i = 0; i < maxSnakeSize; i++)
	{
		if (snakeArr[i][0] == 0)
			break;
		if (i == 0)
			color = snakeColorHead;
		else
			color = snakeColorBody;

		PaintAndWrite(snake, color, snakeArr[i][0], snakeArr[i][1]);
	}
}

void MoveSnake(int directionY, int directionX, int snakeArr[maxSnakeSize][2]) {
	// finding tail of snake
	int tailIndex = snakeLength - 1;

	// emptying current tail position in the main array since it will be lost in the next step
	PaintAndWrite(space, spaceColor, snakeArr[tailIndex][0], snakeArr[tailIndex][1]);

	// moving snake in the snake array towards the direction pressed
	for (int i = tailIndex; i > -1; i--)
	{
		if (i == 0) {
			snakeArr[i][0] += directionY;
			snakeArr[i][1] += directionX;
			PaintAndWrite(snake, snakeColorHead, snakeArr[i][0], snakeArr[i][1]);
		}
		else {
			snakeArr[i][0] = snakeArr[i - 1][0];
			snakeArr[i][1] = snakeArr[i - 1][1];
			PaintAndWrite(snake, snakeColorBody, snakeArr[i][0], snakeArr[i][1]);
		}
	}
	PaintAndWrite(frame, frameColor, 0, 0);
}

void PaintAndWrite(char type, int color, int y, int x) {
	arr[y][x] = type;
	arr[y][x + 1] = type;

	SetColor(color);
	SetCursorPosition(x, y);
	cout << type << type;
}

int GameLogic(int directionY, int directionX, int snakeArr[maxSnakeSize][2]) {
	int additiveScore = 0;
	char nextPos = arr[snakeArr[0][0] + directionY][snakeArr[0][1] + directionX];

	if ((snakeArr[0][0] + directionY) == snakeArr[1][0] && (snakeArr[0][1] + directionX) == snakeArr[1][1])
		return additiveScore;

	if (nextPos == frame) {
		directionY = directionY * (3 - rows);
		directionX = directionX * (6 - columns) / 2;
		nextPos = arr[snakeArr[0][0] + directionY][snakeArr[0][1] + directionX];
	}

	if (nextPos == food) {
		snakeLength++;
		additiveScore += 5;
		PutRandomFood(false);
	}
	else if (nextPos == badFood) {
		snakeLength--;
		additiveScore -= 10;
		PaintAndWrite(space, spaceColor, snakeArr[snakeLength][0], snakeArr[snakeLength][1]);
		PutRandomFood(true);
	}
	else if (nextPos == snake) {
		gameover = true;
	}

	MoveSnake(directionY, directionX, snakeArr);
	if (snakeLength == 1) {
		gameover = true;
	}

	return additiveScore;
}

void UpdateHighscore(int highScore) {
	int temp;
	if (highScore > highScores[9])
		highScores[9] = highScore;
	for (int i = 8; i > -1; i--)
	{
		if (highScore < highScores[i])
			break;
		else {
			temp = highScores[i];
			highScores[i] = highScore;
			highScores[i + 1] = temp;
		}
	}
}

void SetScore(int CurrentScore) {
	SetCursorPosition((columns / 2) - 4, rows);
	SetColor(7);
	cout << "                                 ";
	SetCursorPosition((columns / 2) - 4, rows);
	cout << "Score: " << CurrentScore;
	SetCursorPosition(0, 0);
}