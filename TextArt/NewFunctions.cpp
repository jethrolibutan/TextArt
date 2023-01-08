#include <iostream>
#include <windows.h>
#include <conio.h>
#include "Definitions.h"
using namespace std;


Point::Point(DrawPoint p)
{
	row = (int)round(p.row);
	col = (int)round(p.col);
}

// https://math.stackexchange.com/questions/39390/determining-end-coordinates-of-line-with-the-specified-length-and-angle
DrawPoint findEndPoint(DrawPoint start, int len, int angle)
{
	DrawPoint end;
	end.col = start.col + len * cos(degree2radian(angle));
	end.row = start.row + len * sin(degree2radian(angle));
	return end;
}


// Use this to draw characters into the canvas, with the option of performing animation
void drawHelper(char canvas[][MAXCOLS], Point p, char ch, bool animate)
{
	// Pause time between steps (in milliseconds)
	const int TIME = 50;

	// Make sure point is within bounds
	if (p.row >= 0 && p.row < MAXROWS && p.col >= 0 && p.col < MAXCOLS)
	{
		// Draw character into the canvas
		canvas[p.row][p.col] = ch;

		// If animation is enabled, draw to screen at same time
		if (animate)
		{
			gotoxy(p.row, p.col);
			printf("%c", ch);
			Sleep(TIME);
		}
	}
}


// Fills gaps in a row caused by mismatch between match calculations and screen coordinates
// (i.e. the resolution of our 'canvas' isn't very good)
void drawLineFillRow(char canvas[][MAXCOLS], int col, int startRow, int endRow, char ch, bool animate)
{
	// determine if we're counting up or down
	if (startRow <= endRow)
		for (int r = startRow; r <= endRow; r++)
		{
			Point point(r, col);
			drawHelper(canvas, point, ch, animate);
		}
	else
		for (int r = startRow; r >= endRow; r--)
		{
			Point point(r, col);
			drawHelper(canvas, point, ch, animate);
		}
}


// Draw a single line from start point to end point
void drawLine(char canvas[][MAXCOLS], DrawPoint start, DrawPoint end, bool animate)
{
	char ch;

	Point scrStart(start);
	Point scrEnd(end);

	// vertical line
	if (scrStart.col == scrEnd.col)
	{
		ch = '|';

		drawLineFillRow(canvas, scrStart.col, scrStart.row, scrEnd.row, ch, animate);
	}
	// non-vertical line
	else
	{
		int row = -1, prevRow;

		// determine the slope of the line
		double slope = (start.row - end.row) / (start.col - end.col);

		// choose appropriate characters based on 'steepness' and direction of slope
		if (slope > 1.8)  ch = '|';
		else if (slope > 0.08)  ch = '`';
		else if (slope > -0.08)  ch = '-';
		else if (slope > -1.8) ch = '\'';
		else ch = '|';

		// determine if columns are counting up or down
		if (scrStart.col <= scrEnd.col)
		{
			// for each column from start to end, calculate row values
			for (int col = scrStart.col; col <= scrEnd.col; col++)
			{
				prevRow = row;
				row = (int)round(slope * (col - start.col) + start.row);

				// draw from previous row to current row (to fill in row gaps)
				if (prevRow > -1)
				{
					drawLineFillRow(canvas, col, prevRow, row, ch, animate);
				}
			}
		}
		else
		{
			// for each column from start to end, calculate row values
			for (int col = scrStart.col; col >= scrEnd.col; col--)
			{
				prevRow = row;
				row = (int)round(slope * (col - start.col) + start.row);

				// draw from previous row to current row (to fill in row gaps)
				if (prevRow > -1)
				{
					drawLineFillRow(canvas, col, prevRow, row, ch, animate);
				}
			}
		}
	}
}


// Draws a single box around a center point
void drawBox(char canvas[][MAXCOLS], Point center, int height, bool animate)
{
	int sizeHalf = height / 2;
	int ratio = (int)round(MAXCOLS / (double)MAXROWS * sizeHalf);

	// Calculate where the four corners of the box should be
	DrawPoint points[4];
	points[0] = DrawPoint(center.row - sizeHalf, center.col - ratio);
	points[1] = DrawPoint(center.row - sizeHalf, center.col + ratio);
	points[2] = DrawPoint(center.row + sizeHalf, center.col + ratio);
	points[3] = DrawPoint(center.row + sizeHalf, center.col - ratio);

	// Draw the four lines of the box
	for (int x = 0; x < 3; x++)
	{
		drawLine(canvas, points[x], points[x + 1], animate);
	}
	drawLine(canvas, points[3], points[0], animate);

	// Replace the corners with a better looking character
	for (int x = 0; x < 4; x++)
	{
		drawHelper(canvas, points[x], '+', animate);
	}
}


