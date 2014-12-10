#ifndef __DALI_SLP_PLATFORM_DATA_COMPRESSION_H__
#define __DALI_SLP_PLATFORM_DATA_COMPRESSION_H__

/*
Copyright (c) 2000-2012 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali Adaptor

PROPRIETARY/CONFIDENTIAL

This software is the confidential and proprietary information of
SAMSUNG ELECTRONICS ("Confidential Information"). You shall not
disclose such Confidential Information and shall use it only in
accordance with the terms of the license agreement you entered
into with SAMSUNG ELECTRONICS.

SAMSUNG make no representations or warranties about the suitability
of the software, either express or implied, including but not limited
to the implied warranties of merchantability, fitness for a particular
purpose, or non-infringement. SAMSUNG shall not be liable for any
damages suffered by licensee as a result of using, modifying or
distributing this software or its derivatives.
*/


namespace Dali
{
namespace SlpPlatform
{
namespace DataCompression
{

/**
 * Return the maximum size of a buffer required to hold a number of bytes.
 * Required because compressing data, can end up being bigger
 * than the original data.
 * @return maximum buffer size required to compress inputSize
 */
std::size_t GetMaximumRleCompressedSize(const std::size_t inputSize);

/**
 * RLE Encodes an array of data
 * @param input input data
 * @param inputLength input data length in bytes
 * @param output output data, pre-allocated by caller
 * @param outputLength size of the output buffer in bytes
 * @param encodedSize number of bytes written to outputBuffer
 */
void EncodeRle(  const unsigned char* input,
                 const std::size_t inputLength,
                 unsigned char* output,
                 const std::size_t outputLength,
                 std::size_t &encodedSize );

/**
 * RLE Decodes an array of data.
 * @param[in] input input data
 * @param[in] inputLength input data length in bytes
 * @param[out] output output data, pre-allocated by caller, allocated using GetMaximumRleCompressedSize()
 * @param[in] outputLength size of output data in bytes
 * @param[out] decodedSize the number of bytes decoded into outputBuffer
 */
bool DecodeRle( const unsigned char* input,
                const std::size_t inputLength,
                unsigned char* output,
                const std::size_t outputLength,
               std::size_t& decodedSize);


} // namespace DataCompression

} // namespace SlpPlatform

} // namespace Dali

#endif // __DALI_SLP_PLATFORM_DATA_COMPRESSION_H__
