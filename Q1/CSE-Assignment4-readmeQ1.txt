CSE ASSIGNMENT 4 : REPORT

		
QUESTION 1

The question involves writing a concurrent version of quicksort and comparing the performance of the concurrent version to the normal version of quicksort
The functions present are partition(), normal quicksort() , concurrent quicksort(), insertionSort()
Now each of the functions are explained as snippets in the report

1. int partition(int array[], int lo, int hi) 
	{ 
    	int piv = array[lo];     
   	 int i=lo ;  
  	  for(int j=lo+1 ; j <= hi ; j++)
  	  { 
  	      if ( piv > array[j] ) 
  	      { 
 	           i++;    
 	           swapper(&array[i], &array[j]); 
	        } 
	    } 
	    //Swap the pivot and the last index of smallest element ordered
	    swapper(&array[i], &array[lo]); 
	    return (i); 
	}
This is the partition function for the quicksort accessed by both normal as well as the concurrent versions 


2. Implementation of quicksort:
Quicksort is a divide and conquer algorithm. First the array is taken and a pivot is chosen appropraitely. After choosing the pivot , we have to partition the array into two halves such that the first part contains all elements less than the pivot and the second half contains all elements greater than the pivot.
We then call quicksort function recursively for sorting both the halves.Thus the entire array gets sorted recursively.


3. CONCURRENT QUICKSORT: 
void concurrent_quicksort(int array[] , int lo, int hi)
	{
	    if( lo > hi)
	    {
	        /*The range is inappropriate*/
	        exit(1);
	    }
	    /*If size <= 5 */
	    if( hi-lo+1 <= 5)
	    {
	        // If size is less than 5 insertion sort is efficient than quicksort
	        insertionsort(array, lo, hi);
	        return;
	    }
	
	    int pindex = partition(array,lo,hi);
	
	    int leftside = fork();
	    if(leftside < 0)
	    {
	        printf("Error!! Exiting \n");
	        perror("leftside");
	        exit(1);
	    }
    
	    if( leftside == 0 )
	    {
	        //sort left half of the  array
	        concurrent_quicksort(array, lo , pindex-1);
	        exit(1);	
	   }
    	else{
    	    int rightside;
    	    rightside = fork();
   	     if(rightside < 0 )
  	      {
  	          printf("Error!! Exiting\n");
  	          perror("rightside");
  	      }
  	      if(rightside==0)
  	      {
  	          //sort the right half of the array
  	          concurrent_quicksort(array,pindex+1,hi);
 	           exit(1);
	        }	
        	else
        	{
        	    //wait for the right and the left half to get sorted
        	    int status;
        	    waitpid(leftside, &status, 0);
        	    waitpid(rightside, &status, 0);
        	}
    	}
    	return;
}
	
At first we check the low and high indices.If low > high we cant sort it.So we exit the process. Suppose it is otherwise , then two cases arise. If the number of elements to be sorted is atmost 5, insertion sort is more efficient than the quicksort.Thus , the insertionsort function is called.. If it has greater than 5 elements we partition the array into  left and right .We use fork() system call to fork the parent process.The first child will then sort the left half of the parent processes’ array. The parent process is again forked in order to sort the right side of the array(by child process). The parent process then waits for both the child processes to finish and exit.This is done through waitpid() syscall.

4. PERFORMANCE CALCULATION:

   __clock_t t;
    t = clock();
    normal_quickSort(normal,0,N-1);
    t = clock() - t;
    double timetaken = ((double)t)/CLOCKS_PER_SEC;

    printf("Time taken for normal mergesort: %f\n\n",timetaken);

	
    The performance of both normal and concurrent version of quicksort can be calculated using the clock by including time.h header file.We store the current time before and after we call the functions. We then find the difference in time to print the performance times of both the functions.We accoringly print the times after execution of both the functions.

It is found that the performance of Normal quicksort is higher than that of Concurrent quicksort.It is because multiple processes are created every time we want to sort a part of the array.



		#################################################################################################################################################################


