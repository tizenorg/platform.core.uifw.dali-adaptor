#ifndef __DALI_VECTOR_IMG_STREAMER_H__
#define __DALI_VECTOR_IMG_STREAMER_H__

/*
Copyright (c) 2013 Samsung Electronics Co., Ltd All Rights Reserved

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


namespace Dali
{


#undef MIN
#define MIN(a,b)	((a) < (b) ? (a) : (b))

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
