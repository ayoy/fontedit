#include "mainwindow.h"
#include "f2b.h"
#include "facewidget.h"
#include "fontfaceviewmodel.h"
#include "glyphwidget.h"

#include <QGraphicsGridLayout>
#include <QGraphicsWidget>
#include <QDebug>
#include <QStyle>
#include <QFontDialog>
#include <QScrollBar>

#include <iostream>

static constexpr auto pixel_size = 30;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui_->setupUi(this);
    setupUI();
    setupActions();

    updateUI(viewModel_->uiState());

    connect(viewModel_.get(), &MainWindowModel::uiStateChanged, this, &MainWindow::updateUI);
    connect(viewModel_.get(), &MainWindowModel::faceLoaded, this, &MainWindow::displayFace);
    connect(viewModel_.get(), &MainWindowModel::activeGlyphChanged, this, &MainWindow::displayGlyph);

    connect(ui_->actionImport_Font, &QAction::triggered, this, &MainWindow::showFontDialog);
    connect(ui_->actionQuit, &QAction::triggered, &QApplication::quit);
}

void MainWindow::setupUI()
{
    faceScene_->setBackgroundBrush(QBrush(Qt::lightGray));
    ui_->faceGraphicsView->setScene(faceScene_.get());

    auto scrollBarWidth = ui_->faceGraphicsView->verticalScrollBar()->sizeHint().width();
    auto faceViewWidth = static_cast<int>(FaceWidget::cell_width) * 3 + scrollBarWidth;
    ui_->faceGraphicsView->setMinimumSize({ faceViewWidth,
                                            ui_->faceGraphicsView->minimumSize().height() });
}

void MainWindow::setupActions()
{
    ui_->importFontButton->setDefaultAction(ui_->actionImport_Font);
    ui_->addGlyphButton->setDefaultAction(ui_->actionAdd_Glyph);
    ui_->saveButton->setDefaultAction(ui_->actionSave);
    ui_->copyButton->setDefaultAction(ui_->actionCopy_Glyph);
    ui_->pasteButton->setDefaultAction(ui_->actionPaste_Glyph);
    ui_->undoButton->setDefaultAction(ui_->actionUndo);
    ui_->redoButton->setDefaultAction(ui_->actionRedo);
    ui_->resetGlyphButton->setDefaultAction(ui_->actionReset_Glyph);
    ui_->resetFontButton->setDefaultAction(ui_->actionReset_Font);
}

void MainWindow::updateUI(MainWindowModel::UIState uiState)
{
    ui_->tabWidget->setTabEnabled(1, uiState[MainWindowModel::InterfaceAction::ActionTabCode]);
    ui_->actionImport_Font->setEnabled(uiState[MainWindowModel::InterfaceAction::ActionImportFont]);
    ui_->actionAdd_Glyph->setEnabled(uiState[MainWindowModel::InterfaceAction::ActionAddGlyph]);
    ui_->actionSave->setEnabled(uiState[MainWindowModel::InterfaceAction::ActionSave]);
    ui_->actionCopy_Glyph->setEnabled(uiState[MainWindowModel::InterfaceAction::ActionCopy]);
    ui_->actionPaste_Glyph->setEnabled(uiState[MainWindowModel::InterfaceAction::ActionPaste]);
    ui_->actionUndo->setEnabled(uiState[MainWindowModel::InterfaceAction::ActionUndo]);
    ui_->actionRedo->setEnabled(uiState[MainWindowModel::InterfaceAction::ActionRedo]);
    ui_->actionReset_Glyph->setEnabled(uiState[MainWindowModel::InterfaceAction::ActionResetGlyph]);
    ui_->actionReset_Font->setEnabled(uiState[MainWindowModel::InterfaceAction::ActionResetFont]);
    ui_->actionExport->setEnabled(uiState[MainWindowModel::InterfaceAction::ActionExport]);
    ui_->actionPrint->setEnabled(uiState[MainWindowModel::InterfaceAction::ActionPrint]);
}

void MainWindow::showFontDialog()
{
    bool ok;
    QFont f("Monaco", 24);
    f.setStyleHint(QFont::TypeWriter);
    f = QFontDialog::getFont(&ok, f, this, tr("Select Font"), QFontDialog::MonospacedFonts | QFontDialog::DontUseNativeDialog);
    qDebug() << "selected font:" << f << "ok?" << ok;

//    viewModel_->registerInputEvent(MainWindowModel::ActionImportFont);
    viewModel_->importFont(f);
}

void MainWindow::displayFace(const Font::Face &face)
{
    if (faceWidget_ == nullptr) {
        faceWidget_ = new FaceWidget();
        ui_->faceGraphicsView->scene()->addItem(faceWidget_);

        connect(faceWidget_, &FaceWidget::currentGlyphIndexChanged,
                viewModel_.get(), &MainWindowModel::setActiveGlyphIndex);
    }

    faceWidget_->load(face);

    if (glyphWidget_.get()) {
        glyphWidget_->clear();
    }
}

void MainWindow::displayGlyph(const Font::Glyph &glyph)
{
    if (!glyphWidget_.get()) {
        glyphWidget_ = std::make_unique<GlyphWidget>(pixel_size);
        ui_->glyphGraphicsView->scene()->addItem(glyphWidget_.get());

        connect(glyphWidget_.get(), &GlyphWidget::pixelChanged,
                [&] (Font::Point p, bool is_selected) {
            viewModel_->faceModel()->active_glyph().set_pixel_set(p, is_selected);
            viewModel_->registerInputEvent(MainWindowModel::UserEditedGlyph);
        });
    }

    glyphWidget_->load(glyph);
    ui_->glyphGraphicsView->fitInView(glyphWidget_->rect(), Qt::KeepAspectRatio);
}
