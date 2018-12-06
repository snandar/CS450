/*
 * THIS CODE IS MY OWN WORK, IT WAS WRITTEN WITHOUT CONSULTING A TUTOR OR CODE WRITTEN BY OTHER STUDENTS - NANDAR SOE
 *
 *
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

typedef struct struct_seg {

	int bit[256];
    struct struct_seg *next;

} seg;

typedef struct {

	seg *segpt; //node
	int intnum; //0-256
	int bitnum; //0-31

} coordinate;

/*1 segment = 8192 bits*/
#define BITSPERSEG (8*256*sizeof(int))
#define INTSPERSEG 256

/*functions*/
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

/*variables*/
seg* root;
int segoverflow = 0;

int main(int argc, char **argv){

	int N = atoi(argv[1]); //finding prime less than or equal to N

	printf("Calculating primes up to %d ...\n", N);

	int segment = N/BITSPERSEG;

	root = (seg*)malloc(sizeof(seg));
	seg *ptr = root;


	//allocate memory for num of seg
	for(int i=0; i<segment; i++){
		ptr->next = (seg*)malloc(sizeof(seg));
		ptr=ptr->next;
	}

	ptr->next = NULL;
	ptr = root;

	//clear all bits in each int of seg
	while(ptr != NULL){

		for(int j=0; j<256 ; j++){

				ptr->bit[j] = 0;

		}

		ptr = ptr->next;
	}

	ptr = root;

	sieve(N);

	int oddPrime = countprime(N);

	printf("Found %d odd primes\n", oddPrime);

	char s[128];

	printf("Enter Even Numbers for Twin Prime Differential\n");
	fflush(stdout);

	while(scanf("%s", (char *)&s) != EOF){

		int length = strlen(s);
		for(int i =0; i<length; i++){

			if(!isdigit(s[i])){

				exit(1);

			}

		}

		int k = atoi(s);
		twinprime(N, k);
	}


	return 0;
}

/*twin prime*/
void twinprime(int n, int k){
	//n = up to num
	//k = skip num
	//i = current num

	int one, two;
	int count = 0;

	if(k%2 == 0){

		coordinate c = getcoord(3);
		//should use only testcoord and incrcoord

		for(int i = 3; i+k<=n ; i+=2){

			if(testcoord(c)==0 && testcoord(incrcoord(c, k))==0){
				count++;
				one = i;
				two = i+k;
			}

			c = incrcoord(c, 2);
		}

		if(count > 0){

			printf("%d solutions, largest %d %d\n", count, one, two);

		}

		else{
			printf("0 solutions, largest 0 0\n");
		}
	}

	else{
		printf("1 solutions, largest %d %d\n", 2, 2+k);

	}


}


/*use sieve method to find the number of odd primes less than or equal to n*/
void sieve(int n){

	int i = 3;
	while((i * i) < n){

		//testprime =1 num bit is set to 0.
		if(testprime(i) == 1){

			for(int j=i; j<=n; j+=i){

				if((j%2 == 1) && (j!=i)){

					marknonprime(j);

				}
			}

		}

		i = i+2;

	}

}

/*count num of prime up to N*/
int countprime(int N){

	int count =0;

	for (int i = 3; i<=N; i+=2){

		if(testprime(i) == 1){
			count++;
		}

	}

	return count;
}


/*returns which segment the integer j is in*/
seg* whichseg(int j){

	int segment = j/BITSPERSEG;

	seg *ptr = root;

	for(int i=0; i<segment; i++){
		ptr = ptr->next;
	}

	return ptr;
}

/*returns which integer j from 0-256 it is in*/
int whichint(int j){

	int segremainder = j%BITSPERSEG;
	int integer = segremainder/(8*sizeof(int));

	return integer;

}

/*return which bit j from 0-32 it is in*/
int whichbit(int j){

	int seg = j/BITSPERSEG;
	j = j - seg*BITSPERSEG;

	int intnum = j%32;

	return (intnum-1)/2;
}


/*returns the coordinate of int j*/
coordinate getcoord(int j){

	coordinate c;

	c.segpt = whichseg(j);
	c.intnum = whichint(j);
	c.bitnum = whichbit(j);

	return c;

}

/*set the ith bit to 1*/
void markcoord(coordinate c){

	seg* ptr = root;

	while (ptr!=c.segpt){
		ptr = ptr->next;
	}

	int x = c.segpt->bit[c.intnum];
	int i = c.bitnum;

	if((ptr->bit[c.intnum] & (1 << i)) == 0 ){
		ptr->bit[c.intnum] = x | (1<<i);
	}
}

/*test if the ith bit is 1 or prime*/
int testcoord(coordinate c){

	seg* ptr = root;

	while (ptr!=c.segpt){
		ptr = ptr->next;
	}

	int i = c.bitnum;

	if( (ptr->bit[c.intnum] & (1 << i)) != 0 ){
		//bit at position c.bitnum is one therefore composite
		return 1;
	}

	return 0;

}

/*marks integer j to be nonprime*/
void marknonprime(int j){

	markcoord(getcoord(j));

}

//test if the integer j is prime//
int testprime(int j){

	coordinate cj = getcoord(j);
	int tc = testcoord(cj);

	if (tc == 1){
		return 0; //tc=1 composite thus not prime
	}

	return 1;

}

//increase coordinate by inc bits//
coordinate incrcoord(coordinate c, int inc){

	int num = whichnum(c);
	num = num +inc;

	return getcoord(num);
}

//which number does this coordinate correspond to?//
int whichnum(coordinate c){

	seg* temp = root;
	int segcount = 0;

	for(segcount = 0 ; temp!=c.segpt; segcount++ ){
		temp = temp->next;
	}

	int realbitnum = c.bitnum*2+1;
	int realint = 32*c.intnum;
	int realseg = 8192*segcount;

	int number = realbitnum+realint+realseg;

	return number;
}
