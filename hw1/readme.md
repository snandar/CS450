# Twin Prime

## Installation
```sh
$ gcc -o twinprime twinprime.c
$ ./twinprime 
```

## Note
Purpose: to use the sieve method to find the twin primes. More information [here](https://en.wikipedia.org/wiki/Twin_prime)

### Code Structure
Data Structure

    typedef struct struct_seg {

        int bit[256];
        struct struct_seg *next;

    } seg;

    typedef struct {

        seg *segpt; //node
        int intnum; //0-256
        int bitnum; //0-31

    } coordinate;

Functions
```sh
seg* whichseg(int j);
int whichint(int j);
int whichbit(int j);
coordinate getcoord (int j);
void markcoord(coordinate c);
int testcoord(coordinate c);
void marknonprime(int j);
int testprime(int j);
coordinate incrcoord (coordinate c, int inc);
int whichnum (coordinate c);
void sieve(int n);
int countprime(int N);
void twinprime(int n, int k);
```