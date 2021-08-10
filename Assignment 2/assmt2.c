 /* Sentiment analysis:
 * process to identify opinions in a given piece of text and to 
 * determine whether the writer’s attitude towards a particular 
 * topic (or product, etc.) is positive, negative, or neutral
 *
 * Code written by Haonan Zhong, finished stage one to five, May 2019
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

/* stage numbers */
#define STAGE_NUM_ONE 1 
#define STAGE_NUM_TWO 2
#define STAGE_NUM_THREE 3
#define STAGE_NUM_FOUR 4
#define STAGE_NUM_FIVE 5
#define HEADER "=========================Stage %d=========================\n"

#define MAX_WORD_LENGTH 20               /* max length of a word */
#define MAX_FORM_LENGTH 95               /* max length of a form */
#define MAX_SCORE_LENGTH 2               /* max length of a score */ 
#define MAX_WORD_NUM 100                 /* max number of word */
#define SENTENCE_WORD_LEN 23             /* max length of a word in sentence */
#define OVERALL_SCORE "Overall score:"   /* an output string */
#define NOT_FOUND 0                      /* return value in binary search */
#define FOUND 1                          /* return value in binary search */
#define END_OF_DICT '%'                  /* a character in test files */
#define SUCCESS 0                        /* validator for getword */
#define FIRST_WORD 0                     /* index for the first word */
#define ASTERISK "*"                     /* an asterisk */
#define NOT_FOUND_STR "NOT_FOUND"        /* an output string for stage five */
#define END_OF_STR '\0'                  /* end of string null character */
#define NEXT 1                           /* value adjustment */

typedef int data_t;
typedef struct dictionary dict_t;
typedef struct sentence sentence_t;
typedef char word_t[MAX_WORD_LENGTH+1];
typedef char form_t[MAX_FORM_LENGTH+1];
typedef char score_t[MAX_SCORE_LENGTH+1];

/****************************************************************/

/* structures */
struct dictionary {
	word_t word;
	score_t score;
	form_t form;
};

typedef struct {
	sentence_t *head;
	sentence_t *foot;
} list_t;

struct sentence {
	char word[SENTENCE_WORD_LEN+1];
	sentence_t *next;
};

/****************************************************************/

/* function prototypes */
void stage_one(dict_t dict[], int *num_word);
void stage_two(dict_t dict[], int num_word);
void stage_three(list_t *sentence);
void stage_four(dict_t dict[], list_t *sentence, int num_word);
void stage_five(dict_t dict[], list_t *sentence, int num_word);

list_t *create_empty_list();
void print_stage_header(int stage_num);
void read_dict(dict_t dict[], int *num_word);
void read_one_word(char str[]);
void insert_sentence(list_t *list);
int getword(char W[], int limit);
int b_search(dict_t dict[], int lo, int hi, char target[], int *pos);
int form_matching(form_t form, const char word[]);
void free_list(list_t *mylist);


/****************************************************************/

/* main function controls all the actions */
int
main(int argc, char *argv[]) {
	/* declare an array of dictionary store up to 100 words */
	dict_t dict[MAX_WORD_NUM];
	int num_word = 0;
	
	/* stage one: reading all the dictionary words */
	stage_one(dict, &num_word);
	
	/* stage two: compute the analysis for dictionary */
	stage_two(dict, num_word);
	
	/* malloc memory for sentence */
	list_t *sentence = create_empty_list();
	assert(sentence);
	
	/* stage three: read the sentence */
	stage_three(sentence);
	
	/* stage four: labelling and calculating the sentiment score */
	stage_four(dict, sentence, num_word);
	
	/* stage five: handling the variation forms */
	stage_five(dict, sentence, num_word);
	
	/* free memory from sentence list */
	free_list(sentence);
	
	return 0;
}

/****************************************************************/

/* print stage header with given stage number */
void 
print_stage_header(int stage_num) {
	printf(HEADER, stage_num);
}

/****************************************************************/

/* function for malloc a empty list and return the address */
list_t *
create_empty_list() {
	list_t *new_list = (list_t *)malloc(sizeof(list_t));
	assert(new_list);
	new_list->head = new_list->foot = NULL;
	return new_list;
}

