/*
 * ArrayImpl.h
 *
 *  Created on: Apr 16, 2015
 *      Author: rashid
 */

#ifndef GALOISGPU_OCL_ARRAYIMPL_H_
#define GALOISGPU_OCL_ARRAYIMPL_H_

namespace Galois {
namespace OpenCL {

template<typename T>
struct Array {
   typedef cl_mem DevicePtrType;
   typedef T * HostPtrType;

   size_t num_elements;
   //CL_Device * device;
   cl_mem_flags allocation_flags;
   HostPtrType host_data;
   DevicePtrType device_data;
   int err;

   explicit Array(unsigned long sz) :
         num_elements(sz){
#if 0
      allocation_flags = CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR;
      device_data = clCreateBuffer(device->context(), allocation_flags, sizeof(T) * num_elements, NULL, &err);
      Galois::OpenCL::CHECK_CL_ERROR(err, "Allocation failure...!");
      host_data = (T*) clEnqueueMapBuffer(device->command_queue(), device_data, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0, sizeof(T) * num_elements, 0, NULL, NULL, &err);
#else
      host_data=  nullptr;
      int ret = posix_memalign((void **)&host_data, 4096, sizeof(T)*num_elements);
      assert(ret==0 && "Posix-memalign failed." );
      allocation_flags = CL_MEM_READ_WRITE ;//| CL_MEM_USE_HOST_PTR;
      device_data = clCreateBuffer(env.context , allocation_flags, sizeof(T) * num_elements, NULL/*host_data*/, &err);
      Galois::OpenCL::CHECK_CL_ERROR(err, "Allocation failure...!");
#endif
   }
   ////////////////////////////////////////////////
   ~Array(){
#ifdef _GOPT_DEBUG
      std::cout<<"Deleting array host:: " << host_data << " , device :: " << device_data<<"\n";
#endif
      if(host_data){
         free(host_data);
         host_data = nullptr;
         Galois::OpenCL::CHECK_CL_ERROR(clReleaseMemObject(device_data),"Failed to release device memory object.");
      }
   }
   ////////////////////////////////////////////////
   void copy_to_device(size_t sz) {
      CHECK_CL_ERROR(err = clEnqueueWriteBuffer(env.commands, device_data, CL_TRUE, 0, sizeof(T) * sz, (void*) (host_data), 0, NULL, NULL), " Copying to device ");
   }

   void copy_to_device() {
      CHECK_CL_ERROR(err = clEnqueueWriteBuffer(env.commands, device_data, CL_TRUE, 0, sizeof(T) * num_elements, (void*) (host_data), 0, NULL, NULL),
            " Copying to device ");
   }
   ////////////////////////////////////////////////
   void copy_to_device(cl_event * event) {
      CHECK_CL_ERROR(err = clEnqueueWriteBuffer(env.commands, device_data, CL_FALSE, 0, sizeof(T) * num_elements, (void*) (host_data), 0, NULL, event),
            " Copying async. to device ");
   }
   ////////////////////////////////////////////////
   void copy_to_device(void * aux) {
      CHECK_CL_ERROR(err = clEnqueueWriteBuffer(env.commands, device_data, CL_TRUE, 0, sizeof(T) * num_elements, (void*) (aux), 0, NULL, NULL),
            " Copying aux to device ");
   }
   ////////////////////////////////////////////////
   void copy_to_device(void * aux, size_t sz) {
      CHECK_CL_ERROR(err = clEnqueueWriteBuffer(env.commands, device_data, CL_TRUE, 0, sizeof(T) * sz, (void*) (aux), 0, NULL, NULL), " Copying aux to device ");
   }
   ////////////////////////////////////////////////

   void copy_to_host(size_t sz) {
      CHECK_CL_ERROR(err = clEnqueueReadBuffer(env.commands, device_data, CL_TRUE, 0, sizeof(T) * sz, (void*) (host_data), 0, NULL, NULL), "Copying to host ");
   }
   void copy_to_host(size_t sz, cl_event *event) {
      CHECK_CL_ERROR(err = clEnqueueReadBuffer(env.commands, device_data, CL_FALSE, 0, sizeof(T) * sz, (void*) (host_data), 0, NULL, event), "Copying to host ");
   }

   void copy_to_host() {
      CHECK_CL_ERROR(err = clEnqueueReadBuffer(env.commands, device_data, CL_TRUE, 0, sizeof(T) * num_elements, (void*) (host_data), 0, NULL, NULL), "Copying to host ");
   }
   ////////////////////////////////////////////////
   size_t size() {
      return num_elements;
   }
   ////////////////////////////////////////////////
   operator T*() {
      return host_data;
   }
   T & operator [](size_t idx) {
      return host_data[idx];
   }
   DevicePtrType & device_ptr(void) {
      return device_data;
   }
   HostPtrType & host_ptr(void) {
      return host_data;
   }
   Array<T> * get_array_ptr(void) {
      return this;
   }

protected:
};

} //end namespace OpenCL
} //end namespace Galois

#endif /* GALOISGPU_OCL_ARRAYIMPL_H_ */
