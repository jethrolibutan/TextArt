#include <iostream>
#include <Windows.h>
#include "Definitions.h"
using namespace std;


Node* newCanvas()
{
	// creates a node pointer to return
	Node* newNode = new Node;
	
	// initializes canvas with blank spaces
	initCanvas(newNode->item);

	// initializing next 
	newNode->next = NULL;

	// returns the node with the updated canvas
	return newNode;
}


Node* newCanvas(Node* oldNode)
{
	// creates a node pointer to return
	Node* newNode = newCanvas();

	// initializes new Node to have the same canvas as the old Node
	copyCanvas(newNode->item, oldNode->item);

	// initializing next 
	newNode->next = NULL;
	
	// returns the new node
	return newNode;
}


void play(List& clips)
{
	// loops as long as the ESCAPE key is not currently being pressed
	while (!(GetKeyState(VK_ESCAPE) & 0x8000))
	{
		playRecursive(clips.head, clips.count);
	}
}


void playRecursive(Node* head, int count)
{
	Sleep(100);
 
	// if head is empty, end the function
	if (count > 0)
	{
		// sets curser at top left
		gotoxy(0, 0);

		// displays current item in list
		displayCanvas(head->item);

		// calls the function again for the next item
		playRecursive(head->next, count - 1);
	}
	else
	{
		return;
	}
}


void addUndoState(List& undoList, List& redoList, Node* &current)
{
	// adds a new node to the undo list 
	// the new node should contain a copy of the current canvas

	// adds copy of node to undo list 
	addNode(undoList, newCanvas(current));

	// deletes redo list
	deleteList(redoList); 
}


void restore(List& undoList, List& redoList, Node* &current)
{
	if (undoList.count > 0)
	{

		// addscurrent node to redoList
		addNode(redoList, current);

		// sets current node to the head of undoList
		current = removeNode(undoList);
	}
}


void addNode(List& list, Node* nodeToAdd)
{
	// points new node to whatever the current node is pointing at 
	nodeToAdd->next = list.head;

	// sets list head to point to new Node
	list.head = nodeToAdd;

	// increments count of list
	list.count++;
}


Node* removeNode(List& list)
{
	// removes the first node in the linked list (whatever head is pointing to)
	if (list.head == NULL)
		return NULL;

	// creates a copy of current head node 
	Node* temp = list.head;

	// makes second item in list the new head
	list.head = list.head->next;

	// makes sure new node isnt pointing to the list anymore
	temp->next = NULL;

	// decrements list 1 value 
	list.count--;

	// returns copy of head node
	return temp;
}


void deleteList(List& list)
{
	// creates a node pointer that points to the head of the list and deletes the node next to the current node

	// makes a pointer for current node and net node
	Node* current = list.head;
	Node* next = NULL;

	// while node points to another
	while(current != NULL)
	{
		// next pointer points to the next value of current node 
		next = current->next;

		// deletes current node
		delete current;

		// current node is now the next node
		current = next;
	}

	// sets empty list head to NULL and count to 0
	list.head = NULL;
	list.count = 0;
}


bool loadClips(List& clips, char filename[])
{
    // loads the clips from your desired animation
    // if user loads "box", this function will also load box-1, box-2 and etc.
    // don't forget to update clipCount as well
    // test this function with "walk" and "tunnel" animation

	char fileLocation[FILENAMESIZE];

	// Erases the list of clips to make room for a new list from the files it will read.
	deleteList(clips);

	// counter for while loop
	int i = 1;


	// boolean for ending while loop
	bool ifEnded = false;


	do 
	{		
		// creates a filename to search for
		snprintf(fileLocation, FILENAMESIZE, "SavedFiles/%s-%d.txt", filename, i);

		// creates a new temporary node
		Node* temp = newCanvas();

		// loads the file at filename location to the temp canvas
		bool load = loadCanvas(temp->item, fileLocation);


		// if file isn't found, end loop and delete temp
		if (load == false)
		{
			ifEnded = true;
			delete temp;
		}
		// if file is found, add copy of temp node to clips and then delete temp
		else
		{
			addNode(clips, newCanvas(temp));
		}

		// increments i for the next file name
		i++;
	} 
	while (ifEnded == false);


	// if clips count is 0
	if (clips.count == 0)
	{
		return false; 
	}

	return true;
}


bool saveClips(List& clips, char filename[])
{
	// charactyer used for the file location
	char fileLocation[FILENAMESIZE];

	int fileNum = clips.count;

	// for each node in the clips list
	for (Node* current = clips.head; current != NULL; current = current->next)
	{

		// makes file name for each clip
		snprintf(fileLocation, FILENAMESIZE, "SavedFiles/%s-%d.txt", filename, fileNum);

		// saves each canvas individually 
		if (!saveCanvas(current->item, fileLocation))
		{
			return false;
		}

		// cycles through all the files without changing clips.count
		fileNum--;
	}


	// returns true if you made it through the entire save list
	return true;
}