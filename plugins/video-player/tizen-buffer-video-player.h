#ifndef __DALI_TIZEN_BUFFER_VIDEO_PLAYER_H__
#define __DALI_TIZEN_BUFFER_VIDEO_PLAYER_H__

#include <video-player-plugin.h>
#include <dali/public-api/images/image.h>
#include <tizen-buffer-image.h>
#include <timer.h>
#include <player.h>
//#include <glib.h>
#include <boost/thread.hpp>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

class TizenBufferVideoPlayer : public VideoPlayerPlugin, public ConnectionTracker
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

  virtual void LinkVideoStreamImage(Dali::ImageActor imageActor);

private:

  bool Update();

  void CreateTizenBufferSurface();
  void InitTizenBufferPlayer();

  bool mLoopFlag;
  std::string mVideoFilename;
  player_h mPlayer;
  player_state_e mState;
  Dali::Image mVideoStream;
  Dali::ImageActor mImageActor;

  tbm_surface_h mSurface;
  media_packet_h mPacket;
  Dali::TizenBufferImagePtr mTbImagePtr;
  Dali::Timer mTimer;

  int mError;

public:
//  GMutex mBufferLock;
  boost::mutex mMutex;

  Dali::Vector<media_packet_h> mPacketVector;
};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif
