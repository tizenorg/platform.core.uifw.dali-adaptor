

  //
  //
  //
  struct GlyphInfo
  {
    union
    {
      uint32_t glyphId;    ///< glyph index
      uint32_t unicodeId;  ///< unicode code point
    };
    float top;
    float bottom;
    float left;
    float right;
    float xAdvance;
    float yAdvance;
  };

