/*
 * ocl_header.h
 *
 *  Created on: Feb 7, 2013
 *      Author: rashid
 */

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
extern "C" {
#include "CL/cl.h"
}
;
#endif

//#define  _ALTERA_FPGA_USE_ 1
#ifdef _ALTERA_FPGA_USE_
//   # define _ALTERA_EMULATOR_USE_ 1
#else
   #define _GALOIS_BUILD_INITIALIZER_KERNEL_ 1
#endif

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/stat.h>
#include <assert.h>
#include "common_header.h"
#include "CLErrors.h"


struct CLEnv {
   size_t global;                      // global domain size for our calculation
   size_t local;                       // local domain size for our calculation
   cl_device_id device_id;             // compute device id
   cl_context context;                 // compute context
   cl_command_queue commands;          // compute command queue
   cl_program program;                 // compute program
   cl_kernel kernel1;                   // compute kernel
   cl_kernel kernel2;                   // compute kernel
   cl_platform_id platform;
};


static inline const char *load_program_source(const char *filename) {
   FILE *fh;
   struct stat statbuf;
   char *source;
   if (!(fh = fopen(filename, "rb")))
      return NULL;
   stat(filename, &statbuf);
   source = (char *) malloc(statbuf.st_size + 1);
   fread(source, statbuf.st_size, 1, fh);
   source[statbuf.st_size] = 0;
   return source;
}
/*********************************************************************
 *
 **********************************************************************/
int setup_env(CLEnv & env, const char * _filename , const char * kernel1_name, const char * kernel2_name) {
   int err;
   cl_platform_id platforms[3];
   err = clGetPlatformIDs(3, &platforms[0], NULL);
   env.platform = platforms[DEFAULT_PLATFORM];
   if (err != CL_SUCCESS) {
      printf("Error: Failed to create a platform! :: %d \n", err);
      return EXIT_FAILURE;
   }
   cl_device_id devices[5];
   err = clGetDeviceIDs(env.platform, CL_DEVICE_TYPE_ALL, 5, &devices[0], NULL);
   if (err != CL_SUCCESS) {
      printf("Error: Failed to create a device group! :: %d \n", err);
      return EXIT_FAILURE;
   }
   env.device_id = devices[DEFAULT_DEVICE];
   // Create a compute context
   //
   env.context = clCreateContext(0, 1, &env.device_id, NULL, NULL, &err);
   if (!env.context) {
      printf("Error: Failed to create a compute context!\n");
      return EXIT_FAILURE;
   }
   // Create a command commands
   env.commands = clCreateCommandQueue(env.context, env.device_id, 0, &err);
   if (!env.commands) {
      printf("Error: Failed to create a command commands!\n");
      return EXIT_FAILURE;
   }
   // Create the compute program from the source buffer

   const std::string filename (_filename);
#if _ALTERA_FPGA_USE_
      {
         FILE *fh;
         std::string compiled_file_name(filename);
         compiled_file_name.replace(compiled_file_name.length()-3, compiled_file_name.length(),".aocx");
         fprintf(stderr, "About to load binary file :: %s \n", compiled_file_name.c_str());
         if (!(fh = fopen(compiled_file_name.c_str(), "rb")))
            return EXIT_FAILURE;
         fseek(fh, 0, SEEK_END);
         size_t len = ftell(fh);
         unsigned char * source = (unsigned char *) malloc(len);
         rewind(fh);
         fread(source, len, 1, fh);
         env.program = clCreateProgramWithBinary(env.context, 1, &env.device_id, &len, (const unsigned char **) &source, NULL, &err);
         if(!env.program){
            fprintf(stderr, "clCreateProgramWithSource");
            return EXIT_FAILURE;
         }
         fprintf(stderr, "Loaded program successfully [Len=%d]....\n", len);
      }
#else
   const char * source = load_program_source(filename.c_str());
   env.program = clCreateProgramWithSource(env.context, 1, (const char **) &source, NULL, &err);
   if (!env.program) {
      printf("Error: Failed to create compute program!\n");
      return EXIT_FAILURE;
   }
   // Build the program executable
   err = clBuildProgram(env.program, 0, NULL, NULL, NULL, NULL);
   if (err != CL_SUCCESS) {
      size_t len;
      char buffer[2048];
      printf("Error: Failed to build program executable!\n");
      clGetProgramBuildInfo(env.program, env.device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
      printf("%s\n", buffer);
      exit(1);
   }
#endif
   // Create the compute kernel in the program we wish to run
   env.kernel1 = clCreateKernel(env.program, kernel1_name, &err);
   if (!env.kernel1 || err != CL_SUCCESS) {
      printf("Error: Failed to create compute kernel!\n");
      exit(1);
   }
   err = clGetKernelWorkGroupInfo(env.kernel1, env.device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(env.local), &env.local, NULL);
   if (err != CL_SUCCESS) {
      printf("Error: Failed to retrieve kernel work group info! %d\n", err);
      exit(1);
   }
   printf("Local size detected :: %lu\n", env.local);
   //////////////////////////////////////////////////////
   env.kernel2 = clCreateKernel(env.program, kernel2_name, &err);
      if (!env.kernel2 || err != CL_SUCCESS) {
         printf("Error: Failed to create compute kernel2!\n");
         exit(1);
      }
      err = clGetKernelWorkGroupInfo(env.kernel2, env.device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(env.local), &env.local, NULL);
      if (err != CL_SUCCESS) {
         printf("Error: Failed to retrieve kernel2 work group info! %d\n", err);
         exit(1);
      }
      printf("Local size detected :: %lu\n", env.local);

   return 0;
}
//////////////////////////////////////////
#ifndef GOPT_OCL_HEADER_H_
#define GOPT_OCL_HEADER_H_


#endif /* GOPT_OCL_HEADER_H_ */
