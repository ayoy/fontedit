#include "gtest/gtest.h"
#include "fontsourcecodegenerator.h"
#include "fontfaceviewmodel.h"
#include <gsl/gsl>

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

class test_source_code_generator : public font_source_code_generator
{
public:
    test_source_code_generator(source_code_options options): font_source_code_generator(options) {};

    std::string current_timestamp() override {
        return "<timestamp>";
    }
};

TEST(SourceCodeGeneratorTest, GeneratorAll)
{
    auto faceFileName = asset("monaco8.fontedit");
    auto faceVM = std::make_unique<FontFaceViewModel>(faceFileName);

    font::glyph_size expectedSize {5, 11};
    font::margins expectedMargins {1, 2};

    EXPECT_EQ(faceVM->face().num_glyphs(), 95);
    EXPECT_EQ(faceVM->face().glyphs_size(), expectedSize);
    EXPECT_EQ(faceVM->face().calculate_margins(), expectedMargins);

    source_code_options options;
    options.export_method = f2b::source_code_options::export_all;

    test_source_code_generator g(options);


    auto sourceCode = g.generate<format::c>(faceVM->face());

    auto sourceCodeFileName = asset("monaco8.c-test");
    QFile f(sourceCodeFileName);
    f.open(QFileDevice::ReadOnly);
    auto referenceSourceCode = f.readAll().toStdString();
    f.close();

    ASSERT_EQ(sourceCode, referenceSourceCode);
}

TEST(SourceCodeGeneratorTest, GeneratorSubset)
{
    auto faceFileName = asset("monaco8.fontedit");
    auto faceVM = std::make_unique<FontFaceViewModel>(faceFileName);

    font::glyph_size expectedSize {5, 11};
    font::margins expectedMargins {1, 2};

    EXPECT_EQ(faceVM->face().num_glyphs(), 95);
    EXPECT_EQ(faceVM->face().glyphs_size(), expectedSize);
    EXPECT_EQ(faceVM->face().calculate_margins(), expectedMargins);

    source_code_options options;
    options.export_method = f2b::source_code_options::export_selected;
    options.indentation = f2b::source_code::space { 4 };

    test_source_code_generator g(options);


    auto sourceCode = g.generate<format::c>(faceVM->face());

    auto sourceCodeFileName = asset("monaco8-subset.c-test");
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

    f2b::source_code_options::export_method_type export_methods[] = {
        f2b::source_code_options::export_selected,
        f2b::source_code_options::export_all
    };

    for (auto export_method : gsl::span(export_methods)) {
        source_code_options options;
        options.export_method = export_method;
        font_source_code_generator g(std::move(options));

        std::vector<std::chrono::high_resolution_clock::rep> durations;
        for (int i = 0; i < 5; ++i) {
            auto start = std::chrono::high_resolution_clock::now();
            g.generate<format::c>(faceVM->face());
            auto end = std::chrono::high_resolution_clock::now();
            durations.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count());
        }
        std::cout << "Export Method " << export_method << ": ";
        for (auto& d : durations) {
            std::cout << d << " ";
        }
        std::cout << "(mean: " << std::accumulate(durations.begin(), durations.end(), 0) / durations.size() << ")";
        std::cout << std::endl;
    }

}
