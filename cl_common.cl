/*
 * cl_common.cl
 *
 *  Created on: Sep 16, 2014
 *      Author: rashid
 */

#ifndef CL_COMMON_CL_
#define CL_COMMON_CL_

#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics :enable
#pragma OPENCL EXTENSION cl_khr_global_int32_extended_atomics : enable
#pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics :enable
#pragma OPENCL EXTENSION cl_khr_local_int32_extended_atomics : enable
//#pragma OPENCL EXTENSION cl_nv_compile_options:enable

/////////////////////////////////////////////////////
#ifdef cl_khr_int64_base_atomics
#pragma OPENCL EXTENSION cl_khr_int64_base_atomics : enable
#endif
#ifdef cl_khr_int64_extended_atomics
#pragma OPENCL EXTENSION cl_khr_int64_extended_atomics : enable
#endif
#ifdef cl_khr_fp64
#pragma OPENCL EXTENSION cl_khr_fp64: enable
#endif

#ifdef cl_khr_fp16
#pragma OPENCL EXTENSION cl_khr_fp16: enable
#endif

#ifdef cl_amd_fp64
#pragma OPENCL EXTENSION cl_amd_fp64: enable
#endif
#ifdef cl_amd_vec3
#pragma OPENCL EXTENSION cl_amd_vec3: enable
#endif

#ifdef CL_INTEL
#define INTEL_OVERLOAD_ATTRIBUTE __attribute((overloadable))
#endif

#ifdef CL_NVIDIA
#define NV_OVERLOAD_ATTRIBUTE __OVERLOADABLE__
#endif

#ifdef CL_ALTERA
#define ALTERA_OVERLOAD_ATTRIBUTE __attribute((overloadable))
#endif
//#define OSX_OVERLOAD_ATTRIBUTE __OVERLOAD__


#ifdef CL_INTEL
#define ACTIVE_OVERLOAD_ATTRIBUTE   INTEL_OVERLOAD_ATTRIBUTE
#endif

#ifdef CL_NVIDIA
#define ACTIVE_OVERLOAD_ATTRIBUTE   NV_OVERLOAD_ATTRIBUTE
#endif

#ifdef CL_AMD
#define ACTIVE_OVERLOAD_ATTRIBUTE
#endif

#ifdef CL_ALTERA
#define ACTIVE_OVERLOAD_ATTRIBUTE ALTERA_OVERLOAD_ATTRIBUTE
#endif

//#define ACTIVE_OVERLOAD_ATTRIBUTE   NV_OVERLOAD_ATTRIBUTE //INTEL_OVERLOAD_ATTRIBUTE

/////////////////////////////////////////////////////
/*
Atomic-addition for floats from http://simpleopencl.blogspot.com/2013/05/atomic-operations-and-floats-in-opencl.html
Ideally move to newer version which supports float-atomics in OpenCL-2.0 standard.
*/
void atomic_add_float_global(volatile global float *source, const float operand) {
    union {
        unsigned int intVal;
        float floatVal;
    } newVal;
    union {
        unsigned int intVal;
        float floatVal;
    } prevVal;

    do {
        prevVal.floatVal = *source;
        newVal.floatVal = prevVal.floatVal + operand;
    } while (atomic_cmpxchg((volatile global unsigned int *)source, prevVal.intVal, newVal.intVal) != prevVal.intVal);
}
//////////////////////////
void atomic_add_float_local(volatile local float *source, const float operand) {
    union {
        unsigned int intVal;
        float floatVal;
    } newVal;

    union {
        unsigned int intVal;
        float floatVal;
    } prevVal;

    do {
        prevVal.floatVal = *source;
        newVal.floatVal = prevVal.floatVal + operand;
    } while (atomic_cmpxchg((volatile local unsigned int *)source, prevVal.intVal, newVal.intVal) != prevVal.intVal);
}
/////////////////////////////////////////////////////


#endif /* CL_COMMON_CL_ */