/****************************************************************/

/* insert the word into the dictionary structure */
void
read_dict(dict_t dict[], int *num_word) {
	int c, count = 0;
	/* read all the lines until getchar scanned a '%' charcater */
	while ((c = getchar()) != END_OF_DICT) {
	    read_one_word(dict[count].word);
	    read_one_word(dict[count].score);
	    read_one_word(dict[count].form);
	    count++;
	}
	/* mark down the number of words read */
	*num_word = count;
}

/****************************************************************/

/* read a line of input into the given char array */
/* function adapted from assmt1 code and modified */
void
read_one_word(char str[]) {
	int i = 0, c;
	while (((c = getchar()) != EOF ) && c != '\n' && c != '\r' && c != ' ') {
		if (c != '$') {
			str[i++] = c;
		}
	}
	str[i] = END_OF_STR;
}

/****************************************************************/

/* function for insert the sentence into a linked list */
/* code modify based on insert_at_foot() function written by Alistair Moffat
   https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/listops.c */
void 
insert_sentence(list_t *list) {
	sentence_t *new = (sentence_t *)malloc(sizeof(sentence_t));
	assert(new && list);
	
	/* use getword to scan every words in a sentence */
	while (getword(new->word, SENTENCE_WORD_LEN) == SUCCESS) {
    	
    	/* linked up all the words */
	    new->next = NULL;
        if (list->head == NULL) {
		    list->head = list->foot = new;
	    }
	    else {
		    list->foot->next = new;
		    list->foot = new;
	    }
	    /* once linked malloc new memory for the next word */
	    new = (sentence_t *)malloc(sizeof(sentence_t));
	    assert(new);
	}
	free(new);
}

/****************************************************************/

/* Extract a single word out of the standard input, of not
   more than limit characters. */
/* function written by Alistair Moffat, code adapted from the following website
   https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/getword.c */
int
getword(char W[], int limit) {
	int c, len = 0;
	/* skip any non alphabetics */
	while ((c = getchar()) != EOF && !isalpha(c)) {
	}
	if (c == EOF) {
		return EOF;
	}
	/* first character of next word has been found */
	W[len++] = c;
	while (len < limit && (c = getchar()) != EOF && isalpha(c)) {
		/* another character to be stored */
		W[len++] = c;
	}
	/* close the string */
	W[len] = END_OF_STR;
	return SUCCESS;
}

/****************************************************************/

/* stage one: read all the dictionary words, and print out the first */
void 
stage_one(dict_t dict[], int *num_word) {
	print_stage_header(STAGE_NUM_ONE);
	
	/* insert all the words to dictionary */
	read_dict(dict, num_word);
	
	/* print the first word */
	printf("First word: %s\n", dict[FIRST_WORD].word);
	printf("Sentiment score: %d\n", atoi(dict[FIRST_WORD].score));
	printf("Forms: %s\n\n", dict[FIRST_WORD].form);
}

/****************************************************************/

/* stage two: compute the analysis for the dictionary */
void 
stage_two(dict_t dict[], int num_word) {
	print_stage_header(STAGE_NUM_TWO);
	double score_total = 0, word_len = 0; int i;
	
	/* iterate through each word and add up the length as well as the score */
	for (i = 0; i < num_word; i++) {
		word_len += strlen(dict[i].word);
		score_total += atoi(dict[i].score);
	}
	/* print out the result */
	printf("Number of words: %d\n", num_word);
	printf("Average number of characters: %.2f\n", word_len/num_word);
	printf("Average sentiment score: %.2f\n\n", score_total/num_word);
}

/****************************************************************/

/* stage three: read the given sentence, break it into words, 
   store the words in a linked data structure */
void 
stage_three(list_t *sentence) {
	print_stage_header(STAGE_NUM_THREE);
	int c;
	assert(sentence);
	
	/* skip all the '%' characters */
	while ((c = getchar()) == END_OF_DICT) {
	}
	
	/* read and insert the sentence */
	insert_sentence(sentence);
	
	/* print out each word from the list */
	sentence_t *new = sentence->head;
	while (new) {
		printf("%s\n", new->word);
		new = new->next;
	}
	printf("\n");
}

