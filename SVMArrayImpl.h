/*
 * ArrayImpl.h
 *
 *  Created on: Apr 16, 2015
 *      Author: rashid
 */
#include <CL/cl_ext.h>
#ifndef GALOISGPU_OCL_SVMARRAYIMPL_H_
#define GALOISGPU_OCL_SVMARRAYIMPL_H_

namespace Galois {
namespace OpenCL {

template<typename T>
struct SVMArray {
   typedef T * DevicePtrType;
   typedef T * HostPtrType;

   size_t num_elements;
   //CL_Device * device;
   cl_mem_flags allocation_flags;
   HostPtrType host_data;
   int err;

   explicit SVMArray(unsigned long sz) :
         num_elements(sz){
      host_data=  nullptr;
      host_data = (T*) clSVMAllocAltera(env.context, 0, sizeof(T) * num_elements, 1024);
      if(host_data==nullptr) {
	exit(-1);
       }
      assert(host_data!=0 && "Posix-memalign failed." );
   }
   ////////////////////////////////////////////////
   ~SVMArray(){
      if(host_data){
	clSVMFreeAltera(env.context,host_data);
         host_data = nullptr;
      }
   }
   ////////////////////////////////////////////////
   void copy_to_device(size_t sz) {
   }

   void copy_to_device() {
   }
   ////////////////////////////////////////////////
   void copy_to_device(cl_event * event) {
   }
   ////////////////////////////////////////////////
   void copy_to_device(void * aux) {
	memcpy(host_data, aux, sizeof(T)*num_elements);
   }
   ////////////////////////////////////////////////
   void copy_to_device(void * aux, size_t sz) {
	memcpy(host_data, aux, sizeof(T)*num_elements);
   }
   ////////////////////////////////////////////////

   void copy_to_host(size_t sz) {
   }
   void copy_to_host(size_t sz, cl_event *event) {
   }

   void copy_to_host() {
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
      return host_data;
   }
   HostPtrType & host_ptr(void) {
      return host_data;
   }
   SVMArray<T> * get_array_ptr(void) {
      return this;
   }

protected:
};

} //end namespace OpenCL
} //end namespace Galois

#endif /* GALOISGPU_OCL_SVMARRAYIMPL_H_ */
