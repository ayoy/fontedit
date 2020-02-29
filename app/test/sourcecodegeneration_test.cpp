#include "gtest/gtest.h"
#include "fontsourcecodegenerator.h"
#include "fontfaceviewmodel.h"

#include <QString>
#include <QFile>

template<typename T>
QString asset(const T& fileName)
{
    return QString("%1/%2").arg(ASSETS_DIR, fileName);
}

std::string current_timestamp()
{
    return "<timestamp>";
}

TEST(SourceCodeGeneratorTest, Generator)
{
    auto faceFileName = asset("monaco8.fontedit");
    auto faceVM = std::make_unique<FontFaceViewModel>(faceFileName);

    Font::Size expectedSize {5,11};
    Font::Margins expectedMargins {1, 2};

    ASSERT_EQ(faceVM->face().num_glyphs(), 95);
    ASSERT_EQ(faceVM->face().glyph_size(), expectedSize);
    ASSERT_EQ(faceVM->face().calculate_margins(), expectedMargins);

    FontSourceCodeGenerator g(SourceCodeOptions{});

    auto sourceCode = g.generate<Format::C>(faceVM->face());

    auto sourceCodeFileName = asset("monaco8.c");
    QFile f(sourceCodeFileName);
    f.open(QFileDevice::ReadOnly);
    auto referenceSourceCode = f.readAll().toStdString();
    f.close();

    ASSERT_EQ(sourceCode, referenceSourceCode);
}

TEST(SourceCodeGeneratorTest, GeneratorPerformance)
{
    auto faceFileName = asset("jetbrains260.fontedit");
    auto faceVM = std::make_unique<FontFaceViewModel>(faceFileName);

    ASSERT_EQ(faceVM->face().num_glyphs(), 95);

    FontSourceCodeGenerator g(SourceCodeOptions{});

    g.generate<Format::C>(faceVM->face());
}
