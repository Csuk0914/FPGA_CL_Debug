/*
 * PageRankPull.h
 *
 *  Created on: Apr 24, 2016
 *      Author: rashid
 */
#ifndef GALOISGPU_APPS_MICRO_WRAPPER_H_
#define GALOISGPU_APPS_MICRO_WRAPPER_H_

void test_atomic_single_location(size_t max_num_threads, size_t local_size) {
   setup_env(env, "micro/AtomicSingle.cl", "test_atomic_single", nullptr);
   const int num_steps = 15;
   using namespace Galois::OpenCL;
   Array<int> shared_location(1);
   cl_event event;
   shared_location.host_ptr()[0] = 0;
   shared_location.copy_to_device();

   for (size_t num_threads = local_size; num_threads < max_num_threads; num_threads *= 2) {
      cl_kernel atomic_kernel = env.kernel1; //("apps/pr/PageRankPull.cl", "pageRank");
      size_t k1_global, k1_local = local_size;
      k1_global = (size_t) (ceil(num_threads / ((double) k1_local)) * k1_local);

      int err_code;
      Timer start_timer;
      start_timer.start();
      for (int i = 0; i < num_steps; ++i) {
         Galois::OpenCL::CHECK_CL_ERROR(clSetKernelArg(atomic_kernel, 0, sizeof(cl_mem), &shared_location.device_ptr()), "Arg, compact is NOT set!");
         const int value_to_add = 1;
         Galois::OpenCL::CHECK_CL_ERROR(clSetKernelArg(atomic_kernel, 1, sizeof(cl_int), &value_to_add), "Arg, compact is NOT set!");
         err_code = clEnqueueNDRangeKernel(env.commands, atomic_kernel, 1, nullptr, &k1_global, &k1_local, 0, nullptr, &event);
         Galois::OpenCL::CHECK_CL_ERROR(err_code, "kernel1 failed.");
         clFinish(env.commands);
      }
      start_timer.stop();
      shared_location.copy_to_host();
      fprintf(stderr, "STAT,AtomicSingle, %d, TotalTime, %6.6g,s, AvgTime, %6.6g,s,Runs,%d, Threads,%lu, Local, %lu \n", shared_location.host_ptr()[0], start_timer.get_time_seconds(),
            start_timer.get_time_seconds() / (float) num_steps, num_steps, num_threads, local_size);

   }
   fprintf(stderr, "Completed AtomicSingle successfully!\n");
}

void test_atomic_diff_location(bool use_atomics, size_t max_num_threads, size_t local_size) {
   std::string test_name;
   if(use_atomics){
      test_name="AtomicDiff";
      setup_env(env, "micro/AtomicDifferent.cl", "test_atomic_different", nullptr);
   }
   else{
      test_name="AddDiff";
      setup_env(env, "micro/AddDifferent.cl", "test_add_different", nullptr);
   }
   const int num_steps = 15;
   using namespace Galois::OpenCL;
   Array<int> shared_location(max_num_threads);
   cl_event event;
   for (size_t i = 0; i < max_num_threads; ++i) {
      shared_location.host_ptr()[i] = 0;
   }
   shared_location.copy_to_device();

   for (size_t num_threads = local_size; num_threads < max_num_threads; num_threads *= 2) {
      cl_kernel atomic_kernel = env.kernel1; //("apps/pr/PageRankPull.cl", "pageRank");
      size_t k1_global, k1_local = local_size;
      k1_global = (size_t) (ceil(num_threads / ((double) k1_local)) * k1_local);

      int err_code;
      Timer start_timer;
      start_timer.start();
      for (int i = 0; i < num_steps; ++i) {
         Galois::OpenCL::CHECK_CL_ERROR(clSetKernelArg(atomic_kernel, 0, sizeof(cl_mem), &shared_location.device_ptr()), "Arg, compact is NOT set!");
         const int value_to_add = 1;
         Galois::OpenCL::CHECK_CL_ERROR(clSetKernelArg(atomic_kernel, 1, sizeof(cl_int), &value_to_add), "Arg, compact is NOT set!");
         err_code = clEnqueueNDRangeKernel(env.commands, atomic_kernel, 1, nullptr, &k1_global, &k1_local, 0, nullptr, &event);
         Galois::OpenCL::CHECK_CL_ERROR(err_code, "kernel1 failed.");
         clFinish(env.commands);
      }
      start_timer.stop();
      shared_location.copy_to_host();
      fprintf(stderr, "STAT,%s, %d, TotalTime, %6.6g,s, AvgTime, %6.6g,s,Runs,%d, Threads,%lu, Local, %lu \n", test_name.c_str(),shared_location.host_ptr()[0], start_timer.get_time_seconds(),
            start_timer.get_time_seconds() / (float) num_steps, num_steps, num_threads, local_size);

   }
   fprintf(stderr, "Completed %s successfully!\n", test_name.c_str());
}

