/* ------------------------------------------------------------------------ */
/* Copyright 2018, IBM Corp.                                                */
/*                                                                          */
/* Licensed under the Apache License, Version 2.0 (the "License");          */
/* you may not use this file except in compliance with the License.         */
/* You may obtain a copy of the License at                                  */
/*                                                                          */
/*    http://www.apache.org/licenses/LICENSE-2.0                            */
/*                                                                          */
/* Unless required by applicable law or agreed to in writing, software      */
/* distributed under the License is distributed on an "AS IS" BASIS,        */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. */
/* See the License for the specific language governing permissions and      */
/* limitations under the License.                                           */
/* ------------------------------------------------------------------------ */

#ifndef __SOLID_GPU_APPLY_ELEMWISE3_H__
#define __SOLID_GPU_APPLY_ELEMWISE3_H__

#include "solid.h"
#include "solid/base/gpu/generate_macros.h"
#include "solid/core/gpu/apply_elemwise.h"


/* ------------------------------------------------------------------------ */
/* Interface                                                                */
/* ------------------------------------------------------------------------ */
/* These kernels can be used for two tensors when the sizes match exactly.  */
/* The strides for each dimension can differ. The apply elemwise3 launch    */
/* macros for GPU assume that the following variables have been defined:    */
/*   int        ndims;                                                      */
/*   size_t    *size;                                                       */
/*   ptrdiff_t *strides1;                                                   */
/*   ptrdiff_t *strides2;                                                   */
/*   ptrdiff_t *strides3;                                                   */
/*   void      *ptr1;                                                       */
/*   void      *ptr2;                                                       */
/*   void      *ptr3;                                                       */
/*   int        result;             // Updated to reflect success / failure */
/* ------------------------------------------------------------------------ */
/* Exposed variables in kernel:                                             */
/*   solid_c_type(TYPE1) *_ptr1                                             */
/*   solid_c_type(TYPE2) *_ptr2                                             */
/*   solid_c_type(TYPE3) *_ptr3                                             */
/* ------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------ */
/* Tensor information structure                                             */
/* ------------------------------------------------------------------------ */

#define SOLID_ELEMWISE3_DATA_VAR_SMALL  __elemwise3_data_small
#define SOLID_ELEMWISE3_DATA_VAR_LARGE  __elemwise3_data_large
#define SOLID_ELEMWISE3_DATA_VAR(SIZE)  SOLID_ELEMWISE3_DATA_VAR_## SIZE

#define SOLID_ELEMWISE3_DATA_SMALL      solid_elemwise3_data_small
#define SOLID_ELEMWISE3_DATA_LARGE      solid_elemwise3_data_large
#define SOLID_ELEMWISE3_DATA(SIZE)      SOLID_ELEMWISE3_DATA_##SIZE

typedef struct
{  unsigned int  size[SOLID_MAX_TENSOR_DIMS];
   int           strides1[SOLID_MAX_TENSOR_DIMS];
   int           strides2[SOLID_MAX_TENSOR_DIMS];
   int           strides3[SOLID_MAX_TENSOR_DIMS];
   char         *ptr1;
   char         *ptr2;
   char         *ptr3;
   size_t        nelem;
   int           ndims;
} solid_elemwise3_data_small;

typedef struct
{  size_t        size[SOLID_MAX_TENSOR_DIMS];
   ptrdiff_t     strides1[SOLID_MAX_TENSOR_DIMS];
   ptrdiff_t     strides2[SOLID_MAX_TENSOR_DIMS];
   ptrdiff_t     strides3[SOLID_MAX_TENSOR_DIMS];
   char         *ptr1;
   char         *ptr2;
   char         *ptr3;
   size_t        nelem;
   int           ndims;
} solid_elemwise3_data_large;


/* ------------------------------------------------------------------------ */
/* Analysis function declaration                                            */
/* ------------------------------------------------------------------------ */
SOLID_API int solid_gpu_elemwise3_analyze(int ndims, const size_t *size,
                                          const ptrdiff_t *strides1, void *ptr1,
                                          const ptrdiff_t *strides2, void *ptr2,
                                          const ptrdiff_t *strides3, void *ptr3,
                                          int elemsize1, int elemsize2,
                                          int elemsize3, int flag_unroll, 
                                          solid_elemwise3_data_small *data_small,
                                          solid_elemwise3_data_large *data_large,
                                          solid_gpu_config *config, int *kernel_index);


