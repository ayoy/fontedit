#ifndef F2B_QT_COMPAT_H
#define F2B_QT_COMPAT_H

#include "f2b.h"
#include <QPoint>
#include <QSize>
#include <QBitmap>
#include <QPainter>
#include <QDebug>
#include "common.h"

#include <QDataStream>
#include <optional>
#include <vector>
#include <unordered_map>

namespace Font {

inline Font::Point point_with_qpoint(const QPoint &p)
{
    return { static_cast<std::size_t>(qMax(0, p.x())),
                static_cast<std::size_t>(qMax(0, p.y())) };
}

inline QPoint qpoint_with_point(const Font::Point &p)
{
    return QPoint { static_cast<int>(p.x), static_cast<int>(p.y) };
}

inline QSize qsize_with_size(const Font::Size &s)
{
    return QSize { static_cast<int>(s.width), static_cast<int>(s.height) };
}

inline QPixmap glyph_preview_pixmap(const Font::Glyph &g, Font::Margins m)
{
    auto useful_glyph_size = g.size();
    useful_glyph_size.height -= m.top + m.bottom;

    auto image_size = qsize_with_size(g.size());
    auto useful_image_size = qsize_with_size(useful_glyph_size);

    QImage image(useful_image_size, QImage::Format_Grayscale8);
    image.fill(Qt::white);

    for (std::vector<bool>::size_type y = 0; y < useful_glyph_size.height; ++y) {
        for (std::vector<bool>::size_type x = 0; x < useful_glyph_size.width; ++x) {
            if (g.is_pixel_set({x, y + m.top})) {
                image.setPixel(x, y, Qt::black);
            }
        }
    }

    QPixmap b(image_size);
    QPainter p(&b);
    p.fillRect(b.rect(), Color::glyphMargin);
    p.drawImage(QPoint(0, m.top), image);
    p.end();

    return b;
}

}


static constexpr quint32 std_optional_magic_number = 0x46b13680;
static constexpr quint32 std_vector_magic_number = 0x30612113;
static constexpr quint32 std_unordered_map_magic_number = 0xc9eb6edf;

static constexpr quint32 std_optional_version = 1;
static constexpr quint32 std_vector_version = 1;
static constexpr quint32 std_unordered_map_version = 1;

template<typename T>
inline QDataStream& operator<<(QDataStream& s, const std::optional<T>& opt)
{
    s << std_optional_magic_number;
    s << std_optional_version;
    s.setVersion(QDataStream::Qt_5_7);
    if (opt.has_value()) {
        s << true << QVariant(opt.value());
    } else {
        s << false;
    }
    return s;
}

template<typename T>
inline QDataStream& operator>>(QDataStream& s, std::optional<T>& opt)
{
    quint32 magic_number;
    quint32 version;
    s >> magic_number >> version;
    if (magic_number == std_optional_magic_number && version == std_optional_version) {
        s.setVersion(QDataStream::Qt_5_7);
        bool has_value;
        s >> has_value;

        QVariant value;

        if (has_value) {
            s >> value;
            opt = value.value<T>();
        } else {
            opt = {};
        }
    }

    return s;
}


template<typename T>
inline QDataStream& operator<<(QDataStream& s, const std::vector<T>& vec)
{
    s << std_vector_magic_number;
    s << std_vector_version;
    s.setVersion(QDataStream::Qt_5_7);
    s << (quint32) vec.size();

    for (const auto& value : vec) {
        s << value;
    }

    return s;
}

template<typename T>
inline QDataStream& operator>>(QDataStream& s, std::vector<T>& vec)
{
    quint32 magic_number;
    quint32 version;
    s >> magic_number >> version;
    if (magic_number == std_vector_magic_number && version == std_vector_version) {
        s.setVersion(QDataStream::Qt_5_7);
        quint32 size;
        s >> size;

        vec.clear();
        vec.reserve(size);

        T value;
        for (quint32 i = 0; i < size; ++i) {
            s >> value;
            vec.push_back(value);
        }
    }
    return s;
}


template<typename K, typename V>
inline QDataStream& operator<<(QDataStream& s, const std::unordered_map<K, V>& map)
{
    s << std_unordered_map_magic_number;
    s << std_unordered_map_version;
    s.setVersion(QDataStream::Qt_5_7);
    s << (quint32) map.size();

    for (const auto& [k,v] : map) {
        s << k;
        s << v;
    }

    return s;
}

template<typename V>
inline QDataStream& operator<<(QDataStream& s, const std::unordered_map<std::size_t, V>& map)
{
    s << std_unordered_map_magic_number;
    s << std_unordered_map_version;
    s.setVersion(QDataStream::Qt_5_7);
    s << (quint32) map.size();

    for (const auto& [k,v] : map) {
        s << (quint32) k;
        s << v;
    }

    return s;
}

template<typename K, typename V>
inline QDataStream& operator>>(QDataStream& s, std::unordered_map<K, V>& map)
{
    quint32 magic_number;
    quint32 version;
    s >> magic_number >> version;
    if (magic_number == std_unordered_map_magic_number && version == std_unordered_map_version) {
        s.setVersion(QDataStream::Qt_5_7);
        quint32 size;
        s >> size;

        map.clear();
        map.reserve(size);

        K key;
        V value;
        for (quint32 i = 0; i < size; ++i) {
            s >> key;
            s >> value;
            map[key] = value;
        }
    }
    return s;
}

template<typename V>
inline QDataStream& operator>>(QDataStream& s, std::unordered_map<std::size_t, V>& map)
{
    quint32 magic_number;
    quint32 version;
    s >> magic_number >> version;
    if (magic_number == std_unordered_map_magic_number && version == std_unordered_map_version) {
        s.setVersion(QDataStream::Qt_5_7);
        quint32 size;
        s >> size;

        map.clear();
        map.reserve(size);

        quint32 key;
        V value;
        for (quint32 i = 0; i < size; ++i) {
            s >> key;
            s >> value;
            map[static_cast<std::size_t>(key)] = value;
        }
    }
    return s;
}

QDataStream& operator<<(QDataStream& s, const Font::Glyph& glyph);
QDataStream& operator>>(QDataStream& s, Font::Glyph& glyph);

QDataStream& operator<<(QDataStream& s, const Font::Face& face);
QDataStream& operator>>(QDataStream& s, Font::Face& face);


QVariant to_qvariant(const SourceCode::Indentation& i);
SourceCode::Indentation from_qvariant(const QVariant& v);

#endif // F2B_QT_COMPAT_H
