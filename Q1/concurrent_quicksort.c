#include <stdio.h>
#include <stdlib.h>
#include <wait.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <error.h>
#include <errno.h>
#include <math.h>

void swapper(int* n1, int* n2);

void insertionsort(int arr[], int l,int h) 
{ 
    for (int i = l+1; i <= h; i++) 
    {  
        int key = arr[i];  
        int j = i-1;  
        while (j >= 0 && arr[j] > key) 
        {  
                arr[j+1] = arr[j];  
                j--; 
        }  
        arr[j+1] = key;  
    }  
    return;
    // printf("Printing insertion sorted array\n");
    // for(int g=0; g<n; g++)
    //     printf("%d ", *(arr+g));
} 

int partition(int array[], int lo, int hi) 
{ 
    int piv = array[lo];     
    int i=lo ;  
    for(int j=lo+1 ; j <= hi ; j++)
    { 
        if ( piv > array[j] ) 
        { 
            i++;    
            swapper(&array[i], &array[j]); 
        } 
    } 
    //Swap the pivot and the last index of smallest element ordered
    swapper(&array[i], &array[lo]); 
    return (i); 
} 
void normal_quickSort(int array[], int lo, int hi) 
{ 
    if(lo > hi)
        return;
    if (hi > lo) 
    { 
        
        if(hi-lo+1 <= 5)
        {
            insertionsort(array,lo,hi);
        }
        else
        {
        int pindex = partition(array, lo, hi); 
        
        normal_quickSort(array, lo, pindex - 1); 
        normal_quickSort(array, pindex + 1, hi); 
        }
        
    } 
}
void concurrent_quicksort(int array[] , int lo, int hi)
{
    if( lo > hi)
    {
        /*The range is inappropriate*/
        exit(1);
    }
    /*If size <= 5 */
    if( hi-lo+1 <= 5)
    {
        // If size is less than 5 insertion sort is efficient than quicksort
        insertionsort(array, lo, hi);
        return;
    }

    int pindex = partition(array,lo,hi);

    int leftside = fork();
    if(leftside < 0)
    {
        printf("Error!! Exiting \n");
        perror("leftside");
        exit(1);
    }
    
    if( leftside == 0 )
    {
        //sort left half of the  array
        concurrent_quicksort(array, lo , pindex-1);
        exit(1);
    }
    else{
        int rightside;
        rightside = fork();
        if(rightside < 0 )
        {
            printf("Error!! Exiting\n");
            perror("rightside");
        }
        if(rightside==0)
        {
            //sort the right half of the array
            concurrent_quicksort(array,pindex+1,hi);
            exit(1);
        }
        else
        {
            //wait for the right and the left half to get sorted
            int status;
            waitpid(leftside, &status, 0);
            waitpid(rightside, &status, 0);
        }
    }
    return;
}
  
int main() 
{ 
    printf("Enter the number of elements\n");
    int N;
    scanf("%d",&N);
    printf("Enter the array\n");

    int shmid; 
    int *array; 
    int normal[10000];

    shmid = shmget(IPC_PRIVATE, 128, IPC_CREAT | 0666);
    if(shmid < 0) 
    { 
        perror("shmget"); 
        exit(1); 
    } 
  
    array = shmat(shmid, NULL, 0);
    if( array == NULL ) 
    { 
        perror("shmat"); 
        exit(1); 
    } 

    for(int o=0 ; o < N; o++)
    {
        scanf("%d",&array[o]);
        normal[o]=array[o];
    }
    // printf("Printing sorted array\n");
    printf("\nRunning normal mergesort for n = %d\n", N);
    __clock_t t;
    t = clock();
    normal_quickSort(normal,0,N-1);
    t = clock() - t;
    double timetaken = ((double)t)/CLOCKS_PER_SEC;

    printf("Time taken for normal mergesort: %f\n\n",timetaken);



    printf("Running concurrent mergesort for n = %d\n", N);
    t = clock();
    concurrent_quicksort(array, 0, N-1); 
    t = clock() - t;
    timetaken = ((double)t)/CLOCKS_PER_SEC;

    printf("Time taken for concurrent mergesort: %f\n\n",timetaken);
    
    printf("Sorted array\n");  
    for(int i=0 ; i < N; i++)
        printf("%d ",array[i]);
    printf("\n");
    return 0; 
}

void swapper(int *n1,int* n2)
{
    int temporary = *n1;
    *n1 = *n2;
    *n2 = temporary;
}