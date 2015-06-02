﻿// Done by TheTutor

/*
	The Bubble Sort is just one of many methods out there to "sort lists of data".  It is
	certainly NOT the fastest method, but it's pretty simple to implement and if you don't need
	a blazing fast sort routine, it will get the job done.

	The Bubble Sort works like this:

		1)  First you have to have some "list of data" (let's say an array of integers).
		2)  Next you need to decide if you want the data arranged least to greatest (A - Z)
			or greatest to least (Z - A).  Let's suppose we go with "least to greatest".
		3)  Next, you start with the FIRST element and you loop until you get to the
			end of the array.
		4)  Inside of this loop, you have another loop that also starts at the beginning of
			the array.  You check the element from the outer loop against the element in
			the inner loop.  If the inner loop element is GREATER then the outer loop element,
			you swap the two elements (swap the value stored at those specific array indexes).
		4)  Then you keep repeating until done (yes I know very wordy :)

	We're going to go ahead an implement the bubble sort function --
	It will take an array of integers, the count of integers you want to sort (usually
	this would be the whole array) and a flag specifying to sort from "least to greatest" or
	"greatest to least".

	So with that, let's get rolling...
*/

#include <stdlib.h> // Defines rand()
#include <stdio.h> // Defines printf()

#define NUM 10 // The NUMBER of elements in our array

// In C Land, we don't have the bool so I'm gonna typedef it
typedef unsigned char bool;
const bool true = 1; // Define "true"
const bool false = 0; // Define "false"

// Here's our function for sorting the array
// num == the count of elements to sort
// l2g == a flag stating "how to sort".  If l2g is true then we sort "least - greatest".
//		  if l2g is false we sort "greatest - least".
void BubbleSort(int*, int num, bool l2g);

int main()
{
    int numArray[NUM] = {0}; // Create our array
    int i = 0; // A counter for our for loops

    for(; i < NUM; i++)
        numArray[i] = rand()%1000; // Fill it with random numbers

    // Sort the array from least to greatest
    BubbleSort(numArray,NUM,true);

    // Print it out
    for(i = 0; i < NUM; i++)
        printf("%d ",numArray[i]);

    printf("\n\n"); // A couple blank lines

    // Sort the array from greatest to least
    BubbleSort(numArray,NUM,false);

    // Print it out
    for(i = 0; i < NUM; i++)
        printf("%d ",numArray[i]);

    printf("\n\n"); // A couple blank lines

    return 0; // And we're done
}

// This function sorts an array of ints.
// If l2g == true, the array is sorted from least to greatest.
// If l2g == false, the array is sorted from greatest to least.
void BubbleSort(int *numArray, int num, bool l2g)
{
    int i, j; // Loop counters

    // Error Check -- Quick error check to make sure we didn't get totally
    // bogus data.
    if(numArray == NULL || num <= 0)
        return; // Just don't do anything

    if(l2g) // If we're arranging the data from "least - greatest"
    {
        // This is our "outer for loop" that will walk through the array once
        for(i = 0; i < num; i++)
        {
            // This is our "inner for loop" that will walk through the ENTIRE array
            // one time for EVERY element in the array.
            for(j = 0; j < num; j++)
            {
                // Now this might seem backwards, but this is the comparison you
                // want to make for arranging data "least - greatest".
                if(numArray[j] > numArray[i])
                {
                    int temp = numArray[j]; // Make a temporary copy

                    // Swap the ints
                    numArray[j] = numArray[i];
                    numArray[i] = temp;

                }
            }
        }

    } // end of if(l2g)

    else // We must be arranging from greatest to least
    {
        for(i = 0; i < num; i++)
        {
            for(j = 0; j < num; j++)
            {
                // Again this may seem backwards, but this IS the comparison
                // you want for sorting data "greatest to least".
                // It's probably a good idea to do a small array out on paper and
                // see EXACTLY why this works
                if(numArray[j] < numArray[i])
                {
                    int temp = numArray[j]; // Make a temporary copy

                    // Swap the data
                    numArray[j] = numArray[i];
                    numArray[i] = temp;

                }
            }
        }

    } // end of if...else

} // end of void BubbleSort(int *numArray, int num, bool l2g)


// Well that's the bubble sort in all it's glory --
// If you have any questions, head to www.GameTutorials.com.

/*-------------------------*\
|  Programmed by:  TheTutor	|
|  ©2006 GameTutorials, LLC	|
\*-------------------------*/
