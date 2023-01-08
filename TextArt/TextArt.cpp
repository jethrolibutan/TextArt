/*
* This program allows display and editing of text art (also called ASCII art).
*/

#include <iostream>
#include <fstream>
#include <cctype>
#include <windows.h>
#include <conio.h>
#include "Definitions.h"
using namespace std;


int main()
{

	system("cls");

	// creating the pointer to display the current nodes canvas
	Node* current;

	// creating a List for undo functionality
	List undoList;

	// creating a List for redo functionality
	List redoList;

	// creating a List for animation clips
	List clips;

	// initializing animate value 
	bool animate = false;

	char userInput = 'x';
	char menu[] = "<E>dit / <M>ove / <R>eplace / <D>raw / <C>lear / <L>oad / <S>ave / <Q>uit: ";
	int menuLen = strlen(menu);

	// initializing and filling a blank canvas
	current = newCanvas();



	// Displays user interface untill user quits
	while (userInput != 'Q' && userInput != 'q')
	{
		char oldCh, newCh;
		int rowVal, colVal;

		// clears all possible menu and input lines (5)
		clearLine(MAXROWS + 1, menuLen + 50);
		clearLine(MAXROWS + 2, menuLen + 50);
		clearLine(MAXROWS + 3, menuLen + 50);
		clearLine(MAXROWS + 4, menuLen + 50);
		clearLine(MAXROWS + 5, menuLen + 50);

		// displays canvas through each loop to update changes
		displayCanvas(current->item);


		// displays redo count if the redo exists, and animate if clips exist 
		// I wanted to make another function for this
		displayMenu(undoList.count, redoList.count, clips.count, animate);
		printf(menu);



		// gets input from user
		cin >> userInput;
		cin.ignore();



		while (cin.fail())
		{
			// clears menu for other functions to display prompts for input
			clearLine(MAXROWS + 1, menuLen + 50);
			clearLine(MAXROWS + 2, menuLen + 50);

			displayMenu(undoList.count, redoList.count, clips.count, animate);
			printf(menu);

			cin >> userInput;
			cin.clear();
			cin.ignore();
		}

		

		// Switch statement to send users input to the desired function
		switch (userInput)
		{
		case 'a':
		case 'A':
		{
			animate = !(animate);

			break;
		}
		case 'E':
		case 'e':
		{
			// stores backup canvas for Undo

			// adds undo state for undo functionality
			addUndoState(undoList, redoList, current);

			editCanvas(current->item);

			// increments undoCount for menu
			undoList.count++;

			break;
		}
		case 'M':
		case 'm':
		{
			// adds undo state for undo functionality
			addUndoState(undoList, redoList, current);

			// clears menu for other functions to display prompts for input
			clearLine(MAXROWS + 1, menuLen + 50);
			clearLine(MAXROWS + 2, menuLen + 50);

			cout << "How many rows would you like to move the function?: ";
			cin >> rowVal;

			// clears menu for other functions to display prompts for input
			clearLine(MAXROWS + 1, menuLen + 50);
			clearLine(MAXROWS + 2, menuLen + 50);

			cout << "How many columns would you like to move the function?: ";
			cin >> colVal;

			moveCanvas(current->item, rowVal, colVal);

			break;
		}


		case 'R':
		case 'r':
		{
			// adds undo state for undo functionality
			addUndoState(undoList, redoList, current);

			// clears menu for other functions to display prompts for input
			clearLine(MAXROWS + 1, menuLen + 50);
			clearLine(MAXROWS + 2, menuLen + 50);

			cout << "What character do you want to replace?: ";
			cin.get(oldCh);
			cin.ignore();

			// clears menu for other functions to display prompts for input
			clearLine(MAXROWS + 1, menuLen + 50);
			clearLine(MAXROWS + 2, menuLen + 50);

			cout << "With what character do you want to replace the old character with?: ";
			cin.get(newCh);
			cin.ignore();

			// clears menu for other functions to display prompts for input
			clearLine(MAXROWS + 1, menuLen + 50);
			clearLine(MAXROWS + 2, menuLen + 50);

			replace(current->item, oldCh, newCh);


			break;
		}


		case 'd':
		case 'D':
		{
			// takes user to the second menu
			menuTwo(current, undoList, redoList, clips, animate);
			break;
		}


		case 'c':
		case 'C':
		{
			// adds undo state for undo functionality
			addUndoState(undoList, redoList, current);

			initCanvas(current->item);
			break;
		}


		case 'U':
		case 'u':
		{
			// copies undo head to current canvas
			restore(undoList, redoList, current);

			break;
		}


		case 'L':
		case 'l':
		{
			// adds undo state for undo functionality
			addUndoState(undoList, redoList, current);

			char fileName[FILENAMESIZE];
			char fileLocation[FILENAMESIZE + 15] = "SavedFiles/";
			char fileExtension[5] = ".txt";

			// char for storing user input for type of load
			char typeOfLoad;

			// clears menu 
			clearLine(MAXROWS + 1, menuLen + 50);
			clearLine(MAXROWS + 2, menuLen + 50);
			clearLine(MAXROWS + 3, menuLen + 50);


			// user input
			cout << "Type the name of the file you would like to load: ";
			cin.getline(fileName, FILENAMESIZE);

			// concatenates string to open file in open function 
			strncat_s(fileLocation, FILENAMESIZE, fileName, FILENAMESIZE);
			strncat_s(fileLocation, FILENAMESIZE, fileExtension, sizeof(fileExtension));

			// asks user if they want to save a canvas or an Animation
			cout << "<C>anvas or <A>nimation?: ";
			cin >> typeOfLoad;

			// if user inputs invalid input 
			while (typeOfLoad != 'c' && typeOfLoad != 'C' && typeOfLoad != 'A' && typeOfLoad != 'a')
			{
				cout << "<C>anvas or <A>nimation?: ";
				cin >> typeOfLoad;
				cin.clear();
				cin.ignore();
			}

			// gets multiple canvases 
			if (typeOfLoad == 'a' || typeOfLoad == 'A')
			{
				loadClips(clips, fileName);
			}
			else
			{
				if (!loadCanvas(current->item, fileLocation))
				{
					cerr << "ERROR: File could not be read";
					system("pause");
				}
			}

			break;
		}
		case 'S':
		case 's':
		{	
			char fileName[FILENAMESIZE];
			char fileLocation[FILENAMESIZE + 15] = "SavedFiles/";
			char fileExtension[5] = ".txt";

			// char for storing user input for type of save
			char typeOfSave;

			// clears lines for user input
			clearLine(MAXROWS + 1, menuLen + 50);
			clearLine(MAXROWS + 2, menuLen + 50);

			// user input
			cout << "Type your desired file name (without .txt): ";
			cin.getline(fileName, FILENAMESIZE);

			// asks user if they want to save a canvas or an Animation
			cout << "<C>anvas or <A>nimation?: ";
			cin >> typeOfSave;

			// if user inputs invalid input 
			while (typeOfSave != 'c' && typeOfSave != 'C' && typeOfSave != 'A' && typeOfSave != 'a')
			{
				cout << "<C>anvas or <A>nimation?: ";
				cin >> typeOfSave;
				cin.clear();
				cin.ignore();
			}

			// concatenates string to open file in open function 
			strncat_s(fileLocation, FILENAMESIZE, fileName, FILENAMESIZE);
			strncat_s(fileLocation, FILENAMESIZE, fileExtension, sizeof(fileExtension));

			// gets multiple canvases 
			if (typeOfSave == 'a' || typeOfSave == 'A')
			{
				saveClips(clips, fileName);
			}
			else
			{
				if (!saveCanvas(current->item, fileLocation))
				{
					cerr << "ERROR: File could not be saved";
					system("pause");
				}
			}
			break;
		}
		case 'I':
		case 'i':
		{

			// adds current node to linked list "clips"
			addNode(clips, newCanvas(current));

			break;

		}
		case 'P':
		case 'p':
		{
			// clears lines for user input
			clearLine(MAXROWS + 1, menuLen + 50);
			clearLine(MAXROWS + 2, menuLen + 50);

			// shows user the exit prompt and clip count 
			cout << "\nHold <ESC> to stop	";
			cout << "Clips: " << clips.count;

			// plays the animation 
			play(clips);

			break;
		}
		case 'O':
		case 'o':
		{
			restore(redoList, undoList, current);

			break; 
		}
		}
	}

	// deletes all of our current linked lists to prevent memory leak 
	deleteList(clips);
	deleteList(undoList);
	deleteList(redoList);

	// deletes the current node to prevent memory leak 
	delete current;

	// before the program ends, we need to have all the nodes deleted in the linked lists to prevent a memory leak
	return 0;
}

