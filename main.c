/*----------------------------------------------
 * Project 3:
 * Class: CS 211, Fall 2020
 * By Nicholas Rachfal
 * ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1;
#define FALSE 0;

// struct that holds the maze arr, maze size, and start/end positions
typedef struct mazeStruct
{
    char **arr;
    int xsize, ysize;
    int xstart, ystart;
    int xend, yend;
} maze;

// struct used to navigate the maze
typedef struct stackStruct
{
    int xpos;
    int ypos;
    struct stackStruct *next;
} stack;

/////*FUNCTION PROTOTYPES*/////
int isEmpty (stack *head);
void pop (stack** head, int debug);
void push (stack** head, int x, int y, int debug);
void readFileAndInitMaze(maze *maze, FILE *src);
void printReverse(stack* head);

//begin main
int main (int argc, char **argv)
{
    setbuf(stdout, NULL);

    maze m1;    // declare instance of struct maze

    FILE *src;  // declare pointer to file

    int debugMode = FALSE;  // declare debug variable to toggle debugMode

    /* verify the proper number of command line arguments were given */
    if(argc > 3) {
        fprintf(stderr, "Too many input files.\n");
        exit(-1);
    }
    for (int k =0; k < argc; k++)   // for loop used to read command line arguments
    {
        if (strcmp(argv[k], "-d") == 0) // checks each arguments for the "-d" flag
        {
            debugMode = TRUE;   // if found set debugMode to TRUE
        }
    }

    /* Try to open the input file. */
    if ( ( src = fopen( argv[1], "r" )) == NULL )
    {
        printf ( "Can't open input file: %s", argv[1] );
        exit(-1);
    }


    int lineCount= 0;
    /* read in the size, starting and ending positions in the maze */
    lineCount = lineCount + fscanf (src, "%d %d", &m1.xsize, &m1.ysize);
    lineCount = lineCount + fscanf (src, "%d %d", &m1.xstart, &m1.ystart);
    lineCount = lineCount + fscanf (src, "%d %d", &m1.xend, &m1.yend);

    if (lineCount < 6) // if 3 lines of 6 different values were not read in, prompt user
    {
        fprintf(stderr, "Invalid data file.");
        exit(-1);
    }

    if(m1.xsize < 1 || m1.ysize < 1 )   // check for a valid maze size greater than 0
    {
        // if maze out of range prompt user exit program
        fprintf(stderr, "Maze sizes must be greater than 0.");
        exit(-1);
    }

    // compare the start/end positions of maze with the size of the maze
    if ((m1.xstart > m1.xsize || m1.ystart > m1.ysize) || (m1.xend > m1.xsize || m1.yend > m1.ysize))
    {
        // if start/end position is larger than the column or row size of the maze, prompt user, exit program
        printf ("size: %d, %d\n", m1.xsize, m1.ysize);
        printf ("start: %d, %d\n", m1.xstart, m1.ystart);
        printf("Start/End position outside of maze range.");
        exit(-1);
    }
    /* print them out to verify the input */
    printf ("size: %d, %d\n", m1.xsize, m1.ysize);
    printf ("start: %d, %d\n", m1.xstart, m1.ystart);
    printf ("end: %d, %d\n", m1.xend, m1.yend);

    // read the file coordinates and initialize the maze
    readFileAndInitMaze(&m1, src);

    /*Close the file*/
    fclose(src);


    stack *head = NULL; // declare variable for linked list

    push(&head, m1.xstart, m1.ystart, debugMode); // push the first starting value to the list


    int coinCount = 0;  // declare variable to keep track of collected coins

    // run maze algorithm
    while (head != NULL)
    {
        // if we are at the end of the maze, break from loop
        if(head->xpos == m1.xend && head->ypos == m1.yend)
        {
            printf("\n");
            break;
        }
        // if the next down position is not blocked and not visited, move there, mark as visited, and push position to linked list
        if (m1.arr[head->xpos + 1][head->ypos] != 'V' && m1.arr[head->xpos + 1][head->ypos] != '*' )
        {
            // check if next down position is a coin and store it
            if (m1.arr[head->xpos + 1][head->ypos] == 'C')
            {
                coinCount =coinCount +1;
            }
            m1.arr[head->xpos + 1][head->ypos] = 'V';
            push(&head, head->xpos + 1, head->ypos, debugMode);

        }
        // if the next right position is not blocked and not visited, move there, mark as visited, and push position to linked list
        else if(m1.arr[head->xpos][head->ypos+1] != 'V' && m1.arr[head->xpos][head->ypos+1] != '*')
        {
            // check if next down position is a coin and store it
            if (m1.arr[head->xpos][head->ypos+1] == 'C')
            {
                coinCount =coinCount +1;

            }
            m1.arr[head->xpos][head->ypos+1] = 'V';
            push(&head, head->xpos, head->ypos+1,debugMode);

        }
        // if the next up position is not blocked and not visited, move there, mark as visited, and push position to linked list
        else if(m1.arr[head->xpos-1][head->ypos] != 'V' && m1.arr[head->xpos-1][head->ypos] != '*')
        {
            // check if next down position is a coin and store it
            if (m1.arr[head->xpos-1][head->ypos] == 'C')
            {
                coinCount =coinCount +1;

            }
            m1.arr[head->xpos-1][head->ypos] = 'V';
            push(&head, head->xpos-1, head->ypos, debugMode);

        }
        // if the next left position is not blocked and not visited, move there, mark as visited, and push position to linked list
        else if(m1.arr[head->xpos][head->ypos-1] != 'V' && m1.arr[head->xpos][head->ypos-1] != '*')
        {
            // check if next down position is a coin and store it
            if (m1.arr[head->xpos][head->ypos-1] == 'C')
            {
                coinCount =coinCount +1;

            }
            m1.arr[head->xpos][head->ypos-1] = 'V';
            push(&head, head->xpos, head->ypos-1, debugMode);
        }
        // if no where to move, pop coordinate from list
        else
        {
            pop(&head, debugMode);
        }
    }

    // if the linked list is empty, no solution was found
    if (isEmpty(head))
    {

        printf("\nThis maze has no solution.");
    }
    else
        //otherwise print the solution, the coins collected, and path from start to end
    {
        printf("The maze has a solution.\n");
        printf("The amount of coins collected: %d\n", coinCount);
        printf("The path from start to end:\n");
        printReverse(head);
        printf("\n");
    }
    // free the remaining nodes from linked list
    while(head != NULL )
    {
        pop(&head, debugMode);
    }
    // free the our 2d dynamic array
    for (int i = 0; i < m1.xsize+2; ++i)
    {
        free(m1.arr[i]);
    }
    free(m1.arr);

}
//---------------------------//
/***FUNCTION DEFINITIONS***/
//---------------------------//

