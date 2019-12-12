#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>


/*
Adam Davis
davis.4752@osu.edu
Compile: gcc shellC.c
run: a.out
*/
#define MAXLINE 80 /* 80 chars per line, per command, should be enough. */

/** The setup() routine reads in the next command line string storing it
in the input buffer.
The line is separated into distinct tokens using whitespace as delimiters.  Setup also
modifies the args parameter so that it holds points to the null-terminated strings which
are the tokens in the most recent user command line as well as a NULL pointer, indicating the
end of the argument list, which comes after the string pointers that have been assigned to
args. ***/

void setup(char inBuffer[], char *args[],int *bkgd)

{
	int length,  /* #  characters in the command line */
	start,   /* Beginning of next command parameter           */			
	i,       /* Index for inBuffer arrray          */
	j;       /* Where to place the next parameter into args[] */
	/* Read what the user enters */
	length = read(STDIN_FILENO, inBuffer, MAXLINE);

	start = -1;
	j = 0;

	if (length == 0)
		exit(0);            /* Cntrl-d was entered, end of user command stream */
	if (length < 0){
		perror("error reading command");
		exit(-1);           /* Terminate with error code of -1 */
	}/* Examine every character in the input buffer */
	for (i = 0; i < length; i++) {
		switch (inBuffer[i]){
		case ' ':
		case '\t' :          /* Argument separators */
		
		if(start != -1)
			{args[j] = &inBuffer[start];    /* Set up pointer */
			j++;
		}
		inBuffer[i] = '\0'; /* Add a null char; make a C string */
		start = -1;
		
		break;
	
	case '\n':             /* Final char examined */
	if (start != -1){
		args[j] = &inBuffer[start];
		j++;}inBuffer[i] = '\0';
		args[j] = NULL; /* No more arguments to this command */
		break;
	case '&':
		*  bkgd = 1;
		inBuffer[i] = '\0';
		break;
		default :             /* Some other character */
			if (start == -1)
				start = i;
			}
		}
		args[j] = NULL; /* Just in case the input line was > 80 */
	}
	
	/*node for link list to contain each value in command history*/
typedef struct node{
	int indexNum;		/*Command number*/
	char command[MAXLINE];  /*The command entered*/
	struct node *next;      /*The next node in the link list*/
	struct node *previous;
}history;
	
	
	
void addNode(int index, char *args[], struct node *start){ //Adds node to link List
	
	

	 history*currentNode = NULL;    //Make the current node
	 currentNode = malloc(sizeof(history)); 
	 currentNode->indexNum = index;    //set the nodes index
         strcpy(currentNode->command, "");
	 int n;
	 
	 for(n = 0; args[n] != NULL; n++){   //concatenates all string in args to command
	  strcat(currentNode->command, args[n]);
	   strcat(currentNode->command, " ");
	 }
	  strcat(currentNode->command, "\0"); //Add \0 for end of string
	
	 if(start->next != NULL){  //If node is the first node
	 	start->next->previous = currentNode;
	 }
	 currentNode->previous = start;  //Sets previous to start
	 currentNode->next = start->next; //Sets next to the last top node
	 start->next = currentNode; //Points header to current Node
	
	
}

int removeDuplicates(struct node *start, int num){ //Removes all nodes that match the first
	
	history*hold = start->next; //Temp node to look through history
	int n;
	for(n = 1; n < num; n++){  //loops through each node in history
	
		hold = hold->next;
		if(strcmp(hold->command, start->next->command)== 0){ //if match the oldest node is deleted
			if(hold->next != NULL){
			hold->previous->next = hold->next;
			hold->next->previous = hold->previous;
			}
			num--;
			break;
		}
	}
	return num;  //return the current number of nodes
}

int setHistory(char *args[], int maxNum){	//Sets the length that history can be and checks if value is a positive integer


	int newLimit = atoi(args[1]);  
		if(newLimit > 0){ 
		maxNum = newLimit;
		}
		else{
		printf("ERROR: Number must be greater than 0\n");
		}
	
    return maxNum; //Returns previous max if there is an error or new value if not
}
	
void printHistory(struct node *start, int num){ //Print the link list containing the history of commands
	
	int n;
	history*hold = start;
	for(n = 0; n < num; n++){  //Gets to the end of the link list
		hold= hold->next;	
	}
	for(n = 0; n < num; n++){  //Loops through each node in history printing its index number and it's commands
	printf("%d", hold->indexNum);
	printf(" %s\n", hold->command);
	hold = hold->previous;
	}
	
	
		
}


int findIndexNum(struct node *start, int num, int userInput){ //Finds the position that matches the userInput. Returns -1 if it does not exist
	int pos = -1;
	int n;
	int currentNode = 1;
	history*hold = start;
	for(n = 0; n < num; n++){   //Loops through each node
		hold= hold->next;
		if(hold->indexNum == userInput){ //If there is a matching index the pos is set and the loop is broken
			pos = currentNode;
			break;
		}
		else{
		currentNode++;
		}	
	}

	return pos; //Return position found or -1 if it was not in the history
}

