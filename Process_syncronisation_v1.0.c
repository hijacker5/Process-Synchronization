#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<time.h>
#include<semaphore.h>
#include<unistd.h>
struct dat
{
	int *s,m,k;
};
int top=-1;
sem_t mutex,full,empty;
void push(int s[],int item)
{
		top++;
		s[top]=item;
}
int pop(int s[])
{
	return s[top--];
}
void *producer(void *a)
{
	int i,j;
	struct dat *arg=(struct dat*)a;
	int m=arg->m,*s=arg->s;//k=arg->k;
	srand(time(0)); 
	for(i=0;i<m;i++)
	{
		int item=rand()%100;
		sem_wait(&empty);
		sem_wait(&mutex);
		push(s,item);
		printf("%d ",item);
		sem_post(&mutex);
		sem_post(&full);
	}
	usleep(1000);
}
void *consumer(void *a)
{
	struct dat *arg=(struct dat*)a;
	int i,z;
	int *arr=(int*)malloc(arg->m*sizeof(int));
	for(i=0;i<arg->m;i++)
	{
		sem_wait(&full);
		sem_wait(&mutex);
		arr[i]=pop(arg->s);
		sem_post(&mutex);
		sem_post(&empty);
		sleep(1);
	}
	return (void *)arr;
}
void swap(int *xp, int *yp) 
{ 
    int temp = *xp; 
    *xp = *yp; 
    *yp = temp; 
} 
void selectionsort(int arr[], int n) 
{ 
    int i, j, min_idx; 
    for (i = 0; i < n-1; i++) 
    { 
        min_idx = i; 
        for (j = i+1; j < n; j++) 
          if (arr[j] < arr[min_idx]) 
            min_idx = j; 
    swap(&arr[min_idx], &arr[i]); 
    } 
} 
int main()
{
	int n,m,k;
	double execution_time,throughput;
	printf("Enter Value of n:\t");
	scanf("%d",&n);
	printf("Enter Value of m:\t");
	scanf("%d",&m);
	printf("Enter Value of k:\t");
	scanf("%d",&k);
	struct timespec start, finish;
	double elapsed;
	clock_gettime(CLOCK_MONOTONIC, &start);
	//FILE *f=fopen("output.txt","a+");
	sem_init(&mutex,0,1);
	sem_init(&full,0,0);
	sem_init(&empty,0,k);
	pthread_t p[n],c[n];
		int s[k],i,j;
		top=-1;
		struct dat ag;
		ag.s=s;
		ag.m=m;
		ag.k=k;
		printf("Printing the items produced by the producer processes:\n");
		for(i=0;i<n;i++)
		{	pthread_create(&p[i],NULL,producer,(void*)&ag);
			pthread_create(&c[i],NULL,consumer,(void*)&ag);
		}
		int arr[m*n],count=0;
		void *arr2;
		for(i=0;i<n;i++)
		{
			pthread_join(p[i],NULL);
			pthread_join(c[i],&arr2);
			int *arr3 = (int *)arr2;
			for(j=0;j<m;j++)
				arr[count++]=arr3[j];
		}
		selectionsort(arr,count);
		printf("\nPrinting the items received by the Parent processes:\n");
		for(i=0;i<n*m;i++)
			printf("%d ",arr[i]);
		clock_gettime(CLOCK_MONOTONIC, &finish);
		elapsed = (finish.tv_sec - start.tv_sec);
		elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
		//printf("new time %f \n",elapsed);
	    execution_time = elapsed;
	    throughput = (double)((2*n)+1)/(double)execution_time;
	    printf("\nExecution Time:%lfseconds\nThroughput:%lf\n",execution_time,throughput);
	    //fprintf(f,"%d  %lf\n",k,throughput);
	    printf("\nFinal value of stack top pointer:\t%d\n",top);
	//fclose(f);
}