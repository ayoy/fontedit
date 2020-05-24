#include "gtest/gtest.h"
#include "fontsourcecodegenerator.h"
#include "fontfaceviewmodel.h"

#include <chrono>
#include <iostream>
#include <vector>
#include <numeric>

#include <QString>
#include <QFile>

using namespace f2b;

template<typename T>
QString asset(const T& fileName)
{
    return QString("%1/%2").arg(ASSETS_DIR, fileName);
}

class TestSourceCodeGenerator : public FontSourceCodeGenerator
{
public:
    TestSourceCodeGenerator(SourceCodeOptions options): FontSourceCodeGenerator(options) {};

    std::string current_timestamp() override {
        return "<timestamp>";
    }
};

TEST(SourceCodeGeneratorTest, Generator)
{
    auto faceFileName = asset("monaco8.fontedit");
    auto faceVM = std::make_unique<FontFaceViewModel>(faceFileName);

    Font::Size expectedSize {5, 11};
    Font::Margins expectedMargins {1, 2};

    EXPECT_EQ(faceVM->face().num_glyphs(), 95);
    EXPECT_EQ(faceVM->face().glyph_size(), expectedSize);
    EXPECT_EQ(faceVM->face().calculate_margins(), expectedMargins);

    TestSourceCodeGenerator g(SourceCodeOptions{});


    auto sourceCode = g.generate<Format::C>(faceVM->face());

    auto sourceCodeFileName = asset("monaco8.c-test");
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

    FontSourceCodeGenerator g(SourceCodeOptions{});

    std::vector<std::chrono::high_resolution_clock::rep> durations;
    for (int i = 0; i < 5; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        g.generate<Format::C>(faceVM->face());
        auto end = std::chrono::high_resolution_clock::now();
        durations.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count());
    }
    for (auto& d : durations) {
        std::cout << d << " ";
    }
    std::cout << "(mean: " << std::accumulate(durations.begin(), durations.end(), 0) / durations.size() << ")";
    std::cout << std::endl;
}