int findIndexStr(struct node *start, int num, char userInput[]){ //Finds the positon of the earliest node that matches the string


	int pos = -1; //Current cycle
	int n;        //value for outer for loop
	int i;        //value for inner for loop
	int currentNode = 1; //current node
	int length = 0;      //length of userInput
	history*hold = start;
	char check = userInput[length];
	char nodeCheck;
	while(check != '\0'){   //Sets length to the length of userInput
		length++;
		check = userInput[length];
	}
	for(n = 0; n < num; n++){   //loops through each node
		hold= hold->next;
		for(i = 0; i < length; i++){  //checks the command to see if they match userInput
			char nodeCheck = hold->command[i];
			if(userInput[i] == nodeCheck){
				pos = currentNode;
			}else{
				pos = -1; //If one doesnt match pos set to -1 and cycle broke
				break;
			}
			
		}
		if(pos > 0){ //If node found loop is exited
		break;
		}
		currentNode++;
	}
	
	
	


 
	return pos;
}

void getCommand(struct node *start, int pos, char inBuffer[], char *args[], int *bkgd){ //Goes to the node in the link list and gets it ready to be executed

	strcpy(args[0], " "); //Set the first arg back to Empty
	strcpy(inBuffer, ""); //sets inBuffer back to Empty
	
	history*hold = start;
	
	
	for(pos; pos > 0; pos--){ //Sets node to the desired node in the linked list
		hold = hold->next;
	}


	
	int j = 0; //Args array
	int count = -1; //command position
	int wordStart = 0; //start of the last word	
	char check = 1;  //Check to see if its at the end of string or a space
	int size = -1; //size of the current word
	
	while(check != '\0'){ //Loops until at the end of the string
	
			
			
		while( check != ' '){  //Loops till space is found
			check = hold->command[count];		
			size++;
			count++;
			
		}
		if(j > 0){ //If not the first element gives the args[j] space in memory
			args[j]= malloc(sizeof(args[0])); 
			strcpy(args[j], "");
		}      
		size--;
		
		strncpy(args[j], hold->command + wordStart, size); //Copies the string to args at position j
		args[j][size] = '\0';
		
		wordStart = count;
	
		check = hold->command[count];
		
		j++;
		size = 0;
		
		
	}
		
	strcpy(inBuffer, args[0]); //Set the first value  in args to inBuffer
	
	if(j == 1){ //If theres only one argument set the next to NULL
		args[1] = NULL;
	}

	
}

int *loadHistory(struct node *start , FILE *fp){

         
	 char line[MAXLINE];
	 int index = 1;
	 int EOFcheck; //Check to see if it's at the end of the file
	 static int  LoadValues[3];
	 LoadValues[0] = 12;  //Default max Size of history
	 LoadValues[1] = 0;   //Default size of Current Command number
	 LoadValues[2] = 0;   //Default size of number of commands
	 if(fp != NULL){
		fgets(line, MAXLINE, fp);  //Gets first line = max size of history
		LoadValues[0] = atoi(line);
		EOFcheck = fgetc(fp);
		index = EOFcheck - 48; //sets char to int
		int hold = fgetc(fp);  //checks for space
		while(hold > 47 && hold < 10 + 48){ //cylces till space
			index = (index * 10) + (hold - 48);
			hold = fgetc(fp);		
		}
		fgets(line, MAXLINE, fp); //Gets command
		
		while(EOFcheck != EOF){ //Ends at end of the history file
			
			
			

			
			LoadValues[1] = index; //Current code number = last command number
			LoadValues[2]++;       //incrment number of commands
			
			
			char *pos;
			if ((pos=strchr(line, '\n')) != NULL){ //replace the \n at the end of the sting with \0
    				*pos = '\0';
			}
			
			history*currentNode = NULL;    //Make the current node
	 		currentNode = malloc(sizeof(history)); 
	 		currentNode->indexNum = index;    //set the nodes index
	 			
	 		strcat(currentNode->command, line);
	 			
	 		if(start->next != NULL){  //If node is the first node
	 			start->next->previous = currentNode;
			 }
	 		currentNode->previous = start;  //Sets previous to start
	 		currentNode->next = start->next; //Sets next to the last top node
	 		start->next = currentNode; //Points header to current Node
	 			
			
			
			EOFcheck = fgetc(fp);  //check for EOF
			index = EOFcheck - 48; //Turns it in to a int
			hold = fgetc(fp);              
			while(hold > 47 && hold < 10 + 48){   //Loops till space
				index = (index * 10) + (hold - 48);
				hold = fgetc(fp);
				
			}
			fgets(line, MAXLINE, fp); //Gets net command
		}
		
	 
	 }
	 
	 return LoadValues; //Returns maxNumber, currentCommand, and numOfCommands
}



