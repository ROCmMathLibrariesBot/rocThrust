/*
 *  Copyright 2008-2013 NVIDIA Corporation
 *  Modifications Copyright© 2019 Advanced Micro Devices, Inc. All rights reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include <limits>
#include <thrust/detail/config.h>
#include <thrust/system/hip/detail/malloc_and_free.h>
#include <thrust/system/hip/memory.h>

namespace thrust
{

// Not needed for HIP
// // XXX WAR an issue with MSVC 2005 (cl v14.00) incorrectly implementing
// //     pointer_raw_pointer for pointer by specializing it here
// //     note that we specialize it here, before the use of raw_pointer_cast
// //     below, which causes pointer_raw_pointer's instantiation
// #if (THRUST_HOST_COMPILER == THRUST_HOST_COMPILER_MSVC) && (_MSC_VER <= 1400)
// namespace detail
// {

// template<typename T>
//   struct pointer_raw_pointer< thrust::hip_rocprim::pointer<T> >
// {
//   typedef typename thrust::hip_rocprim::pointer<T>::raw_pointer type;
// }; // end pointer_raw_pointer

// } // end detail
// #endif

namespace hip_rocprim
{

template <typename T>
template <typename OtherT>
THRUST_HIP_FUNCTION reference<T>& reference<T>::operator=(const reference<OtherT>& other)
{
    return super_t::operator=(other);
} // end reference::operator=()

template <typename T>
THRUST_HIP_FUNCTION reference<T>& reference<T>::operator=(const value_type& x)
{
    return super_t::operator=(x);
} // end reference::operator=()

template <typename T>
THRUST_HIP_FUNCTION void swap(reference<T> a, reference<T> b)
{
    a.swap(b);
} // end swap()

THRUST_HIP_FUNCTION pointer<void> malloc(std::size_t n)
{
    tag hip_tag;
    return pointer<void>(thrust::hip_rocprim::malloc(hip_tag, n));
} // end malloc()

template <typename T>
THRUST_HIP_FUNCTION pointer<T> malloc(std::size_t n)
{
    pointer<void> raw_ptr = thrust::hip_rocprim::malloc(sizeof(T) * n);
    return pointer<T>(reinterpret_cast<T*>(raw_ptr.get()));
} // end malloc()

THRUST_HIP_FUNCTION void free(pointer<void> ptr)
{
    tag hip_tag;
    return thrust::hip_rocprim::free(hip_tag, ptr.get());
} // end free()

} // end hip_rocprim
} // end thrust
