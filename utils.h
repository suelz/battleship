#ifndef __UTILS_H__
#define __UTILS_H__

enum characterCaseType {
	CC_UPPER_CASE = 0,
	CC_LOWER_CASE,
	CC_EITHER
};

char getCharacter(const char * prompt, const char* error, characterCaseType charCase);

char getCharacter(const char *prompt, const char* error, const char validInput[], int validInputLength, characterCaseType charCase);

int getInteger(const char * prompt, const char* error, const int validInput[], int validInputLength);

void clearScreen();

void waitForKeyPress();



#endif // !__UTILS_H__
