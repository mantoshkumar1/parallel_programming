#include "familytree.h"
#include <omp.h>

void traverse(tree *node, int numThreads){
	
	if(node != NULL){

		#pragma omp parallel num_threads(numThreads)
		#pragma omp sections nowait
		{
		    #pragma omp section
		    {
			node->IQ = compute_IQ(node->data);
			genius[node->id] = node->IQ;
		    }

		    #pragma omp section
		    {
			traverse(node->left, numThreads);
		    }
		    #pragma omp section
		    {
			traverse(node->right, numThreads);
		    }
		

		}		
	}
}