void SaveHistory(struct node *start, int num, int Max, FILE *fp){ //Print the link list containing the history of commands
	
	int n;
	fprintf(fp, "%d\n", Max); //Prints the Maximum size ofthe history
	history*hold = start;
	for(n = 0; n < num; n++){  //Gets to the end of the link list
		hold= hold->next;	
	}
	for(n = 0; n < num; n++){  //Loops through each node in history printing its index number and it's commands
	fprintf(fp,"%d ", hold->indexNum);  //Prints the index
	fprintf(fp,"%s\n", hold->command);  //Prints command
	hold = hold->previous;
	}
	
		
}

	
	
int main(void){
		
	char inBuffer[MAXLINE]; /* Input buffer  to hold the command entered */
	char *args[MAXLINE/2+1];/* Command line arguments */
	int bkgd;             /* Equals 1 if a command is followed by '&', else 0 */
  
	int runChildProcess; //0 dosn't run, 1 runs
	int restart;
	char filePath[100];
	strcpy(filePath, getenv("HOME"));
	strcat(filePath, "/history.txt");
	history*head = NULL; //Creates a head node for link list
	head = malloc(sizeof(history));
	head->indexNum = -1;
	head->next = NULL;
	head->previous= NULL;
	FILE *fp;
	fp = fopen (filePath, "r");
	int *values = loadHistory(head, fp);
	if(fp != NULL){
		fclose(fp);
	}
	int maxNumber  = *(values);  /*The current number of jobs in History*/  
	int currentCommand = *(values + 1); 
	int numOfCommands = *(values + 2); 
	
	
	while (1){            /* Program terminates normally inside setup */			
			
		bkgd = 0;                /* Equals 1 if a command is followed by '&', else 0 */
		printf("2431SysII: ");  /* Shell prompt */
		fflush(0);
		setup(inBuffer, args, &bkgd);       /* Get next command */
		runChildProcess = 1;             /*if child process need to be created*/
		restart = 0;			/*if everything needs to be reset*/
		
			/* Fill in the code for these steps:
			(1) Fork a child process using fork(), 
			(2) The child process will invoke execvp(), 
			(3) If bkgd == 0, the parent will wait,
				o/w returns to the setup() function. */
		
		if(strcmp(inBuffer, "rr")== 0){ //If users first argument is rr
	
			if(numOfCommands > 0){
				
				getCommand(head, 1, inBuffer, args, &bkgd); //Set up the last command to be executed
			}
			else{
				printf("ERROR: No commands in history\n"); //Error if nothing is in history
				restart = 1;
			}

			
		}
		
		else if(strcmp(inBuffer, "r") == 0){ //If users first argument is r
			
			if(args[1] == NULL){
				printf("ERROR: no command after r\n"); //If user only entered r
				restart=1;
			}
			else{
				int pos = -1;
				pos = findIndexNum(head, numOfCommands, atoi(args[1]));   //Check if matching index
			
				if(pos < 1){
					pos = findIndexStr(head, numOfCommands, args[1]); //check if matching str	
				}
				
				if(pos > 0){
					getCommand(head, pos, inBuffer, args, &bkgd); //runs if a match is found
				}
				else{
					printf("ERROR: no file with the index or starts with your input\n"); //error message if nothing is found
					restart = 1;
				}

			}
		
		
		}
		if(restart == 0){
			
			if(strcmp(inBuffer, "sethistory")== 0){ //changes the size of the history
				maxNumber = setHistory(args, maxNumber);
				runChildProcess  = 0;
			
			}
			else{ //If regular command / bad command

			
				currentCommand++;   //Increase Command Index
				addNode(currentCommand, args, head);
				numOfCommands++;	//Increase the # of commands in history
			
				numOfCommands = removeDuplicates(head, numOfCommands); //Removes duplicates and make sure there is only a certain amount of entires
				if(numOfCommands > maxNumber){
					numOfCommands = maxNumber;
				}
			
			
				
		}
			if(strcmp(inBuffer, "history")== 0 || strcmp(inBuffer, "h") == 0){ //Prints History
				printHistory(head, numOfCommands);
				runChildProcess = 0;	
			}
			
			if(strcmp(inBuffer, "cd")== 0){ //Changes Directory if it exists
				int check = chdir(("%s", args[1]));
				if(check == -1){
				   printf("Error: No Such Directory\n");
				}
			
				 runChildProcess = 0;		
			} 
			
			
		}else{
			runChildProcess = 0;
		}	
			
		
		if(runChildProcess == 1){
		
			pid_t pid = fork();    /*Child process is created*/


			if(pid == -1){     /*Fork fails*/
		                              
				printf("Error: child proccess failed\n");
				exit(1);
			}

			if(pid == 0){    /*If child process*/
			
				int status = execvp(inBuffer, args);   /* Perform user's command */

				if(status == -1){           
			 		printf("ERROR: Command not Found\n");
			  	}
				 exit(1);                        /*End child Process*/
			}
			else if(bkgd == 0){               /* If command didn't ended in '&',*/
							/* then parent process waits for child process*/
				wait(&pid);
				
			}
			

		}//runChildProccess
		
		fp = fopen (filePath, "w"); 			//Saves history to file
		SaveHistory(head, numOfCommands, maxNumber, fp);
		fclose(fp);

	}//while
}//main
