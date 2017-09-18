#include <stdlib.h>
#include <stdio.h>


/*MARK: TYPEDEFS*/
typedef struct _seg{
	int bits[256];
	struct _seg *next, *prev;
}seg;

seg * primes;	/*global primes linkedlist pointer*/
seg * tempSeg;	/*used by whichseg to remember current position*/
int currSegNumber = 0;
int segSize = sizeof(int) * 8 * 256;
int N;


/*MARK: PROTOTYPES*/
void makeSieve(seg *pointer, int N);
void zeroTheSieve();
void markAllNonPrimes(seg *pointer, int N);
int getNextPrime(int curr);
int getNumPrimes(seg *pointer, int N);
seg* whichseg(int j);
int whichint(int j);
int whichbit(int j);
int marknonprime(int j);
int testprime(int j);

/*MARK: BEGIN FUNCTIONS*/
int main(int argc, char *argv[]){
	int numPrimes;
	int evenInt;
	int result;

	if (argc == 2) sscanf(argv[1],"%d",&N);	/*Accept command line arguments*/
    else scanf("%d",&N);	/*If invalid command line arguments, take next num as N*/

	//result = scanf("%d", &N);	/*Get N*/
	//if (result == EOF){
	//	printf("Error, invalid input\n");
	//}

	/*build linked list*/
	primes = (seg*) malloc(sizeof(seg));
	tempSeg = primes;
	primes->prev = NULL;
	primes->next = NULL;
	makeSieve(primes, N);	/*Build the empty liked list*/
	zeroTheSieve();
	markAllNonPrimes(primes, N);	/*mark nonprimes to zero*/

	numPrimes = getNumPrimes(primes, N);
	printf("The number of odd primes less than or equal to N is: %d\n", numPrimes);

	/*Read in even integers until eof*/
	while(result != EOF){  
		result = scanf("%d", &evenInt);
		if (result == 0){
			while (fgetc(stdin) != '\n'){}	/*skip trash characters*/
			continue;
		}else if (result == EOF){
			return 0;
		}
		decomposition(evenInt);
	}

	/* clean up */
	while (primes != NULL){
		tempSeg = primes;
		primes = primes->next;
		free(tempSeg);
	}
	return 0;
}
void makeSieve(seg *pointer, int N){
	/*Decide how many more segments to make*/
	int numsToMark;
	int numSegments;
	int i;
	seg *curr;
	seg *prev;

	if (N < 0) return;

	numsToMark = (N - 2) / 2; 	/*numbers 3, 5, 7, ...*/
	numSegments = numsToMark / segSize;/*number of additional segments to make*/
	
	/*initialize the rest of the segments*/
	curr = pointer;
	prev = pointer;

	for (i = 0; i < numSegments; ++i){
		int j;
		curr = (seg*) calloc(1, sizeof(seg));
		for (j = 0; j < 256; ++j){
			curr->bits[j] = 0;
		}
		curr->prev = prev;
		prev->next = curr;
		prev = curr;
	}

	/*make sure linked list ends with null*/
	curr->next = NULL;
	return;
}

