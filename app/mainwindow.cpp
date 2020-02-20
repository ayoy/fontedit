#include "mainwindow.h"
#include "f2b.h"
#include "facewidget.h"
#include "fontfaceviewmodel.h"
#include "command.h"

#include <QGraphicsGridLayout>
#include <QGraphicsWidget>
#include <QDebug>
#include <QStyle>
#include <QFontDialog>
#include <QScrollBar>

#include <iostream>

static constexpr auto codeTabIndex = 1;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui_->setupUi(this);
    setupUI();
    setupActions();

    updateUI(viewModel_->uiState());

    connect(ui_->actionImport_Font, &QAction::triggered, this, &MainWindow::showFontDialog);
    connect(ui_->actionReset_Glyph, &QAction::triggered, this, &MainWindow::resetCurrentGlyph);

    connect(ui_->actionQuit, &QAction::triggered, &QApplication::quit);
    connect(ui_->tabWidget, &QTabWidget::currentChanged, [&](int index) {
        if (index == codeTabIndex) {
            viewModel_->prepareSourceCodeTab();
        }
    });
    connect(ui_->invertBitsCheckBox, &QCheckBox::stateChanged, [&](int state) {
        viewModel_->setInvertBits(state == Qt::Checked);
    });
    connect(ui_->bitNumberingCheckBox, &QCheckBox::stateChanged, [&](int state) {
        viewModel_->setMSBEnabled(state == Qt::Checked);
    });
    connect(ui_->formatComboBox, &QComboBox::currentTextChanged,
            viewModel_.get(), &MainWindowModel::setOutputFormat);


    connect(viewModel_.get(), &MainWindowModel::uiStateChanged, this, &MainWindow::updateUI);
    connect(viewModel_.get(), &MainWindowModel::faceLoaded, this, &MainWindow::displayFace);
    connect(viewModel_.get(), &MainWindowModel::activeGlyphChanged, this, &MainWindow::displayGlyph);
    connect(viewModel_.get(), &MainWindowModel::sourceCodeChanged, ui_->sourceCodeTextBrowser, &QTextBrowser::setText);
}

void MainWindow::setupUI()
{
    faceScene_->setBackgroundBrush(QBrush(Qt::lightGray));
    ui_->faceGraphicsView->setScene(faceScene_.get());

    auto scrollBarWidth = ui_->faceGraphicsView->verticalScrollBar()->sizeHint().width();
    auto faceViewWidth = static_cast<int>(FaceWidget::cell_width) * 3 + scrollBarWidth;
    ui_->faceGraphicsView->setMinimumSize({ faceViewWidth,
                                            ui_->faceGraphicsView->minimumSize().height() });

    ui_->invertBitsCheckBox->setCheckState(viewModel_->invertBits());
    ui_->bitNumberingCheckBox->setCheckState(viewModel_->msbEnabled());

    for (const auto &pair : viewModel_->outputFormats().toStdMap()) {
        ui_->formatComboBox->addItem(pair.second, pair.first);
    }
    ui_->formatComboBox->setCurrentText(viewModel_->outputFormat());

    QFont f("Monaco", 13);
    f.setStyleHint(QFont::TypeWriter);
    ui_->sourceCodeTextBrowser->setFont(f);
}

void MainWindow::setupActions()
{
    auto undo = undoStack_->createUndoAction(this);
    undo->setIcon(QIcon {":/toolbar/assets/undo.svg"});

    auto redo = undoStack_->createRedoAction(this);
    redo->setIcon(QIcon {":/toolbar/assets/redo.svg"});

    ui_->importFontButton->setDefaultAction(ui_->actionImport_Font);
    ui_->addGlyphButton->setDefaultAction(ui_->actionAdd_Glyph);
    ui_->saveButton->setDefaultAction(ui_->actionSave);
    ui_->copyButton->setDefaultAction(ui_->actionCopy_Glyph);
    ui_->pasteButton->setDefaultAction(ui_->actionPaste_Glyph);
    ui_->undoButton->setDefaultAction(undo);
    ui_->redoButton->setDefaultAction(redo);
    ui_->resetGlyphButton->setDefaultAction(ui_->actionReset_Glyph);
    ui_->resetFontButton->setDefaultAction(ui_->actionReset_Font);

    ui_->menuEdit->insertAction(ui_->actionCopy_Glyph, undo);
    ui_->menuEdit->insertAction(ui_->actionCopy_Glyph, redo);
}

void MainWindow::updateUI(MainWindowModel::UIState uiState)
{
    ui_->tabWidget->setTabEnabled(1, uiState[MainWindowModel::InterfaceAction::ActionTabCode]);
    ui_->actionImport_Font->setEnabled(uiState[MainWindowModel::InterfaceAction::ActionImportFont]);
    ui_->actionAdd_Glyph->setEnabled(uiState[MainWindowModel::InterfaceAction::ActionAddGlyph]);
    ui_->actionSave->setEnabled(uiState[MainWindowModel::InterfaceAction::ActionSave]);
    ui_->actionCopy_Glyph->setEnabled(uiState[MainWindowModel::InterfaceAction::ActionCopy]);
    ui_->actionPaste_Glyph->setEnabled(uiState[MainWindowModel::InterfaceAction::ActionPaste]);
//    ui_->undoButton->defaultAction()->setEnabled(uiState[MainWindowModel::InterfaceAction::ActionUndo]);
//    ui_->redoButton->defaultAction()->setEnabled(uiState[MainWindowModel::InterfaceAction::ActionRedo]);
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

void MainWindow::displayFace(const Font::Face& face)
{
    if (faceWidget_ == nullptr) {
        faceWidget_ = new FaceWidget();
        ui_->faceGraphicsView->scene()->addItem(faceWidget_);

        connect(faceWidget_, &FaceWidget::currentGlyphIndexChanged,
                viewModel_.get(), &MainWindowModel::setActiveGlyphIndex);
    }

    faceWidget_->load(face);

    if (glyphWidget_.get()) {
        glyphWidget_.release();
    }
}

void MainWindow::displayGlyph(const Font::Glyph& glyph)
{
    if (!glyphWidget_.get()) {
        glyphWidget_ = std::make_unique<GlyphWidget>(glyph);
        ui_->glyphGraphicsView->scene()->addItem(glyphWidget_.get());

        connect(glyphWidget_.get(), &GlyphWidget::pixelsChanged,
                [&] (const BatchPixelChange &change)
        {
            undoStack_->push(new Command("Edit Glyph", [&, change] {
                change.apply(viewModel_->faceModel()->active_glyph(), true);
                glyphWidget_->applyChange(change, true);
            }, [&, change] {
                change.apply(viewModel_->faceModel()->active_glyph());
                glyphWidget_->applyChange(change);
            }));

            viewModel_->registerInputEvent(MainWindowModel::UserEditedGlyph);
        });
    } else {
        glyphWidget_->load(glyph);
    }
    ui_->glyphGraphicsView->fitInView(glyphWidget_->rect(), Qt::KeepAspectRatio);
}

void MainWindow::resetCurrentGlyph()
{
    viewModel_->faceModel()->reset_active_glyph();
    displayGlyph(viewModel_->faceModel()->active_glyph());
}
