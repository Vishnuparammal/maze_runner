/*
This is an example code for using maze library

the function scanMaze returns the next direction(s)
in which the bot must move to completely scan the maze

the inputs have been taken from user here but
the inputs can be easily recieved from sensors

X, Y, dist - encoders
dir[4], nodeEnd, nodeDetected - line sensors
prevDir, currDir - magnetometer
                or maintain a dir variable
                that starts with NORTH and
                updates on every turn

nextDir -   turn the bot in this direction(s)
            if NULL, scan is complete

followes NESW priority order fixed w.r.t the arena
the direction in which bot faces at the start node is defined as NORTH
stores nodes in a graph in adjacency list format
uses dijkstras algorithm for finding shortest path

*/
#include "maze.h"

int main()
{
    float X=0, Y=0;
    int prevDir = -1, currDir = -1;
    float dist = 0;
    int nodeEnd = 0;
    int dir[4];
    float error = 0;

    bool nodeDetected = true;

    // holds return values of scanMaze and shortestPath()
    int* nextDir = NULL;
    int dirSize = 0;
    
    initializeMaze();

    while(nodeDetected)
    {
        printf("input> X: ");
        scanf("%f", &X);
        printf("input> Y: ");
        scanf("%f", &Y);
        printf("input> prevDir: ");
        scanf("%d", &prevDir);
        printf("input> currDir: ");
        scanf("%d", &currDir);
        printf("input> dist: ");
        scanf("%f", &dist);
        printf("input> nodeEnd: ");
        scanf("%d", &nodeEnd);
        printf("input> NORTH: ");
        scanf("%d", &dir[NORTH]);
        printf("input> EAST: ");
        scanf("%d", &dir[EAST]);
        printf("input> SOUTH: ");
        scanf("%d", &dir[SOUTH]);
        printf("input> WEST: ");
        scanf("%d", &dir[WEST]);
        nextDir = scanMaze(X, Y, prevDir, currDir, dist, nodeEnd, dir, &dirSize, error);
        if(!nextDir)
            break;
        printDir(nextDir, dirSize);
    }

    nextDir = shortestPath(&dirSize);
    printDir(nextDir, dirSize);
    return 0;
}