void zeroTheSieve(){
	seg* runner;
	runner = primes;

	while (runner != NULL){
		for (int i = 0; i < 256; ++i){
			runner->bits[i] = 0;
		}
		runner = runner->next;
	}
}
void markAllNonPrimes(seg *pointer, int N){
	int curr;
	curr = 3;		/*start on 3 as first odd prime*/
	while (curr * curr <= N){	/*only use primes <= sqrt(N)*/
		int multiple;
		multiple = curr;	/*the first multiple, begin on prime squared*/
		while(curr * multiple <= N){		/*mark all multiples of curr as non prime*/
			if (curr * multiple == 263){
				printf("marking 263 non prime\n");
			}
			marknonprime(curr * multiple);
			multiple = multiple + 2;	/*only use odd multiples, evens arn't eligible*/
		}
		curr = getNextPrime(curr);
		if (curr == -1) break;	/*no more primes*/
	}
}
int getNextPrime(int curr){
	curr = curr + 2;
	while(curr <= N){		/*don't exceed seive sieve size*/
		if (testprime(curr) == 0){
			return curr;	/*return prime if found*/
		}
		curr = curr + 2;	/*look through odds*/
	}
	return -1;	/*no more primes in Seive*/
}
int getNumPrimes(seg *pointer, int N){
	int i, counter = 0;
	for (i = 3; i <= N; i = i+2){
		if (testprime(i) == 0){
			counter++;
		}else{
		}
	}
	return counter;
}
seg* whichseg(int j){
	/*returns correct segment.*/
	int segNumber;
	j = (j - 2) / 2;
	segNumber = j / segSize;	/*0 indexed segment index*/
	while(currSegNumber < segNumber){
		tempSeg = tempSeg->next;
		currSegNumber++;
	}
	while(currSegNumber > segNumber){
		tempSeg = tempSeg->prev;
		currSegNumber--;
	}
	return tempSeg;
}
int whichint(int j){
	j = (j - 2) / 2; 
	return (j % segSize) / (sizeof(int) * 8) ;
}
int whichbit(int j){
	j = (j-2)/2;
	return j % (sizeof(int) * 8);
}
int marknonprime(int j){
	int index, bit, shifter;
	if (j == 263){
		printf("marking nonprime 263\n");
	}
	seg * curr;
	if (j % 2 == 0) return -1;	/*ignore even numbers*/
	curr = whichseg(j);
	index = whichint(j);
	bit = whichbit(j);
	shifter = 1 << bit;
	curr->bits[index] = curr->bits[index] | shifter; 
	return j;
}
int testprime(int j){				/*0 if prime, 1 if nonprime*/
	int testIndex, testbit, shifter;
	seg * testSeg;
	if (j > N || j % 2 == 0) return -1;			/*invalid input*/
	testSeg = whichseg(j);
	testIndex = whichint(j);
	testbit = whichbit(j);
	shifter = 1 << testbit;
	return testSeg->bits[testIndex] & shifter;	/*0 if prime, other for nonprime*/
}
int decomposition(int k){
	unsigned int i, iIndex, ishift, big, bigindex, bigshift, solution, numSolutions;
	seg *iseg, *bigseg;

	//if (k > N){
		//printf("Error, cannot decompose %d because it is out of bounds\n", k);
		//return -1;
	 if (k % 2 != 0){	/*skip odd numbers*/
		printf("Error, only meant to decompose even numbers\n");
		return -1;
	}
	i = 3;
	big = k - i;

	if (k > N){
		big = N;
		i = k - N;
		if (i % 2 == 0){
			i++;
			big--;
		}
	}

	iIndex = whichint(i);
	ishift = 1 << whichbit(i);

	bigindex = whichint(big);
	bigshift = 1 << whichbit(big);

	iseg = primes;
	bigseg = whichseg(big);
	solution = -1;	/* start with no solution */
	numSolutions = 0;

	while (i < k && i < N){ /*i must be less than N to enssure primality*/

		/*check if i and k are both prime*/
		if((iseg->bits[iIndex] & ishift) == 0){
			/*i is prime*/
			
			if((bigseg->bits[bigindex] & bigshift) == 0 && big <= N){
				/*big is prime, save solution*/
				solution = i;
				numSolutions++;
			}
		}

		/*Increment i Coordinate*/
		i = i + 2;
		if (i >= k) break;
		if (ishift != 1 <<31){
			ishift = ishift << 1;
		}else{	
			/*shifter rolls to start, increment index*/
			ishift = 1;
			if (iIndex != 255){
				iIndex++;
			}else{
				/*index rolls to zero, increment segment*/
				iIndex = 0;
				iseg = iseg->next;
			}
		}

		/*Increment big Coordinate*/
		big = big - 2;
		if (big < 3) break;
		if (bigshift != 1){
			bigshift = bigshift >> 1;
		}else{	
			/*shifter resents to end, decrement index*/
			bigshift = 1 << 31;
			if (bigindex != 0){
				bigindex--;
			}else{
				/*index resets to 255, decrement segment*/
				bigindex = 0;
				bigseg = bigseg->prev;
			}
		}

	}

	if (solution == -1){
		printf("No decomposition for %d!\n", k);
	}else{
		big = k - solution;
		printf("decomposition of %d is %d and %d\n", k, solution, big);
	}
	
	return solution;
}






