#ifndef __DALI_VIDEO_PLAYER_H__
#define __DALI_VIDEO_PLAYER_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// EXTERNAL INCLUDES
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/actors/image-actor.h>

// INTERNAL INCLUDES

namespace Dali DALI_IMPORT_API
{

namespace Internal DALI_INTERNAL
{

namespace Adaptor
{
class VideoPlayer;
}

}

class VideoPlayer: public BaseHandle
{

public:
  VideoPlayer();

  ~VideoPlayer();

  static VideoPlayer New();

  bool Initialize();

  void SetVideoFile(const std::string& strFilename);

  void Play();

  void Pause();

  void Backward(int millisecond);

  void Forward(int millisecond);

  void Stop();

  void Resume();

  void LinkVideoStreamImage(Dali::ImageActor imageActor);

public:

  VideoPlayer( Internal::Adaptor::VideoPlayer* player );
};

} // namespace Dali

#endif
