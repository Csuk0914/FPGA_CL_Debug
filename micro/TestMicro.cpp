
/*
 * test_pregel.cpp
 *
 *  Created on: Nov 6, 2014
 *      Author: rashid
 */
///#define TEST_PART 1
//#define MICRO_TEST 1
#define _USE_CL 1

//#define _GOPT_DEBUG 1
#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <math.h>
#include <set>
#include <string>
#include <vector>
#include <libgen.h>
#include <thread>
#include <atomic>

#define DEFAULT_DEVICE 0
#define DEFAULT_PLATFORM 0

#include "ocl_header.h"
#include "timer.h"

CLEnv env;
#include "ArrayImpl.h"
#ifdef _ALTERA_SVM_
   #include "SVMArrayImpl.h"
#endif
#include  "LC_LinearArray_Graph.h"


#include "MicroWrapper.h"
const int NUM_TESTS = 9;
enum MICRO_VARIANTS{ATOMIC_SINGLE, ATOMIC_DIFF, ATOMIC_DIFF_UNCOAL,ATOMIC_DIFF_COAL, ADD_DIFF, ADD_DIFF_UNCOAL,ADD_DIFF_COAL, MEM_READ, SVM_MEM_READ};
const char  * test_names []= {"atomic_single", "atomic_diff" , "atomic_diff_uncoal" ,"atomic_diff_coal" ,  "add_diff" , "add_diff_uncoal" , "add_diff_coal", "mem_read" , "svm_mem_read"};
////////////##############################################################///////////
////////////##############################################################///////////
int main(int argc, char ** args) {


    if(argc<2){
       fprintf(stderr, "Usage \"./%s test_type  max_range local_size\"\n", args[0]);
       fprintf(stderr, "Where test_type is one of the following\n");
       for(int i=0; i<NUM_TESTS; ++i){
          fprintf(stderr, "\t\t %s\n",test_names[i]);
       }
       return -1;
    }

    char * test_type = args[1];
    int test_index=0;
    size_t num_threads = atoi(args[2]);
    size_t local_size=256;
    if(argc==4){
       local_size = atoi(args[3]);
    }
    for(int i=0; i<NUM_TESTS; ++i){
       if(strcmp(test_type, test_names[i])==0){
          test_index=i;
          break;
       }
    }
    if(test_index == MICRO_VARIANTS::ATOMIC_SINGLE){
       test_atomic_single_location(num_threads, local_size);
    }else if(test_index == MICRO_VARIANTS::ATOMIC_DIFF){
       test_atomic_diff_location(true, num_threads, local_size);
    }else if(test_index == MICRO_VARIANTS::ATOMIC_DIFF_UNCOAL){
       test_atomic_diff_location_uncoal(true,true, num_threads, local_size);
    }else if(test_index == MICRO_VARIANTS::ATOMIC_DIFF_COAL){
       test_atomic_diff_location_uncoal(true,false,num_threads, local_size);
    }else if(test_index == MICRO_VARIANTS::ADD_DIFF){
       test_atomic_diff_location(false, num_threads, local_size);
    }else if(test_index == MICRO_VARIANTS::ADD_DIFF_UNCOAL){
       test_atomic_diff_location_uncoal(false, true,num_threads, local_size);
    }else if(test_index == MICRO_VARIANTS::ADD_DIFF_COAL){
       test_atomic_diff_location_uncoal(false, false,num_threads, local_size);
    }else if(test_index == MICRO_VARIANTS::MEM_READ){
       test_memory_reads(num_threads, local_size);
    }else if(test_index == MICRO_VARIANTS::SVM_MEM_READ){
       test_svm_memory_reads(num_threads, local_size);
    }
    return 0;
}
