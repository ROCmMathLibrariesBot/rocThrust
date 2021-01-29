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

#include <thrust/iterator/zip_iterator.h>
#include <thrust/scan.h>

#include "test_header.hpp"

TESTS_DEFINE(ZipIteratorScanVariablesTests, NumericalTestsParams);

template <typename Tuple>
struct TuplePlus
{
    __host__ __device__ Tuple operator()(Tuple x, Tuple y) const
    {
        using namespace thrust;
        return make_tuple(get<0>(x) + get<0>(y), get<1>(x) + get<1>(y));
    }
}; // end SumTuple

TYPED_TEST(ZipIteratorScanVariablesTests, TestZipIteratorScan)
{
    using T = typename TestFixture::input_type;

    SCOPED_TRACE(testing::Message() << "with device_id= " << test::set_device_from_ctest());

    for(auto size : get_sizes())
    {
        SCOPED_TRACE(testing::Message() << "with size= " << size);

        for(auto seed : get_seeds())
        {
            SCOPED_TRACE(testing::Message() << "with seed= " << seed);

            thrust::host_vector<T> h_data0 = get_random_data<T>(
                size, std::numeric_limits<T>::min(), std::numeric_limits<T>::max(), seed);
            thrust::host_vector<T> h_data1 = get_random_data<T>(
                size,
                std::numeric_limits<T>::min(),
                std::numeric_limits<T>::max(),
                seed + seed_value_addition
            );

            thrust::device_vector<T> d_data0 = h_data0;
            thrust::device_vector<T> d_data1 = h_data1;

            typedef thrust::tuple<T, T> Tuple;

            thrust::host_vector<Tuple>   h_result(size);
            thrust::device_vector<Tuple> d_result(size);

            // inclusive_scan (tuple output)
            thrust::inclusive_scan(
                thrust::make_zip_iterator(thrust::make_tuple(h_data0.begin(), h_data1.begin())),
                thrust::make_zip_iterator(thrust::make_tuple(h_data0.end(), h_data1.end())),
                h_result.begin(),
                TuplePlus<Tuple>());
            thrust::inclusive_scan(
                thrust::make_zip_iterator(thrust::make_tuple(d_data0.begin(), d_data1.begin())),
                thrust::make_zip_iterator(thrust::make_tuple(d_data0.end(), d_data1.end())),
                d_result.begin(),
                TuplePlus<Tuple>());
            thrust::host_vector<Tuple> h_result_d(d_result);
            for(size_t index = 0; index < h_result.size(); index++)
            {
              Tuple host_tuple = h_result[index];
              Tuple device_tuple = h_result_d[index];
              ASSERT_NEAR(thrust::get<0>(host_tuple),thrust::get<0>(device_tuple),std::abs(thrust::get<0>(host_tuple)*0.001));
            }

            // exclusive_scan (tuple output)
            thrust::exclusive_scan(
                thrust::make_zip_iterator(thrust::make_tuple(h_data0.begin(), h_data1.begin())),
                thrust::make_zip_iterator(thrust::make_tuple(h_data0.end(), h_data1.end())),
                h_result.begin(),
                thrust::make_tuple<T, T>(0, 0),
            TuplePlus<Tuple>());
            thrust::exclusive_scan(
                thrust::make_zip_iterator(thrust::make_tuple(d_data0.begin(), d_data1.begin())),
                thrust::make_zip_iterator(thrust::make_tuple(d_data0.end(), d_data1.end())),
                d_result.begin(),
                thrust::make_tuple<T, T>(0, 0),
                TuplePlus<Tuple>());

            h_result_d = d_result;
            for(size_t index = 0; index < h_result.size(); index++)
            {
              Tuple host_tuple = h_result[index];
              Tuple device_tuple = h_result_d[index];
              ASSERT_NEAR(thrust::get<0>(host_tuple),thrust::get<0>(device_tuple),std::abs(thrust::get<0>(host_tuple)*0.001));
            }

            thrust::host_vector<T>   h_result0(size);
            thrust::host_vector<T>   h_result1(size);
            thrust::device_vector<T> d_result0(size);
            thrust::device_vector<T> d_result1(size);

            // inclusive_scan (zip_iterator output)
            thrust::inclusive_scan(
                thrust::make_zip_iterator(thrust::make_tuple(h_data0.begin(), h_data1.begin())),
                thrust::make_zip_iterator(thrust::make_tuple(h_data0.end(), h_data1.end())),
                thrust::make_zip_iterator(thrust::make_tuple(h_result0.begin(), h_result1.begin())),
                TuplePlus<Tuple>());
            thrust::inclusive_scan(
                thrust::make_zip_iterator(thrust::make_tuple(d_data0.begin(), d_data1.begin())),
                thrust::make_zip_iterator(thrust::make_tuple(d_data0.end(), d_data1.end())),
                thrust::make_zip_iterator(thrust::make_tuple(d_result0.begin(), d_result1.begin())),
                TuplePlus<Tuple>());
            h_result_d = d_result0;
            for(size_t index = 0; index < h_result.size(); index++)
            {
              Tuple host_tuple = h_result0[index];
              Tuple device_tuple = h_result_d[index];
              ASSERT_NEAR(thrust::get<0>(host_tuple),thrust::get<0>(device_tuple),std::abs(thrust::get<0>(host_tuple)*0.001));
            }

            h_result_d = d_result1;
            for(size_t index = 0; index < h_result.size(); index++)
            {
              Tuple host_tuple = h_result1[index];
              Tuple device_tuple = h_result_d[index];
              ASSERT_NEAR(thrust::get<0>(host_tuple),thrust::get<0>(device_tuple),std::abs(thrust::get<0>(host_tuple)*0.001));
            }

            // exclusive_scan (zip_iterator output)
            thrust::exclusive_scan(
                thrust::make_zip_iterator(thrust::make_tuple(h_data0.begin(), h_data1.begin())),
                thrust::make_zip_iterator(thrust::make_tuple(h_data0.end(), h_data1.end())),
                thrust::make_zip_iterator(thrust::make_tuple(h_result0.begin(), h_result1.begin())),
                thrust::make_tuple<T, T>(0, 0),
                TuplePlus<Tuple>());
            thrust::exclusive_scan(
                thrust::make_zip_iterator(thrust::make_tuple(d_data0.begin(), d_data1.begin())),
                thrust::make_zip_iterator(thrust::make_tuple(d_data0.end(), d_data1.end())),
                thrust::make_zip_iterator(thrust::make_tuple(d_result0.begin(), d_result1.begin())),
                thrust::make_tuple<T, T>(0, 0),
                TuplePlus<Tuple>());

            h_result_d = d_result0;
            for(size_t index = 0; index < h_result.size(); index++)
            {
              Tuple host_tuple = h_result0[index];
              Tuple device_tuple = h_result_d[index];
              ASSERT_NEAR(thrust::get<0>(host_tuple),thrust::get<0>(device_tuple),std::abs(thrust::get<0>(host_tuple)*0.001));
            }

            h_result_d = d_result1;
            for(size_t index = 0; index < h_result.size(); index++)
            {
              Tuple host_tuple = h_result1[index];
              Tuple device_tuple = h_result_d[index];
              ASSERT_NEAR(thrust::get<0>(host_tuple),thrust::get<0>(device_tuple),std::abs(thrust::get<0>(host_tuple)*0.001));
            }
        }
    }
}