void gotoxy(short row, short col)
{
	COORD pos = { col, row };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}


void clearLine(int lineNum, int numOfChars)
{
	// Move cursor to the beginning of the specified line on the console
	gotoxy(lineNum, 0);

	// Write a specified number of spaces to overwrite characters
	for (int x = 0; x < numOfChars; x++)
		cout << ' ';

	// Move cursor back to the beginning of the line
	gotoxy(lineNum, 0);
}


void replace(char canvas[][MAXCOLS], char oldCh, char newCh)
{
	// cycles through array to replace the desired character
	for(int row = 0; row < MAXROWS; row++)
	{
		for(int col = 0; col < MAXCOLS; col++)
		{
			if(canvas[row][col] == oldCh)
			{
				canvas[row][col] = newCh;
			}
		}
	}
}


void editCanvas(char canvas[][MAXCOLS])
{
	char input;
	int row = 0, col = 0;

	
	// Move cursor to row,col and then get
	// a single character from the keyboard

	gotoxy(row, col);
	input = _getch();

	// ESC ends the edit functionality
	while (input != ESC )
	{
		if (input == SPECIAL)  // if input isnt traditional a-z or 1-9
		{
			input = _getch();

			switch (input)
			{
			case LEFTARROW:
				if (col > 0)
					col--;
				break;
			case RIGHTARROW:
				if (col < (MAXCOLS - 1))
					col++;
				break;
			case UPARROW:
				if (row > 0)
					row--;
				break;
			case DOWNARROW:
				if (row < (MAXCOLS - 1))
					row++;
				break;
			}
		}
		else if (input == '\0') 
		{
			input = _getch();
		}
		else if (input > 31 && input < 127)
		{
			canvas[row][col] = input;
			cout << input;
		}

		gotoxy(row, col);
		input = _getch();

		if (row > MAXROWS-2)
			row--;
		else if (col > MAXCOLS)
			col--;
	}
}


