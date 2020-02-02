#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include "fontfaceviewmodel.h"
#include "facewidget.h"
#include "glyphwidget.h"

#include <optional>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void displayFace(const Font::Face &face);
    void displayGlyph(const Font::Glyph &glyph);
    void setupViewModel(FontFaceViewModel &&viewModel);

    Ui::MainWindow *ui;

    std::unique_ptr<GlyphWidget> glyph_widget_ { nullptr };
    FaceWidget *face_widget_ { nullptr };
    std::optional<FontFaceViewModel> view_model_ { std::nullopt };
    std::unique_ptr<QGraphicsScene> face_scene_ { std::make_unique<QGraphicsScene>() };
};

#endif // MAINWINDOW_H