/* --------------------------------------------------------------------- */
/* Name macros                                                           */
/* --------------------------------------------------------------------- */

#define SOLID_KERNEL_ELEMWISE3_SMALL(PREFIX, NDIMS, UNROLL) \
   PREFIX##_kernel_##NDIMS##_##UNROLL 
#define SOLID_KERNEL_ELEMWISE3_LARGE(PREFIX, NDIMS, UNROLL) \
   PREFIX##_kernel_##NDIMS##_##UNROLL##L
#define SOLID_KERNEL_ELEMWISE3_NAME(PREFIX, NDIMS, UNROLL, SIZE) \
   SOLID_KERNEL_ELEMWISE3_##SIZE(PREFIX, NDIMS, UNROLL)

#define SOLID_KERNEL_ELEMWISE3_ITF_0(PREFIX, NDIMS, UNROLL, SIZE) \
   SOLID_KERNEL_ELEMWISE3_NAME(PREFIX, NDIMS, UNROLL, SIZE)\
                               (SOLID_ELEMWISE3_DATA(SIZE) data)
#define SOLID_KERNEL_ELEMWISE3_ITF_1(PREFIX, NDIMS, UNROLL, SIZE) \
   SOLID_KERNEL_ELEMWISE3_NAME(PREFIX, NDIMS, UNROLL, SIZE)\
                               (SOLID_ELEMWISE3_DATA(SIZE) data, SOLID_KERNEL_PARAM_PREFIX(PREFIX) param)
#define SOLID_KERNEL_ELEMWISE3_ITF(PREFIX, FLAG_PARAM, NDIMS, UNROLL, SIZE) \
   SOLID_KERNEL_ELEMWISE3_ITF_##FLAG_PARAM(PREFIX, NDIMS, UNROLL, SIZE)


/* ------------------------------------------------------------------------ */
/* Create all cuda kernels                                                  */
/* ------------------------------------------------------------------------ */

#define SOLID_CREATE_KERNELS_ELEMWISE3_0(PREFIX, FLAG_PARAM, DTYPE1, DTYPE2, DTYPE3, CODE) \
   SOLID_CREATE_KERNEL_ELEMWISE3(PREFIX, DTYPE1, DTYPE2, DTYPE3, FLAG_PARAM, CODE, 1, 1, SMALL, 512) \
   SOLID_CREATE_KERNEL_ELEMWISE3(PREFIX, DTYPE1, DTYPE2, DTYPE3, FLAG_PARAM, CODE, 2, 1, SMALL, 512) \
   SOLID_CREATE_KERNEL_ELEMWISE3(PREFIX, DTYPE1, DTYPE2, DTYPE3, FLAG_PARAM, CODE, 3, 1, SMALL, 512) \
   SOLID_CREATE_KERNEL_ELEMWISE3(PREFIX, DTYPE1, DTYPE2, DTYPE3, FLAG_PARAM, CODE, N, 1, SMALL, 512) \
   \
   SOLID_CREATE_KERNEL_ELEMWISE3(PREFIX, DTYPE1, DTYPE2, DTYPE3, FLAG_PARAM, CODE, 1, 1, LARGE, 512) \
   SOLID_CREATE_KERNEL_ELEMWISE3(PREFIX, DTYPE1, DTYPE2, DTYPE3, FLAG_PARAM, CODE, 2, 1, LARGE, 512) \
   SOLID_CREATE_KERNEL_ELEMWISE3(PREFIX, DTYPE1, DTYPE2, DTYPE3, FLAG_PARAM, CODE, 3, 1, LARGE, 512) \
   SOLID_CREATE_KERNEL_ELEMWISE3(PREFIX, DTYPE1, DTYPE2, DTYPE3, FLAG_PARAM, CODE, N, 1, LARGE, 512)

