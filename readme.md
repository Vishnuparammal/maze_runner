*maze* is a C library for solving maze by a line follower bot 

Build the library as object file

	$ gcc -c maze.c -o maze

Build the example code and link with library

    $ gcc bot.c maze

Run the example code

	$ ./a.out

To solve the maze in `samples/maze1`
Copy everything from the file `samples/input1.txt`
and paste it in the program after running