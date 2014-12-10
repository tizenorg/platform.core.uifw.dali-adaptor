#ifndef DALI_SLP_PLATFORM_METRICS_CACHE_H
#define DALI_SLP_PLATFORM_METRICS_CACHE_H

/*
Copyright (c) 2000-2013 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali

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

#include <dali/integration-api/glyph-set.h>
#include <string>

namespace Dali
{
namespace SlpPlatform
{
namespace MetricsCache
{

/**
 * Read from the metrics file into the global metrics parameter
 * @param[in] fontFamily The name of the font family
 * @param[in] fontStyle The name of the font style
 * @param[out] globalMetrics The data store to write into
 * @return true if the operation succeeded
 */
bool ReadGlobal( std::string fontFamily, std::string fontStyle, Integration::GlobalMetrics& globalMetrics);

/**
 * Write the global metrics parameter to the metrics file
 * @param[in] fontFamily The name of the font family
 * @param[in] fontStyle The name of the font style
 * @param[out] globalMetrics The data store to write
 */
void WriteGlobal( std::string fontFamily, std::string fontStyle, const Integration::GlobalMetrics& globalMetrics);

/**
 * Read the metrics from the given file into the supplied vector
 * @param[in] fontFamily The name of the font family
 * @param[in] fontStyle The name of the font style
 * @param[out] glyphMetricsContainer The vector of metrics to write
 * @return true if the operation succeeded
 */
bool Read( std::string fontFamily, std::string fontStyle, std::vector<Integration::GlyphMetrics>& glyphMetricsContainer );

/**
 * Write the metrics into the given file
 * @param[in] fontFamily The name of the font family
 * @param[in] fontStyle The name of the font style
 * @param[in] glyphSet The set of metrics to write
 */
void Write( std::string fontFamily, std::string fontStyle, const Integration::GlyphSet& glyphSet );

} // MetricsCache
} // SlpPlatform
} // Dali

#endif //DALI_INTEGRATION_METRICS_CACHE_H
