
#include "utils.h"
#include <iostream>
#include <cctype>

using namespace std;

char getCharacter(const char * prompt, const char* error, characterCaseType charCase) {
	const int IGNORE_CHARS = 256;

	char input;
	bool failure;

	do
	{
		failure = false;

		cout << prompt;
		cin >> input;

		if (cin.fail())
		{
			cin.clear();
			cin.ignore(IGNORE_CHARS, '\n');
			cout << error << endl;
			failure = true;
		}
		else
		{
			cin.ignore(IGNORE_CHARS, '\n');

			if (charCase == CC_UPPER_CASE) {
				input = toupper(input);
			}
			else if (charCase == CC_LOWER_CASE) {
				input = tolower(input);
			}
			else
			{
				cout << error << endl;
				failure = true;
			}
		}

	} while (failure);

	return input;
}

char getCharacter(const char *prompt, const char* error, const char validInput[], int validInputLength, characterCaseType charCase) {

	const int IGNORE_CHARS = 256;

	char input;
	bool failure;

	do
	{
		failure = false;

		cout << prompt;
		cin >> input;

		if (cin.fail())
		{
			cin.clear();
			cin.ignore(IGNORE_CHARS, '\n');
			cout << error << endl;
			failure = true;
		}
		else
		{
			cin.ignore(IGNORE_CHARS, '\n');

			if (isalpha(input))
			{
				if (charCase == CC_UPPER_CASE) {
					input = toupper(input);
				}
				else if (charCase == CC_LOWER_CASE) {
					input = tolower(input);
				}


				for (int i = 0; i < validInputLength; i++)
				{
					if (input == validInput[i])
					{
						return input;
					}
				}
			}

			cout << error << endl;
			failure = true;

		}

	} while (failure);

	return input;
}

int getInteger(const char * prompt, const char * error, const int validInput[], int validInputLength)
{
	const int IGNORE_CHARS = 256;

	int input;
	bool inputFailure;

	do {

		inputFailure = false;
		cout << prompt;
		cin >> input;

		if (cin.fail()) {
			cin.clear();
			cin.ignore(IGNORE_CHARS, '\n');
			cout << error << endl;
			inputFailure = true;
		}
		else {
			for (int i = 0; i < validInputLength; i++) {
				if (input == validInput[i]) {
					return input;
				}
			}
		}

		cout << error << endl;
		inputFailure = true;
	} while (inputFailure);

	return -1;
}

void clearScreen() {
	system("cls");
}

void waitForKeyPress() {
	system("pause");
}