#define SOLID_CREATE_KERNELS_ELEMWISE3_1(PREFIX, FLAG_PARAM, DTYPE1, DTYPE2, DTYPE3, CODE) \
   /* Create all regular kernels */ \
   SOLID_CREATE_KERNELS_ELEMWISE3_0(PREFIX, FLAG_PARAM, DTYPE1, DTYPE2, DTYPE3, CODE) \
   \
   /* Create all unrolled kernels */ \
   SOLID_CREATE_KERNEL_ELEMWISE3(PREFIX, DTYPE1, DTYPE2, DTYPE3, FLAG_PARAM, CODE, 2, 2, LARGE, 512) \
   SOLID_CREATE_KERNEL_ELEMWISE3(PREFIX, DTYPE1, DTYPE2, DTYPE3, FLAG_PARAM, CODE, 3, 2, LARGE, 512) \
   SOLID_CREATE_KERNEL_ELEMWISE3(PREFIX, DTYPE1, DTYPE2, DTYPE3, FLAG_PARAM, CODE, N, 2, LARGE, 512) \
   SOLID_CREATE_KERNEL_ELEMWISE3(PREFIX, DTYPE1, DTYPE2, DTYPE3, FLAG_PARAM, CODE, N, 4, LARGE, 512)

#define SOLID_CREATE_KERNELS_ELEMWISE3(PREFIX, FLAG_UNROLLED, FLAG_PARAM, DTYPE1, DTYPE2, DTYPE3, CODE) \
   SOLID_CREATE_KERNELS_ELEMWISE3_## FLAG_UNROLLED(PREFIX, FLAG_PARAM, DTYPE1, DTYPE2, DTYPE3, CODE)


/* Create types and kernels */
#define SOLID_KERNELS_ELEMWISE3_PREFIX_B(PREFIX, FLAG_UNROLLED, DTYPE1, DTYPE2, DTYPE3, FLAG_PARAM, PARAM, CODE) \
   SOLID_CREATE_KERNEL_TYPES(PREFIX, FLAG_PARAM, PARAM) \
   SOLID_CREATE_KERNELS_ELEMWISE3(PREFIX, FLAG_UNROLLED, FLAG_PARAM, DTYPE1, DTYPE2, DTYPE3, CODE)

#define SOLID_KERNELS_ELEMWISE3_PREFIX(PREFIX, FLAG_UNROLLED, DTYPE1, DTYPE2, DTYPE3, FLAG_PARAM, PARAM, CODE) \
   SOLID_KERNELS_ELEMWISE3_PREFIX_B(PREFIX, FLAG_UNROLLED, DTYPE1, DTYPE2, DTYPE3, FLAG_PARAM, PARAM, CODE)


/* Main interfaces */
#define SOLID_KERNELS_ELEMWISE3_FULL(DTYPE1, DTYPE2, DTYPE3, NAME_TYPE, UNROLLING, NAME, PARAM, CODE) \
   SOLID_KERNELS_ELEMWISE3_PREFIX(SOLID_FUNCTION3_TYPES(NAME, NAME_TYPE, DTYPE1, DTYPE2, DTYPE3), \
                                  SOLID_FLAG_UNROLLED(UNROLLING), DTYPE1, DTYPE2, DTYPE3, \
                                  1, PARAM, CODE)

#define SOLID_KERNELS_ELEMWISE3_PARAM(UNROLLING, NAME, PARAM, CODE) \
   SOLID_KERNELS_ELEMWISE3_FULL(SDXTYPE, SDXTYPE, SDXTYPE, 1, UNROLLING, NAME, PARAM, CODE)

#define SOLID_KERNELS_ELEMWISE3_TYPES(DTYPE1, DTYPE2, DTYPE3, NAME_TYPE, UNROLLING, NAME, CODE) \
   SOLID_KERNELS_ELEMWISE3_PREFIX(SOLID_FUNCTION3_TYPES(NAME, NAME_TYPE, DTYPE1, DTYPE2, DTYPE3), \
                                  SOLID_FLAG_UNROLLED(UNROLLING), DTYPE1, DTYPE2, DTYPE3, \
                                  0, { }, CODE)

#define SOLID_KERNELS_ELEMWISE3(UNROLLING, NAME, CODE) \
   SOLID_KERNELS_ELEMWISE3_TYPES(SDXTYPE, SDXTYPE, SDXTYPE, 1, UNROLLING, NAME, CODE)


/* ------------------------------------------------------------------------ */
/* Launching the kernels                                                    */
/* ------------------------------------------------------------------------ */

#define SOLID_SUBMIT_ELEMWISE3_PARAM_0(PARAM, SIZE) \
      (SOLID_ELEMWISE3_DATA_VAR(SIZE))