void test_atomic_diff_location_uncoal(bool use_atomics, bool shuffle_address, size_t max_num_threads, size_t local_size) {
   std::string test_name;
   if(use_atomics){
      test_name="AtomicDiff";
      setup_env(env, "micro/AtomicDifferentUncoal.cl", "test_atomic_different_uncoal", nullptr);
   }
   else{
      test_name="AddDiff";
      setup_env(env, "micro/AddDifferentUncoal.cl", "test_add_different_uncoal", nullptr);
   }
   if(shuffle_address){
      test_name+="Uncoal";
   }else{
      test_name+="Coal";
   }
   const int num_steps = 15;
   using namespace Galois::OpenCL;
   Array<int> shared_location(max_num_threads);
   Array<int> indirect_address(max_num_threads);
   cl_event event;
   for (size_t i = 0; i < max_num_threads; ++i) {
      shared_location.host_ptr()[i] = 0;
   }
   shared_location.copy_to_device();

   for (size_t num_threads = local_size; num_threads < max_num_threads; num_threads *= 2) {
      for (size_t i = 0; i < num_threads; ++i) {
         indirect_address.host_ptr()[i] = i;
      }
      if(shuffle_address){
         std::random_shuffle(&indirect_address.host_ptr()[0], &indirect_address.host_ptr()[num_threads]);
      }
      indirect_address.copy_to_device();

      cl_kernel atomic_kernel = env.kernel1; //("apps/pr/PageRankPull.cl", "pageRank");
      size_t k1_global, k1_local = local_size;
      k1_global = (size_t) (ceil(num_threads / ((double) k1_local)) * k1_local);

      int err_code;
      Timer start_timer;
      start_timer.start();
      for (int i = 0; i < num_steps; ++i) {
         Galois::OpenCL::CHECK_CL_ERROR(clSetKernelArg(atomic_kernel, 0, sizeof(cl_mem), &shared_location.device_ptr()), "Arg, compact is NOT set!");
         Galois::OpenCL::CHECK_CL_ERROR(clSetKernelArg(atomic_kernel, 1, sizeof(cl_mem), &indirect_address.device_ptr()), "Arg, compact is NOT set!");
                  const int value_to_add = 1;
         Galois::OpenCL::CHECK_CL_ERROR(clSetKernelArg(atomic_kernel, 2, sizeof(cl_int), &value_to_add), "Arg, compact is NOT set!");
         err_code = clEnqueueNDRangeKernel(env.commands, atomic_kernel, 1, nullptr, &k1_global, &k1_local, 0, nullptr, &event);
         Galois::OpenCL::CHECK_CL_ERROR(err_code, "kernel1 failed.");
         clFinish(env.commands);
      }
      start_timer.stop();
      shared_location.copy_to_host();
      fprintf(stderr, "STAT,%s, %d, TotalTime, %6.6g,s, AvgTime, %6.6g,s,Runs,%d, Threads,%lu, Local, %lu \n", test_name.c_str(),shared_location.host_ptr()[0], start_timer.get_time_seconds(),
            start_timer.get_time_seconds() / (float) num_steps, num_steps, num_threads, local_size);

   }
   fprintf(stderr, "Completed %s successfully!\n", test_name.c_str());
}
#if 0
void SSSPHost(int local_size, int num_steps, char * fname) {
   using namespace Galois::OpenCL;
   typedef Galois::OpenCL::LC_LinearArray_Graph<unsigned int, unsigned int> Graph;

//   fprintf(stderr, "Launching SSSP(Pull) :: %s\n", fname);
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
      //    fprintf(stderr, "Launched kernel %d\n", i);
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
   fprintf(stderr, "\nInput, %s, Time, %6.6g,s,Rounds,%d, Threads,%d \n", fname, start_timer.get_time_seconds(), num_steps, local_size);
   fprintf(stderr, "Copying to host ...\n");
   g.copy_to_host();
   fprintf(stderr, "Done copying to host ...\n");
   {

      char out_file_name[1024];
      sprintf(out_file_name, "%s.SSSP_%s_%d-step_%d-thread.log", fname
#ifdef SSSP_PULL_VER
            "PULL"
#elif defined (SSSP_PUSH_VER)
            "PUSH"
#endif 
            , num_steps, local_size);

      std::ofstream out_file(out_file_name);

      for (size_t i = 0; i < g.num_nodes(); ++i) {
         out_file << i << ", " << g.node_data()[i] << "\n";
//         fprintf(stderr, "SSSP [%lu ]  = %d\n", i, g.node_data()[i]);
      }
      out_file.close();

   }
   if (false) {
      for (size_t i = 0; i < g.num_nodes(); ++i) {
         fprintf(stderr, "SSSP [%lu ]  = %d\n", i, g.node_data()[i]);
      }
   }
   fprintf(stderr, "Completed SSSP successfully!\n");
}
#endif // end commented code


