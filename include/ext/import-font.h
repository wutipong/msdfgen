
#pragma once

#include <cstdlib>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "../core/Shape.h"

namespace msdfgen {

typedef unsigned unicode_t;

/// Global metrics of a typeface (in font units).
struct FontMetrics {
    /// The size of one EM.
    double emSize;
    /// The vertical position of the ascender and descender relative to the baseline.
    double ascenderY, descenderY;
    /// The vertical difference between consecutive baselines.
    double lineHeight;
    /// The vertical position and thickness of the underline.
    double underlineY, underlineThickness;
};

/// Outputs the metrics of a font file.
bool getFontMetrics(FontMetrics &metrics, FT_Face face);
/// Outputs the width of the space and tab characters.
bool getFontWhitespaceWidth(double &spaceAdvance, double &tabAdvance, FT_Face face);
/// Loads the geometry of a glyph from a font file.
bool loadGlyph(Shape &output, FT_Face face, unicode_t unicode, double *advance = NULL);
/// Loads the geometry of a glyph from a font file.
bool loadGlyphIndex(Shape &output, FT_Face face, FT_UInt index, double *advance = NULL);
/// Outputs the kerning distance adjustment between two specific glyphs.
bool getKerning(double &output, FT_Face face, unicode_t unicode1, unicode_t unicode2);

}
