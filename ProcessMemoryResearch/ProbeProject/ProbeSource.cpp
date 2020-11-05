#include <iostream>
#include <string>
#include <stdio.h>

using namespace std;
const char constString[] = "My name is Vlad";
char commonString[] = "My name is Vlad";

void main()
{
	string localString = "My name is Vlad";

	char consoleChar;
	consoleChar = getchar();
	while (consoleChar != 'e')
	{
		if (consoleChar == 'r') {
			cout << "constString - " << constString << endl;
			cout << "commonString - " << commonString << endl;
			cout << "localString - " << localString << endl;
		}
		consoleChar = getchar();
	}
}

