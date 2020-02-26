#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QUndoStack>

#include "./ui_mainwindow.h"
#include "mainwindowmodel.h"
#include "facewidget.h"
#include "glyphwidget.h"
#include "batchpixelchange.h"

#include <memory>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void displayFace(const Font::Face& face);

private:
    void setupUI();
    void setupActions();

    void showFontDialog();
    void showOpenFaceDialog();
    void saveOrSaveAs();
    void resetCurrentGlyph();
    void resetFont();
    void displayGlyph(const Font::Glyph& glyph);
    void updateUI(MainWindowModel::UIState uiState);
    void editGlyph(const BatchPixelChange& change);
    void switchActiveGlyph(std::size_t newIndex);
    void updateResetActions();
    void updateFaceInfoLabel(const FaceInfo& faceInfo);
    void updateDocumentTitle(const QString& title);

    void exportSourceCode();

    std::unique_ptr<Ui::MainWindow> ui_ { std::make_unique<Ui::MainWindow>() };

    std::unique_ptr<GlyphWidget> glyphWidget_ {};
    FaceWidget *faceWidget_ { nullptr };
    std::unique_ptr<MainWindowModel> viewModel_ { std::make_unique<MainWindowModel>() };
    std::unique_ptr<QGraphicsScene> faceScene_ { std::make_unique<QGraphicsScene>() };
    std::unique_ptr<QUndoStack> undoStack_ { std::make_unique<QUndoStack>() };
};

#endif // MAINWINDOW_H
