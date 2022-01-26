// Projeto_EDA_jogo.cpp 
// GROUP 06
// MEMBERS: Martim Sousa + Martim Chaves

#include <iostream>
#include <iomanip>
#include <vector>
#include <ctime>
#include <fstream>
#include <string>
#include <cstddef>
#include <sstream>
#include <Windows.h>

using namespace std;

struct Tile
{
	int topValue;
	int downValue;
	bool visible;
	bool onBoard;
};

struct Board
{
	vector<vector<Tile>> data;
};

struct Player
{
	string name;
	int numberTurns;
	int timeTaken;
	int score;
};

struct Deck
{
	vector<Tile> data;
};

struct Hand
{
	vector<Tile> data;
};

//color of tiles-------------------------------------------------------------------
void setColor(unsigned int color);

//Randomizes board with swap function----------------------------------------------
void randomizeBoard(vector<vector<Tile>> &v);

//Generate board-------------------------------------------------------------------
void generateTiles(vector<vector<Tile>> &v);

//shows tile-----------------------------------------------------------------------
void showTile(Tile T);

//check user input-----------------------------------------------------------------
bool inputErrors(int &coordLine, int &coordColumn, char &separator, vector<vector<Tile>> &v);

//show board visible---------------------------------------------------------------
void showBoard(vector<vector<Tile>> &v);

//show board hidden----------------------------------------------------------------
int showHiddenBoard(vector<vector<Tile>> &v);

//player choice--------------------------------------------------------------------
int playerTileChoice(vector<vector<Tile>> &v);

//get name and number of turns from string-----------------------------------------
void getInfoFromString(const string &scoreLine, Player &player);

//sort players by turns------------------------------------------------------------
void sortPlayersByTurns(vector<Player> &group);

//FIRST FASE-----------------------------------------------------------------------
void scoreUpdateFirstFase(ifstream& mainFile, ofstream& tempFile, Player user);

//SECOND FASE----------------------------------------------------------------------
void scoreUpdateSecondFase(ofstream& mainFile, ifstream& tempFile, Player user);

//Displays game menu---------------------------------------------------------------
bool displayMenu();

//Starts the game------------------------------------------------------------------
bool runGame();

//Generates a deck-----------------------------------------------------------------
void generateDeck(Deck &setOfTiles, Board totalSet);

int main()
{

	bool play = runGame();

	return 0;
}

//start the game-------------------------------------------------------------------
bool runGame()
{
	//Menu
	bool checkStartGame = displayMenu();
	if (!checkStartGame)
	{
		cout << '\n' << "Thank you.";
		return false;
	}

	//intialization

	srand((unsigned)time(NULL)); //tempo
	time_t ti, tf, tspan; //limites tempo
	vector<vector<Tile>> board(4, vector<Tile>(7)); //board
	Player user;
	int checkEndGame;
	Deck testDeck;
	Board testBoard;
	Hand playerHand;

	//GAME
	cout << '\n' << "What is your user name?" << '\n';

	do
	{
		getline(cin, user.name);
		if (user.name.size() > 20)
			cout << "Please enter a smaller name (maximum amount of characters is 20)." << '\n';
	} while (user.name.size() > 20);

	generateTiles(board); //board generation

	randomizeBoard(board); //randomized board
	//showBoard(board);

	/*testBoard.data = board; //DECK AND HAND STRUCT
	generateDeck(testDeck, testBoard);
	for (int s = 0; s < 28; s++)
		showTile(testDeck.data.at(s));
	for (int f = 0; f < 7; f++) //Number of dominoes per hand is 7 (assuming 4 players)
		playerHand.data.push_back(testDeck.data.at(f));
	cout << '\n' << '\n' << "Possible Hand: ";
	for (int q = 0; q < 7; q++)
		showTile(playerHand.data.at(q));*/

		//hide board
	cout << '\n' << "Time is now ticking, " << user.name << "!" << '\n' << '\n' << "Board:" << '\n';
	showHiddenBoard(board);

	ti = time(NULL);

	do
	{
		user.numberTurns = playerTileChoice(board);
		cout << '\n';

		checkEndGame = showHiddenBoard(board);
		cout << '\n';

	} while (checkEndGame != 28);


	tf = time(NULL);
	tspan = tf - ti;
	user.timeTaken = (int)tspan;
	user.score = user.numberTurns * 10 + user.timeTaken;
	cout << '\n' << "Time taken: " << (int)tspan << " seconds" << '\n';
	cout << '\n' << "Number of turns: " << user.numberTurns << "." << '\n';
	cout << '\n' << "Final score: " << user.score << " points." << '\n';

	//SCORE DEFINITION
	//FIRST FASE
	ofstream fout("best_temp.txt");
	ifstream fin("best.txt");
	scoreUpdateFirstFase(fin, fout, user);

	//SECOND FASE
	ifstream fin_final("best_temp.txt");
	ofstream fout_final("best.txt");
	scoreUpdateSecondFase(fout_final, fin_final, user);

	remove("best_temp.txt");

	return true;
}

