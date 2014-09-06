/*
CopyRightToLeft }, (c) 2000-2012 Samsung Electronics Co., Ltd All Rights Reserved

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

// CLASS HEADER
#include "locale-utils.h"

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace Locale
{

using Dali::VirtualKeyboard::TextDirection;
using Dali::VirtualKeyboard::LeftToRight;
using Dali::VirtualKeyboard::RightToLeft;

namespace
{

struct LocaleDirection
{
  const char * locale;
  const char * name;
  TextDirection direction;
};

const LocaleDirection LOCALE_DIRECTION_LOOKUP_TABLE[] =
{
  { "af", "Afrikaans",          LeftToRight },
  { "am", "Amharic",            LeftToRight },
  { "ar", "Arabic",             RightToLeft },
  { "as", "Assamese",           LeftToRight },
  { "az", "Azeri",              LeftToRight },
  { "be", "Belarusian",         LeftToRight },
  { "bg", "Bulgarian",          LeftToRight },
  { "bn", "Bengali",            LeftToRight },
  { "bo", "Tibetan",            LeftToRight },
  { "bs", "Bosnian",            LeftToRight },
  { "ca", "Catalan",            LeftToRight },
  { "cs", "Czech",              LeftToRight },
  { "cy", "Welsh",              LeftToRight },
  { "da", "Danish",             LeftToRight },
  { "de", "German",             LeftToRight },
  { "dv", "Divehi",             RightToLeft },
  { "el", "Greek",              LeftToRight },
  { "en", "English",            LeftToRight },
  { "es", "Spanish",            LeftToRight },
  { "et", "Estonian",           LeftToRight },
  { "eu", "Basque",             LeftToRight },
  { "fa", "Farsi",              RightToLeft },
  { "fi", "Finnish",            LeftToRight },
  { "fo", "Faroese",            LeftToRight },
  { "fr", "French",             LeftToRight },
  { "gd", "Gaelic",             LeftToRight },
  { "gl", "Galician",           LeftToRight },
  { "gn", "Guarani",            LeftToRight },
  { "gu", "Gujarati",           LeftToRight },
  { "he", "Hebrew",             RightToLeft },
  { "hi", "Hindi",              LeftToRight },
  { "hr", "Croatian",           LeftToRight },
  { "hu", "Hungarian",          LeftToRight },
  { "hy", "Armenian",           LeftToRight },
  { "id", "Indonesian",         LeftToRight },
  { "is", "Icelandic",          LeftToRight },
  { "it", "Italian",            LeftToRight },
  { "ja", "Japanese",           LeftToRight },
  { "ka", "Georgian",           LeftToRight },
  { "kk", "Kazakh",             RightToLeft },
  { "km", "Khmer",              LeftToRight },
  { "kn", "Kannada",            LeftToRight },
  { "ko", "Korean",             LeftToRight },
  { "ks", "Kashmiri",           RightToLeft },
  { "la", "Latin",              LeftToRight },
  { "lo", "Lao",                LeftToRight },
  { "lt", "Lithuanian",         LeftToRight },
  { "lv", "Latvian",            LeftToRight },
  { "mi", "Maori",              LeftToRight },
  { "mk", "FYRO Macedonia",     LeftToRight },
  { "ml", "Malayalam",          LeftToRight },
  { "mn", "Mongolian",          LeftToRight },
  { "mr", "Marathi",            LeftToRight },
  { "ms", "Malay",              LeftToRight },
  { "mt", "Maltese",            LeftToRight },
  { "my", "Burmese",            LeftToRight },
  { "nb", "Norwegian: Bokml",   LeftToRight },
  { "ne", "Nepali",             LeftToRight },
  { "nl", "Dutch",              LeftToRight },
  { "nn", "Norwegian: Nynorsk", LeftToRight },
  { "or", "Oriya",              LeftToRight },
  { "pa", "Punjabi",            LeftToRight },
  { "pl", "Polish",             LeftToRight },
  { "pt", "Portuguese",         LeftToRight },
  { "rm", "Raeto-Romance",      LeftToRight },
  { "ro", "Romanian",           LeftToRight },
  { "ru", "Russian",            LeftToRight },
  { "sa", "Sanskrit",           LeftToRight },
  { "sb", "Sorbian",            LeftToRight },
  { "sd", "Sindhi",             LeftToRight },
  { "si", "Sinhala",            LeftToRight },
  { "sk", "Slovak",             LeftToRight },
  { "sl", "Slovenian",          LeftToRight },
  { "so", "Somali",             LeftToRight },
  { "sq", "Albanian",           LeftToRight },
  { "sr", "Serbian",            LeftToRight },
  { "sv", "Swedish",            LeftToRight },
  { "sw", "Swahili",            LeftToRight },
  { "ta", "Tamil",              LeftToRight },
  { "te", "Telugu",             LeftToRight },
  { "tg", "Tajik",              RightToLeft },
  { "th", "Thai",               LeftToRight },
  { "tk", "Turkmen",            LeftToRight },
  { "tn", "Setsuana",           LeftToRight },
  { "tr", "Turkish",            LeftToRight },
  { "ts", "Tsonga",             LeftToRight },
  { "tt", "Tatar",              LeftToRight },
  { "uk", "Ukrainian",          LeftToRight },
  { "ur", "Urdu",               RightToLeft },
  { "uz", "Uzbek",              LeftToRight },
  { "vi", "Vietnamese",         LeftToRight },
  { "xh", "Xhosa",              LeftToRight },
  { "yi", "Yiddish",            RightToLeft },
  { "zh", "Chinese",            LeftToRight },
  { "zu", "Zulu",               LeftToRight },

  { NULL, NULL, LeftToRight }
};

} // unnamed namespace

TextDirection GetTextDirection( std::string locale )
{
  TextDirection direction( LeftToRight );

  if ( !locale.empty() && locale.size() > 2 )
  {
    // We're only interested in the first two characters
    locale.resize(2);

    for ( const LocaleDirection* iter = &LOCALE_DIRECTION_LOOKUP_TABLE[0]; iter->locale; ++iter )
    {
      if ( !locale.compare( iter->locale ) )
      {
        direction = iter->direction;
        break;
      }
    }
  }

  return direction;
}

} // namespace Locale

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