#define SOLID_SUBMIT_ELEMWISE3_PARAM_1(PARAM, SIZE) \
      (SOLID_ELEMWISE3_DATA_VAR(SIZE), PARAM)
#define SOLID_SUBMIT_ELEMWISE3_PARAM(FLAG_PARAM, PARAM, SIZE) \
      SOLID_SUBMIT_ELEMWISE3_PARAM_## FLAG_PARAM(PARAM, SIZE)

#define SOLID_SUBMIT_ELEMWISE3(PREFIX, CONFIG, SHAREDMEM, STREAM, FLAG_PARAM, PARAM, NDIMS, UNROLL, SIZE) \
      SOLID_KERNEL_ELEMWISE3_NAME(PREFIX, NDIMS, UNROLL, SIZE)\
      <<<(CONFIG)->blocks,(CONFIG)->threads,SHAREDMEM,STREAM>>>\
      SOLID_SUBMIT_ELEMWISE3_PARAM(FLAG_PARAM, PARAM, SIZE)

#define SOLID_LAUNCH_ELEMWISE3_CASES_0(PREFIX, CONFIG, SHAREDMEM, STREAM, FLAG_PARAM, PARAM) \
      case 0 : SOLID_SUBMIT_ELEMWISE3(PREFIX, CONFIG, SHAREDMEM, STREAM, FLAG_PARAM, PARAM, 1, 1, SMALL); break; \
      case 1 : SOLID_SUBMIT_ELEMWISE3(PREFIX, CONFIG, SHAREDMEM, STREAM, FLAG_PARAM, PARAM, 2, 1, SMALL); break; \
      case 2 : SOLID_SUBMIT_ELEMWISE3(PREFIX, CONFIG, SHAREDMEM, STREAM, FLAG_PARAM, PARAM, 3, 1, SMALL); break; \
      case 3 : SOLID_SUBMIT_ELEMWISE3(PREFIX, CONFIG, SHAREDMEM, STREAM, FLAG_PARAM, PARAM, N, 1, SMALL); break; \
      case 4 : SOLID_SUBMIT_ELEMWISE3(PREFIX, CONFIG, SHAREDMEM, STREAM, FLAG_PARAM, PARAM, 1, 1, LARGE); break; \
      case 5 : SOLID_SUBMIT_ELEMWISE3(PREFIX, CONFIG, SHAREDMEM, STREAM, FLAG_PARAM, PARAM, 2, 1, LARGE); break; \
      case 6 : SOLID_SUBMIT_ELEMWISE3(PREFIX, CONFIG, SHAREDMEM, STREAM, FLAG_PARAM, PARAM, 3, 1, LARGE); break; \
      case 7 : SOLID_SUBMIT_ELEMWISE3(PREFIX, CONFIG, SHAREDMEM, STREAM, FLAG_PARAM, PARAM, N, 1, LARGE); break;

#define SOLID_LAUNCH_ELEMWISE3_CASES_1(PREFIX, CONFIG, SHAREDMEM, STREAM, FLAG_PARAM, PARAM) \
      SOLID_LAUNCH_ELEMWISE3_CASES_0(PREFIX, CONFIG, SHAREDMEM, STREAM, FLAG_PARAM, PARAM) \
      case 8 : SOLID_SUBMIT_ELEMWISE3(PREFIX, CONFIG, SHAREDMEM, STREAM, FLAG_PARAM, PARAM, 2, 2, LARGE); break; \
      case 9 : SOLID_SUBMIT_ELEMWISE3(PREFIX, CONFIG, SHAREDMEM, STREAM, FLAG_PARAM, PARAM, 3, 2, LARGE); break; \
      case 10: SOLID_SUBMIT_ELEMWISE3(PREFIX, CONFIG, SHAREDMEM, STREAM, FLAG_PARAM, PARAM, N, 2, LARGE); break; \
      case 11: SOLID_SUBMIT_ELEMWISE3(PREFIX, CONFIG, SHAREDMEM, STREAM, FLAG_PARAM, PARAM, N, 4, LARGE); break;

#define SOLID_LAUNCH_ELEMWISE3_CASES(PREFIX, FLAG_UNROLLED, CONFIG, SHAREDMEM, STREAM, FLAG_PARAM, PARAM) \
      SOLID_LAUNCH_ELEMWISE3_CASES_##FLAG_UNROLLED(PREFIX, CONFIG, SHAREDMEM, STREAM, FLAG_PARAM, PARAM)