void moveCanvas(char canvas[][MAXCOLS], int rowValue, int colValue)
{

	char temp[MAXROWS][MAXCOLS];

	// copy canvas to a temp 
	for (int x = 0; x < MAXROWS; x++)
		for (int y = 0; y < MAXCOLS; y++)
			temp[x][y] = canvas[x][y];
	

	// copy temp back into canvas in new position 
	for (int x = 0; x < MAXROWS; x++)
		for (int y = 0; y < MAXCOLS; y++)
		{
			if (x >= rowValue && y >= colValue)  		// copies moved array
			{
				canvas[x][y] = temp[x - rowValue][y - colValue];
			}
			else
			{ 
				canvas[x][y] = ' ';                     // adds white space where is now empty
			}
		}

}


void initCanvas(char canvas[][MAXCOLS])
{
	// sets every array value to space
	for (int row = 0; row < MAXROWS; row++)
		for (int col = 0; col < MAXCOLS; col++)
			canvas[row][col] = ' ';
}


void displayCanvas(char canvas[][MAXCOLS])
{
	// system("cls");

	// display canvas and borders
	for (int x = 0; x < MAXROWS; x++)
	{
		for (int y = 0; y < MAXCOLS; y++)
		{
			gotoxy(x, y);
			cout << canvas[x][y];
		}

		cout << "|" << endl;
	}


	for (int i = 0; i < MAXCOLS; i++)
	{
		cout << "-";
	}
	
}


void copyCanvas(char to[][MAXCOLS], char from[][MAXCOLS])
{
	// copies canvas, mainly used for backupCanvas
	for(int i = 0; i < MAXROWS; i++)
		for(int j = 0; j < MAXCOLS; j++)
			to[i][j] = from[i][j];
}


bool saveCanvas(char canvas[][MAXCOLS], char fileLocation[])
{
	// opening inFile
	ofstream outFile;
	outFile.open(fileLocation);

	// if inFile doesnt open
	if (!outFile)
	{
		cout << "ERROR";
		system("pause");
		return false;
	}
	else if (outFile.is_open())
	{
		// if outFile works 
		for (int r = 0; r < MAXROWS; r++)
		{
			for (int c = 0; c < MAXCOLS; c++)
			{
				if (canvas[r][c] == '\0')
					outFile << " ";
				else
					outFile << canvas[r][c];
			}

			outFile << "\n";
		}

	}

	return true;
}


bool loadCanvas(char canvas[][MAXCOLS], char fileLocation[])
{
	char buffer[MAXCOLS + 1];

	// opening inFile
	ifstream inFile;
	inFile.open(fileLocation);



	int row = 0;

	// if inFile doesnt open
	if (!inFile)
	{
		return false;
	}
	else
	{

		// for each row in inFile
		do
		{
			// we get the line of the inFile and store it to tmp array 
			// 50 extra characters to ensure that the lines do not overlap

			inFile.getline(buffer, (MAXCOLS + 1));

			if (!inFile)
			{
				inFile.clear();
				inFile.ignore((numeric_limits<streamsize>::max)(), '\n');
			}

			int lineLength = strnlen(buffer, MAXCOLS + 1);

		
			if (lineLength < MAXCOLS)
			{
				for (int i = lineLength; i < MAXCOLS; i++)
				{
					buffer[i] = ' ';
				}
			}



			memcpy_s(canvas[row], MAXCOLS, buffer, MAXCOLS);
			row++;
		} while (!inFile.eof() && row < MAXROWS);
	}


	inFile.close();

	return true;
}



void displayMenu(int undoListCount, int redoListCount, int clipsCount, bool animate)
{
	//<A>nimate %c / <U>ndo: %d / Red<O>: %d / Cl<I>p: %d / <P>lay\n
	
	// prints the menu 
	cout << "\n<A>nimate: " << (animate ? 'Y' : 'N') << " / <U>ndo: " << undoListCount;
	
	if (redoListCount > 0)
	{
		cout << " / Red<O>: " << redoListCount;
	}

	cout << " / Cl<i>p: " << clipsCount;

	if (clipsCount > 1)
	{
		cout << " / <P>lay";
	}

	cout << endl;
}