void test_memory_reads(size_t max_num_threads, size_t local_size) {
   std::string test_name;
   test_name="MemInit";
   setup_env(env, "micro/MemoryInitialize.cl", "initialize_memory", nullptr);
   const int num_steps = 1;
   using namespace Galois::OpenCL;
   Array<int> shared_location(max_num_threads);
   cl_event event;
   for (size_t i = 0; i < max_num_threads; ++i) {
      shared_location.host_ptr()[i] = 0;
   }
   shared_location.copy_to_device();
   Timer write_timer, read_timer;
   for (size_t num_threads = local_size; num_threads < max_num_threads; num_threads *= 2) {

      int fail_counter=0;
      cl_kernel init_kernel= env.kernel1; //("apps/pr/PageRankPull.cl", "pageRank");
      size_t k1_global, k1_local = local_size;
      k1_global = (size_t) (ceil(num_threads / ((double) k1_local)) * k1_local);

      int err_code;
      Timer start_timer;
      start_timer.start();
      for (int i = 0; i < num_steps; ++i) {
	      write_timer.clear();
	      write_timer.start();
	      for(int n=0; n<num_threads; ++n){
		      int res=0;
		      err_code = clEnqueueWriteBuffer(env.commands, shared_location.device_ptr(), CL_TRUE, n*sizeof(int), sizeof(int), &res, 0, nullptr, nullptr);
		      //Galois::OpenCL::CHECK_CL_ERROR(err_code, "Write int failed.");
		      if(err_code){
			fprintf(stderr, "Write int failed.%d.", err_code);
		}
		      //fprintf(stderr, "%d, ",res) ;
	      }
		write_timer.stop();
	      Galois::OpenCL::CHECK_CL_ERROR(clSetKernelArg(init_kernel, 0, sizeof(cl_mem), &shared_location.device_ptr()), "Arg, compact is NOT set!");
	      err_code = clEnqueueNDRangeKernel(env.commands, init_kernel, 1, nullptr, &k1_global, &k1_local, 0, nullptr, &event);
	      Galois::OpenCL::CHECK_CL_ERROR(err_code, "kernel1 failed.");
	      clFinish(env.commands);

	      read_timer.clear();
	      read_timer.start();
	      for(int n=0; n<num_threads; ++n){
		      int res=0;
		      err_code = clEnqueueReadBuffer(env.commands, shared_location.device_ptr(), CL_TRUE, n*sizeof(int), sizeof(int), &res, 0, nullptr, nullptr);
		      Galois::OpenCL::CHECK_CL_ERROR(err_code, "Read int failed.");
		      if(n!=res)
   			      fail_counter++;
	//	      fprintf(stderr, "%d, ",res) ;
	      }
	      read_timer.stop();
	      fprintf(stderr, "\n");
      }
      start_timer.stop();
      shared_location.copy_to_host();
      fprintf(stderr, "STAT,%s,FailCounter, %d, WriteTime, %6.6g,s, ReadTime, %6.6g,s, TotalTime, %6.6g,s, AvgTime, %6.6g,s,Runs,%d, Threads,%lu, Local, %lu \n", 
			test_name.c_str(),fail_counter, write_timer.get_time_seconds(), read_timer.get_time_seconds(), start_timer.get_time_seconds(),
            start_timer.get_time_seconds() / (float) num_steps, num_steps, num_threads, local_size);
	{

	}	

   }
   fprintf(stderr, "Completed %s successfully!\n", test_name.c_str());
}
///////////////////////////////////////////////////////////////////////

