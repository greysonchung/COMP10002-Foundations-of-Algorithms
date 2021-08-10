/* Tweet cleanser:
 * Removes non-alphanumeric characters from tweets
 *
 * Skeleton code written by Yi Han and Jianzhong Qi, April 2019
 * Edited by Haonan Zhong (867492), finished stage 1 to 5
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* stage numbers */
#define STAGE_NUM_ONE 1 
#define STAGE_NUM_TWO 2
#define STAGE_NUM_THREE 3
#define STAGE_NUM_FOUR 4
#define STAGE_NUM_FIVE 5
#define STAGE_HEADER "Stage %d\n==========\n"  /* stage header format string */

#define MAX_TWEET_LENGTH 280                   /* maximum length of a tweet */
#define MAX_NUM_TWEETS 100	                   /* maximum number of tweets */
#define END_OF_STRING '\0'                     /* end of string character */
#define VALID 1                                /* validator for functions */
#define INVALID 0                              /* validator for functions */
#define NEXT 1                                 /* get the next character */
#define FIRST_CHAR 0                           /* first character of string */
#define CONTAINED 1                            /* validator for functions */
#define NOT_CONTAINED 0                        /* validator for functions */
#define ASTERISK '*'                           /* asterisks */

typedef char tweet_t[MAX_TWEET_LENGTH+1];	   /* a tweet */

/****************************************************************/

/* function prototypes */
void read_one_tweet(tweet_t one_tweet, int max_len);
void print_stage_header(int stage_num);
void tweet_tolower(tweet_t one_tweet);

void stage_one(tweet_t one_tweet);
void stage_two(tweet_t tweets[], int *num_tweets);
void stage_three(tweet_t tweets[], int num_tweets);
void stage_four(tweet_t tweets[], int num_tweets);
void stage_five(tweet_t tweets[], int num_tweets);

/* add your own function prototypes here */
int str_len(const tweet_t one_tweet);
int is_contained(tweet_t source_tweet, tweet_t target_tweet);
void swap_tweet(tweet_t tweet1, tweet_t tweet2);
void remove_char(tweet_t target_tweet, int index);


/****************************************************************/

/* main function controls all the action, do NOT modify this function */
int
main(int argc, char *argv[]) {
	/* to hold all input tweets */
	tweet_t tweets[MAX_NUM_TWEETS];	
	/* to hold the number of input tweets */
	int num_tweets = 0;					

	/* stage 1: reading the first tweet */
	stage_one(tweets[num_tweets]); 
	num_tweets++;
	
	/* stage 2: removing non-alphanumeric characters */
	stage_two(tweets, &num_tweets);
	
	/* stage 3: removing extra asterisks and finding the longest tweet */ 
	stage_three(tweets, num_tweets);
	
	/* stage 4: finding the non-contained tweets */
	stage_four(tweets, num_tweets);
	
	/* stage 5: sorting the tweets */
	stage_five(tweets, num_tweets);
	
	/* all done; take some rest */
	return 0;
}

/****************************************************************/

/* read a line of input into one_tweet */
void
read_one_tweet(tweet_t one_tweet, int max_len) {
	int i = 0, c;
	
	while (((c = getchar()) != EOF) && (c != '\n') && (c != '\r')) {
		if (i < max_len) {
			one_tweet[i++] = c;
		} else {
			printf("Invalid input line, toooooooo long.\n");
			exit(EXIT_FAILURE);
		}
	}
	one_tweet[i] = END_OF_STRING;
}

/****************************************************************/

/* print stage header given stage number */
void 
print_stage_header(int stage_num) {
	printf(STAGE_HEADER, stage_num);
}

/****************************************************************/

/* scan a tweet and convert all English letters into lowercase */
void
tweet_tolower(tweet_t one_tweet) {
	int i;
	for (i = 0; one_tweet[i]; i++) {
		one_tweet[i] = tolower(one_tweet[i]);
	}
}

/****************************************************************/

/* stage 1: reading the first tweet */
void
stage_one(tweet_t one_tweet) {
	/* print stage header */
	print_stage_header(STAGE_NUM_ONE); 
	
	read_one_tweet(one_tweet, MAX_TWEET_LENGTH);
	
	/*convert every alphabetic characters in the tweet to lower case */
	tweet_tolower(one_tweet);
	
	printf("%s\n\n", one_tweet);
}

/****************************************************************/

/* stage 2: read the remaining tweet, removing non-alphanumeric characters */
void
stage_two(tweet_t tweets[], int *num_tweets) {
	int i, j;
	print_stage_header(STAGE_NUM_TWO);
	
	/* read the remaining tweet */
	for (i = *num_tweets; i < MAX_NUM_TWEETS; i++) {
		read_one_tweet(tweets[i], MAX_TWEET_LENGTH);
		tweet_tolower(tweets[i]);
		if (tweets[i][FIRST_CHAR] != END_OF_STRING) {
			(*num_tweets)++;
		}
	}
	
	/* remove all the unrelated characters from every tweet */
	for (i = 0; i < *num_tweets; i++) {
		for (j = 0; tweets[i][j]; j++) {
			if (tweets[i][j] == ASTERISK) {
				continue;
			}
			else if (isalpha(tweets[i][j]) || isdigit(tweets[i][j])) {
				continue;
			}
			else {
				remove_char(tweets[i], j);
				j--;
			}
		}
		printf("%s\n", tweets[i]);
	}
	printf("\n");
}

