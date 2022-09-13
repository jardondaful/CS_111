# Pipe Up

This program implements the pipe (|) operator in shells.

## Building

Run 'make' to build the program.

## Running

Running './pipe ls cat wc' will first redirect a list of the contents of
the current working directory (the output of the 'ls' command) into the
'cat' command. Then, the output lines from 'cat' will be redirected into
the 'wc' command. Finally, 'wc' will display the number of lines, words,
and characters to stdout. The user will see these 3 numbers on the console.

## Cleaning up

Run 'make clean' to get rid of the object and executable files.
