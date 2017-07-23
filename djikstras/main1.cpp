#include "mpi.h"
#include <stdio.h>
#include<stdlib.h>

#define MAX 10000;

int min(int *res,int n)
{
	int i,index=-1,val=MAX;
	//fprintf(stdout,"\n\n");fflush(stdout);
	for(i=0;i<n;i++)
	{
		//fprintf(stdout,"%d\t",res[i]);fflush(stdout);
		if(res[i]<val)
		{index=i;val=res[i];}
	}
	//fprintf(stdout,"\tok %d\t",index);fflush(stdout);
	return index;

}

void iniatial(int *res,int n)
{
	int i;
	for(i=0;i<n;i++)
		res[i]=MAX;
}

void display(int *dis,int n)
{
	int i,j;
	fprintf(stdout,"\nOutput:\n");fflush(stdout);
	for(i=0;i<n;i++)
	{
		for(j=0;j<n;j++)
		{fprintf(stdout,"%d\t",dis[i*n+j]);fflush(stdout);}
		fprintf(stdout,"\n");fflush(stdout);
	}

}

int main(int argc,char *argv[])
{
	int size,rank,*dis,*vis,*res,*result,*parent,res1,i,j,n,k,temp,p,*p1;

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);

	dis=(int*)calloc(size*size,sizeof(int));
	vis=(int*)calloc(size,sizeof(int));
	res=(int*)calloc(size,sizeof(int));
	p1=(int*)calloc(size,sizeof(int));
	parent=(int*)calloc(size,sizeof(int));
	result=(int*)calloc(size,sizeof(int));
	if(rank==0)
	{
		fprintf(stdout,"The number of nodes is %d\n",size);fflush(stdout);
		fprintf(stdout,"\n\t\t PUT -1 for infinity \n");fflush(stdout);
		fprintf(stdout,"Enter %d X %d Distance matrix\n",size,size);fflush(stdout);
		
		for(i=0;i<size;i++)
			{
				for(j=0;j<size;j++)
					{
						scanf("%d",&dis[i*size+j]);
						if(i==j)
							dis[i*size+j]=0;
						if(dis[i*size+j]==-1)
							dis[i*size+j]=MAX;
					}
			}
		fprintf(stdout,"Enter the starting index :\t");fflush(stdout);
		scanf("%d",&n);
		vis[n]=1;
		result[n]=0;
		parent[n]=-1;
		//display(dis,size);
	}
	MPI_Bcast(dis,size*size,MPI_INT,0,MPI_COMM_WORLD);
	//MPI_Bcast(vis,size,MPI_INT,0,MPI_COMM_WORLD);
	//MPI_Bcast(result,size,MPI_INT,0,MPI_COMM_WORLD);
	
	for(i=0;i<size;i++)
	{
		MPI_Barrier(MPI_COMM_WORLD);
		if(i>0)
		{
			MPI_Gather(&res1,1,MPI_INT,res,1,MPI_INT,0,MPI_COMM_WORLD);
			MPI_Gather(&p,1,MPI_INT,p1,1,MPI_INT,0,MPI_COMM_WORLD);
			if(rank==0)
			{
				k=min(res,size);
				if(k==-1)
				{fprintf(stdout,"bad\n");fflush(stdout);}
				if(vis[k]!=1)
					{
					vis[k]=1;
					result[k]=res[k];
					parent[k]=p1[k];
					}
			}
			MPI_Bcast(vis,size,MPI_INT,0,MPI_COMM_WORLD);
			MPI_Bcast(result,size,MPI_INT,0,MPI_COMM_WORLD);
			
			MPI_Barrier(MPI_COMM_WORLD);
		
		}
		res1=MAX;p=-1;
		if(vis[rank]==1)
		{
			continue;
		}
	
		else
		{
			for(j=0;j<size;j++)
			{
			if(vis[j]==1)
				{
					temp=dis[j*size+rank]+result[j];
					if(temp<res1)
					{res1=temp;p=j;}
				}	
				
			}
		}

	}
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Gather(&res1,1,MPI_INT,res,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Gather(&p,1,MPI_INT,p1,1,MPI_INT,0,MPI_COMM_WORLD);
	if(rank==0)
	{
		//display output
		k=min(res,size);
		vis[k]=1;
		result[k]=res[k];parent[k]=p1[k];
		
		fprintf(stdout,"\n\nNode\tdistance\tparent\tvisited\n");fflush(stdout);
		for(j=0;j<size;j++)
		{fprintf(stdout,"%d\t%d\t\t%d\t%d\n",j,result[j],parent[j],vis[j]);fflush(stdout);}
	}
	MPI_Finalize();
	return 0;
}