#define SOLID_LAUNCH_ELEMWISE3_CODE(PREFIX, FLAG_UNROLLED, DTYPE1, DTYPE2, DTYPE3, \
                                    SHAREDMEM, STREAM, FLAG_PARAM, PARAM, RESULT) \
   {  SOLID_ELEMWISE3_DATA_SMALL SOLID_ELEMWISE3_DATA_VAR_SMALL; \
      SOLID_ELEMWISE3_DATA_LARGE SOLID_ELEMWISE3_DATA_VAR_LARGE; \
      solid_gpu_config           __elemwise3_kernel_config; \
      int                        __elemwise3_kernel_index; \
      cudaError_t                __cuda_status; \
      \
      /* Initialize the data and determine which kernel to use */ \
      (RESULT) = solid_gpu_elemwise3_analyze(ndims, size, strides1, ptr1, strides2, ptr2, strides3, ptr3, \
                                             sizeof(SOLID_C_TYPE(DTYPE1)), \
                                             sizeof(SOLID_C_TYPE(DTYPE2)), \
                                             sizeof(SOLID_C_TYPE(DTYPE3)), FLAG_UNROLLED, \
                                             &(SOLID_ELEMWISE3_DATA_VAR_SMALL), \
                                             &(SOLID_ELEMWISE3_DATA_VAR_LARGE), \
                                             &(__elemwise3_kernel_config), \
                                             &(__elemwise3_kernel_index)); \
      \
      /* Call the appropriate kernel */ \
      if (((RESULT) == 0) && (SOLID_ELEMWISE3_DATA_VAR_SMALL.nelem > 0)) \
      {  switch (__elemwise3_kernel_index) \
         {  SOLID_LAUNCH_ELEMWISE3_CASES(PREFIX, FLAG_UNROLLED, &(__elemwise3_kernel_config), \
                                         SHAREDMEM, STREAM, FLAG_PARAM, PARAM) \
            default : SOLID_ERROR_MESSAGE("Unrecognized kernel index (%d)", __elemwise3_kernel_index); \
         } \
         if ((__cuda_status = cudaGetLastError()) != cudaSuccess) \
         {  SOLID_ERROR_MESSAGE("Cuda error: %s", cudaGetErrorString(__cuda_status)); \
            (RESULT) = -1; \
         } \
      } \
   }

#define SOLID_LAUNCH_ELEMWISE3_PREFIX_B(PREFIX, FLAG_UNROLLED, DTYPE1, DTYPE2, DTYPE3, \
                                        SHAREDMEM, STREAM, FLAG_PARAM, PARAM, RESULT) \
   SOLID_LAUNCH_ELEMWISE3_CODE(PREFIX, FLAG_UNROLLED, DTYPE1, DTYPE2, DTYPE3, \
                               SHAREDMEM, STREAM, FLAG_PARAM, PARAM, RESULT)

#define SOLID_LAUNCH_ELEMWISE3_PREFIX(PREFIX, FLAG_UNROLLED, DTYPE1, DTYPE2, DTYPE3, \
                                      SHAREDMEM, STREAM, FLAG_PARAM, PARAM, RESULT) \
   SOLID_LAUNCH_ELEMWISE3_PREFIX_B(PREFIX, FLAG_UNROLLED, DTYPE1, DTYPE2, DTYPE3, \
                                   SHAREDMEM, STREAM, FLAG_PARAM, PARAM, RESULT)

/* Main interfaces */
#define SOLID_LAUNCH_ELEMWISE3_FULL(DTYPE1, DTYPE2, DTYPE3, NAME_TYPE, UNROLLING, NAME, \
                                    SHAREDMEM, STREAM, PARAM, RESULT) \
   SOLID_LAUNCH_ELEMWISE3_PREFIX(SOLID_FUNCTION3_TYPES(NAME, NAME_TYPE, DTYPE1, DTYPE2, DTYPE3), \
                                 SOLID_FLAG_UNROLLED(UNROLLING), DTYPE1, DTYPE2, DTYPE3, \
                                 SHAREDMEM, STREAM, 1, PARAM, RESULT)

