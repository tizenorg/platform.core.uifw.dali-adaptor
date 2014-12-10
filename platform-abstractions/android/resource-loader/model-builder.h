#ifndef __DALI_SLP_PLATFORM_MODEL_BUILDER_H__
#define __DALI_SLP_PLATFORM_MODEL_BUILDER_H__

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

#include <dali/public-api/modeling/model-data.h>

namespace Dali
{
namespace SlpPlatform
{

/**
 * Provides a means of creating a Model object from a file, resource or data stream
 * passed in it's ctor.
 */
class ModelBuilder
{
public:
  /**
   * Build the given model
   * @param[in] model - the model to build
   */
  virtual bool Build(ModelData model) = 0;

  /**
   * Return the model name (usually the basename of the file resource)
   */
  virtual const std::string& GetModelName() = 0;
};

}//SlpPlatform
}//Dali

#endif // __DALI_SLP_PLATFORM_MODEL_BUILDER_H__
