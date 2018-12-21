# myar.c

## Installation
```sh
$ gcc -o myar myar.c
```
#
## Note
Purpose : to recreate the some ar command on linux. More information [here](https://linux.die.net/man/1/ar) .
#
## Usage


```sh
./myar [name] [option] [file]
```
Options:
- q : quickly append named file to archive
- x : extract named files
- t : print a concise table of contents of the archive
- A : quickly append all ordinary files in the current directory that have been modified within the last two hours(except the archive itself).


Example:
```sh
./myar Hello q hello.c
```