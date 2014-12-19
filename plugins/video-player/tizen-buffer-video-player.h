#ifndef __DALI_TIZEN_BUFFER_VIDEO_PLAYER_H__
#define __DALI_TIZEN_BUFFER_VIDEO_PLAYER_H__

#include <VideoPlayerPlugin.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

class TizenBufferVideoPlayer : public VideoPlayerPlugin
{
public:

  TizenBufferVideoPlayer();

  virtual ~TizenBufferVideoPlayer();

  virtual void SetVideoFile(const std::string& strFilename);

  virtual void Play();

  virtual void Pause();

  virtual void Backward(int millisecond);

  virtual void Forward(int millisecond);

  virtual void Stop();

  virtual void Resume();

  virtual void Initialize();

  virtual void LinkVideoStreamImage(Dali::ImageActor imageActor);

private:

  bool Update();

  void CreateTizenBufferSurface();

  bool mLoopFlag;
  std::string mVideoFilename;
  player_h mPlayer;
  Dali::Image mVideoStream;
  Dali::ImageActor mImageActor;

  media_packet_h mPacket;
  Dali::TizenBufferImagePtr mTbImagePtr;
  Dali::Timer mTimer;

public:
  GMutex mBufferLock;
};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif
