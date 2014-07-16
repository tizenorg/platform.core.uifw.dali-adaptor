#ifndef EMSCRIPTENCALLBACKS_H
#define EMSCRIPTENCALLBACKS_H

#include <dali/integration-api/platform-abstraction.h>
#include <dali/integration-api/resource-cache.h>
#include <dali/public-api/common/dali-common.h>

namespace EmscriptenCallbacks
{
  Dali::Integration::BitmapPtr GetGlyphImage( const std::string& fontFamily, const std::string& fontStyle, float fontSize, uint32_t character );

  void LoadImageMetadata(const std::string fileName, Dali::Vector2 &size);

  Dali::Integration::BitmapPtr GetImage( const Dali::ImageAttributes& attributes, const std::string& filename );

};

/* class EmscriptenCallbacks */
/* { */
/* public: */

/*   EmscriptenCallbacks(); */

/*   /\* /\\** *\/ */
/*   /\*  * @copydoc PlatformAbstraction::GetGlyphData() *\/ */
/*   /\*  *\\/ *\/ */
/*   /\* virtual Integration::GlyphSet* GetGlyphData ( const Dali::Integration::TextResourceType& textRequest, *\/ */
/*   /\*                                               const std::string& fontFamily, *\/ */
/*   /\*                                               bool getBitmap) const; *\/ */

/*   /\* /\\** *\/ */
/*   /\*  * @copydoc PlatformAbstraction::GetCachedGlyphData() *\/ */
/*   /\*  *\\/ *\/ */
/*   /\* virtual Integration::GlyphSet* GetCachedGlyphData( const Integration::TextResourceType& textRequest, *\/ */
/*   /\*                                                    const std::string& fontFamily ) const; *\/ */

/*   /\* /\\** *\/ */
/*   /\*  * @copydoc PlatformAbstraction::GetGlobalMetrics() *\/ */
/*   /\*  *\\/ *\/ */
/*   /\* virtual void GetGlobalMetrics( const std::string& fontFamily, *\/ */
/*   /\*                                const std::string& fontStyle, *\/ */
/*   /\*                                Integration::GlobalMetrics& globalMetrics ) const; *\/ */

/*   /\* /\\** *\/ */
/*   /\*  * @copydoc PlatformAbstraction::AllGlyphsSupported() *\/ */
/*   /\*  *\\/ *\/ */
/*   /\* bool AllGlyphsSupported(const std::string &fontFamily, const std::string& fontStyle, const TextArray& charsRequested) const; *\/ */

/*   /\* /\\** *\/ */
/*   /\*  * @copydoc PlatformAbstraction::ValidateFontFamilyName() *\/ */
/*   /\*  *\\/ *\/ */
/*   /\* virtual bool ValidateFontFamilyName(const std::string& fontFamily, const std::string& fontStyle, bool& isDefaultSystemFont, std::string& closestMatch, std::string& closestStyleMatch) const; *\/ */

/*   /\* /\\** *\/ */
/*   /\*  * @copydoc PlatformAbstraction::GetFontList() *\/ */
/*   /\*  *\\/ *\/ */
/*   /\* virtual std::vector<std::string> GetFontList ( Dali::Integration::PlatformAbstraction::FontListMode mode ) const; *\/ */


/*   virtual Integration::BitmapPtr GetGlyphImage( const std::string& fontFamily, const std::string& fontStyle, float fontSize, uint32_t character ) const; */


/* protectd: */
/*   ~EmscriptenCallbacks(); */

/* private: */
/*   EmscriptenCallbacksImpl *mImpl; */

/* }; */


#endif // header
