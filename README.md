
<p align="center"">
    <img width="512" alt="honey-64x" src="https://github.com/user-attachments/assets/a91b7ff5-3900-41d6-9d7a-57237862c6c6"/>
</p>

# Honey

Honey is a testing framework made to help test the METL Project for the CS244 module for Stellenbosch 2026. The goal of honey is to provide an easy way to generate and run test cases.

## Building honey

To avoid issues with multiple git repos in one directory, I would instead just download the source code instead of cloning.

Once downloaded, put all the source code in a directory called 'honey' in your metl project directory.

CD into the honey directory and compile it with:
```make```

## Installing test cases

Create a directory called 'tests' in your honey directory. Download and unzip test folders into this directory.

## Running honey to test

ALWAYS run honey from your metl root directory.

This is easiest way to run honey:
```
./honeyc [testfile]
```

Where `testfile` is replaced with one of the test suite files you've downloaded. Honey looks for the testfile by doing `./honey/tests/[testfile].honey`.

## Flags

There are also optional flags to run honey with. To see them you can run `./honeyc -h`

There are five optional flags:

- c: cli (runs honey as a command line interface and reads from stdin instead of an input file)
- s: simplified view (forces dump and shows the differences in your code with the output)
- h: help (shows the help screen)

## Honey CLI and writing .honey files
Honey provides a simple CLI. This is useful for once of tests or for generating test cases. To access the CLI, simply run `./honeyc -c`. When writing and sharing `.honey` files, all that it is doing is treating the `.honey` file as the input to the CLI. These are the commands you can run.

All honey commands through the CLI follow the basic syntax: ```cmdname [arg1] [arg2]``` Commands are separated by newlines. There should be NO whitespace after the second argument.
These are the valid commands:

### run
run takes one additional argument, and sets the command line prefix to use for testing. Let's say you want to use the `testscanner` binary to run your tests, you would then run the following honey command:
```run ./bin/testscanner```

### tst
tst (test) performs a single test. It takes two arguments: the input file path to pass to the `run` binary, and a path to a file containing the expected output. Honey looks in the `honey/tests` path for these files.
Let's say we had a `test.metl` file in `honey/tests/test` and the expected output in `honey/tests/test/test.out`, then we could test our code with:
```tst test/test.metl test/test.out```

When we terminate STDIN with `CTRL+D`, honey will then print out all the results of our tests.

### bat
bat (batch) performs batch testing. It takes two arguments: the test directory, and the number of test files that are located in that directory. Batch testing expects the test files to be named `1.in`, `1.out`, `2.in`, `2.out`, etc.
Let's say we had a folder `testbat` in `honey/tests` and inside it had `1.in`, `2.in` as well as the corresponding `.out` files. Assuming we had a valid `run` command, then we could run a batch test with:
```bat testbat 2``` 

### gen
gen (generate) makes generating test files easy. It works the same way as `bat` and has the same arguments, and runs the command with `1.in`, `2.in`, etc. but instead of checking your commands output against `.out`, it pipes stdout and stderr into the corresponding `.out` file. These files can then be sent to a friend to test your outputs against each other.
Let's stick with the same example as we had in bat, but now we want to generate the output:
```gen testbat 2```

### exe
exe (execute) executes an arbitrary once of command. Let's say you want to compile the users code before running test cases:
```exe cd src && make testscanner && cd ..```

## That's it!

That's it! Honey is a super lightweight testing framework. It doesn't provide a lot of feedback, but provides a basic interface to test.

Also the code is horrible.
