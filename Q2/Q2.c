#define _POSIX_C_SOURCE 199309L //required for clock
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include "headerfiles.h"

int M,N,K;

pthread_t Robocook[30],students[100];
pthread_t server[30];

pthread_mutex_t birwait,total,freetotal;
int studentsarrived = 0;

int Vessels[30], P[30];
int CurrentRobot[50];
int Servingfinish[50],CurrSlotsfree[30] ,studentarr[100];

long long int VesselTot , birwaiting , Freetot;

void CookFood(int i)
{
	while(1)
	{
		if(Vessels[i]==0)
		{
			int c;
			srand((i+1)*time(0)+i);
			//w is the  time taken for the robot to create vessels
			c = rand()%10 + 1;
			// We have to calculate the new number of vessels created by robot
			int tempvalue=rand()%10 + 1;
			P[i] = rand()%15 + 1;
			printf(" Robot %d is preparing %d briyani Vessels in %d seconds \n",i,tempvalue,c );
			sleep(c);
			//Storing the new value of vessels[i] after robot creation
			Vessels[i] = tempvalue;
			//Total number of vessels present must be increased after creation
			VesselTot += Vessels[i];
			printf(" Robot %d has finished cooking %d Vessels\n",i,Vessels[i]);
		}
	}
}

void ServeTheTable(int tableno)
{
	int qtypresent=0;
	while(1)
	{
		if(qtypresent==0) 
		{
			//lock the critical section before entering
			//Here lock total 
			pthread_mutex_lock(&total); 
			//Run only if total vessels is positive.
			//Else call the Robots to cook CookFood()
			if( VesselTot > 0 )
			{
				for(int l=0; l<M;l++)
				{
					//If there number of slots in a vessel is positive
					if( Vessels[l] > 0 )
					{
						//Decrease Vessel[i] to allocate food to a student
						printf("Serving table %d entering serving phase\n",tableno);
						Vessels[l]--;
						VesselTot--; //mutex
						qtypresent=P[l];
						printf(" Vessel was loaded onto the serving table %d with %d quantity \n",tableno,qtypresent);
						break;
					}
				}
			}
			pthread_mutex_unlock(&total);
		}
		//Now lock the biriyani waiting mutex
		pthread_mutex_lock(&birwait);
		if(CurrSlotsfree[tableno] == 0 && qtypresent>0 && birwaiting>0)
		{
			// create_slots();
			srand(tableno*time(0));
			int extraslot=0;
			if(qtypresent >= 10)
			{
				extraslot = rand()%10 +2;
			}
			else
			{
				extraslot = rand()%qtypresent + 2;
			}

			if(qtypresent > 0)
			{

				if( extraslot >= birwaiting)
				{
					int numberslotleft = extraslot - birwaiting;
					CurrSlotsfree[tableno] = numberslotleft;
					birwaiting = 0; //mutex
				}
				else
				{
					CurrSlotsfree[tableno]=0;
					birwaiting = birwaiting - extraslot;
				}
				Freetot += CurrSlotsfree[tableno];
			}
		}
		//Unlock the mutex before exiting critical section so other threads 
		//can access birwait
		pthread_mutex_unlock(&birwait);
	}
}

void StudentAlloc(int sleeptime)
{
	while(1)
	{
		//Temporarily make the thread wait for sleeptime sec passed as parameter
		//The arrival times are appropriately waited for
		sleep(sleeptime);
		studentsarrived++;
		// if(studentsarrived ==K)
		// {
		// 	printf("Simulation over\n");
		// 	exit(1);
		// }
		//Give a time delay before student gets allocated
		if(Freetot != 0) 
		{
			printf("Student is waiting to be allocated a slot on the table\n");
			pthread_mutex_lock(&freetotal);
			//Entering critical section to allot slots to student.

			int stuwasalloc=0;
			for(int iter = 0; iter < N; iter ++)
			{
				if(CurrSlotsfree[iter]>0)
				{
					printf(" Slot allocated for a student %d\n",CurrSlotsfree[iter]);
					//After allocating slots decrement the number of free slots.
					Freetot--; 
					CurrSlotsfree[iter]--;
					stuwasalloc=1;
					break;
				}
			}
			if(stuwasalloc==1)
				break;
			pthread_mutex_unlock(&freetotal);
		}

		else if(Freetot ==0)
		{
			//Lock the mutex for total number of free slots present
			pthread_mutex_lock(&freetotal);
			birwaiting=1;//Give food to  the one guy waiting
			pthread_mutex_unlock(&freetotal);
		}
		
	}
}

int main()
{
	//input m n and k
	scanf("%d %d %d",&M,&N,&K);
	srand(time(0));
	//calculating arrival times of students using rand function
	for(int bcc = 0; bcc < K; bcc++)
	{
		int time = rand()%10 + 5;
		studentarr[bcc];
	}
	//Initialise all mutexes
	pthread_mutex_init(&freetotal, NULL);
	pthread_mutex_init(&total, NULL);
	pthread_mutex_init(&birwait, NULL);
	//Start creating threads for the cooking robots,
	//  serving tables as well
	//  as the students incoming

	for(int l=0; l < M;l++)
	{
		CurrentRobot[l]= 0;
	}
	for(int q=0; q < N;q++)
	{
		Servingfinish[q]= 0;
	}

	for(int p=0;p<M;p++)
	{
		pthread_create(&(Robocook[p]),NULL,&CookFood,p);
	}

	for(int j=0;j<N;j++)
	{
		pthread_create(&(server[j]),NULL,&ServeTheTable,j);
	}
	for(int stu = 0; stu < K; stu++)
	{
		pthread_create(&(students[stu]),NULL,&StudentAlloc,studentarr[stu]);

	}
	for(int sd = 0; sd < K; sd++)
	{
		pthread_join(students[sd],NULL);

	}

	for(int d = 0; d < K; d++)
	{
		pthread_join(server[d],NULL);
	}

	for(int srv = 0 ; srv < N;srv++)
	{
		pthread_create(&(server[srv]),NULL,&ServeTheTable,srv);
	}

	for(int rb= 0; rb <M ;rb++)
	{
		pthread_join(Robocook[rb],NULL);
	}

	pthread_mutex_destroy(&birwait);
	pthread_mutex_destroy(&total);
	pthread_mutex_destroy(&freetotal);
	return 0;
}