//Displays game menu---------------------------------------------------------------
bool displayMenu()
{
	vector<string> menu;
	cout << '\n' << '\n';
	menu.push_back("||||        ||||   =============   ||              ||  ||            ||");
	menu.push_back("|| ||      || ||   ||              ||||            ||  ||            ||");
	menu.push_back("||   ||  ||   ||   ||              ||  ||          ||  ||            ||");
	menu.push_back("||     ||     ||   ||              ||    ||        ||  ||            ||");
	menu.push_back("||            ||   =============   ||      ||      ||  ||            ||");
	menu.push_back("||            ||   ||              ||        ||    ||  ||            ||");
	menu.push_back("||            ||   ||              ||          ||  ||  ||            ||");
	menu.push_back("||            ||   ||              ||            ||||    ||        ||  ");
	menu.push_back("||            ||   =============   ||              ||      ||||||||    ");

	for (auto row : menu)
	{
		for (auto col : row)
			cout << col;
		cout << endl;
	}

	string answerInstructions;
	char menuOption;
	bool checkSize;

	do
	{
		cout << '\n' << '\n' << "PRESS: " << '\n' << "1 - PLAY" << '\n' << "2 - INSTRUCTIONS" << '\n' << "3 - QUIT" << '\n';
		getline(cin, answerInstructions);
		istringstream userInstructionsInput(answerInstructions);
		userInstructionsInput >> menuOption;
		if (answerInstructions.size() == 1 && (menuOption == '1' || menuOption == '2' || menuOption == '3'))
		{
			checkSize = true;
		}
		else
		{
			checkSize = false;
			cout << '\n' << "Please choose one of the options - 1, 2 or 3.";
		}
	} while (!checkSize);

	if (menuOption == '2')
	{
		cout << '\n' << '\n' << "All the tiles will be face-down. Write the coordinates and start turning them over to find matches. To choose a Tile, enter the first and second coordinates with the symbol '|' in between. A match, in this case, is a pair of tiles that totals 12 points. A 6-6 plus a 0-0, for example, or a 1-3 plus a 3-5. If the two tiles you turn over add up to 12, they are removed from the table, being presented with '-|-'. Keep going until you've found all the matches. A combination of the number of attempts you required and the time taken is your score. The fewer points you have the better. Good Luck!" << '\n' << '\n';
	}
	else if (menuOption == '3')
		return false;

	string answer;
	char userStartGameDecision;
	bool startGameTest = true;

	do
	{
		cout << '\n' << "Ready to play? Press 'y'. If you want to quit, press 'n'." << '\n';
		getline(cin, answer);
		istringstream userAnswerStartGame(answer);
		userAnswerStartGame >> userStartGameDecision;
		userStartGameDecision = toupper(userStartGameDecision);
		if ((userStartGameDecision == 'Y' || userStartGameDecision == 'N') && answer.size() == 1)
		{
			startGameTest = true;
		}
		else
		{
			startGameTest = false;
			cout << "Please insert valid character (y or n).";
		}

	} while (!startGameTest);

	if (userStartGameDecision == 'N')
		return false;
	else
		return true;
}

