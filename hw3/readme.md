# Uniqify
## Note
Purpose: Basic inter process communication using pipe. 
```sh
 ┌───────────┐   ┌───────────┐    ┌───────────┐ 
 │ Parse     │-->│   Sort    │--> │ Surpress  │ 
 │           │   │           │    │           │
 └───────────┘   └───────────┘    └───────────┘ 
```
Parse: parse the input file <br>
Sort: use usr/bin/sort to sort the list of words<br>
Surpress: Remove duplicate and print count

## Installation
```sh
$ gcc -o uniqify uniqify.c
```

## Usage
```sh
./uniqify < input.txt
```