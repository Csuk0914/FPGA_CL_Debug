# FPGA_CL_Debug
Debugging code for OpenCL on FPGAs. 

Use Makefile.armhf for cross-compiling.
`make -f Makefile.armhf`

There are four targets to build:
* pr_pull - A pull implementation of page-rank.
* pr_push - A push implementation of page-rank.
* sssp_pull - A pull implementation of single-source shortest path.
* sssp_push - A push implementation of single-source shortest path. 

The pull versions need to be run on transpose graphs, and the push versions can be run on regular graphs. 
To run an application, specify the graph and the number of rounds.

`app_name graph_file_name num_steps`