//Swaps values---------------------------------------------------------------------
template<typename T>
void swapValues(T &x, T &y)
{
	T temp = x;
	x = y;
	y = temp;
}

//Generate deck--------------------------------------------------------------------
void generateDeck(Deck &setOfTiles, Board totalSet)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			setOfTiles.data.push_back(totalSet.data.at(i).at(j));
		}
	}
}

//Randomizes board with swap function----------------------------------------------
void randomizeBoard(vector<vector<Tile>> &board)
{
	int k, l;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			k = rand() % (4); //random number between 0 and 3
			l = rand() % (7); //random number between 0 and 6
			swapValues(board.at(i).at(j), board.at(k).at(l));
		}
	}
}

//Generate board-------------------------------------------------------------------
void generateTiles(vector<vector<Tile>> &board)
{
	Tile T = { 1,1 };
	int k(0), l(0);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			if (l == 7)
			{
				k++;
				l = k;
			}
			T.topValue = l;
			T.downValue = k;
			T.visible = false;
			T.onBoard = true;

			l = l + 1;
			board.at(i).at(j) = T;
		}
	}

}

//shows tile-----------------------------------------------------------------------
void showTile(Tile T)
{
	setColor(79);
	cout << ' ' << T.topValue << '|' << T.downValue << ' ';
	setColor(15);
	cout << ' ';
}

//color of tiles-------------------------------------------------------------------
void setColor(unsigned int color)
{
	HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hCon, color);
}

//show board visible---------------------------------------------------------------
void showBoard(vector<vector<Tile>> &board)
{
	cout << setw(5) << "a" << setw(6) << "b" << setw(6) << "c" << setw(6) << "d" << setw(6) << "e" << setw(6) << "f" << setw(6) << "g" << '\n';
	char L;
	for (size_t i = 0; i < 4; i++)
	{
		L = (char)i + 65;
		cout << L << ' ';
		for (size_t j = 0; j < 7; j++)
			showTile(board.at(i).at(j));
		cout << '\n' << '\n';
	}
}

//show board hidden----------------------------------------------------------------
int showHiddenBoard(vector<vector<Tile>> &board)
{
	cout << setw(5) << "a" << setw(6) << "b" << setw(6) << "c" << setw(6) << "d" << setw(6) << "e" << setw(6) << "f" << setw(6) << "g" << '\n'; //CONSIDERAR CICLO
	char Line;
	int sum(0), lineIndexFirst, lineIndexSecond, columnIndexFirst, columnIndexSecond, tileCounter(1), visibleCounter(0), endGameCounter(0);

	for (size_t i = 0; i < 4; i++)
	{
		Line = (char)i + 65; //Line grid char
		cout << Line << ' ';
		for (size_t j = 0; j < 7; j++)
			if (!((board.at(i).at(j)).onBoard))
			{
				setColor(240);
				cout << " -|- ";
				setColor(15);
				cout << ' ';
				endGameCounter++;
			}
			else if ((board.at(i).at(j)).visible)
			{
				showTile(board.at(i).at(j));
				sum += (board.at(i).at(j)).topValue + (board.at(i).at(j)).downValue;
				if (tileCounter == 1)
				{
					lineIndexFirst = i;
					columnIndexFirst = j;
					tileCounter++;
				}
				else
				{
					lineIndexSecond = i;
					columnIndexSecond = j;
					tileCounter = 1;
				}
				visibleCounter++;
			}
			else
			{
				cout << " X|X  ";
			}

		cout << '\n' << '\n';
	}
	if (2 == visibleCounter)
	{
		if (12 == sum)
		{
			cout << "Sum is 12!" << '\n';
			board.at(lineIndexFirst).at(columnIndexFirst).onBoard = false;
			board.at(lineIndexSecond).at(columnIndexSecond).onBoard = false;
		}
		else
		{
			cout << "Sum is not 12." << '\n';
			board.at(lineIndexFirst).at(columnIndexFirst).visible = false;
			board.at(lineIndexSecond).at(columnIndexSecond).visible = false;
		}
	}

	return endGameCounter;
}

