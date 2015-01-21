#ifndef __DALI_TIZEN_BUFFER_VIDEO_PLAYER_H__
#define __DALI_TIZEN_BUFFER_VIDEO_PLAYER_H__

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

// INTERNAL INCLUDES
#include <video-player-plugin.h>
#include <tizen-buffer-image.h>
#include <timer.h>

// EXTERNAL INCLUDES
#include <dali/public-api/images/image.h>
#include <player.h>
#include <boost/thread.hpp>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

/**
 * @brief Implementation of the TizenBufferVideoPlayer class which has Tizen platform dependency.
 */
class TizenBufferVideoPlayer : public VideoPlayerPlugin, public ConnectionTracker
{
public:

  /**
   * @brief Constructor
   */
  TizenBufferVideoPlayer();

  /**
   * @brief Virtual destructor
   */
  virtual ~TizenBufferVideoPlayer();

  /**
   * @copydoc Dali::VideoPlayerPlugin::SetVideoFile()
   */
  virtual void SetVideoFile( const std::string& filename );

  /**
   * @copydoc Dali::VideoPlayerPlugin::SetVideoLooping()
   */
  virtual void SetVideoLooping( bool looping );

  /**
   * @copydoc Dali::VideoPlayerPlugin::IsVideoLooping()
   */
  virtual bool IsVideoLooping();

  /**
   * @copydoc Dali::VideoPlayerPlugin::Play()
   */
  virtual void Play();

  /**
   * @copydoc Dali::VideoPlayerPlugin::Pause()
   */
  virtual void Pause();

  /**
   * @copydoc Dali::VideoPlayerPlugin::Backward()
   */
  virtual void Backward( int millisecond );

  /**
   * @copydoc Dali::VideoPlayerPlugin::Forward()
   */
  virtual void Forward( int millisecond );

  /**
   * @copydoc Dali::VideoPlayerPlugin::Stop()
   */
  virtual void Stop();

  /**
   * @copydoc Dali::VideoPlayerPlugin::Resume()
   */
  virtual void Resume();

  /**
   * @copydoc Dali::VideoPlayerPlugin::LinkVideoStreamImage()
   */
  virtual void LinkVideoStreamImage( Dali::ImageActor imageActor );

private:

  /**
   * @brief Updates the linked ImageActor by new tbm surface.
   */
  bool Update();

  /**
   * @brief Creates TizenBufferImage instance and Sets fragment shader codes for TizenBufferImage.
   */
  void CreateTizenBufferImage();

  /**
   * @brief Initializes the media player.
   */
  void InitTizenBufferPlayer();

private:

  std::string mVideoFilename;                  ///< Video file path and name
  player_h mPlayer;                            ///< A handle to the media player
  player_state_e mState;                       ///< The current state of the player
  Dali::Image mVideoStream;                    ///< Video stream (frame) image from tbm surface
  Dali::ImageActor mImageActor;                ///< ImageActor linked with external ImageActor

  tbm_surface_h mSurface;                      ///< tbm surface
  media_packet_h mPacket;                      ///< Packet from mPacketVector
  Dali::TizenBufferImagePtr mTbImagePtr;       ///< Timer for updating EGL image
  Dali::Timer mTimer;

  int mError;                                  ///< Error returned from the media player

public:

  boost::mutex mMutex;                         ///< This mutex must be locked before handling a packet
  Dali::Vector< media_packet_h > mPacketVector;  ///< Container for packets obtained from the media player
};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif
