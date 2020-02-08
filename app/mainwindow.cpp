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
    ui_->actionImport_Font->setEnabled(actionsState[MainWindowAction::ActionImportFont]);
    ui_->actionAdd_Glyph->setEnabled(actionsState[MainWindowAction::ActionAddGlyph]);
    ui_->actionSave->setEnabled(actionsState[MainWindowAction::ActionSave]);
    ui_->actionCopy_Glyph->setEnabled(actionsState[MainWindowAction::ActionCopy]);
    ui_->actionPaste_Glyph->setEnabled(actionsState[MainWindowAction::ActionPaste]);
    ui_->actionUndo->setEnabled(actionsState[MainWindowAction::ActionUndo]);
    ui_->actionRedo->setEnabled(actionsState[MainWindowAction::ActionRedo]);
    ui_->actionReset_Glyph->setEnabled(actionsState[MainWindowAction::ActionResetGlyph]);
    ui_->actionReset_Font->setEnabled(actionsState[MainWindowAction::ActionResetFont]);
    ui_->actionExport->setEnabled(actionsState[MainWindowAction::ActionExport]);
    ui_->actionPrint->setEnabled(actionsState[MainWindowAction::ActionPrint]);
}

void MainWindow::displayFace(const Font::Face &face)
{
    if (faceWidget_ == nullptr) {
        faceWidget_ = new FaceWidget();
        ui_->faceGraphicsView->scene()->addItem(faceWidget_);

        connect(faceWidget_, &FaceWidget::currentGlyphIndexChanged,
                [&] (std::size_t index) {
            viewModel_->faceModel()->set_active_glyph_index(index);
            viewModel_->setState(MainWindowModel::WithGlyph);
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
            viewModel_->setState(MainWindowModel::WithEditedGlyph);
        });
    }

    glyphWidget_->load(glyph);
    ui_->glyphGraphicsView->fitInView(glyphWidget_->rect(), Qt::KeepAspectRatio);
}
