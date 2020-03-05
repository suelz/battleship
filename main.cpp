#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include "utils.h"


using namespace std;

const char * INPUT_ERROR_STRING = "input error, try again";

enum {
	AIRCRAFT_CARRIER_SIZE = 5,
	BATTLESHIP_SIZE = 4,
	CRUISER_SIZE = 3,
	DESTROYER_SIZE = 3,
	SUBMARINE_SIZE = 2,
	BOARD_SIZE = 10,
	NUM_SHIPS = 5,
	PLAYER_NAME_SIZE = 8,
	MAX_SHIP_SIZE = AIRCRAFT_CARRIER_SIZE
};
enum ShipType {
	ST_NONE = 0,
	ST_AIRCRAFT_CARRIER,
	ST_BATTLESHIP,
	ST_CRUISER,
	ST_DESTROYER,
	ST_SUBMARINE
};

enum ShipOrientationType {
	SO_HORIZONTAL = 0,
	SO_VERTICAL
};

struct ShipPositionType {
	int row;
	int col;
};

struct Ship {
	ShipType shipType;
	int shipSize;
	ShipOrientationType orientation;
	ShipPositionType position;
};


enum GuessType {
	GT_NONE = 0,
	GT_MISSED,
	GT_HIT
};

struct ShipPartType {
	ShipType shipType;
	bool isHit;
};

enum PlayerType {
	PT_HUMAN = 0,
	PT_AI
};

struct Player {
	PlayerType playerType;
	char playerName[PLAYER_NAME_SIZE];
	Ship ships[NUM_SHIPS];
	GuessType guessBoard[BOARD_SIZE][BOARD_SIZE];
	ShipPartType shipBoard[BOARD_SIZE][BOARD_SIZE];
};

void initializePlayer(Player & player, const char * playerName);
void initializeShip(Ship& ship, int shipSize, ShipType shipType);
void playGame(Player& player1, Player& player2);
bool wantToPlayAgain();
void setupBoards(Player & player);
void clearBoards(Player & player);
void drawBoards(const Player & player);

void drawSeparatorLine();
void drawColumnsRow();
void drawShipBoardRow(const Player& player, int row);
void drawGuessBoardRow(const Player& player, int row);
char getGuessRepresentation(const Player& player, int row, int col);
char getShipRepresentation(const Player& player, int row, int col);
const char * getShipNameForShipType(ShipType shipType);

ShipPositionType getBoardPosition();
ShipOrientationType getShipOrientation();

bool isValidPlacement(const Player& player, const Ship& currentShip, const ShipPositionType& shipPosition, ShipOrientationType orientation);

void placeShipOnBoard(Player& player, Ship& currentShip, const ShipPositionType& shipPosition, ShipOrientationType orientation);

ShipType updateBoards(ShipPositionType guess, Player& currentPlayer, Player& otherPlayer);

bool isGameOver(const Player& player1, const Player& player2);
bool isSunk(const Player& player, const Ship& ship);
bool areAllShipsSunk(const Player& player);
void switchPlayers(Player ** currentPlayer, Player ** otherPlayer);
PlayerType getPlayer2Type();
void displayWinner(const Player& player1, const Player& player2);


ShipPositionType getAIGuess(const Player& currentPlayer);
void setupAIBoards(Player& player);






int main() {
	srand(time(NULL));
	Player player1;
	Player player2;

	initializePlayer(player1, "Player1");
	initializePlayer(player2, "Player2");

	do {

		playGame(player1, player2);


	} while (wantToPlayAgain());

	return 0;
}

void initializePlayer(Player & player, const char * playerName) {
	if (playerName != nullptr && strlen(playerName) > 0) {
		strcpy_s(player.playerName, playerName);
	}
	initializeShip(player.ships[0], AIRCRAFT_CARRIER_SIZE, ST_AIRCRAFT_CARRIER);
	initializeShip(player.ships[1], BATTLESHIP_SIZE, ST_BATTLESHIP);
	initializeShip(player.ships[2], CRUISER_SIZE, ST_CRUISER);
	initializeShip(player.ships[3], DESTROYER_SIZE, ST_DESTROYER);
	initializeShip(player.ships[4], SUBMARINE_SIZE, ST_SUBMARINE);


	
}