/****************************************************************/

/* stage 3: removing extra asterisks and finding the longest tweet */ 
void 
stage_three(tweet_t tweets[], int num_tweets) {
	int i, j, count = 0, max = 0, max_index;
	print_stage_header(STAGE_NUM_THREE);
	
	/* scan for extra asterisks in each tweet and remove it */
	for (i = 0; i < num_tweets; i++) {
		for (j = 0; tweets[i][j]; j++) {
			
			if (tweets[i][FIRST_CHAR] == ASTERISK) {
				remove_char(tweets[i], FIRST_CHAR);
			}
			if (tweets[i][j] == ASTERISK && (tweets[i][j+NEXT] == ASTERISK || 
				tweets[i][j+NEXT] == END_OF_STRING)) {
				remove_char(tweets[i], j);
				j--;
			}
		}
		
		/* print out each tweet as well as find the longest tweet */
		printf("%s\n", tweets[i]);
		count = str_len(tweets[i]);
		if (count > max) {
			max = count;
			max_index = i;
		}
	}
	printf("Total: %d\nLongest: %s\nLength: %d\n\n", 
		num_tweets, tweets[max_index], max);
}

/****************************************************************/

/* stage 4: finding the non-contained tweets */
void 
stage_four(tweet_t tweets[], int num_tweets) {
	int i, j, verifier, contained_num = 0;
	print_stage_header(STAGE_NUM_FOUR);
	
	/* check if a tweet is fully contained in another tweet */
	for (i = 0; i < num_tweets; i++) {
		verifier = VALID;
		
		for (j = 0; j < num_tweets; j++) {
			/* skip the current loop when comparing itself */
			if (i == j) {
				continue;
			}
			else if (is_contained(tweets[i], tweets[j])) {
				verifier = INVALID;
				contained_num++;
				break;
			}
		}
		/* print the tweet if not contained in another tweet */
		if (verifier == VALID) {
			printf("%s\n", tweets[i]);
		}
	}
	printf("Non-contained: %d\n\n", num_tweets - contained_num);
}

/****************************************************************/

/* stage 5: sorting the tweets */
void 
stage_five(tweet_t tweets[], int num_tweets) {
	int i, j;
	print_stage_header(STAGE_NUM_FIVE);
	
	/* sort each tweet by it's length using insertion sort */
	for (i = 1; i < num_tweets; i++) {
		for (j = i-1; j >= 0; j--) {
			
			if (str_len(tweets[j+NEXT]) < str_len(tweets[j])) {
				swap_tweet(tweets[j+NEXT], tweets[j]);
			}
			
			/* when two tweets has the same length */
			else if (str_len(tweets[j+NEXT]) == str_len(tweets[j])) {
				/* sort it based on alphabetic order */
				if (strcmp(tweets[j+NEXT], tweets[j]) < 0) {
					swap_tweet(tweets[j+NEXT], tweets[j]);
				}
				
				else {
					continue;
				}
			}
		}
	}
	
	for (i = 0; i < num_tweets; i++) {
		printf("%s\n", tweets[i]);
	}
}

/****************************************************************/

/* function for remove character from the given tweet */
void
remove_char(tweet_t target_tweet, int index) {
	int i;
	for (i = index; target_tweet[i]; i++) {
		/* assigning the next character to the current position */
		target_tweet[i] = target_tweet[i+NEXT];
	}
}

/****************************************************************/

/* function to check if the source tweet is contained in the target tweet */
/* code inspired by is_subsequence() function written by Jianzhong Qi
   Exercise 1 in lec06 slides Foundations of Algorithms (COMP10002) */
 
int
is_contained(tweet_t source_tweet, tweet_t target_tweet) {
	if (str_len(source_tweet) > str_len(target_tweet)) {
		return NOT_CONTAINED;
	}
	int i, j, n, verifier, mark;
	
	/* scan the target tweet for match up character */
	for (i = 0; i < str_len(target_tweet); i++) {
		n = 0; 
		verifier = CONTAINED;
		if (source_tweet[n] == target_tweet[i]) {
			
			/* match found mark the position, move on for further match up */
			mark = i;
			for (j = 0; j < str_len(source_tweet); j++) {
				if (source_tweet[n] == target_tweet[mark]) {
					n++; mark++;
					continue;
				}
				else {
					verifier = NOT_CONTAINED;
					break;
				}
			}
		}
		/* if not found, continue scan for the next match up character */
		else {
			continue;
		}
		
		/* if the verifier has not been recorded as NOT_CONTAINED
		   return CONTAINED */
		if (verifier == CONTAINED) {
			return CONTAINED;
		}
	}
	return NOT_CONTAINED;
}

/****************************************************************/

/* function for swapping two tweets */
void
swap_tweet(tweet_t tweet1, tweet_t tweet2) {
	tweet_t temp;
	strcpy(temp, tweet1);
	strcpy(tweet1, tweet2);
	strcpy(tweet2, temp);
}

/****************************************************************/

/* function for counting the length of a tweet */
int 
str_len(const tweet_t one_tweet) {
	int len = 0;
	while (one_tweet[len]) {
		len++;
	}
	return len;
}

/****************************************************************/

/* Algorithms are fun! */