//player choice------------------------------------------------------------------
int playerTileChoice(vector<vector<Tile>> &board)
{
	char separator;
	int i, j;
	bool testUserInput;
	static int count(0);
	int k = 1;

	for (int k = 1; k < 3; k++) //K < 3!!
	{

		do
		{

			cout << '\n' << "Choose tile " << k << " (line|column):" << '\n';
			testUserInput = inputErrors(i, j, separator, board);
			if (testUserInput == false)
				cout << '\n' << "Please enter a valid set of coordinates - example: 'A|b'." << '\n';

		} while (testUserInput == false);

		board.at(i).at(j).visible = true;
		showHiddenBoard(board); //Shows Tile when chosen
	}
	count++;
	cout << "Number of turns: " << count << '\n';
	return count;
}

//User input errors check
bool inputErrors(int &coordLine, int &coordColumn, char &separator, vector<vector<Tile>> &board)
{
	string line;
	char firstCoord, secondCoord;
	//bool test = false;

	getline(cin, line);

	istringstream userInput(line);

	userInput >> firstCoord;
	coordLine = firstCoord - 'A';
	if (!(coordLine <= 3 && coordLine >= 0) || line.size() > 3)
		return false;

	userInput >> separator;
	if (separator != '|')
		return false;

	userInput >> secondCoord;
	coordColumn = secondCoord - 'a';
	if (!(coordColumn <= 6 && coordColumn >= 0))
		return false;

	if (board.at(coordLine).at(coordColumn).visible == true) //If onboard = true, then visible also true
		return false;

	return true;
}

//get name and number of turns from string-----------------------------------------
void getInfoFromString(const string &scoreLine, Player &player)
{
	size_t nameTurnsSeparator = scoreLine.find_last_of(":");
	size_t endOfString = scoreLine.find_last_of(".");
	player.name = scoreLine.substr(0, nameTurnsSeparator);
	string score = scoreLine.substr(nameTurnsSeparator + 1, endOfString);
	int scoreInt = stoi(score);
	player.score = scoreInt;
}

//sort players by turns------------------------------------------------------------
void sortPlayersByTurns(vector<Player> &group)
{
	for (size_t i = 0; i < group.size() - 1; i++)
	{
		for (size_t j = 0; j < group.size() - i - 1; j++)
		{
			if (group.at(j).score > group.at(j + 1).score) //sort by score
				swapValues(group.at(j), group.at(j + 1));
		}
	}
}

//FIRST FASE-----------------------------------------------------------------------
void scoreUpdateFirstFase(ifstream& mainFile, ofstream& tempFile, Player user)
{

	vector<Player> groupOfPlayers;
	Player fillingPlayer;

	string score_content_final, score_Line;

	while (getline(mainFile, score_Line)) //SAVES INFO PLAYERS 
	{
		getInfoFromString(score_Line, fillingPlayer);
		groupOfPlayers.push_back(fillingPlayer);
	}

	groupOfPlayers.push_back(user); //ADD USER TO GROUP OF PLAYERS
	sortPlayersByTurns(groupOfPlayers); //SCORE SORTING

	for (size_t j = 0; j < groupOfPlayers.size(); j++) //SAVES RESULTS
	{
		tempFile << left << setw(20) << groupOfPlayers.at(j).name << ": " << setw(4) << groupOfPlayers.at(j).score << ".\n";
	}

	mainFile.close();
	tempFile.close();

}

//SECOND FASE----------------------------------------------------------------------
void scoreUpdateSecondFase(ofstream& mainFile, ifstream& tempFile, Player user)
{

	string score_Line_Final;
	vector<string> content_Score_Final;

	while (getline(tempFile, score_Line_Final))
	{
		content_Score_Final.push_back(score_Line_Final);
	}

	for (size_t j = 0; (j < 11) && (j < content_Score_Final.size()); j++) //TOP TEN
	{
		mainFile << content_Score_Final[j] << '\n';
	}

	mainFile.close();
	tempFile.close();
}