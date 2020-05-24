#include "gtest/gtest.h"
#include "f2b_qt_compat.h"

#include <optional>
#include <vector>
#include <set>
#include <unordered_map>

#include <QByteArray>
#include <QBuffer>
#include <QDataStream>
#include <QString>

using namespace f2b;

template<typename T>
void serialize_and_deserialize(const T& input, T& output)
{
    QByteArray b;
    QBuffer buf(&b);
    QDataStream s(&buf);

    buf.open(QIODevice::WriteOnly);
    s << input;
    buf.close();

    buf.open(QIODevice::ReadOnly);
    s >> output;
    buf.close();
}

TEST(SerializationTest, optional_int)
{
    std::optional<int> data {5};
    std::optional<int> deserialized;

    serialize_and_deserialize(data, deserialized);
    EXPECT_TRUE(deserialized.has_value());
    EXPECT_EQ(deserialized.value(), data.value());

    data = {};
    serialize_and_deserialize(data, deserialized);
    EXPECT_FALSE(deserialized.has_value());
}

TEST(SerializationTest, optional_qstring)
{
    std::optional<QString> data { "A quick brown fox jumps over a lazy dog." };
    std::optional<QString> deserialized;

    serialize_and_deserialize(data, deserialized);

    EXPECT_TRUE(deserialized.has_value());
    EXPECT_EQ(deserialized.value(), data.value());
}

TEST(SerializationTest, vector_bool)
{
    std::vector<bool> data;
    std::vector<bool> deserialized = { true, true, false };

    serialize_and_deserialize(data, deserialized);
    EXPECT_TRUE(deserialized.empty());
    EXPECT_EQ(data, deserialized);


    data = { false, false, false, true, true, false, true, false, true, true, true };

    serialize_and_deserialize(data, deserialized);
    EXPECT_EQ(data, deserialized);
}

TEST(SerializationTest, set_size_t)
{
    std::set<std::size_t> data;
    std::set<std::size_t> deserialized = { 1, 2, 3 };

    serialize_and_deserialize(data, deserialized);
    EXPECT_TRUE(deserialized.empty());
    EXPECT_EQ(data, deserialized);


    data = { 100, 152, 3, 6, 17, 0x541882, 1500, 4, 2, 8 };

    serialize_and_deserialize(data, deserialized);
    EXPECT_EQ(data, deserialized);
}

TEST(SerializationTest, unordered_map_int_qstring)
{
    std::unordered_map<int,QString> data;
    std::unordered_map<int,QString> deserialized = { { 5, "boo" } };

    serialize_and_deserialize(data, deserialized);
    EXPECT_TRUE(deserialized.empty());
    EXPECT_EQ(data, deserialized);


    data = {
        { 0, "foo" },
        { 100, "bar" },
        { -50, "baz" }
    };

    serialize_and_deserialize(data, deserialized);
    EXPECT_EQ(data, deserialized);
}

TEST(SerializationTest, font_glyph)
{
    Font::Glyph data({2, 3}, { false, true, false, false, true, true });
    Font::Glyph deserialized;

    serialize_and_deserialize(data, deserialized);

    EXPECT_EQ(data, deserialized);
}

TEST(SerializationTest, font_face)
{
    Font::Glyph glyph({2, 3}, { false, true, false, false, true, true });
    Font::Face data({2, 3}, { glyph, glyph, glyph, glyph });
    Font::Face deserialized;

    serialize_and_deserialize(data, deserialized);

    EXPECT_EQ(data, deserialized);
}
