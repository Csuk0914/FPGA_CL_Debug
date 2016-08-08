#OS_NAME_STRING=$(shell uname)


CFLAGS= -m64 -Wall -Wextra -lrt -std=c++0x -g -I${CUDA_INC_PATH} -L${CUDA_LIB_PATH} -L/lib64 #-lamdocl64
#ALTERA_LIB_FLAGS=$(shell aocl link-config)
#ALTERA_INC_FLAGS=$(shell aocl compile-config)
#CFLAGS+=${ALTERA_INC_FLAGS} -D_ALTERA_FPGA_USE_=1 ${ALTERA_LIB_FLAGS}
#CL_COMPILE_OPTIONS+=-I./CL_HEADER -lpthread -lamdocl64
CL_COMPILE_OPTIONS+=-I/opt/apps/sysnet/cuda/7.0/cudatk/include -L/opt/apps/sysnet/cuda/7.0/cudatk/lib64  -L/usr/lib64/nvidia -lOpenCL
	

all : pr sssp

clean: sssp_clean pr_clean

pr : pr_pull pr_push

pr_clean : 
	rm pr_pull pr_push

pr_pull: TestPageRank.cpp PageRankPull.h
	${CXX} ${CFLAGS}  -D_USE_CL -DPR_PULL_VER -I. -m64  TestPageRank.cpp  -o pr_pull $(CL_COMPILE_OPTIONS)
	
pr_push: TestPageRank.cpp PageRankPush.h
	${CXX} ${CFLAGS}  -D_USE_CL -DPR_PUSH_VER -I. -m64 TestPageRank.cpp  -o pr_push $(CL_COMPILE_OPTIONS)
	 
	 
sssp : sssp_pull sssp_push

sssp_clean : 
	rm sssp_pull sssp_push

sssp_pull: TestSSSP.cpp SSSPHost.h
	${CXX} ${CFLAGS}  -D_USE_CL -DSSSP_PULL_VER -I. -m64  TestSSSP.cpp  -o sssp_pull $(CL_COMPILE_OPTIONS)
	
sssp_push: TestSSSP.cpp SSSPHost.h
	${CXX} ${CFLAGS}  -D_USE_CL -DSSSP_PUSH_VER -I. -m64 TestSSSP.cpp  -o sssp_push $(CL_COMPILE_OPTIONS)
	 

#==========================================
micro: micro/*
	${CXX} ${CFLAGS}  -D_USE_CL -I. -I./micro/ -m64  micro/TestMicro.cpp  -o test_micro $(CL_COMPILE_OPTIONS)

#==========================================
bfs : bfs_pull bfs_push

bfs_clean : 
	rm bfs_pull bfs_push

bfs_pull: bfs/TestBFS.cpp  bfs/BFSHost.h
	${CXX} ${CFLAGS}  -D_USE_CL -DBFS_PULL_VER -I. -Ibfs/ -m64  bfs/TestBFS.cpp  -o bfs_pull $(CL_COMPILE_OPTIONS)
	
bfs_push: bfs/TestBFS.cpp bfs/BFSHost.h
	${CXX} ${CFLAGS}  -D_USE_CL -DBFS_PUSH_VER -I. -Ibfs/ -m64 bfs/TestBFS.cpp  -o bfs_push $(CL_COMPILE_OPTIONS)
	 
	
#===========================================================================================================

