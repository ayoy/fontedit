#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include "fontfaceviewmodel.h"
#include <optional>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class GlyphWidget;
class FaceWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateGlyphPixel(Font::Point pos, bool isSelected);

private:
    void displayGlyph(const Font::Glyph &glyph);
    void setupViewModel(FontFaceViewModel &&viewModel);

    Ui::MainWindow *ui;

    GlyphWidget *glyphWidget_ { nullptr };
    FaceWidget *faceWidget_ { nullptr };
    std::optional<Font::Glyph> glyph_ { std::nullopt };
    std::optional<FontFaceViewModel> viewModel_ { std::nullopt };
};

#endif // MAINWINDOW_H