void test_svm_memory_reads(size_t max_num_threads, size_t local_size) {
#ifdef _ALTERA_SVM_
   std::string test_name;
   test_name="SVMMemInit";
   setup_env(env, "micro/MemoryInitialize.cl", "initialize_memory", nullptr);
   const int num_steps = 1;
   using namespace Galois::OpenCL;
   SVMArray<int> shared_location(max_num_threads);
   cl_event event;
   for (size_t i = 0; i < max_num_threads; ++i) {
      shared_location.host_ptr()[i] = 0;
   }
   shared_location.copy_to_device();
   Timer write_timer, read_timer;
   for (size_t num_threads = local_size; num_threads < max_num_threads; num_threads *= 2) {

      int fail_counter=0;
      cl_kernel init_kernel= env.kernel1; //("apps/pr/PageRankPull.cl", "pageRank");
      size_t k1_global, k1_local = local_size;
      k1_global = (size_t) (ceil(num_threads / ((double) k1_local)) * k1_local);

      int err_code;
      Timer start_timer;
      start_timer.start();
      for (int i = 0; i < num_steps; ++i) {
	      write_timer.clear();
	      write_timer.start();
		write_timer.stop();
	      Galois::OpenCL::CHECK_CL_ERROR(clSetKernelArgSVMPointerAltera(init_kernel, 0, (void*)shared_location.device_ptr()), "Arg, compact is NOT set!");
	      //Galois::OpenCL::CHECK_CL_ERROR(clSetKernelArg(init_kernel, 0, sizeof(cl_mem), &shared_location.device_ptr()), "Arg, compact is NOT set!");
	      err_code = clEnqueueNDRangeKernel(env.commands, init_kernel, 1, nullptr, &k1_global, &k1_local, 0, nullptr, &event);
	      Galois::OpenCL::CHECK_CL_ERROR(err_code, "kernel1 failed.");
	      clFinish(env.commands);

	      read_timer.clear();
	      read_timer.start();
	      for(int n=0; n<num_threads; ++n){
		      int res=shared_location.host_ptr()[n];
		      //err_code = clEnqueueReadBuffer(env.commands, shared_location.device_ptr(), CL_TRUE, n*sizeof(int), sizeof(int), &res, 0, nullptr, nullptr);
		      Galois::OpenCL::CHECK_CL_ERROR(err_code, "Read int failed.");
		      if(n!=res){
   			      fail_counter++;
		      fprintf(stderr, "%d->%d, ",res,n) ;
}
	      }
	      read_timer.stop();
	      fprintf(stderr, "\n");
      }
      start_timer.stop();
      shared_location.copy_to_host();
      fprintf(stderr, "STAT,%s,FailCounter, %d, WriteTime, %6.6g,s, ReadTime, %6.6g,s, TotalTime, %6.6g,s, AvgTime, %6.6g,s,Runs,%d, Threads,%lu, Local, %lu \n", 
			test_name.c_str(),fail_counter, write_timer.get_time_seconds(), read_timer.get_time_seconds(), start_timer.get_time_seconds(),
            start_timer.get_time_seconds() / (float) num_steps, num_steps, num_threads, local_size);
	{

	}	

   }
   fprintf(stderr, "Completed %s successfully!\n", test_name.c_str());
#else
   fprintf(stderr, "Failed - _ALTERA_SVM_ not defined!!\n");
#endif
}


#endif /* GALOISGPU_APPS_MICRO_WRAPPER_H_ */
