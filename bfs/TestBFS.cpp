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
#include  "LC_LinearArray_Graph.h"


#include "BFSHost.h"

////////////##############################################################///////////
////////////##############################################################///////////
int main(int argc, char ** args) {


    if(argc<2){
       fprintf(stderr, "Usage \"./%s <filename> <numIterations>\"\n", args[0]);
       return -1;
    }
    int num_steps = atoi(args[2]);
    int local_size=256;
    if(argc==4){
	local_size=atoi(args[3]);
	}
#if BFS_PULL_VER
    setup_env(env, "BFSPull.cl", "initialize" , "sssp_kernel" );
    BFSHost(local_size, num_steps, args[1]);
#endif
#if BFS_PUSH_VER
    setup_env(env, "BFSPush.cl", "initialize" , "sssp_kernel" );
    BFSHost(local_size, num_steps,args[1]);
#endif
    return 0;
}
