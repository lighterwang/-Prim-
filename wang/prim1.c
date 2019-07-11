#include <stdio.h>
#include <math.h>
#include "prim1.h"
#define MAXV 100
#define INF 32767

double edges[1000][1000];
int n=6;
int root[100][1000][3];

void prim(int v)
{
    int sum =0 ;
	int amount=0;int lowcost[MAXV];
	int MIN;
	int closest[MAXV],i,j,k;
	for(i=1;i<n+1;i++)
	{
		lowcost[i]=edges[v][i];
		closest[i]=v;
	}
	for(i=2;i<n+1;i++)
	{
		MIN=INF;
		for(j=1;j<n+1;j++)
			if(lowcost[j]!=0&&lowcost[j]<MIN)
			{
				MIN=lowcost[j];
				k=j;
			}
        root[v][++amount][0]=closest[k];
        root[v][amount][1]=k;
        root[v][amount][2]=MIN;
	    lowcost[k]=0;
	    sum+=MIN;
	    for(j=1;j<n+1;j++)
            if(lowcost[j]!=0&&edges[k][j]<lowcost[j])
            {
                lowcost[j]=edges[k][j];
                closest[j]=k;
            }
	}
	return sum;
}




void initm()
{
    n=6;
    edges[1][2]=edges[2][1]=5;
    edges[1][3]=edges[3][1]=3;
    edges[1][4]=edges[4][1]=7;
    edges[1][5]=edges[5][1]=4;
    edges[1][6]=edges[6][1]=2;
    edges[2][3]=edges[3][2]=4;
    edges[2][4]=edges[4][2]=6;
    edges[2][5]=edges[5][2]=2;
    edges[2][6]=edges[6][2]=6;
    edges[3][4]=edges[4][3]=6;
    edges[3][5]=edges[5][3]=1;
    edges[3][6]=edges[6][3]=1;
    edges[4][5]=edges[5][4]=10;
    edges[4][6]=edges[6][4]=8;
    edges[5][6]=edges[6][5]=3;

}
