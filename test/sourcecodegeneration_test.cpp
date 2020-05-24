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

class TestSourceCodeGenerator : public font_source_code_generator
{
public:
    TestSourceCodeGenerator(source_code_options options): font_source_code_generator(options) {};

    std::string current_timestamp() override {
        return "<timestamp>";
    }
};

TEST(SourceCodeGeneratorTest, Generator)
{
    auto faceFileName = asset("monaco8.fontedit");
    auto faceVM = std::make_unique<FontFaceViewModel>(faceFileName);

    font::glyph_size expectedSize {5, 11};
    font::margins expectedMargins {1, 2};

    EXPECT_EQ(faceVM->face().num_glyphs(), 95);
    EXPECT_EQ(faceVM->face().glyphs_size(), expectedSize);
    EXPECT_EQ(faceVM->face().calculate_margins(), expectedMargins);

    TestSourceCodeGenerator g(source_code_options{});


    auto sourceCode = g.generate<format::c>(faceVM->face());

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

    font_source_code_generator g(source_code_options{});

    std::vector<std::chrono::high_resolution_clock::rep> durations;
    for (int i = 0; i < 5; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        g.generate<format::c>(faceVM->face());
        auto end = std::chrono::high_resolution_clock::now();
        durations.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count());
    }
    for (auto& d : durations) {
        std::cout << d << " ";
    }
    std::cout << "(mean: " << std::accumulate(durations.begin(), durations.end(), 0) / durations.size() << ")";
    std::cout << std::endl;
}