int isEmpty (stack *head) {  // function that returns true/false value depending on the amount of nodes in linked list
    if (head == NULL) {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
void pop (stack** head, int debug)  // pass by reference pop function that free the top of the stack and updates
                                    // the head of the linked list
{
    stack *ptr = *head;  // point at the first node

    if (ptr != NULL)    // if stack is not empty
    {
        *head = (*head)->next;  // assign head to second node

        // if debug mode is enabled, print the character pushed onto the stack
        if (debug == 1)
        {
            printf ( "(%d,%d) popped off the stack.\n", ptr->xpos ,ptr->ypos);
        }
        free(ptr);  // free the first node
    }
}
void push (stack** head, int x, int y, int debug)  // pass by reference push function to push nodes on stack
{
    stack *ptr = (stack*) malloc (sizeof(stack));  // allocate memory for new node
    ptr->xpos = x;
    ptr->ypos = y;                                  // assign data items
    ptr->next = *head;
    *head = ptr;                                    // update head

    // if debug mode is enabled, print the character pushed onto the stack
    if (debug == 1)
    {
        printf ( "(%d,%d) pushed into the stack.\n", x ,y);
    }
}
void readFileAndInitMaze(maze *maze, FILE *src)  // function that reads file and initializes the maze accordingly
{
    stack stack;        // declare instance of struct stack
    int i,j;

    // allocate memory for our 2d maze array
    maze->arr = (char **) malloc(sizeof(char *) * (maze-> xsize + 2));
    for(i = 0; i < maze -> xsize + 2; i++){
        maze->arr[i] = (char *) malloc(sizeof(char) * (maze -> ysize + 2));
    }

    /* initialize the maze to empty */
    for (i = 0; i < maze ->xsize+2; i++)
        for (j = 0; j < maze->ysize+2; j++)
            maze ->arr[i][j] = '.';

    /* mark the borders of the maze with *'s */
    for (i=0; i < maze->xsize+2; i++)
    {
        maze->arr[i][0] = '*';
        maze->arr[i][maze->ysize+1] = '*';
    }
    for (i=0; i < maze->ysize+2; i++)
    {
        maze->arr[0][i] = '*';
        maze->arr[maze->xsize+1][i] = '*';
    }

    /* mark the starting and ending positions in the maze */
    maze->arr[maze->xstart][maze->ystart] = 's';
    maze->arr[maze->xend][maze->yend] = 'e';


    // read in the lines that contain the 3 values: coordinate x,y, and type
    char character;
    while (fscanf (src, "%d %d %c", &stack.xpos, &stack.ypos, &character) != EOF)
    {
        // check for valid type. if not valid continue to next input
        if (character != 'b' && character != 'c')
        {
            fprintf(stderr,"Invalid type: type is not recognized.\n");
            continue;
        }
        // check for valid start. if not valid continue to next input
        if (stack.xpos == maze->xstart && stack.ypos == maze->ystart)
        {
            fprintf(stderr,"Invalid coordinates: attempting to block start/end position.\n");
            continue;
        }
        // check for valid end. if not valid continue to next input
        if (stack.xpos == maze->xend && stack.ypos == maze->yend)
        {
            fprintf(stderr,"Invalid coordinates: attempting to block start/end position.\n");
            continue;
        }
        // check for valid in range coordinates. if not valid continue to next input
        if ((stack.xpos < 1 || stack.xpos > maze->xsize) || (stack.ypos < 1 || stack.ypos > maze->ysize))
        {
            fprintf(stderr, "Invalid coordinates: outside of maze range.\n");
            continue;
        }
        // if character b is read in from file, assign the border
        if (character == 'b')
        {
            character = '*';
            maze->arr[stack.xpos][stack.ypos] = character;
        }
        //otherwise the character has to be a C
        else
        {
            character = 'C';
            maze->arr[stack.xpos][stack.ypos] = character;
        }
    }
    /* print out the initial maze */
    for ( i = 0; i < maze->xsize+2; i++)
    {
        for (j = 0; j < maze->ysize+2; j++)
        {
            printf("%c", maze->arr[i][j]);
        }
        printf("\n");
    }
}

void printReverse(stack* head)
{
    // Base case
    if (head == NULL)
        return;

    // print the list after head node
    printReverse(head->next);

    // After everything else is printed, print head
    printf("(%d,%d) ", head->xpos, head->ypos);
}
