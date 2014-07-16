#ifndef __DALI_SDL_PLATFORM_ABSTRACTION_H__
#define __DALI_SDL_PLATFORM_ABSTRACTION_H__

/*
Copyright (c) 2000-2013 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali

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

// EXTERNAL INCLUDES
#include <set>
#include <stdint.h>
#include <cstring>
#include <queue>

// INTERNAL INCLUDES
#include <dali/integration-api/platform-abstraction.h>
#include <dali/integration-api/glyph-set.h>
#include <dali/integration-api/bitmap.h>
/* #include <test-trace-call-stack.h> */

namespace Dali
{

/**
 * Concrete implementation of the platform abstraction class.
 */
class DALI_IMPORT_API SdlPlatformAbstraction : public Dali::Integration::PlatformAbstraction
{

public:

  struct Resources
  {
    bool                         loaded;
    Integration::ResourceId      loadedId;
    Integration::ResourceTypeId  loadedType;
    Integration::ResourcePointer loadedResource;

    bool                         loadFailed;
    Integration::ResourceId      loadFailedId;
    Integration::ResourceFailure loadFailure;

    bool                         saved;
    Integration::ResourceId      savedId;
    Integration::ResourceTypeId  savedType;

    bool                         saveFailed;
    Integration::ResourceId      saveFailedId;
    Integration::ResourceFailure saveFailure;
  };

  struct LoadFileResult
  {
    LoadFileResult()
    : loadResult(false)
    {

    }

    bool loadResult;
    std::vector< unsigned char> buffer;
  };


  /**
   * Constructor
   */
  SdlPlatformAbstraction();


  /**
   * Destructor
   */
  virtual ~SdlPlatformAbstraction();

  /**
   * @copydoc PlatformAbstraction::GetTimeMicroseconds()
   */
  virtual void GetTimeMicroseconds(unsigned int &seconds, unsigned int &microSeconds);

  void IncrementGetTimeResult(size_t milliseconds);

  /**
   * @copydoc PlatformAbstraction::Suspend()
   */
  virtual void Suspend();

  /**
   * @copydoc PlatformAbstraction::Resume()
   */
  virtual void Resume();

  /**
   * @copydoc PlatformAbstraction::LoadImageMetadata()
   */
  virtual void LoadImageMetadata(const std::string fileName, Vector2 &size);

  /**
   * @copydoc PlatformAbstraction::GetClosestImageSize()
   */
  virtual void GetClosestImageSize( const std::string& filename,
                                    const ImageAttributes& attributes,
                                    Vector2& closestSize );

  /**
   * @copydoc PlatformAbstraction::GetClosestImageSize()
   */
  virtual void GetClosestImageSize( Integration::ResourcePointer resourceBuffer,
                                    const ImageAttributes& attributes,
                                    Vector2& closestSize );

  /**
   * @copydoc PlatformAbstraction::LoadResource()
   */
  virtual void LoadResource(const Integration::ResourceRequest& request);

  virtual Integration::ResourcePointer LoadResourceSynchronously( const Integration::ResourceType& resourceType, const std::string& resourcePath );

  /**
   * @copydoc PlatformAbstraction::SaveResource()
   */
  virtual void SaveResource(const Integration::ResourceRequest& request);

  /**
   * @copydoc PlatformAbstraction::CancelLoad()
   */
  virtual void CancelLoad(Integration::ResourceId id, Integration::ResourceTypeId typeId);

  /**
   * @copydoc PlatformAbstraction::GetResources()
   */
  virtual void GetResources(Integration::ResourceCache& cache);

  /**
   * @copydoc PlatformAbstraction::IsLoading()
   */
  virtual bool IsLoading();

  /**
   * @copydoc PlatformAbstraction::GetDefaultFontFamily()
   */
  virtual std::string GetDefaultFontFamily() const;

  /**
   * @copydoc PlatformAbstraction::GetDefaultFontSize()
   */
  virtual const float GetDefaultFontSize() const;

  virtual const PixelSize GetFontLineHeightFromCapsHeight(const std::string& fontFamily, const std::string& fontStyle, const CapsHeight& capsHeight) const;

  virtual Integration::GlyphSet* GetCachedGlyphData( const Integration::TextResourceType& textRequest,
                                                     const std::string& fontFamily ) const;