#define SOLID_LAUNCH_ELEMWISE3_PARAM(UNROLLING, NAME, SHAREDMEM, STREAM, PARAM, RESULT) \
   SOLID_LAUNCH_ELEMWISE3_FULL(SDXTYPE, SDXTYPE, SDXTYPE, 1, UNROLLING, NAME, SHAREDMEM, STREAM, PARAM, RESULT)

#define SOLID_LAUNCH_ELEMWISE3_TYPES(DTYPE1, DTYPE2, DTYPE3, NAME_TYPE, UNROLLING, NAME, \
                                     SHAREDMEM, STREAM, RESULT) \
   SOLID_LAUNCH_ELEMWISE3_PREFIX(SOLID_FUNCTION3_TYPES(NAME, NAME_TYPE, DTYPE1, DTYPE2, DTYPE3), \
                                 SOLID_FLAG_UNROLLED(UNROLLING), DTYPE1, DTYPE2, DTYPE3, \
                                 SHAREDMEM, STREAM, 0, NULL, RESULT)

#define SOLID_LAUNCH_ELEMWISE3(UNROLLING, NAME, SHAREDMEM, STREAM, RESULT) \
   SOLID_LAUNCH_ELEMWISE3_TYPES(SDXTYPE, SDXTYPE, SDXTYPE, 1, UNROLLING, NAME, SHAREDMEM, STREAM, RESULT)


/* --------------------------------------------------------------------- */
/* KERNEL OFFSET COMPUTATION                                             */
/* --------------------------------------------------------------------- */
#define SOLID_ELEMWISE3_OFFSET_1(SIZE) \
   _offset1 = _idx * data.strides1[0]; \
   _offset2 = _idx * data.strides2[0]; \
   _offset3 = _idx * data.strides3[0];

#define SOLID_ELEMWISE3_OFFSET_2(SIZE) \
   {  SOLID_ELEMWISE_INDEX_TYPE(SIZE) _index; \
      SOLID_ELEMWISE_INDEX_TYPE(SIZE) _s; \
      \
      _s        = _idx % data.size[0];       \
      _index    = _idx / data.size[0];       \
      _offset1  = _s * data.strides1[0];     \
      _offset1 += _index * data.strides1[1]; \
      _offset2  = _s * data.strides2[0];     \
      _offset2 += _index * data.strides2[1]; \
      _offset3  = _s * data.strides3[0];     \
      _offset3 += _index * data.strides3[1]; \
   }

#define SOLID_ELEMWISE3_OFFSET_3(SIZE) \
   {  SOLID_ELEMWISE_INDEX_TYPE(SIZE) _index; \
      SOLID_ELEMWISE_INDEX_TYPE(SIZE) _s; \
      \
      _s        = _idx % data.size[0];       \
      _index    = _idx / data.size[0];       \
      _offset1  = _s * data.strides1[0];     \
      _offset2  = _s * data.strides2[0];     \
      _offset3  = _s * data.strides3[0];     \
      _s        = _index % data.size[1];     \
      _index    = _index / data.size[1];     \
      _offset1 += _s * data.strides1[1];     \
      _offset2 += _s * data.strides2[1];     \
      _offset3 += _s * data.strides3[1];     \
      _offset1 += _index * data.strides1[2]; \
      _offset2 += _index * data.strides2[2]; \
      _offset3 += _index * data.strides3[2]; \
   }

#define SOLID_ELEMWISE3_OFFSET_N(SIZE) \
   {  SOLID_ELEMWISE_INDEX_TYPE(SIZE) _index; \
      SOLID_ELEMWISE_INDEX_TYPE(SIZE) _s; \
      int _i; \
      \
      _index    = _idx; \
      _offset1  = 0; \
      _offset2  = 0; \
      _offset3  = 0; \
      for (_i = 0; _i < data.ndims-1; _i++) \
      {  _s        = _index % data.size[_i]; \
         _index    = _index / data.size[_i]; \
         _offset1 += _s * data.strides1[_i]; \
         _offset2 += _s * data.strides2[_i]; \
         _offset3 += _s * data.strides3[_i]; \
      } \
      _offset1 += _index * data.strides1[data.ndims-1]; \
      _offset2 += _index * data.strides2[data.ndims-1]; \
      _offset3 += _index * data.strides3[data.ndims-1]; \
   }

