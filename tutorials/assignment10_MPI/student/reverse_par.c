#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include "helper.h"

void reverse(char *str, int strlen)
{
	// parallelize this function and make sure to call reverse_str()
	// on each processor to reverse the substring.
	
	int np, rank;
	int root = 0;
	int *sendcounts, *displs;
	int i;
        char *substring = NULL;
//	MPI_Status status;

    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    //creating two required integer arrays for MPI_Scatterv
    displs = (int *)malloc(np * sizeof(int)); 
    sendcounts = (int *)malloc(np * sizeof(int));

    //calculating length of substrings for each process and specially case for last process
    int substring_len = strlen / np;
    int last_substring_len = substring_len + (strlen % np);

    for (i = 0; i <= np - 2; i++) { 
        displs[i] = i * substring_len; 
        sendcounts[i] = substring_len;
    } 

    //last process
    displs[np-1] = (np - 1) * substring_len; 
    sendcounts[np-1] = last_substring_len;

    if(rank == (np - 1)) {
       substring_len = last_substring_len;
    }
    substring = (char *)malloc(substring_len * sizeof(char));

    //root process(0) scattering "str" to np number of processes
    MPI_Scatterv(str, sendcounts, displs, MPI_CHAR, substring, substring_len, MPI_CHAR, root, MPI_COMM_WORLD);

    //each process reverses their part of substring
    reverse_str(substring, substring_len);

    
    //each process sending their result to root process(0)
    if(rank != 0) {
        MPI_Send(substring, substring_len, MPI_CHAR, root, 1234, MPI_COMM_WORLD);
    }

    //root collecting all result and placing them at their expected position in char array
    if(rank == 0) {
	int index = 0;
	int j = 0;


        //saving for process 0, no send-recv was used (doing now as substring buffer will get overwriiten in recv)
	for (j = 0; j < substring_len ; j++) {
	    str[strlen - substring_len + j] = substring[j];
	}
	//printf("rank 0: %s \n", substring);
	//printf("Str: %s \n", str);

        //saving for last process because of variable buffer length
        char *last_substring = (char *)malloc(last_substring_len * sizeof(char));
        MPI_Recv(last_substring, last_substring_len, MPI_CHAR, np - 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	//printf("last rank : %s \n", last_substring);
	//printf("Str: %s \n", str);

        //MPI_Recv(last_substring, last_substring_len, MPI_CHAR, np - 1, 0, MPI_COMM_WORLD, &status);
	for (j = 0 ; j < last_substring_len; j++) {
            str[index] = last_substring[j];
            index++;
        }
	free(last_substring);
	

        //saving for rest processes with same buffer length
        for (i = np - 2; i >= 1; i--) { 
            MPI_Recv(substring, substring_len, MPI_CHAR, i, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	    printf("rank %d : %s \n", i, substring);
	    for (j = 0 ; j < substring_len; j++) {
                str[index] = substring[j];
                index++;
            }
	    //printf("Str: %s \n", str);

	}
	
    }
   
    free(substring);
    free(sendcounts);
    free(displs);
    
}