  /**
   * @copydoc PlatformAbstraction::GetGlyphData()
   */
  virtual Integration::GlyphSet* GetGlyphData ( const Integration::TextResourceType& textRequest,
                                                const std::string& fontFamily,
                                                bool getBitmap) const;

  /**
   * @copydoc PlatformAbstraction::GetGlobalMetrics()
   */
  virtual void GetGlobalMetrics( const std::string& fontFamily,
                                 const std::string& fontStyle,
                                 Integration::GlobalMetrics& globalMetrics ) const;

  /**
   * @copydoc PlatformAbstraction::GetFontPath()
   */
  virtual std::string GetFontPath(const std::string& family, bool bold, bool italic) const;

  /**
   * @copydoc PlatformAbstraction::SetDpi()
   */
  virtual void SetDpi (unsigned int dpiHorizontal, unsigned int dpiVertical);

  /**
   * @copydoc PlatformAbstraction::GetFontFamilyForChars()
   */
  virtual std::string GetFontFamilyForChars(const TextArray& charsRequested) const;

  /**
   * @copydoc PlatformAbstraction::AllGlyphsSupported()
   */
  virtual bool AllGlyphsSupported(const std::string& name, const std::string& fontStyle, const TextArray& text) const;

  /**
   * @copydoc PlatformAbstraction::ValidateFontFamilyName()
   */
  virtual bool ValidateFontFamilyName(const std::string& fontFamily, const std::string& fontStyle, bool& isDefaultSystemFont, std::string& closestMatch, std::string& closestStyleMatch) const;


  /**
   * @copydoc PlatformAbstraction::GetFontList()
   */
  virtual std::vector<std::string> GetFontList( FontListMode mode ) const;

  /**
   * @copydoc PlatformAbstraction::LoadFile()
   */
  virtual bool LoadFile( const std::string& filename, std::vector< unsigned char >& buffer ) const;

  /**
   * @copydoc PlatformAbstraction::SaveFile()
   */
  virtual bool SaveFile(const std::string& filename, std::vector< unsigned char >& buffer) const;

  virtual void JoinLoaderThreads();

  virtual void UpdateDefaultsFromDevice();

  virtual Integration::DynamicsFactory* GetDynamicsFactory();

  virtual bool ReadGlobalMetricsFromCache( const std::string& fontFamily,
                                           const std::string& fontStyle,
                                           Integration::GlobalMetrics& globalMetrics );


  virtual void WriteGlobalMetricsToCache( const std::string& fontFamily,
                                          const std::string& fontStyle,
                                          const Integration::GlobalMetrics& globalMetrics );

  virtual bool ReadMetricsFromCache( const std::string& fontFamily,
                                     const std::string& fontStyle,
                                     std::vector<Integration::GlyphMetrics>& glyphMetricsContainer );

  virtual void WriteMetricsToCache( const std::string& fontFamily,
                                    const std::string& fontStyle,
                                    const Integration::GlyphSet& glyphSet );

  virtual void GetFileNamesFromDirectory( const std::string& directoryName,
                                          std::vector<std::string>& fileNames );

  virtual Integration::BitmapPtr GetGlyphImage( const std::string& fontFamily, const std::string& fontStyle, float fontSize, uint32_t character ) const;

private:
  size_t                        mSeconds;
  size_t                        mMicroSeconds;
  std::string                   mGetDefaultFontFamilyResult;
  float                         mGetDefaultFontSizeResult;
  std::string                   mGetFontPathResult;
  Resources                     mResources;
  Vector2                       mSize;
  bool                          mReadGlobalMetricsResult;
  bool                          mReadMetricsResult;
  Integration::GlobalMetrics mReadGlobalMetrics;
  std::vector<Integration::GlyphMetrics> mReadMetrics;

  LoadFileResult                mLoadFileResult;
  mutable FontListMode          mFontListMode;

  /* std::map<Integration::ResourceId, Bitmap> mResources; */
  typedef std::pair< Integration::ResourceId, Integration::BitmapPtr > ResourceIdBitmapPair;
  std::queue<ResourceIdBitmapPair> mResourceQueue;


};

} // Dali

#endif /* __DALI_TET_PLATFORM_ABSTRACTION_H__ */