void initializeShip(Ship & ship, int shipSize, ShipType shipType) {
	ship.shipType = shipType;
	ship.shipSize = shipSize;
	ship.position.row = 0;
	ship.position.col = 0;
	ship.orientation = SO_HORIZONTAL;
}

void playGame(Player& player1, Player& player2) {
	clearScreen();

	player1.playerType = PT_HUMAN;
	player2.playerType = getPlayer2Type();

	setupBoards(player1);
	setupBoards(player2);

	Player * currentPlayer = &player1;
	Player * otherPlayer = &player2;

	ShipPositionType guess;
	do {
		if (currentPlayer->playerType == PT_HUMAN) {
			drawBoards(*currentPlayer);
		}
		
		bool isValidGuess;

		do {
			if (currentPlayer->playerType == PT_HUMAN) {
				cout << currentPlayer->playerName << "what is you guess: " << endl;
				guess = getBoardPosition();
			}
			else {
				guess = getAIGuess(*currentPlayer);
			}

			isValidGuess = currentPlayer->guessBoard[guess.row][guess.col] == GT_NONE;

			if (!isValidGuess && currentPlayer->playerType == PT_HUMAN) {
				cout << "not a valid guess try again" << endl;
			}
		} while (!isValidGuess);

		ShipType type = updateBoards(guess, *currentPlayer, *otherPlayer);


		if (currentPlayer->playerType == PT_AI) {
			drawBoards(*otherPlayer);
			cout << currentPlayer->playerName << "row: " << char(guess.row + 'A') << " ,col: " << guess.col + 1 << endl;
		}
		else {
			drawBoards(*currentPlayer);
		}
	
		if (type != ST_NONE && isSunk(*otherPlayer, otherPlayer->ships[type - 1])) {
			if (currentPlayer->playerType == PT_AI) {
				cout << currentPlayer->playerName << " sunk your" << getShipNameForShipType(type) << "!" << endl;

			}
			else {
				cout << "You sunk " << otherPlayer->playerName << "'s " << getShipNameForShipType(type) << "!" << endl;

			}
			
		}

		waitForKeyPress();
		switchPlayers(&currentPlayer, &otherPlayer);

	} while (!isGameOver(player1, player2));

	displayWinner(player1, player2);


}

void displayWinner(const Player& player1, const Player& player2) {
	if (areAllShipsSunk(player1)) {
		cout << player2.playerName << " is the winner";
	}
	else{
		cout << player1.playerName << " is the winner";
	}
	
}


void switchPlayers(Player ** currentPlayer, Player ** otherPlayer) {
	Player * temp = *currentPlayer;
	*currentPlayer = *otherPlayer;
	*otherPlayer = temp;
	
}


bool isSunk(const Player& player, const Ship& ship) {
	if (ship.orientation == SO_HORIZONTAL) {
		for (int c = ship.position.col; c < (ship.position.col + ship.shipSize); c++) {
			if (!player.shipBoard[ship.position.row][c].isHit) {
				return false;
			}
		}
	}
	else {
		for (int r = ship.position.row; r < (ship.position.row + ship.shipSize); r++) {
			if (!player.shipBoard[r][ship.position.col].isHit) {
				return false;
			}
		}
	}
	return true;
}

bool areAllShipsSunk(const Player& player) {
	for (int i = 0; i < NUM_SHIPS; i++) {
		if (!isSunk(player, player.ships[i])) {
			return false;
		}
	}
	return true;
}

bool isGameOver(const Player& player1, const Player& player2) {
	return areAllShipsSunk(player1) || areAllShipsSunk(player2);
}


ShipType updateBoards(ShipPositionType guess, Player& currentPlayer, Player& otherPlayer) {
	if (otherPlayer.shipBoard[guess.row][guess.col].shipType != ST_NONE) {
		//hit
		currentPlayer.guessBoard[guess.row][guess.col] = GT_HIT;
		otherPlayer.shipBoard[guess.row][guess.col].isHit = true;
	}
	else {
		currentPlayer.guessBoard[guess.row][guess.col] = GT_MISSED;
	}

	return otherPlayer.shipBoard[guess.row][guess.col].shipType;
}



