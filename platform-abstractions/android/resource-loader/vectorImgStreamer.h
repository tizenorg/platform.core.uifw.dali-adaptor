#ifndef __DALI_VECTOR_IMG_STREAMER_H__
#define __DALI_VECTOR_IMG_STREAMER_H__


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
 */


namespace Dali
{


#undef MIN
#define MIN(a,b)  ((a) < (b) ? (a) : (b))

// This class wraps a vector that contains an image character array.
// This is to enable mimicking of file operations when processing *.gif and *.bmp images.
// mIndex: represents the current position in the mimicked file
// m_VectorSrc: contains the contents of the mimicked file
class VectorImgStreamer
{
public:

  // mimick SEEK_SET and SEEK_CUR in fseek
  enum eSeek
  {
    V_SEEK_SET,
    V_SEEK_CUR
  };

  VectorImgStreamer()
  {
    mIndex=0;
    m_VectorSrc = std::vector<uint8_t>(0);
  }

  VectorImgStreamer(const std::vector<uint8_t> VectorSrc)
  {
    loadVector(VectorSrc);
  }

  void loadVector(const std::vector<uint8_t> &VectorSrc)
  {
    mIndex=0;
    m_VectorSrc = VectorSrc;
  }

  // Mimick an fread  style operation.
  // Read "numOfEntriesToRead" bytes into dataDest from index position "mIndex" of
  // uint8_t array contained in m_VectorSrc.
  // update mIndex to show the new current position after the read operation.
  int vRead( char *dataDest, int numOfEntriesToRead)
  {
    int itemsRead=0;

    int itemsAvailable = m_VectorSrc.size() - mIndex ;
    int totalItemsAvailable = (itemsAvailable > 0)? itemsAvailable: 0;
    itemsRead = MIN(totalItemsAvailable, numOfEntriesToRead);
    memcpy(dataDest, &m_VectorSrc[mIndex], itemsRead);
    mIndex += itemsRead;

    return itemsRead;
  }

  // mimick fseek
  bool vSeek(int index, eSeek seekType)
  {
    int seekIndex = (V_SEEK_CUR == seekType)? index+mIndex : index;
    seekIndex =(m_VectorSrc.size() > seekIndex )? seekIndex : 0;
    return (seekIndex > 0);
  }

private:
  unsigned int mIndex;  // Index into character array containerd in m_VectorSrc
  std::vector<uint8_t> m_VectorSrc; // vector containing .bmp or .gif formated image

};

} // namespace Dali

#endif // __DALI_VECTOR_IMG_STREAMER_H__
