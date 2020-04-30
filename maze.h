#ifndef _MAZE_H_
#define _MAZE_H_

#include <stdbool.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <limits.h>
#include <math.h>

#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

// A structure to represent a node in adjacency list 
struct AdjListNode 
{ 
	int dest; 
	float weight;
	int dir; 
	struct AdjListNode* next; 
}; 

// A structure to represent an adjacency liat 
struct AdjList 
{ 
	struct AdjListNode* head; // pointer to head node of list 
	float X;
	float Y;
	int incomplete;
	int dir[4];
}; 

// A structure to represent a graph. A graph is an array of adjacency lists. 
// Size of array will be V (number of vertices in graph) 
struct Graph 
{ 
	int V; 
	struct AdjList* array; 
}; 

// Structure to represent a min heap node 
struct MinHeapNode 
{ 
    int  v; 
    float dist; 
}; 
  
// Structure to represent a min heap 
struct MinHeap 
{ 
    int size;      // Number of heap nodes present currently 
    int capacity;  // Capacity of min heap 
    int *pos;     // This is needed for decreaseKey() 
    struct MinHeapNode **array; 
}; 

// call: 		at the start of maze
// 				Initializes the maze and adds the starting node
extern void initializeMaze();

// call: 		when a node is encountered
// parameters: 	X	 	current X coordinate of bot
//				Y	 	current Y coordinate of bot
//				prevDir	direction in which bot left the previous node
//				currDir	direction from which bot entered current node
//				dist	distance between previous and current node
//				nodeEnd	is the current node end
//				dir[4]	availability of paths from current node in NESW order (-1: no path, 0: path available)
//				dirSize	reference variable that holds the number of turns to be taken
// returns: 	direction array to the next node
//				NULL when the entire maze is scanned
extern int* scanMaze(float X, float Y, int prevDir, int currDir, float dist, bool nodeEnd, int dir[], int* dirSize, float error);

// print the graph
extern void printGraph();

// call:	when the scan is complete
// returns: an array of directions for shortest path from start to end 
extern int* shortestPath(int* dirSize);

// used to print the result of scanMaze() or shortestPath()
extern void printDir(int dist[], int n);

#endif