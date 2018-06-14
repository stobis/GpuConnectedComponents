/****************************************************************************************
 *       CONNECTED COMPONENTS ON THE GPU                                                        
 *       ==============================
 *
 *
 *
 *       Copyright (c) 2010 International Institute of Information Technology,
 *       Hyderabad.
 *       All rights reserved.
 *
 *       Permission to use, copy, modify and distribute this software and its
 *       documentation for research purpose is hereby granted without fee,
 *       provided that the above copyright notice and this permission notice appear
 *       in all copies of this software and that you do not sell the software.
 *
 *       THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
 *       EXPRESS, IMPLIED OR OTHERWISE.
 *
 *       Please report any issues to Jyothish Soman (first.last at gmail)
 *
 *       Please cite following paper, if you use this software for research purpose
 *
 *       "Fast GPU Algorithms for Graph Connectivity, Jyothish Soman, K. Kothapalli, 
 *       and P. J. Narayanan, in Proc. of Large Scale Parallel Processing, 
 *       IPDPS Workshops, 2010.
 *
 *
 *
 *
 *       Created by Jyothish Soman
 *											
 ****************************************************************************************/

// includes, system
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include<time.h>


#include<helper_cuda.h>
#include<helper_functions.h>
#include<cuda.h>
#include<cuda_runtime.h>
#include"conn.cuh"

using namespace cc;

/*
 *
 *   Function to load graph to the CPU memory.
 *   load_graph
 *   input is currently hard coded as edge.txt and parameters.txt
 *   edge.txt contains the edges, in the format n1 n2
 *   parameters.txt contains the number of edges and number of nodes respectively
 *   Do note that this is a connected components for undirected graphs, each edge is undirected. 
 *   The input here is assumed to be undirected edges, no duplicate edges assumed, but not necessary.   
 *
 *   #TODO make this code more cleaner and natural, add a config file or a shell wrapper to make this more user friendly
 *
 *
 *
 */

void load_graph(edge** ed_list,int*num_n,int*num_e){
    FILE*fp,*fp2;
    edge*edl;
    int i,j,x,y,a;
    long long int v;
    fp=fopen("edge.txt","r");
    fp2=fopen("parameters.txt","r");
    fscanf(fp2,"%d%d",&i,&j);
    *ed_list=(edge*)calloc(i,sizeof(edge));
    edl=*ed_list;
    if(edl==NULL){
        printf("Insufficient memory, data lost");
        exit(0);
    }
    for(a=0;a<i;a++){
        fscanf(fp,"%d%d",&x,&y);
        x=x-1;
        y=y-1;
        v=0;
        v=(long long int)x;
        v=v<<32;
        v+=(long long int) y;
        edl[a].x=v;
    }
    *num_n=j;
    *num_e=i;
    fclose(fp);
    fclose(fp2);
    return;
}



////////////////////////////////////////////////////////////////////////////////
// Program main
////////////////////////////////////////////////////////////////////////////////
int main( int argc, char** argv) 
{
    findCudaDevice(argc,(const char**) argv);

    edge* ed_list,*d_ed_list;
    int num_n,num_e;//,nnx,nny,nex,ney;	
//    unsigned int timer1 = 0;
//    checkCudaErrors( cutCreateTimer( &timer1));
//    checkCudaErrors( cutStartTimer( timer1));

    load_graph(&ed_list,&num_n,&num_e);

    checkCudaErrors(cudaMalloc((void**)&d_ed_list,num_e*sizeof(edge)));
    checkCudaErrors(cudaMemcpy(d_ed_list,ed_list,num_e*sizeof(edge),cudaMemcpyHostToDevice));

    int * is_tree, *d_is_tree;
    checkCudaErrors(cudaMalloc((void**)&d_is_tree,num_e*sizeof(int)));
    checkCudaErrors(cudaMemset(d_is_tree, 0, num_e * sizeof(int)));
    
    compute(num_n, num_e, d_ed_list, d_is_tree);

    is_tree=(int*)calloc(num_e,sizeof(int));
    checkCudaErrors(cudaMemcpy(is_tree,d_is_tree,num_e*sizeof(int),cudaMemcpyDeviceToHost));

    for (int i = 0; i < num_e; ++i) {
        printf("%d\n", is_tree[i]);
    } 

    return 0;
}