// Menu for the drawing tools
void menuTwo(Node*& current, List& undoList, List& redoList, List& clips, bool& animate)
{
	char userInput = 'x';

	do
	{
		// Getting the length of the menu for the clearline function
		char menu[] = "<F>ill / <L>ine / <B>ox / <N>ested Boxes / <T>ree / <M>ain Menu: ";
		int menuLen = strlen(menu);

		// clears menu for other functions to display prompts for input
		clearLine(MAXROWS + 1, menuLen + 50);
		clearLine(MAXROWS + 2, menuLen + 50);
		clearLine(MAXROWS + 3, menuLen + 50);
		clearLine(MAXROWS + 4, menuLen + 50);

		// displays urrent canvas
		displayCanvas(current->item);


		// displays menu
		displayMenu(undoList.count, redoList.count, clips.count, animate);
		printf(menu);

		cin >> userInput;
		cin.clear();
		cin.ignore();


		// if input fails
		while (cin.fail())
		{
			// clears menu for other functions to display prompts for input
			clearLine(MAXROWS + 1, menuLen + 50);
			clearLine(MAXROWS + 2, menuLen + 50);
			
			// displays menu and gets userInputs
			displayMenu(undoList.count, redoList.count, clips.count, animate);
			printf(menu);

			cin >> userInput;
			cin.clear();
			cin.ignore();
		}


		// switch statement for menu options 
		switch (userInput)
		{
		case 'a':
		case 'A':
		{
			animate = !(animate);

			break;
		}
		case 'f':
		case 'F':
		{
			Point fillPoint;

			// stores backup canvas for Undo
			// copyCanvas(backupCanvas, current->item);

			// clears menu for other functions to display prompts for input
			clearLine(MAXROWS + 1, menuLen + 50);
			clearLine(MAXROWS + 2, menuLen + 50);

			cout << "Type any letter to choose fill character / <ESC> to cancel";



			// gets pint and position from user 
			char userInput = getPoint(fillPoint);

			// if user input is ESC key, break out of switch statement
			if (userInput == ESC)
				break;

			// undo state for fill function
			addUndoState(undoList, redoList, current);

			// calls the function to fill the section of the canvas
			fillRecursive(current->item, fillPoint.row, fillPoint.col, current->item[fillPoint.row][fillPoint.col], userInput, animate);




			break;
		}
		case 't':
		case 'T':
		{
			Point treeStart;
			DrawPoint treeDrawStart;

			int height, branchAngle, trunkAngle;

			// adds undo state
			addUndoState(undoList, redoList, current);

			// clears menu for other functions to display prompts for input
			clearLine(MAXROWS + 1, menuLen + 50);
			clearLine(MAXROWS + 2, menuLen + 50);

			// prompts user for input
			cout << "Enter approximate tree height: ";
			cin >> height;

			clearLine(MAXROWS + 1, menuLen + 5);
			cout << "Enter branch angle: ";
			cin >> branchAngle;

			// trunk angle is always 270 (North)
			trunkAngle = 270;

			// clears menu for other functions to display prompts for input
			clearLine(MAXROWS + 1, menuLen + 50);
			clearLine(MAXROWS + 2, menuLen + 50);

			cout << "Type any letter to choose tree base, or <C> for bottom center / <ESC> to cancel";

			// gets point from user
			char treeStartChar = getPoint(treeStart);

			// if user presses C
			if (treeStartChar == 'C' || treeStartChar == 'c')
			{
				treeStart.row = MAXROWS;
				treeStart.col = MAXCOLS / 2;
			}

			treeDrawStart.row = treeStart.row;
			treeDrawStart.col = treeStart.col;


			// calls function to write the tree
			treeRecursive(current->item, treeDrawStart, height, trunkAngle, branchAngle, animate);

			break;
		}
		case 'n':
		case 'N':
		{
            int height = 0;
			Point boxStartCenter;
			Point center;

			// stores backup canvas for Undo
			// adds undo state
			addUndoState(undoList, redoList, current);


			// clears menu for other functions to display prompts for input
			clearLine(MAXROWS + 1, menuLen + 50);
			clearLine(MAXROWS + 2, menuLen + 50);

			cout << "Enter size of largest box: ";
			cin >> height;
		
			// clears menu for other functions to display prompts for input
			clearLine(MAXROWS + 1, menuLen + 50);
			clearLine(MAXROWS + 2, menuLen + 50);

			cout << "Type any letter to choose box center, or <C> for screen center / <ESC> to cancel";

			// allows user to choose location
			char boxStart = getPoint(boxStartCenter);

			// if user presses c
			if (boxStart == 'C' || boxStart == 'c')
			{
				center.row = MAXROWS / 2;
				center.col = MAXCOLS / 2;
			}

			// calls function to draw recursive nested boxes 
			drawBoxesRecursive(current->item, center, height, animate);


			break;
		}
		case 'L':
		case 'l':
		{
			DrawPoint start;
			DrawPoint end;

			Point startPoint;
			Point endPoint;
			
			// clears menu for other functions to display prompts for input
			clearLine(MAXROWS + 1, menuLen + 50);
			clearLine(MAXROWS + 2, menuLen + 50);

			cout << "Type any letter to choose starting point / (type ESC to cancel): ";
			char startLine = getPoint(startPoint);

			// breaks statement if user inputs ESC
			if (startLine == ESC)
				break;

			// places the character they choose in the spot they chose 
			gotoxy(startPoint.row, startPoint.col);
			cout << startLine;

			// clears menu for other functions to display prompts for input
			clearLine(MAXROWS + 1, menuLen + 50);
			clearLine(MAXROWS + 2, menuLen + 50);

			// collects ending position from user
			cout << "Type any letter to choose ending point / (type ESC to cancel): ";
			char endLine = getPoint(endPoint);

			// breaks statement if user inputs ESC
			if (endLine == ESC)
				break;

			// assigning row and column based on user input
			start.row = startPoint.row;
			start.col = startPoint.col;
			end.row = endPoint.row;
			end.col = endPoint.col;

			// adds undo state
			addUndoState(undoList, redoList, current);

			// draws line
			drawLine(current->item, start, end, animate);

			break;
		}
		case 'B':
		case 'b':
		{

			// adds undo state for undo functionality
			addUndoState(undoList, redoList, current);

			int height = 0;
			Point boxStart;
			Point center;

			// gets point from user
			clearLine(MAXROWS + 1, menuLen + 50);
			clearLine(MAXROWS + 2, menuLen + 50);

			cout << "Enter height: ";
			cin >> height;

			// gets desired height from user
			clearLine(MAXROWS + 1, menuLen + 50);
			clearLine(MAXROWS + 2, menuLen + 50);

			cout << "Type any letter to choose start point, or <C> for bottom center / <ESC> to cancel";
			char boxStartChar = getPoint(boxStart);

			// if user presses C
			if (boxStartChar == 'C' || boxStartChar == 'c')
			{
				center.row = MAXROWS / 2;
				center.col = MAXCOLS / 2;
			}

			// draws box
			drawBox(current->item, center, height, animate);

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
	} while (userInput != 'M' && userInput != 'm');


}


// Get a single point from screen, with character entered at that point
char getPoint(Point& pt)
{
	char input, returnCh;
	int row = 0, col = 0;

	// Move cursor to row,col and then get
	// a single character from the keyboard

	gotoxy(row, col);
	input = _getch();

	// ESC ends the getPoint functionality
	while (input != ESC)
	{
		if (input == SPECIAL)  // allows user to navigate canvas with arrow keys
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
			default:
				break;
			}

			// go where the user specifies with arrow key
			gotoxy(row, col);
		}
		else if (input >= 32 && input <= 126)
		{
			// collect current row and column
			pt.row = row;
			pt.col = col;

			// return the character the user presses
			returnCh = input;
			return returnCh;
		}
		else if (input == '\0')
		{
			input = _getch();
		}

		// lets user keep inputting more inputs 
		input = _getch();
	}
	return ESC;
}





