/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <haptic-player.h>

// INTERNAL INCLUDES
#include <haptic-player-impl.h>

namespace Dali
{

HapticPlayer::HapticPlayer()
{
}

HapticPlayer HapticPlayer::Get()
{
  return Internal::Adaptor::HapticPlayer::Get();
}

HapticPlayer::~HapticPlayer()
{
}

void HapticPlayer::PlayMonotone(unsigned int duration)
{
  GetImplementation(*this).PlayMonotone(duration);
}

void HapticPlayer::PlayFile(const std::string filePath)
{
  GetImplementation(*this).PlayFile(filePath);
}

void HapticPlayer::Stop()
{
  GetImplementation(*this).Stop();
}

int HapticPlayer::PlaySound( const std::string& fileName )
{
  return GetImplementation(*this).PlaySound(fileName);
}

void HapticPlayer::StopSound( int handle )
{
  GetImplementation(*this).StopSound(handle);
}

void HapticPlayer::PlayFeedbackPattern( int type, int pattern )
{
  GetImplementation(*this).PlayFeedbackPattern(type, pattern);
}

HapticPlayer::HapticPlayer( Internal::Adaptor::HapticPlayer* player )
: BaseHandle( player )
{
}

} // namespace Dali
