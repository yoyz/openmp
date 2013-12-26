#include <omp.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/time.h>

#define N 0x01FFFFFF //Matrix size

double  tf_sec=0;    //time float second 
double  tf_usec=0;   //time fload micro second
double  tf=0;        //time second.usecond
double  tf_last=0;   //time measured from last iter
double  tf_delta=0;  //delta between two measures

int64_t * vecsrc;       //vector which will be read
int64_t * vecdst;       //vector which will be filled 
int64_t filler=0xDEADBEBEDEADBEBE; //filler value

double mygtod()
{
  struct timeval  tv2;
  struct timezone tz2;
  gettimeofday(&tv2,&tz2);
  tf_sec=tv2.tv_sec;
  tf_usec=tv2.tv_usec;  
  tf=(tf_usec/1000000)+tf_sec;  
  return tf;
}



int main()
{
  int i;
  int j;
  double tp_cpu;
  double tp_global;
  int    forend=0;
  int    nthreads;
  int    tid;
  int    thread=0;
  double res;

#pragma omp parallel private(nthreads, tid)
  {
    tid = omp_get_thread_num();

    if (tid==0)
      {   
	nthreads = omp_get_num_threads();
	printf("mystream \n");
	printf("Number of threads = %d\n", nthreads);
	thread=nthreads;
      }
  }

  double throughput[thread];

#pragma omp master
    {
      
      int mbpercpu=(sizeof(int64_t)*N)/1024/1024;
      printf("MB  per cpu        :  %d\n",mbpercpu);
    }

#pragma omp parallel private(j,vecsrc,vecdst,tf_delta,tf_last,tp_cpu)
  {
    vecsrc=malloc(sizeof(int64_t)*N);
    vecdst=malloc(sizeof(int64_t)*N);
    if (!vecsrc) { printf("malloc error\n"); exit(1); }
    if (!vecdst) { printf("malloc error\n"); exit(1); }

    printf("touching data on cpu %d\n",omp_get_thread_num()); 
    for (i=0;i<N;i++)
      {
	vecsrc[i]=filler;
      }


    tf_last=mygtod();
    for (j=0;j<100;j++)
      {
	tf=mygtod();
	tf_delta=tf -tf_last; 
	tf_last=tf;
	//tp_cpu=(((((double)N*64)/tf_delta)/8)/1024)/1024;
	// tp_cpu 64bit*2(in + out) / temps / 8 bit / 1Ko / 1Ko
	tp_cpu=(((((double)N*64*2)/tf_delta)/8)/1024)/1024;

	tp_global=0;
	throughput[omp_get_thread_num()]=tp_cpu;

	//Display a nice chart
	#pragma omp master
	{
	  int i=0;
	  res=0;
	  for (i=0;i<thread;i++) 
	    res+=throughput[i];

	  if (j>1) // bypass the first iter
	    {
	      for (i=0;i<thread;i++)	    
		printf("CPU:%d\t%f\n",i,throughput[i]);
	      printf("SYS:\t%f\n\n",res);
	    }
	}
	
	//Stream it
	for (i=0;i<N;i++)
	  {
	    vecdst[i]=vecsrc[i];
	  }
	forend=1;
      }
    
    free(vecsrc);
    free(vecdst);
  } 
  
}

