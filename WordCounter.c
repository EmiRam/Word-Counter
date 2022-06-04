/*
 * File Name: WordCounter.c
 * Created: August 9th, 2018
 * Author: Emily Ramanna
 * Description: - Word Summary for a text file
 *		- Accepts a command line argument for a filename and an
 *                optional command line argument for how many words to list
 *              - Reads a file, calculates number of words, unique words,
 *                displays summary also showing most frequently occurring words.
 *              
 *                
 */

/** REQUIRED HEADER FILES */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/** REQUIRED DEFINE STATEMENTS */
#define MAX_FILE 1000000
#define MAX_LINE_SIZE 1024
#define MAX_WORD 32
#define MAX_HASHTABLE 3000
#define MAX_UNIQUE_WORDS 5000

/** STRUCTURE DEFINITIONs */

typedef struct word {
	char str[MAX_WORD]; //char array containting word
	int freq; //int containing frequency of word
} Word;

typedef struct node {
	Word wrd; //word struct
	struct node* pNext; //pointer to next node
} Node;

typedef struct{
	Node* pHead; //pointer to top of list
} LinkedList;

/** FUNCTION PROTOTYPES */
int readFile(char fileName[], LinkedList*, Node**, int*);

int insertWord(char word[], LinkedList*, Node**, int*);

Node* createNode(char newWord[]);

int getHashKey(char newWord[]);

void addAtHead(LinkedList*, Node*);

int compare (const void*, const void*);

/** MAIN PROGRAM*/
 
 /*
 *main: calls functions to calculate word information from a file
 *
 *inputs: Accepts a string filename through a command line argument,
 *        can also accept an optional int command line argument to
 *        specify how many frequent words to list.
 *
 *returns: int which can represent successful or failed completion.
 */
int main(int argc, char *argv[]) {
	
	int numSummaryItems; //number of frequent words to list in summary
	int sortIndex = 0; // index for the Node pointers array
	int numWords = 0; //will contain number of all words in the file
	
	if (argc < 2){
		printf("Invalid number of arguments\n");
		printf("Usage: %s file_name\n", argv[0]);
		return 1;
	}
	
	//handles optional command line argument regarding how many words are shown
	if(argc == 3){
		numSummaryItems = atoi(argv[2]);
	} else {
		numSummaryItems = 10;
	}
	
	//creates a hash table which will contain a Linked List at each index
	LinkedList* pHashTable = calloc(MAX_HASHTABLE + 1, sizeof(LinkedList*));
	if(pHashTable == NULL){
		printf("Not enough memory to create hash table.\n");
		return 1;
	}

	//creates an array of Node pointers
	Node** uniqueWords = calloc(MAX_UNIQUE_WORDS, sizeof(Node*));
	if(uniqueWords == NULL){
		printf("Not enough memory for sorting array.\n");
		return 1;
	}
	
	//gets number of words from function which also organizes word data
	numWords = readFile(argv[1], pHashTable, uniqueWords, &sortIndex);
	
	//if the file could not be read...
	if ( numWords == -1) {
		printf("Failed to load file\n");
		return 1;
	}
	
	/* this is for very small files where the number of unique words
	* is less than the number requested for the output
	*/
	if(numSummaryItems >= sortIndex){
		numSummaryItems = sortIndex;
	}
	
	//prints first part of summary
	printf("%s\nSUMMARY:\n\t%d words\n\t%d unique words\n\nWORD FREQUENCIES (TOP %d):\n",
		argv[1], numWords, sortIndex, numSummaryItems);
	
	//frees hash Table array
	free(pHashTable);
	
	//uses qsort() to sort in descending order contents of Node pointers array
	qsort(uniqueWords, sortIndex, sizeof(Node*), compare);
	
	//prints frequent word data
	for(int i = 0; i < numSummaryItems; i++){
			
		printf("%-32s%d\n", uniqueWords[i]->wrd.str, uniqueWords[i]->wrd.freq);
	}
	
	//frees individual Nodes
	for(int i = 0; i > sortIndex; i++){
		free(uniqueWords[i]);
	}
	
	//frees Node pointers array
	free(uniqueWords);
	
	return 0;
}

/*
 *compare: a simple function to compare Nodes for qsort()
 *
 *inputs: void pointers which point to Nodes
 *
 *returns: int with difference between dereferenced pointer values
 */
int compare (const void * a, const void * b){
	/*Compares word frequencies, and since descending order is desired,
	* the first frequency is subtracted from the second.
	*/
	return (*(Node**)b)->wrd.freq - (*(Node**)a)->wrd.freq;
}

/*
 *insertWord: Finds a word's place in a hash table, determines if the word 
 *            is already in the table. If it is, updates the frequency of
 *            the word. If not, creates a new Node for the Hash Table, adds it,
 *            keeps a pointer to the Node in an array, and updates that array's
 *            index.
 *
 *inputs: char array (string) of word to be processed, pointer to a Linked List
 *        (which is a collection of Linked Lists representing a hash table), an
 *        array of pointers to Nodes, int pointer which holds current index of
 *        previously mentioned array.
 *
 *returns: int to indicate whether word passed was legitimate or not
 */
