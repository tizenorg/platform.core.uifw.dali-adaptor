#ifndef __DALI_VIDEO_PLAYER_PLUGIN_H__
#define __DALI_VIDEO_PLAYER_PLUGIN_H__

#include <dali/public-api/actors/image-actor.h>

namespace Dali DALI_IMPORT_API
{

class VideoPlayerPlugin
{
public:

  VideoPlayerPlugin(){}

  virtual ~VideoPlayerPlugin(){}

  virtual void SetVideoFile(const std::string& strFilename) = 0;

  virtual void Play() = 0;

  virtual void Pause() = 0;

  virtual void Backward(int millisecond) = 0;

  virtual void Forward(int millisecond) = 0;

  virtual void Stop() = 0;

  virtual void Resume() = 0;

  virtual void LinkVideoStreamImage(Dali::ImageActor imageActor) = 0;
};

} // namespace Dali

#endif
