## About the project
**Maze** is a C library that is capable of scanning a complex maze and find the shortest path to end using Dijkstras algorithm. It can function independently as well as on any simulator or hardware. The same has been used to simulate a line follower robot on Coppeliasim ( VREP ) using its legacy remote API in C. 
### Code
`samples`			-> sample mazes for tests
`src` 					-> contains the *maze* library, *vrep* sim control code and api libraries
`vrep_scenes`	-> contains coppeliasim scenes

## Getting Started
### Prerequisites
* [Coppeliasim](https://coppeliarobotics.com/)
* [GNU make](https://www.gnu.org/software/make/)
### Build
If your vrep executable is in the default location
Windows 10: `C:\Program Files\CoppeliaRobotics\CoppeliaSimEdu`
Ubuntu 16.04: `~/CoppeliaSim_Edu_V4_0_0_Ubuntu16_04/`

	$ make all

If the path is different

	$ make all VREPPATH=<path> 

*<path>* must be in double quotes if it contains space characters
### Usage
* Library
	1. `src/maze.c` is the core library implementing the maze solving algorithm.
	2. `src/maze.h` has all the implementation details.

* Simulation
	1. Open the Coppeliasim scenes from `vrep_scenes`.
	2. Press the run button.
	3. When the bot stops, press pause button and move the bot to its initial position.
	4. Press the play button again.
	5. The path taken by bot now is the shortest path.