#define SOLID_ELEMWISE3_OFFSET_B(NDIMS, SIZE) \
           SOLID_ELEMWISE3_OFFSET_##NDIMS(SIZE)
#define SOLID_ELEMWISE3_OFFSET(NDIMS, SIZE) \
           SOLID_ELEMWISE3_OFFSET_B(NDIMS, SIZE)


/* --------------------------------------------------------------------- */
/* KERNEL CODE UNROLLING                                                 */
/* --------------------------------------------------------------------- */

#define SOLID_ELEMWISE3_CODE_C1(DTYPE1, DTYPE2, DTYPE3, CODE) \
   /* Determine the pointers */ \
   _ptr1 = (SOLID_C_TYPE(DTYPE1) *)(data.ptr1 + _offset1); \
   _ptr2 = (SOLID_C_TYPE(DTYPE2) *)(data.ptr2 + _offset2); \
   _ptr3 = (SOLID_C_TYPE(DTYPE3) *)(data.ptr3 + _offset3); \
   \
   /* CODE */ \
   CODE

#define SOLID_ELEMWISE3_CODE_C2(DTYPE1, DTYPE2, DTYPE3, CODE) \
   SOLID_ELEMWISE3_CODE_C1(DTYPE1, DTYPE2, DTYPE3, CODE) \
   \
   /* Next element */ \
   _offset1 += data.strides1[0]; \
   _offset2 += data.strides2[0]; \
   _offset3 += data.strides3[0]; \
   \
   SOLID_ELEMWISE3_CODE_C1(DTYPE1, DTYPE2, DTYPE3, CODE)

#define SOLID_ELEMWISE3_CODE_C4(DTYPE1, DTYPE2, DTYPE3, CODE) \
   SOLID_ELEMWISE3_CODE_C2(DTYPE1, DTYPE2, DTYPE3, CODE) \
   \
   /* Next element */ \
   _offset1 += data.strides1[0]; \
   _offset2 += data.strides2[0]; \
   _offset3 += data.strides3[0]; \
   \
   SOLID_ELEMWISE3_CODE_C2(DTYPE1, DTYPE2, DTYPE3, CODE)

#define SOLID_ELEMWISE3_CODE_B(DTYPE1, DTYPE2, DTYPE3, UNROLL, CODE) \
   SOLID_ELEMWISE3_CODE_C##UNROLL(DTYPE1, DTYPE2, DTYPE3, CODE)
#define SOLID_ELEMWISE3_CODE(DTYPE1, DTYPE2, DTYPE3, UNROLL, CODE) \
   SOLID_ELEMWISE3_CODE_B(DTYPE1, DTYPE2, DTYPE3, UNROLL, CODE)


/* --------------------------------------------------------------------- */
/* KERNELS                                                               */
/* --------------------------------------------------------------------- */

#define SOLID_CREATE_KERNEL_ELEMWISE3(PREFIX, DTYPE1, DTYPE2, DTYPE3, FLAG_PARAM, CODE, NDIMS, UNROLL, SIZE, BOUNDS) \
   __launch_bounds__(BOUNDS) \
   __global__ void SOLID_KERNEL_ELEMWISE3_ITF(PREFIX, FLAG_PARAM, NDIMS, UNROLL, SIZE) \
   {  SOLID_ELEMWISE_SIZE_TYPE(SIZE)  _idx; \
      SOLID_ELEMWISE_INDEX_TYPE(SIZE) _offset1; \
      SOLID_ELEMWISE_INDEX_TYPE(SIZE) _offset2; \
      SOLID_ELEMWISE_INDEX_TYPE(SIZE) _offset3; \
      SOLID_C_TYPE(DTYPE1)            *_ptr1; \
      SOLID_C_TYPE(DTYPE2)            *_ptr2; \
      SOLID_C_TYPE(DTYPE3)            *_ptr3; \
      \
      for (_idx = (UNROLL) * (blockIdx.x * blockDim.x + threadIdx.x); \
           _idx < data.nelem; \
           _idx += (UNROLL) * (gridDim.x * blockDim.x)) \
      { \
         /* Determine the offsets */ \
         SOLID_ELEMWISE3_OFFSET(NDIMS, SIZE) \
         \
         /* Expand the code */ \
         SOLID_ELEMWISE3_CODE(DTYPE1, DTYPE2, DTYPE3, UNROLL, CODE) \
      } \
   }

#endif
