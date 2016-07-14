/*
 * PageRankPull.h
 *
 *  Created on: Apr 24, 2016
 *      Author: rashid
 */

#ifndef GALOISGPU_APPS_BFS_H_
#define GALOISGPU_APPS_BFS_H_

void BFSHost(int local_size, int num_steps, char * fname) {
#ifdef BFS_PULL_VER
      const char * app_name = "BFS_PULL";
#elif defined (BFS_PUSH_VER)
      const char * app_name = "BFS_PUSH";
#endif

   using namespace Galois::OpenCL;
   typedef Galois::OpenCL::LC_LinearArray_Graph<unsigned int, unsigned int> Graph;

   unsigned int dev_clk_frequency;
   clGetDeviceInfo(env.device_id, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(unsigned int), &dev_clk_frequency, NULL);
   std::cout << "Timer resolutions :: " << CL_DEVICE_PROFILING_TIMER_RESOLUTION << " clock cycles @ " << dev_clk_frequency << "MHz\n";
   Graph g;
   cl_event event;
   g.read(fname);
   for (size_t i = 0; i < g.num_nodes(); ++i) {
      g.node_data()[i] = 0;
   }
   g.copy_to_device();
   cl_kernel init_kernel = env.kernel1; //("apps/pr/PageRankPull.cl", "initialize");
   cl_kernel sssp_kernel = env.kernel2; //("apps/pr/PageRankPull.cl", "pageRank");

   size_t num_items = g.num_nodes();
   size_t k1_global, k1_local = local_size, k2_global, k2_local = local_size;
   k2_global = (size_t) (ceil(num_items / ((double) k2_local)) * k2_local);
   k1_global = (size_t) (ceil(num_items / ((double) k1_local)) * k1_local);

   Galois::OpenCL::CHECK_CL_ERROR(clSetKernelArg(init_kernel, 0, sizeof(cl_mem), &g.device_ptr()), "Arg, compact is NOT set!");

   Galois::OpenCL::CHECK_CL_ERROR(clSetKernelArg(sssp_kernel, 0, sizeof(cl_mem), &g.device_ptr()), "Arg, compact is NOT set!");

   Galois::OpenCL::CHECK_CL_ERROR(clEnqueueNDRangeKernel(env.commands, init_kernel, 1, nullptr, &k1_global, &k1_local, 0, nullptr, &event), "kernel1 failed.");

   clFinish(env.commands);
   int err_code;
//   fprintf(stderr, "Launched kernel\n");
//   clReleaseKernel(sssp_kernel);
//   cl_event start_event = clCreateUserEvent(env.context,&err_code);
   Timer start_timer;
   start_timer.start();
   for (int i = 0; i < num_steps; ++i) {
      //sssp_kernel();
//      sssp_kernel =load_kernel(env, "sssp_kernel");
      Galois::OpenCL::CHECK_CL_ERROR(clSetKernelArg(sssp_kernel, 0, sizeof(cl_mem), &g.device_ptr()), "Arg, compact is NOT set!");

      err_code = clEnqueueNDRangeKernel(env.commands, sssp_kernel, 1, nullptr, &k2_global, &k2_local, 0, nullptr, &event);
      Galois::OpenCL::CHECK_CL_ERROR(err_code, "kernel2 failed.");
      fprintf(stdout, "\b\b\b\b\b\b[%4d]", i);
      clFinish(env.commands);
//      clReleaseKernel(sssp_kernel);
//      fprintf(stderr, "Launched kernel %d\n", i);
   }
   start_timer.stop();
//   cl_event end_event = clCreateUserEvent(env.context,&err_code);
//   unsigned long start_time, end_time;
//   size_t start_time_size = sizeof(start_time);
//   clGetEventProfilingInfo(start_event, CL_PROFILING_COMMAND_SUBMIT, sizeof(unsigned long ), &start_time, NULL);
//   clGetEventProfilingInfo(end_event, CL_PROFILING_COMMAND_END, sizeof(unsigned long), &end_time, NULL);
//   fprintf(stderr, "Time diff %ld \n", end_time-start_time);
//   fprintf(stderr, "Time diff %6.6g \n", (end_time-start_time)/(1000000000.0f));
   fprintf(stderr, "\nInput, %s, Time, %6.6g,s,Rounds,%d, Threads,%d \n", fname,start_timer.get_time_seconds(),num_steps,local_size);
   fprintf(stderr, "Copying to host ...\n");
   g.copy_to_host();
   fprintf(stderr, "Done copying to host ...\n");
   {

      char out_file_name[1024];
      sprintf(out_file_name, "%s.%s_%d-step_%d-thread.log", fname,app_name ,num_steps, local_size);
      std::ofstream out_file(out_file_name);

      for (size_t i = 0; i < g.num_nodes(); ++i) {
         out_file << i << ", " << g.node_data()[i] << "\n";
//         fprintf(stderr, "SSSP [%lu ]  = %d\n", i, g.node_data()[i]);
      }
      out_file.close();

   }
   if (false) {
      for (size_t i = 0; i < g.num_nodes(); ++i) {
         fprintf(stderr, "BFS [%lu ]  = %d\n", i, g.node_data()[i]);
      }
   }
   fprintf(stderr, "Completed %s successfully!\n",app_name);
}

#endif /* GALOISGPU_APPS_BFS_H_ */