int insertWord(char word[], LinkedList* pHashTable, Node** uniqueWords, int* pSortIndex){
	
	//word is too long to be legitimate for this application's purposes
	if(strlen(word) > 31){
		return 1;
	}
	
	//ensures word from file is correct size
	char toPrint[MAX_WORD];
	strcpy(toPrint, word);
	
	int hashKey = getHashKey(toPrint); //hash key of word to be processed
	
	int wordIsFound = 0;//flag to show if a word already exists in hash table
	
	//Node pointer to help iterate through hash table list
	Node* pTemp = pHashTable[hashKey].pHead;
	
	while(pTemp != NULL){
		
		//if a word already exists in the hash table, update its frequency
		if(strcmp(pTemp->wrd.str, toPrint) == 0){
			pTemp->wrd.freq++;
			wordIsFound = 1;
			break;
		}
		pTemp = pTemp->pNext;
	}
	
	//if a word does not already exist in the hash table...
	if(wordIsFound != 1){
		//create a new Node for it
		Node* pNewNode = createNode(toPrint);
		
		/*add that Node to the head of the Linked List located at the hash key
		* index of the hash table
		*/
		addAtHead(&pHashTable[hashKey], pNewNode);
		
		//add pointer to the Node to the unique words array
		uniqueWords[*pSortIndex] = pNewNode;
		
		//update current index of the unique words array
		*pSortIndex = (*pSortIndex) + 1;
		
		//checks if there are too many unique words for the array to hold
		if(*pSortIndex == (MAX_UNIQUE_WORDS - 1)){
			printf("Exceeded number of allowed unique words...exiting");
			exit(1);
		}
	}
	
	return 0;
}

/*
 *readFile: Attempts to open and read a file a line at a time. Breaks line into
 *          tokens, which for the purposes of this application will be 
 *          considered words. Calls insertWord() to process words read.      
 *
 *inputs: char array (string) of of a file name, pointer to a Linked List
 *        (which is a collection of Linked Lists representing a hash table), an
 *        array of pointers to Nodes, int pointer which holds current index of
 *        previously mentioned array.
 *
 *returns: int that contains total amount of words read from the file.
 */
int readFile(char fileName[], LinkedList* hashTable, Node** uniqueWords, int* pSortIndex)
{
	FILE *fptr; //file pointer to be used for reading
	
	/*Please note this list to be used with strtok() is taken from:
	*https://stackoverflow.com/questions/27647959/hash-table-sort-structure-with-qsort
	*written by user: Scotoner, On: December 25th 2014 
	*/
    const char *splitOn = " `?!'\";,.:+-*&%(){}[]<>\\\t\n";

    char line[MAX_LINE_SIZE]; //array to hold lines from file
    char *pTokens; //char pointer to hold results from strtok()
    int numWords = 0; //will contain total number of words in the file
    int index = 0; //used to iterate through pTokens char array
	
	if((fptr = fopen(fileName, "r")) == NULL){
		printf("Error: failed to open %s\n", fileName);
		return -1;
	}
	
    while((fgets(line,MAX_LINE_SIZE, fptr)) != NULL)
    {
        for(pTokens = strtok(line, splitOn); pTokens; pTokens = strtok(NULL, splitOn)){
        	//builds each word string
			while(pTokens[index] != '\0'){
				pTokens[index] = tolower(pTokens[index]);
				index++;
			}
            
            //function call to process newly read word
            insertWord(pTokens, hashTable, uniqueWords, pSortIndex);
           
            numWords++;
            index = 0;
        }      
    }

	fclose(fptr);

	return numWords;	
}

/*
 *createNode: Dynamically allocates memory to create a new Node.
 *
 *inputs: char array (string) that the data member of the Node should carry.
 *
 *returns: Pointer to the newly created Node.
 */
Node* createNode(char newWord[]){
	
	Node* pNewNode = (Node*) malloc(sizeof(Node));
	
	//checks if malloc was successful
	if(pNewNode == NULL){
		printf("Out of memory...exiting");
		exit(1);
	}
	
	//initializes the new Node's data members 
	strcpy(pNewNode->wrd.str, newWord);
	pNewNode->wrd.freq = 1;
	pNewNode->pNext = NULL;
	return pNewNode;
}

/*
 *getHashKey: Generates a hash key for a word based on its characters.
 *           
 *inputs: char array (string) that should be hashed.
 *
 *returns: int containing hash key.
 */
int getHashKey(char newWord[]){
	
	int hashKey = 0;
	int index = 0;
	
	/*Simply adds the characters, but gives more weight to the first 3 
	* characters of the string.
	*/
	while(newWord[index] != '\0'){
		if(index == 0){
			hashKey += (int) (newWord[index] * 7);
		} else if (index == 1){
			hashKey += (int) (newWord[index] * 5);
		} else if (index == 2){
			hashKey += (int) (newWord[index] * 3);
		} else {
			hashKey += (int) newWord[index];
		}
		index++;
	}
	
	//performs modulus so that the hash key is a viable index in the hash table
	hashKey = hashKey % MAX_HASHTABLE;
	
	return hashKey;
}

/*
 *addAtHead: Adds a Node to the head of a linked list.
 *
 *inputs: Pointer to a linked list structure. Pointer to the Node that will be added.
 *
 *returns: void
 */
void addAtHead(LinkedList* pList, Node* pNewNode){
	//if the list is empty, sets head and tail pointers to the new Node
	if(pList->pHead == NULL){
		pList->pHead = pNewNode;
	//else put the new Node at the head	
	}else{
		pNewNode->pNext = pList->pHead;
		pList->pHead = pNewNode;
	}
}
