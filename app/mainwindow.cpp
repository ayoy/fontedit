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

#include <iostream>

static constexpr auto pixel_size = 30;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui_->setupUi(this);
    setupActions();

    faceScene_->setBackgroundBrush(QBrush(Qt::lightGray));
    ui_->faceGraphicsView->setScene(faceScene_.get());

    connect(viewModel_.get(), &MainWindowModel::actionsChanged, this, &MainWindow::updateActions);
    updateActions(viewModel_->menuActions());

    connect(ui_->actionQuit, &QAction::triggered, &QApplication::quit);

    connect(ui_->actionNew, &QAction::triggered, [=] () {
        qDebug() << "new";
    });

    connect(ui_->actionImport_Font, &QAction::triggered, [&] () {
        bool ok;
        QFont f("Monaco", 24);
        f.setStyleHint(QFont::TypeWriter);
        f = QFontDialog::getFont(&ok, f, this, tr("Select Font"), QFontDialog::MonospacedFonts | QFontDialog::DontUseNativeDialog);
        qDebug() << "selected font:" << f << "ok?" << ok;

        viewModel_->loadFont(f);
        displayFace(viewModel_->faceModel()->face());
    });
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
    ui_->exportButton->setDefaultAction(ui_->actionExport);
    ui_->printButton->setDefaultAction(ui_->actionPrint);
}

void MainWindow::updateActions(MainWindowModel::ActionsState actionsState)
{
    ui_->actionImport_Font->setEnabled(actionsState[MainWindowModel::ButtonAction::ActionImportFont]);
    ui_->actionAdd_Glyph->setEnabled(actionsState[MainWindowModel::ButtonAction::ActionAddGlyph]);
    ui_->actionSave->setEnabled(actionsState[MainWindowModel::ButtonAction::ActionSave]);
    ui_->actionCopy_Glyph->setEnabled(actionsState[MainWindowModel::ButtonAction::ActionCopy]);
    ui_->actionPaste_Glyph->setEnabled(actionsState[MainWindowModel::ButtonAction::ActionPaste]);
    ui_->actionUndo->setEnabled(actionsState[MainWindowModel::ButtonAction::ActionUndo]);
    ui_->actionRedo->setEnabled(actionsState[MainWindowModel::ButtonAction::ActionRedo]);
    ui_->actionReset_Glyph->setEnabled(actionsState[MainWindowModel::ButtonAction::ActionResetGlyph]);
    ui_->actionReset_Font->setEnabled(actionsState[MainWindowModel::ButtonAction::ActionResetFont]);
    ui_->actionExport->setEnabled(actionsState[MainWindowModel::ButtonAction::ActionExport]);
    ui_->actionPrint->setEnabled(actionsState[MainWindowModel::ButtonAction::ActionPrint]);
}

void MainWindow::displayFace(const Font::Face &face)
{
    if (faceWidget_ == nullptr) {
        faceWidget_ = new FaceWidget();
        ui_->faceGraphicsView->scene()->addItem(faceWidget_);

        connect(faceWidget_, &FaceWidget::currentGlyphIndexChanged,
                [&] (std::size_t index) {
            viewModel_->faceModel()->set_active_glyph_index(index);
            viewModel_->registerInputEvent(MainWindowModel::UserLoadedGlyph);
            displayGlyph(viewModel_->faceModel()->active_glyph());
        });
    }

    faceWidget_->load(face);
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