const char * getShipNameForShipType(ShipType shipType) {
	if (shipType == ST_AIRCRAFT_CARRIER) {
		return "Aircraft Carrier";
	}
	else if (shipType == ST_BATTLESHIP) {
		return "Battleship";
	}
	else if (shipType == ST_CRUISER) {
		return "Cruiser";
	}
	else if (shipType == ST_DESTROYER) {
		return "Destroyer";
	}
	else if (shipType == ST_SUBMARINE) {
		return "Submarine";
	}
	else {
		return "None";
	}
}

void setupBoards(Player & player) {
	clearBoards(player);
	if (player.playerType == PT_AI) {
		setupAIBoards(player);
		return;
	}
	for (int i = 0; i < NUM_SHIPS; i++) {
		drawBoards(player);
		Ship& currentShip = player.ships[i];

		ShipPositionType shipPosition;
		ShipOrientationType orientation; 

		bool isValid = false;

		do {
			cout << player.playerName << " set position and orientation for you ship " << getShipNameForShipType(currentShip.shipType) << endl;

			shipPosition = getBoardPosition(); 
			orientation = getShipOrientation();

			isValid = isValidPlacement(player, currentShip, shipPosition, orientation);

			if (!isValid) {
				cout << "not a valid placement please try again" << endl;
				waitForKeyPress();
			}
		} while (!isValid);

		placeShipOnBoard(player, currentShip, shipPosition, orientation);
	}

	drawBoards(player);
	waitForKeyPress();
	
}

ShipPositionType MapBoardPosition(char rowInput, int colInput) {
	int realRow = rowInput - 'A';
	int realCol = colInput - 1;
	ShipPositionType boardPosition;
	boardPosition.row = realRow;
	boardPosition.col = realCol;
	return boardPosition;
}

