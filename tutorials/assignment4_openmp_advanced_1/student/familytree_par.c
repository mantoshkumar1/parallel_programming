#include "familytree.h"
#include <omp.h>


void traverse(tree *node, int numThreads){
	
	if(node != NULL){
		omp_set_num_threads(numThreads);
		//omp_set_nested(1);
		#pragma omp parallel shared(node)
		{
	 	 
		    #pragma omp single nowait 
		    {

		       #pragma omp task shared(node, genius)
		       {
   		          node->IQ = compute_IQ(node->data);
		          genius[node->id] = node->IQ;
		       }
		       #pragma omp task shared(node)
		       {
		          traverse(node->right, numThreads);
		       }
		
		       #pragma omp task shared(node)
		       {
		          traverse(node->left, numThreads);
		       }


		    }



		}
	}

}

