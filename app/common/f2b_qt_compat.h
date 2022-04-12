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
#include <QVariant>
#include <optional>
#include <vector>
#include <unordered_map>
#include <set>

namespace f2b {

namespace font {

inline font::point point_with_qpoint(const QPoint &p)
{
    return { static_cast<std::size_t>(qMax(0, p.x())),
                static_cast<std::size_t>(qMax(0, p.y())) };
}

inline QPoint qpoint_with_point(const font::point &p)
{
    return QPoint { static_cast<int>(p.x), static_cast<int>(p.y) };
}

inline font::glyph_size size_with_qsize(const QSize &s)
{
    return { static_cast<std::size_t>(qMax(0, s.width())),
                static_cast<std::size_t>(qMax(0, s.height())) };
}

inline QSize qsize_with_size(const font::glyph_size &s)
{
    return QSize { static_cast<int>(s.width), static_cast<int>(s.height) };
}

inline QImage glyph_preview_image(const font::glyph &g, font::margins m)
{
    auto useful_glyph_size = g.size();
    useful_glyph_size.height -= m.top + m.bottom;

    auto useful_image_size = qsize_with_size(useful_glyph_size);

    QImage image(useful_image_size, QImage::Format_Mono);
    image.fill(1);

    for (std::vector<bool>::size_type y = 0; y < useful_glyph_size.height; ++y) {
        for (std::vector<bool>::size_type x = 0; x < useful_glyph_size.width; ++x) {
            if (g.is_pixel_set({x, y + m.top})) {
                image.setPixel(x, y, 0);
            }
        }
    }

    return image;
}

} // namespace Font

} // namespace f2b


static constexpr quint32 std_optional_magic_number = 0x46b13680;
static constexpr quint32 std_vector_magic_number = 0x30612113;
static constexpr quint32 std_set_magic_number = 0x254c2e1e;
static constexpr quint32 std_unordered_map_magic_number = 0xc9eb6edf;

static constexpr quint32 std_optional_version = 1;
static constexpr quint32 std_vector_version = 1;
static constexpr quint32 std_set_version = 1;
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


template<typename T>
inline QDataStream& operator<<(QDataStream& s, const std::set<T>& set)
{
    s << std_set_magic_number;
    s << std_set_version;
    s.setVersion(QDataStream::Qt_5_7);
    s << (quint32) set.size();

    for (const auto& value : set) {
        s << value;
    }

    return s;
}

template<typename T>
inline QDataStream& operator>>(QDataStream& s, std::set<T>& set)
{
    quint32 magic_number;
    quint32 version;
    s >> magic_number >> version;
    if (magic_number == std_set_magic_number && version == std_set_version) {
        s.setVersion(QDataStream::Qt_5_7);
        quint32 size;
        s >> size;

        set.clear();

        T value;
        for (quint32 i = 0; i < size; ++i) {
            s >> value;
            set.insert(value);
        }
    }
    return s;
}

template<>
inline QDataStream& operator<<(QDataStream& s, const std::set<std::size_t>& set)
{
    s << std_set_magic_number;
    s << std_set_version;
    s.setVersion(QDataStream::Qt_5_7);
    s << (quint32) set.size();

    for (const auto& value : set) {
        s << (quint32) value;
    }

    return s;
}

template<>
inline QDataStream& operator>>(QDataStream& s, std::set<std::size_t>& set)
{
    quint32 magic_number;
    quint32 version;
    s >> magic_number >> version;
    if (magic_number == std_set_magic_number && version == std_set_version) {
        s.setVersion(QDataStream::Qt_5_7);
        quint32 size;
        s >> size;

        set.clear();

        quint32 value;
        for (quint32 i = 0; i < size; ++i) {
            s >> value;
            set.insert(static_cast<std::size_t>(value));
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

QDataStream& operator<<(QDataStream& s, const f2b::font::glyph& glyph);
QDataStream& operator>>(QDataStream& s, f2b::font::glyph& glyph);

QDataStream& operator<<(QDataStream& s, const f2b::font::face& face);
QDataStream& operator>>(QDataStream& s, f2b::font::face& face);


QVariant to_qvariant(const f2b::source_code::indentation& i);
f2b::source_code::indentation from_qvariant(const QVariant& v);

#endif // F2B_QT_COMPAT_H