// Recursively fill a section of the screen
void fillRecursive(char canvas[][MAXCOLS], int row, int col, char oldCh, char newCh, bool animate)
{
	// Position must be in bounds 
	if ((row < MAXROWS && row >= 0) && (col < MAXCOLS && col >= 0) && (canvas[row][col] == oldCh))
	{
		// draws character to screen (allows us to use animate as well)
		drawHelper(canvas, Point(row, col), newCh, animate); 

		// Recursive calls for all directions
		fillRecursive(canvas, row + 1, col, oldCh, newCh, animate);
		fillRecursive(canvas, row - 1, col, oldCh, newCh, animate);
		fillRecursive(canvas, row, col + 1, oldCh, newCh, animate);
		fillRecursive(canvas, row, col - 1, oldCh, newCh, animate);
	}
}



// Recursively draw a tree
void treeRecursive(char canvas[][MAXCOLS], DrawPoint start, int height, int startAngle, int branchAngle, bool animate)
{
	if (start.row >= MAXROWS || start.col >= MAXCOLS || start.row < 0 || start.col < 0)
	{
		return;
	}

	if (height > 2)
	{
		// making an endpoint and drawing trunk (base case)
		DrawPoint endPoint = findEndPoint(start, height / 3, startAngle);
		drawLine(canvas, start, endPoint, animate);

		// Creates branches to left and right of last line, new branch is 2 char shorter (Recursive step)
		treeRecursive(canvas, endPoint, height - 2, branchAngle + startAngle, branchAngle, animate);
		treeRecursive(canvas, endPoint, height - 2, startAngle - branchAngle, branchAngle, animate);
	}
}



// Recursively draw nested boxes
void drawBoxesRecursive(char canvas[][MAXCOLS], Point center, int height, bool animate)
{
	if (height > 1)
	{
		// Draws box (base Case)
		drawBox(canvas, center, height, animate);

		// Makes nested boxes of 2 less height (Recursive Step)
		drawBoxesRecursive(canvas, center, height - 2, animate);
	}
}