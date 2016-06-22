/*
 * PageRankPull.h
 *
 *  Created on: Apr 24, 2016
 *      Author: rashid
 */

#ifndef GALOISGPU_APPS_PR_PAGERANKPULL_H_
#define GALOISGPU_APPS_PR_PAGERANKPULL_H_

struct NodeDataPull {
   int nout;
   float value;
};

void PR_Pull(int num_steps, char * fname) {
   num_steps = 10;
   using namespace Galois::OpenCL;
   typedef Galois::OpenCL::LC_LinearArray_Graph<NodeDataPull, unsigned int> Graph;

   fprintf(stderr, "Launching PageRank(Pull) :: %s\n", fname);
   Graph g;
   cl_event event;
   g.read(fname);
   for (size_t i = 0; i < g.num_nodes(); ++i) {
      NodeDataPull * d = &g.node_data()[i];
      d->nout = 0;
      d->value = 0;
   }
   g.copy_to_device();
   cl_kernel init_kernel = env.kernel1;//("apps/pr/PageRankPull.cl", "initialize");
   cl_kernel pr_kernel = env.kernel2;//("apps/pr/PageRankPull.cl", "pageRank");

   size_t num_items = g.num_nodes();
   size_t k1_global, k1_local=256, k2_global, k2_local=256;
   k2_global = (size_t) (ceil(num_items / ((double) k2_local)) * k2_local);
   k1_global = (size_t) (ceil(num_items / ((double) k1_local)) * k1_local);


//   init_kernel.set_work_size(g.num_nodes(), 256);
//   init_kernel.set_arg(0, &g);
   Galois::OpenCL::CHECK_CL_ERROR(clSetKernelArg(init_kernel, 0, sizeof(cl_mem), &g.device_ptr()), "Arg, compact is NOT set!");

//   pr_kernel.set_work_size(g.num_nodes(), 256);
//   pr_kernel.set_arg(0, &g);
   Galois::OpenCL::CHECK_CL_ERROR(clSetKernelArg(pr_kernel, 0, sizeof(cl_mem), &g.device_ptr()), "Arg, compact is NOT set!");


   //init_kernel();
   Galois::OpenCL::CHECK_CL_ERROR(clEnqueueNDRangeKernel(env.commands, init_kernel, 1, nullptr, &k1_global, &k1_local, 0, nullptr, &event), "kernel1 failed.");

   clFinish(env.commands);
   fprintf(stderr, "Launched kernel\n");

   for (int i = 0; i < num_steps; ++i) {
      //pr_kernel();
      Galois::OpenCL::CHECK_CL_ERROR(clEnqueueNDRangeKernel(env.commands, pr_kernel, 1, nullptr, &k2_global, &k2_local, 0, nullptr, &event), "kernel2 failed.");
      fprintf(stderr, "Launched kernel %d\n", i);
      clFinish(env.commands);
      fprintf(stderr, "Launched kernel %d\n", i);
   }
   fprintf(stderr, "Copying to host ...\n");
   g.copy_to_host();
   fprintf(stderr, "Done copying to host ...\n");
   if (false)
      for (size_t i = 0; i < g.num_nodes(); ++i) {
         fprintf(stderr, "PR [%lu, %d]  = %6.6g\n", i, g.node_data()[i].nout, g.node_data()[i].value);
         //fprintf(stderr, "PR [%lu]  = %d\n", i, g.node_data()[i].nout);
      }
   fprintf(stderr, "Completed PageRankPull successfully!\n");
}

#endif /* GALOISGPU_APPS_PR_PAGERANKPULL_H_ */
