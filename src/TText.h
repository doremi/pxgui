#ifndef TTEXTH
#define TTEXTH

#include <ft2build.h>
#include <freetype2/freetype/freetype.h>

class TText : public TGuiElement
{
public:
    TText(TGui *Parent, int x, int y, int fsize, char *name, char *str);
    ~TText();
    void Draw();
private:
    char *str;
    TColors Col;
    static const struct font_desc *fonts[];

// FreeType
    FT_Library library;
    FT_Face face;
    FT_GlyphSlot slot;
    FT_Vector pen;
    void drawtext(wchar_t *text);
    void draw_bitmap( FT_Bitmap *bitmap, FT_Int x, FT_Int y);
};

#endif