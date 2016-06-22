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

CLEnv env;
#include "ArrayImpl.h"
#include  "LC_LinearArray_Graph.h"


#if PR_PULL_VER

#include "PageRankPull.h"
#endif
#if PR_PUSH_VER

#include "PageRankPush.h"
#endif

////////////##############################################################///////////
////////////##############################################################///////////
int main(int argc, char ** args) {


    if(argc<1){
       fprintf(stderr, "No filename provided\n");
       return -1;
    }
#if PR_PULL_VER
    setup_env(env, "PageRankPull.cl", "initialize" , "pageRank" );
    PR_Pull(10, args[1]);
#endif
#if PR_PUSH_VER
    setup_env(env, "PageRank.cl", "initialize" , "pageRank" );
    PR_Push(10,args[1]);
#endif
    return 0;
}
////////////##############################################################///////////
////////////##############################################################///////////


////////////##############################################################///////////
////////////##############################################################///////////

