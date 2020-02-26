#include "f2b_qt_compat.h"

static constexpr quint32 font_glyph_magic_number = 0x92588c12;
static constexpr quint32 font_face_magic_number = 0x03f59a82;

static constexpr quint32 font_glyph_version = 1;
static constexpr quint32 font_face_version = 1;

QDataStream& operator<<(QDataStream& s, const Font::Glyph& glyph)
{
    s << font_glyph_magic_number;
    s << font_glyph_version;
    s.setVersion(QDataStream::Qt_5_7);
    s << (quint32) glyph.size().width;
    s << (quint32) glyph.size().height;
    s << glyph.pixels();

    return s;
}

QDataStream& operator>>(QDataStream& s, Font::Glyph& glyph)
{
    quint32 magic_number;
    quint32 version;
    s >> magic_number >> version;
    if (magic_number == font_glyph_magic_number && version == font_glyph_version) {
        s.setVersion(QDataStream::Qt_5_7);
        quint32 width, height;
        s >> width >> height;

        std::vector<bool> pixels;
        pixels.reserve(width * height);
        s >> pixels;

        glyph = Font::Glyph({width, height}, pixels);
    }

    return s;
}

QDataStream& operator<<(QDataStream& s, const Font::Face& face)
{
    s << font_face_magic_number;
    s << font_face_version;
    s.setVersion(QDataStream::Qt_5_7);
    s << (quint32) face.glyph_size().width;
    s << (quint32) face.glyph_size().height;
    s << face.glyphs();

    return s;

}

QDataStream& operator>>(QDataStream& s, Font::Face& face)
{
    quint32 magic_number;
    quint32 version;
    s >> magic_number >> version;
    if (magic_number == font_face_magic_number && version == font_face_version) {
        s.setVersion(QDataStream::Qt_5_7);
        quint32 width, height;
        s >> width >> height;

        std::vector<Font::Glyph> glyphs;
        s >> glyphs;

        face = Font::Face({width, height}, glyphs);
    }

    return s;
}
