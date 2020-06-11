
#include "ext/import-font.h"

#include <cstdlib>
#include <queue>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H

namespace msdfgen {

#define REQUIRE(cond) { if (!(cond)) return false; }
#define F26DOT6_TO_DOUBLE(x) (1/64.*double(x))

struct FtContext {
    Point2 position;
    Shape *shape;
    Contour *contour;
};

static Point2 ftPoint2(const FT_Vector &vector) {
    return Point2(F26DOT6_TO_DOUBLE(vector.x), F26DOT6_TO_DOUBLE(vector.y));
}

static int ftMoveTo(const FT_Vector *to, void *user) {
    FtContext *context = reinterpret_cast<FtContext *>(user);
    if (!(context->contour && context->contour->edges.empty()))
        context->contour = &context->shape->addContour();
    context->position = ftPoint2(*to);
    return 0;
}

static int ftLineTo(const FT_Vector *to, void *user) {
    FtContext *context = reinterpret_cast<FtContext *>(user);
    Point2 endpoint = ftPoint2(*to);
    if (endpoint != context->position) {
        context->contour->addEdge(new LinearSegment(context->position, endpoint));
        context->position = endpoint;
    }
    return 0;
}

static int ftConicTo(const FT_Vector *control, const FT_Vector *to, void *user) {
    FtContext *context = reinterpret_cast<FtContext *>(user);
    context->contour->addEdge(new QuadraticSegment(context->position, ftPoint2(*control), ftPoint2(*to)));
    context->position = ftPoint2(*to);
    return 0;
}

static int ftCubicTo(const FT_Vector *control1, const FT_Vector *control2, const FT_Vector *to, void *user) {
    FtContext *context = reinterpret_cast<FtContext *>(user);
    context->contour->addEdge(new CubicSegment(context->position, ftPoint2(*control1), ftPoint2(*control2), ftPoint2(*to)));
    context->position = ftPoint2(*to);
    return 0;
}

bool getFontMetrics(FontMetrics &metrics, FT_Face face) {
    metrics.emSize = F26DOT6_TO_DOUBLE(face->units_per_EM);
    metrics.ascenderY = F26DOT6_TO_DOUBLE(face->ascender);
    metrics.descenderY = F26DOT6_TO_DOUBLE(face->descender);
    metrics.lineHeight = F26DOT6_TO_DOUBLE(face->height);
    metrics.underlineY = F26DOT6_TO_DOUBLE(face->underline_position);
    metrics.underlineThickness = F26DOT6_TO_DOUBLE(face->underline_thickness);
    return true;
}

bool getFontWhitespaceWidth(double &spaceAdvance, double &tabAdvance, FT_Face face) {
    FT_Error error = FT_Load_Char(face, ' ', FT_LOAD_NO_SCALE);
    if (error)
        return false;
    spaceAdvance = F26DOT6_TO_DOUBLE(face->glyph->advance.x);
    error = FT_Load_Char(face, '\t', FT_LOAD_NO_SCALE);
    if (error)
        return false;
    tabAdvance = F26DOT6_TO_DOUBLE(face->glyph->advance.x);
    return true;
}

bool loadGlyph(Shape &output, FT_Face face, unicode_t unicode, double *advance) {
    FT_Error error = FT_Load_Char(face, unicode, FT_LOAD_NO_SCALE);
    if (error)
        return false;
    output.contours.clear();
    output.inverseYAxis = false;
    if (advance)
        *advance = F26DOT6_TO_DOUBLE(face->glyph->advance.x);

    FtContext context = { };
    context.shape = &output;
    FT_Outline_Funcs ftFunctions;
    ftFunctions.move_to = &ftMoveTo;
    ftFunctions.line_to = &ftLineTo;
    ftFunctions.conic_to = &ftConicTo;
    ftFunctions.cubic_to = &ftCubicTo;
    ftFunctions.shift = 0;
    ftFunctions.delta = 0;
    error = FT_Outline_Decompose(&face->glyph->outline, &ftFunctions, &context);
    if (error)
        return false;
    if (!output.contours.empty() && output.contours.back().edges.empty())
        output.contours.pop_back();
    return true;
}

bool getKerning(double &output, FT_Face face, unicode_t unicode1, unicode_t unicode2) {
    FT_Vector kerning;
    if (FT_Get_Kerning(face, FT_Get_Char_Index(face, unicode1), FT_Get_Char_Index(face, unicode2), FT_KERNING_UNSCALED, &kerning)) {
        output = 0;
        return false;
    }
    output = F26DOT6_TO_DOUBLE(kerning.x);
    return true;
}

}
