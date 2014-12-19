#ifndef __DALI_X11_VIDEO_PLAYER_H__
#define __DALI_X11_VIDEO_PLAYER_H__

#include <VideoPlayerPlugin.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

class X11VideoPlayer: public Dali::VideoPlayerPlugin
{
public:

  virtual void SetVideoFile(const std::string& strFilename);

  virtual void Play();

  virtual void Pause();

  virtual void Backward(int millisecond);

  virtual void Forward(int millisecond);

  virtual void Stop();

  virtual void Resume();

  virtual void Initialize(int width, int height);

  virtual void LinkVideoStreamImage(Dali::ImageActor imageActor);

private:

  X11VideoPlayer();
  X11VideoPlayer(const VideoPlayer&);

  virtual ~VideoPlayer();

  void CreateXPixmapSurface(int width, int height);
  void InitX11Player();

private:

  bool mLoopFlag;
  std::string mVideoFilename;
  player_h mPlayer;
  int mViewWidth;
  int mViewHeight;
  Dali::Image mVideoStream;
  Dali::ImageActor mImageActor;

  Ecore_X_Pixmap mPixmap;
  Dali::PixmapImagePtr mPixmapImage;
};

} // namepsace Adaptor

} // namespace Internal

} // namespace Dali

#endif