/****************************************************************/

/* stage four: labelling and calculating the sentiment score */
void
stage_four(dict_t dict[], list_t *sentence, int num_word) {
	print_stage_header(STAGE_NUM_FOUR);
	assert(sentence);
	sentence_t *new = sentence->head;
	int tot_score = 0, pos;
	/* the lower bound for binary search */
	int lo = 0;
	
	/* iterate each word from the sentence */
	while (new) {
		
		/* use binary search to match up words from dictionary */
		if (b_search(dict, lo, num_word, new->word, &pos) == NOT_FOUND) {
			printf("%-25s0\n", new->word);
		}
		/* if found print the word and it's sentiment score */
		else {
			printf("%-25s%d\n", new->word, atoi(dict[pos].score));
			tot_score += atoi(dict[pos].score);
		}
		new = new->next;
	}
	
	/* print the overall score */
	printf("%-25s%d\n\n", OVERALL_SCORE, tot_score);
}
			
/****************************************************************/

/* stage five: handling the variation forms */
void 
stage_five(dict_t dict[], list_t *sentence, int num_word) {
	print_stage_header(STAGE_NUM_FIVE);
	assert(sentence);
	int i, flag, tot_score = 0;
	sentence_t *new = sentence->head;
	
	/* walk through the whole sentence list */
	while (new) {
		flag = NOT_FOUND;
		
		/* comparing word with dictionary words and forms */
		for (i = 0; i < num_word; i++) {
			if (strcmp(dict[i].word, new->word) == 0) {
				tot_score += atoi(dict[i].score);
				printf("%-25s%-25s%d\n", new->word, dict[i].word, 
					atoi(dict[i].score));
				flag = FOUND;
			}
			else if (form_matching(dict[i].form, new->word) == FOUND) {
				tot_score += atoi(dict[i].score);
				printf("%-25s%-25s%d\n", new->word, dict[i].word, 
					atoi(dict[i].score));
				flag = FOUND;
			}
		}
		/* if no match up print NOT_FOUND */
		if (flag == NOT_FOUND) {
			printf("%-25s%-25s0\n", new->word, NOT_FOUND_STR);
		}
		new = new->next;
	}
	/* print the overall result */
	printf("%-50s%d\n", OVERALL_SCORE, tot_score);
}

/****************************************************************/

/* function for matching sentence word and variation forms */
int
form_matching(form_t form, const char word[]) {
	form_t temp;
	strcpy(temp, form);
	
	/* get the first token */
	char *token = strtok(temp, ASTERISK);
	while (token) {
		/* scan for match up, if match return FOUND */
		if (strcmp(token, word) == 0) {
			return FOUND;
		}
		token = strtok(NULL, ASTERISK);
    }
    return NOT_FOUND;
}

/****************************************************************/

/* search a string within a 2D char array using binary search */
/* Written by Alistair Moffat, code adapted from the following website
   https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/binarysearch.c */
int
b_search(dict_t dict[], int lo, int hi, char target[], int *pos) {
	int mid, outcome;
	if (lo >= hi) {
		return NOT_FOUND;
	}
	/* compute the middle point */
	mid = (lo + hi)/2;
	
	/* do recursive binary search */
	if ((outcome = strcmp(target, dict[mid].word)) < 0) {
		return b_search(dict, lo, mid, target, pos);
	}
	else if (outcome > 0) {
		return b_search(dict, mid+NEXT, hi, target, pos);
	}
	/* match found */
	else {
		*pos = mid;
		return FOUND;
	}
}

/****************************************************************/

/* function to free the sentence linked list */
/* function adapted and modified based on code written by Alistair Moffat
   from the following website */
/* https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/listops.c */
void 
free_list(list_t *list) {
    sentence_t *new;
    assert(list);
    while (list->head != NULL) {
        new = list->head->next;
        free(list->head);
        list->head = new;
    }
    free(list);
    list = NULL;
}

/****************************************************************/

/* Algorithms are fun! */