ShipPositionType getBoardPosition() {
	char rowInput;
	int colInput;

	const char validRowInput[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J' };
	const int validColumnInput[] = { 1,2,3,4,5,6,7,8,9,10 };

	rowInput = getCharacter("Please enter a row (A-J): ", INPUT_ERROR_STRING, validRowInput, BOARD_SIZE, CC_UPPER_CASE);
	colInput = getInteger("Please enter a col (1-10): ", INPUT_ERROR_STRING, validColumnInput, BOARD_SIZE );
	
	return MapBoardPosition(rowInput, colInput);
}

ShipOrientationType getShipOrientation() {
	ShipOrientationType orientation;

	const char validInput[2] = { 'H', 'V' };
	char input = getCharacter("please choose an orientation (H/V): ", INPUT_ERROR_STRING, validInput, 2, CC_UPPER_CASE);
	if (input == validInput[0]) {
		return SO_HORIZONTAL;
	}
	else {
		return SO_VERTICAL;
	}
}

bool isValidPlacement(const Player& player, const Ship& currentShip, const ShipPositionType& shipPosition, ShipOrientationType orientation) {
	if (orientation == SO_HORIZONTAL) {
		for (int c = shipPosition.col; c < (shipPosition.col + currentShip.shipSize); c++) {
			if (player.shipBoard[shipPosition.row][c].shipType != ST_NONE || c >= BOARD_SIZE) {
				return false;
			}
		}
	}
	else {
		for (int r = shipPosition.row; r < (shipPosition.row + currentShip.shipSize); r++) {
			if (player.shipBoard[r][shipPosition.col].shipType != ST_NONE || r >= BOARD_SIZE) {
				return false;
			}
		}
	}
	return true;
}

void placeShipOnBoard(Player& player, Ship& currentShip, const ShipPositionType& shipPosition, ShipOrientationType orientation) {
	currentShip.position = shipPosition;
	currentShip.orientation = orientation;

	if (orientation == SO_HORIZONTAL) {
		for (int c = shipPosition.col; c < (shipPosition.col + currentShip.shipSize); c++) {
			player.shipBoard[shipPosition.row][c].shipType = currentShip.shipType;
			player.shipBoard[shipPosition.row][c].isHit = false;
		}
	}
	else {
		for (int r = shipPosition.row; r < (shipPosition.row + currentShip.shipSize); r++) {
			player.shipBoard[r][shipPosition.col].shipType = currentShip.shipType;
			player.shipBoard[r][shipPosition.col].isHit = false;
		}

	}
}



void clearBoards(Player & player) {
	for (int r = 0; r < BOARD_SIZE; r++) {
		for (int c = 0; c < BOARD_SIZE; c++) {
			player.guessBoard[r][c] = GT_NONE;
			player.shipBoard[r][c].shipType = ST_NONE;
			player.shipBoard[r][c].isHit = false;
		}
	}
}

void drawSeparatorLine() {
	cout << " ";

	for (int c = 0; c < BOARD_SIZE; c++) {
		cout << "+---";
	}

}

char getShipRepresentation(const Player& player, int row, int col) {
	if (player.shipBoard[row][col].isHit) {
		return '*';
	}
	if (player.shipBoard[row][col].shipType == ST_AIRCRAFT_CARRIER) {
		return 'A';
	}
	else if (player.shipBoard[row][col].shipType == ST_BATTLESHIP) {
		return 'B';
	}
	else if (player.shipBoard[row][col].shipType == ST_CRUISER) {
		return 'C';
	}
	else if (player.shipBoard[row][col].shipType == ST_DESTROYER) {
		return 'D';
	}
	else if (player.shipBoard[row][col].shipType == ST_SUBMARINE) {
		return 'S';
	}
	else {
		return ' ';
	}
}

char getGuessRepresentation(const Player& player, int row, int col) {
	if (player.guessBoard[row][col] == GT_HIT) {
		return '*';
	}
	else if (player.guessBoard[row][col] == GT_MISSED) {
		return 'o';
	}
	else {
		return ' ';
	}
}

void drawShipBoardRow(const Player & player, int row) {
	char rowName = row + 'A';
	cout << rowName << "|";
	for (int c = 0; c < BOARD_SIZE; c++) {
		cout << " " << getShipRepresentation(player, row, c) << " |";
	}
}

void drawGuessBoardRow(const Player& player, int row) {
	char rowName = row + 'A';
	cout << rowName << "|";
	for (int c = 0; c < BOARD_SIZE; c++) {
		cout << " " << getGuessRepresentation(player, row, c) << " |";
	}
}

void drawColumnsRow() {
	cout << "  ";
	for (int c = 0; c < BOARD_SIZE; c++) {
		int columnName = c + 1;
		cout << " " << columnName << "  ";
	}

}

void drawBoards(const Player & player) {
	clearScreen();

	drawColumnsRow();
	drawColumnsRow();

	cout << endl;

	for (int r = 0; r < BOARD_SIZE; r++) {
		drawSeparatorLine();
		cout << " "; 
		drawSeparatorLine();
		cout << endl;
		drawShipBoardRow(player, r);
		cout << " ";
		drawGuessBoardRow(player, r);
		cout << endl;

	}
	drawSeparatorLine();
	cout << " ";
	drawSeparatorLine();
	cout << endl;

}


PlayerType getPlayer2Type() {
	const int validInputs[2] = { 1, 2 };
	int input = getInteger("Who would you like to play? \n1. Human\n2. AI\n", INPUT_ERROR_STRING, validInputs, 2);
	if (input == 1) {
		return PT_HUMAN;
	}
	else {
		return PT_AI;
	}
}

ShipPositionType getRandomPosition() {
	ShipPositionType guess;
	guess.row = rand() % BOARD_SIZE;
	guess.col = rand() % BOARD_SIZE;
	return guess;
}
ShipPositionType getAIGuess(const Player& currentPlayer) {
	return getRandomPosition();
}

void setupAIBoards(Player& player) {
	ShipPositionType pos;
	ShipOrientationType orientation;

	for (int i = 0; i < NUM_SHIPS; i++) {
		Ship& currentShip = player.ships[i];

		do {
			pos = getRandomPosition();
			orientation = ShipOrientationType(rand() % 2);
		} while (!isValidPlacement(player, currentShip, pos, orientation));

		placeShipOnBoard(player, currentShip, pos, orientation);
	}
}





bool wantToPlayAgain() {
	char input;

	const char validInput[2] = { 'y', 'n' };

	input = getCharacter("Would you like to play again? (y/n): ", INPUT_ERROR_STRING, validInput, 2, CC_LOWER_CASE);

	return input == 'y';
}


