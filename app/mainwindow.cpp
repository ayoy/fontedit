#include "mainwindow.h"
#include "./ui_mainwindow.h"
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
    , ui {new Ui::MainWindow}
{
    ui->setupUi(this);
    setupActions();

    faceScene_->setBackgroundBrush(QBrush(Qt::lightGray));
    ui->faceGraphicsView->setScene(faceScene_.get());

    connect(viewModel_.get(), &MainWindowModel::actionsChanged, this, &MainWindow::updateActions);
    updateActions(viewModel_->menuActions());

    connect(ui->actionNew, &QAction::triggered, [=] () {
        qDebug() << "new";
    });

    connect(ui->actionImport_Font, &QAction::triggered, [&] () {
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
    ui->importFontButton->setDefaultAction(ui->actionImport_Font);
    ui->addGlyphButton->setDefaultAction(ui->actionAdd_Glyph);
    ui->saveButton->setDefaultAction(ui->actionSave);
    ui->copyButton->setDefaultAction(ui->actionCopy_Glyph);
    ui->pasteButton->setDefaultAction(ui->actionPaste_Glyph);
    ui->undoButton->setDefaultAction(ui->actionUndo);
    ui->redoButton->setDefaultAction(ui->actionRedo);
    ui->resetGlyphButton->setDefaultAction(ui->actionReset_Glyph);
    ui->resetFontButton->setDefaultAction(ui->actionReset_Font);
    ui->exportButton->setDefaultAction(ui->actionExport);
    ui->printButton->setDefaultAction(ui->actionPrint);
}

void MainWindow::updateActions(MainWindowModel::ActionsState actionsState)
{
    ui->actionImport_Font->setEnabled(actionsState[MainWindowAction::ActionImportFont]);
    ui->actionAdd_Glyph->setEnabled(actionsState[MainWindowAction::ActionAddGlyph]);
    ui->actionSave->setEnabled(actionsState[MainWindowAction::ActionSave]);
    ui->actionCopy_Glyph->setEnabled(actionsState[MainWindowAction::ActionCopy]);
    ui->actionPaste_Glyph->setEnabled(actionsState[MainWindowAction::ActionPaste]);
    ui->actionUndo->setEnabled(actionsState[MainWindowAction::ActionUndo]);
    ui->actionRedo->setEnabled(actionsState[MainWindowAction::ActionRedo]);
    ui->actionReset_Glyph->setEnabled(actionsState[MainWindowAction::ActionResetGlyph]);
    ui->actionReset_Font->setEnabled(actionsState[MainWindowAction::ActionResetFont]);
    ui->actionExport->setEnabled(actionsState[MainWindowAction::ActionExport]);
    ui->actionPrint->setEnabled(actionsState[MainWindowAction::ActionPrint]);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::displayFace(const Font::Face &face)
{
    if (faceWidget_ == nullptr) {
        faceWidget_ = new FaceWidget();
        ui->faceGraphicsView->scene()->addItem(faceWidget_);

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
        ui->glyphGraphicsView->scene()->addItem(glyphWidget_.get());

        connect(glyphWidget_.get(), &GlyphWidget::pixelChanged,
                [&] (Font::Point p, bool is_selected) {
            viewModel_->faceModel()->active_glyph().set_pixel_set(p, is_selected);
            viewModel_->setState(MainWindowModel::WithEditedGlyph);
        });
    }

    glyphWidget_->load(glyph);
    ui->glyphGraphicsView->fitInView(glyphWidget_->rect(), Qt::KeepAspectRatio);
}
