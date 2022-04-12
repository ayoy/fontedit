#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "f2b.h"
#include "facewidget.h"
#include "fontfaceviewmodel.h"
#include "command.h"
#include "aboutdialog.h"
#include "addglyphdialog.h"
#include "common.h"

#include <QGraphicsGridLayout>
#include <QGraphicsWidget>
#include <QDebug>
#include <QStyle>
#include <QFontDialog>
#include <QFileDialog>
#include <QScrollBar>
#include <QMessageBox>
#include <QKeySequence>
#include <QElapsedTimer>
#include <QStandardPaths>
#include <QDesktopServices>

#include <iostream>
#include <stdexcept>

#include <QFile>
#include <QTextStream>

static constexpr auto codeTabIndex = 1;
static constexpr auto exportAllButtonIndex = -3;
static constexpr auto fileFilter = "FontEdit documents (*.fontedit)";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui_ { new Ui::MainWindow },
      statusLabel_ { new QLabel() }
{
    ui_->setupUi(this);

    initUI();
    setupActions();
    updateUI(viewModel_->uiState());

    connectUpdateHelper();
    connectUIInputs();
    connectViewModelOutputs();
    viewModel_->restoreSession();

    ui_->statusBar->addPermanentWidget(statusLabel_);

    updateHelper_->checkForUpdatesIfNeeded();
}

MainWindow::~MainWindow()
{
    delete ui_;
}

void MainWindow::connectUpdateHelper()
{
    connect(updateHelper_.get(), &UpdateHelper::updateAvailable,
            [&] (UpdateHelper::Update update) {
        showUpdateDialog(update);
    });
    connect(updateHelper_.get(), &UpdateHelper::updateNotAvailable,
            [&] {
        showUpdateDialog({});
    });
}

void MainWindow::connectUIInputs()
{
    connect(ui_->actionAbout, &QAction::triggered, this, &MainWindow::showAboutDialog);
    connect(ui_->actionImport_Font, &QAction::triggered, this, &MainWindow::showFontDialog);
    connect(ui_->actionOpen, &QAction::triggered, this, &MainWindow::showOpenDocumentDialog);
    connect(ui_->actionAdd_Glyph, &QAction::triggered, this, &MainWindow::showAddGlyphDialog);
    connect(ui_->actionDelete_Glyph, &QAction::triggered, this, &MainWindow::showDeleteGlyphDialog);
    connect(ui_->actionReset_Glyph, &QAction::triggered, this, &MainWindow::resetCurrentGlyph);
    connect(ui_->actionReset_Font, &QAction::triggered, this, &MainWindow::resetFont);

    connect(ui_->actionSave, &QAction::triggered, this, &MainWindow::save);
    connect(ui_->actionSave_As, &QAction::triggered, this, &MainWindow::saveAs);
    connect(ui_->actionClose, &QAction::triggered, this, &MainWindow::showCloseDocumentDialogIfNeeded);

    connect(ui_->actionExport, &QAction::triggered, this, &MainWindow::exportSourceCode);
    connect(ui_->exportButton, &QPushButton::clicked, this, &MainWindow::exportSourceCode);

    connect(ui_->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(ui_->tabWidget, &QTabWidget::currentChanged, [&](int index) {
        if (index == codeTabIndex) {
            displaySourceCode();
            viewModel_->registerInputEvent(UIState::InterfaceAction::ActionTabCode);
        } else {
            viewModel_->registerInputEvent(UIState::InterfaceAction::ActionTabEdit);
        }
    });
    connect(ui_->exportMethodButtonGroup, qOverload<int>(&QButtonGroup::idClicked), [&](int buttonID) {
        viewModel_->setExportAllEnabled(buttonID == exportAllButtonIndex);
    });
    connect(ui_->invertBitsCheckBox, &QCheckBox::stateChanged, [&](int state) {
        viewModel_->setInvertBits(state == Qt::Checked);
    });
    connect(ui_->bitNumberingCheckBox, &QCheckBox::stateChanged, [&](int state) {
        viewModel_->setMSBEnabled(state == Qt::Checked);
    });
    connect(ui_->lineSpacingCheckBox, &QCheckBox::stateChanged, [&](int state) {
        viewModel_->setIncludeLineSpacing(state == Qt::Checked);
    });
    connect(ui_->formatComboBox, &QComboBox::currentTextChanged,
            viewModel_.get(), &MainWindowModel::setOutputFormat);
    connect(ui_->indentationComboBox, &QComboBox::currentTextChanged,
            viewModel_.get(), &MainWindowModel::setIndentation);
    connect(ui_->fontArrayNameEdit, &QLineEdit::textChanged, [&](const QString& fontArrayName) {
        auto fontName = fontArrayName.isEmpty() ? ui_->fontArrayNameEdit->placeholderText() : std::move(fontArrayName);
        debounceFontNameChanged(fontName);
    });

    connect(ui_->actionCheck_for_Updates, &QAction::triggered, [&] {
        updateHelper_->checkForUpdates(true);
    });
}

void MainWindow::connectViewModelOutputs()
{
    connect(viewModel_.get(), &MainWindowModel::documentTitleChanged, [&](const QString& title) {
        setWindowTitle(QString("FontEdit (%1)").arg(title));
    });
    connect(viewModel_.get(), &MainWindowModel::uiStateChanged, this, &MainWindow::updateUI);
    connect(viewModel_.get(), &MainWindowModel::faceLoaded, [&](f2b::font::face& face) {
        undoStack_->clear();
        displayFace(face);
    });
    connect(viewModel_.get(), &MainWindowModel::documentError, this, &MainWindow::displayError);
    connect(viewModel_.get(), &MainWindowModel::activeGlyphChanged, [&](std::optional<f2b::font::glyph> glyph) {
        if (glyph.has_value()) {
            displayGlyph(glyph.value());
        } else if (auto g = glyphWidget_.get()) {
            ui_->glyphGraphicsView->scene()->removeItem(g);
            glyphWidget_.release();
        }
    });
    connect(viewModel_.get(), &MainWindowModel::sourceCodeUpdating, [&]() {
//        ui_->stackedWidget->setCurrentWidget(ui_->spinnerContainer);
    });
    connect(viewModel_.get(), &MainWindowModel::sourceCodeChanged, [&]() {
        if (ui_->tabWidget->currentIndex() == codeTabIndex) {
            displaySourceCode();
        }
    });
    connect(viewModel_.get(), &MainWindowModel::documentClosed, this, &MainWindow::closeCurrentDocument);
}

void MainWindow::initUI()
{
    // hide not implemented UI
    ui_->actionCopy_Glyph->setVisible(false);
    ui_->actionPaste_Glyph->setVisible(false);
    ui_->copyButton->setVisible(false);
    ui_->pasteButton->setVisible(false);
    ui_->printButton->setVisible(false);
    ui_->actionPrint->setVisible(false);
    ui_->actionNew->setVisible(false);

    faceScene_->setBackgroundBrush(QBrush(Qt::lightGray));
    ui_->faceGraphicsView->setScene(faceScene_.get());

    ui_->actionShow_non_exported_Glyphs->setChecked(viewModel_->shouldShowNonExportedGlyphs());
    ui_->showNonExportedGlyphsCheckBox->setCheckState(viewModel_->shouldShowNonExportedGlyphs());

    ui_->showNonExportedGlyphsCheckBox->setVisible(false);
    ui_->faceInfoSeparator->setVisible(false);
    ui_->faceInfoLabel->setVisible(false);

    auto scrollBarWidth = ui_->faceGraphicsView->verticalScrollBar()->sizeHint().width();
    auto faceViewWidth = static_cast<int>(FaceWidget::cell_width) * 3 + scrollBarWidth;
    ui_->faceGraphicsView->setMinimumSize({ faceViewWidth,
                                            ui_->faceGraphicsView->minimumSize().height() });

    ui_->exportAllButton->setChecked(viewModel_->exportAllEnabled());
    ui_->exportSubsetButton->setChecked(!viewModel_->exportAllEnabled());
    ui_->invertBitsCheckBox->setCheckState(viewModel_->invertBits());
    ui_->bitNumberingCheckBox->setCheckState(viewModel_->msbEnabled());
    ui_->lineSpacingCheckBox->setCheckState(viewModel_->includeLineSpacing());

    for (const auto& [identifier, name] : viewModel_->outputFormats().toStdMap()) {
        ui_->formatComboBox->addItem(name, identifier);
    }
    for (const auto& [indent, name] : viewModel_->indentationStyles()) {
        ui_->indentationComboBox->addItem(name);
    }

    ui_->formatComboBox->setCurrentText(viewModel_->outputFormat());
    ui_->indentationComboBox->setCurrentText(viewModel_->indentationStyleCaption());

    QFont f(consoleFontName, 12);
    f.setStyleHint(QFont::TypeWriter);
    ui_->sourceCodeTextBrowser->setFont(f);
}

void MainWindow::showUpdateDialog(std::optional<UpdateHelper::Update> update)
{
    QMessageBox messageBox;
    messageBox.setCheckBox(new QCheckBox(tr("Check for updates at start-up")));
    messageBox.checkBox()->setChecked(updateHelper_->shouldCheckAtStartup());

    connect(messageBox.checkBox(), &QCheckBox::toggled, [&] (bool isChecked) {
        updateHelper_->setShouldCheckAtStartup(isChecked);
    });

    if (update.has_value()) {
        auto updateInfo = update.value();
        messageBox.setText(tr("Update Available"));
        messageBox.setInformativeText(tr("FontEdit %1 is available (you have %2).\nGet the new version from GitHub Releases Page.")
                                      .arg(updateInfo.latestVersion, updateInfo.currentVersion));
        messageBox.setDetailedText(updateInfo.releaseNotes);

        auto visitPageButton = messageBox.addButton(tr("Go to Releases Page"), QMessageBox::YesRole);
        messageBox.addButton(tr("Dismiss"), QMessageBox::NoRole);

        messageBox.setDefaultButton(visitPageButton);
        messageBox.exec();

        if (messageBox.clickedButton() == visitPageButton) {
            QDesktopServices::openUrl(updateInfo.webpageURL);
        }
    } else {
        messageBox.setText(tr("No Update Available"));
        messageBox.setInformativeText(tr("You're using the latest version of FontEdit"));

        messageBox.exec();
    }

}

void MainWindow::closeCurrentDocument()
{
    ui_->showNonExportedGlyphsCheckBox->setVisible(false);
    ui_->faceInfoSeparator->setVisible(false);
    ui_->faceInfoLabel->setVisible(false);

    if (faceWidget_ != nullptr) {
        faceScene_->removeItem(faceWidget_);
        delete faceWidget_;
        faceWidget_ = nullptr;
    }

    if (auto g = glyphWidget_.get()) {
        ui_->glyphGraphicsView->scene()->removeItem(g);
        glyphWidget_.release();
    }

    undoStack_->clear();
    updateResetActions();
}

void MainWindow::setupActions()
{
    auto undo = undoStack_->createUndoAction(this);
    undo->setIcon(QIcon {":/toolbar/assets/undo.svg"});
    undo->setShortcut(QKeySequence::Undo);

    auto redo = undoStack_->createRedoAction(this);
    redo->setIcon(QIcon {":/toolbar/assets/redo.svg"});
    redo->setShortcut(QKeySequence::Redo);

    ui_->openButton->setDefaultAction(ui_->actionOpen);
    ui_->importFontButton->setDefaultAction(ui_->actionImport_Font);
    ui_->addGlyphButton->setDefaultAction(ui_->actionAdd_Glyph);
    ui_->deleteGlyphButton->setDefaultAction(ui_->actionDelete_Glyph);
    ui_->saveButton->setDefaultAction(ui_->actionSave);
    ui_->copyButton->setDefaultAction(ui_->actionCopy_Glyph);
    ui_->pasteButton->setDefaultAction(ui_->actionPaste_Glyph);
    ui_->undoButton->setDefaultAction(undo);
    ui_->redoButton->setDefaultAction(redo);
    ui_->resetGlyphButton->setDefaultAction(ui_->actionReset_Glyph);
    ui_->resetFontButton->setDefaultAction(ui_->actionReset_Font);
    ui_->actionReset_Glyph->setEnabled(false);
    ui_->actionReset_Font->setEnabled(false);

    ui_->menuEdit->insertAction(ui_->actionCopy_Glyph, undo);
    ui_->menuEdit->insertAction(ui_->actionCopy_Glyph, redo);
}

void MainWindow::updateUI(UIState uiState)
{
    ui_->tabWidget->setTabEnabled(1, uiState.actions[UIState::InterfaceAction::ActionTabCode]);
    ui_->actionAdd_Glyph->setEnabled(uiState.actions[UIState::InterfaceAction::ActionAddGlyph]);
    ui_->actionDelete_Glyph->setEnabled(uiState.actions[UIState::InterfaceAction::ActionDeleteGlyph]);
    ui_->actionSave->setEnabled(uiState.actions[UIState::InterfaceAction::ActionSave]);
    ui_->actionSave_As->setEnabled(uiState.actions[UIState::InterfaceAction::ActionSave]);
    ui_->actionClose->setEnabled(uiState.actions[UIState::InterfaceAction::ActionClose]);
    ui_->actionCopy_Glyph->setEnabled(uiState.actions[UIState::InterfaceAction::ActionCopy]);
    ui_->actionPaste_Glyph->setEnabled(uiState.actions[UIState::InterfaceAction::ActionPaste]);
    ui_->actionExport->setEnabled(uiState.actions[UIState::InterfaceAction::ActionExport]);
    ui_->actionPrint->setEnabled(uiState.actions[UIState::InterfaceAction::ActionPrint]);

    switch (uiState.statusBarMessage) {
    case UIState::MessageIdle:
        statusLabel_->setText(tr("Start by importing a system font or opening an existing document"));
        break;
    case UIState::MessageLoadedFace:
        statusLabel_->setText(tr("Select a glyph on the right to edit it"));
        break;
    case UIState::MessageLoadedGlyph:
        statusLabel_->setText(tr("Click and drag to paint, hold Alt or Ctrl to erase."));
        break;
    }

    if (ui_->tabWidget->currentIndex() == codeTabIndex) {
        statusLabel_->setVisible(false);
    } else {
        statusLabel_->setVisible(true);
    }
    ui_->statusBar->clearMessage();
}

QString MainWindow::defaultDialogDirectory() const
{
    QString directoryPath = viewModel_->lastVisitedDirectory();
    if (directoryPath.isNull()) {
        directoryPath = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).last();
    }
    return directoryPath;
}

void MainWindow::showAboutDialog()
{
    auto about = new AboutDialog(this);
    about->show();
}

void MainWindow::showFontDialog()
{
    switch (promptToSaveDirtyDocument()) {
    case Save:
        save();
    case DontSave:
        break;
    case Cancel:
        return;
    }

    bool ok;
    QFont f(consoleFontName, 24);
    f.setStyleHint(QFont::TypeWriter);
    f = QFontDialog::getFont(&ok, f, this, tr("Select Font"), QFontDialog::MonospacedFonts | QFontDialog::DontUseNativeDialog);

    if (ok) {
        qDebug() << "selected font:" << f;
        viewModel_->importFont(f);
    }
}

void MainWindow::showOpenDocumentDialog()
{
    switch (promptToSaveDirtyDocument()) {
    case Save:
        save();
    case DontSave:
        break;
    case Cancel:
        return;
    }

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Document"), std::move(defaultDialogDirectory()), tr(fileFilter));

    if (!fileName.isNull())
        viewModel_->openDocument(fileName);
}

void MainWindow::showCloseDocumentDialogIfNeeded()
{
    switch (promptToSaveDirtyDocument()) {
    case Save:
        save();
    case DontSave:
        break;
    case Cancel:
        return;
    }

    viewModel_->closeCurrentDocument();
}

void MainWindow::showAddGlyphDialog()
{
    auto addGlyph = new AddGlyphDialog(*viewModel_->faceModel(), this);
    addGlyph->show();

    connect(addGlyph, &AddGlyphDialog::glyphSelected, [&](const std::optional<f2b::font::glyph>& glyph) {
        if (glyph.has_value()) {

            auto numberOfGlyphs = viewModel_->faceModel()->face().num_glyphs();
            auto activeGlyphIndex = viewModel_->faceModel()->activeGlyphIndex();

            pushUndoCommand(new Command(tr("Add Glyph"), [&, numberOfGlyphs, activeGlyphIndex] {
                viewModel_->deleteGlyph(numberOfGlyphs);
                viewModel_->setActiveGlyphIndex(activeGlyphIndex);
                displayFace(viewModel_->faceModel()->face());
            }, [&, glyph] {
                viewModel_->appendGlyph(glyph.value());
                viewModel_->setActiveGlyphIndex(viewModel_->faceModel()->face().num_glyphs()-1);
                displayFace(viewModel_->faceModel()->face());
            }));
        }
    });
}

void MainWindow::showDeleteGlyphDialog()
{
    auto currentIndex = viewModel_->faceModel()->activeGlyphIndex();
    auto isLastGlyph = currentIndex.value() == viewModel_->faceModel()->face().num_glyphs() - 1;
    if (!isLastGlyph) {
        QMessageBox::information(this, tr("Delete Glyph"),
                                 tr("You can only delete Glyphs at the end of the list. "
                                    "This Glyph will be cleared instead of deleted "
                                    "(to ensure that other Glyphs' indexes are unchanged)."),
                                 QMessageBox::StandardButton::Ok);
    }

    auto glyph = viewModel_->faceModel()->activeGlyph();
    if (glyph.has_value()) {

        auto commandName = isLastGlyph ? tr("Delete Glyph") : tr("Clear Glyph");

        pushUndoCommand(new Command(commandName, [&, currentIndex, isLastGlyph, glyph] {
            if (isLastGlyph) {
                viewModel_->appendGlyph(glyph.value());
                viewModel_->setActiveGlyphIndex(viewModel_->faceModel()->face().num_glyphs()-1);
                displayFace(viewModel_->faceModel()->face());
            } else {
                viewModel_->modifyGlyph(currentIndex.value(), glyph.value());
                faceWidget_->updateGlyphInfo(currentIndex.value(), viewModel_->faceModel()->activeGlyph().value());
                displayGlyph(viewModel_->faceModel()->activeGlyph().value());
            }
        }, [&, currentIndex, isLastGlyph] {
            viewModel_->deleteGlyph(currentIndex.value());
            if (isLastGlyph) {
                displayFace(viewModel_->faceModel()->face());
            } else {
                faceWidget_->updateGlyphInfo(currentIndex.value(), viewModel_->faceModel()->activeGlyph().value());
                displayGlyph(viewModel_->faceModel()->activeGlyph().value());
            }
        }));

    }
}

void MainWindow::save()
{
    auto currentPath = viewModel_->currentDocumentPath();
    if (currentPath.has_value()) {
        viewModel_->saveDocument(currentPath.value());
    } else {
        saveAs();
    }
}

void MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Document"), std::move(defaultDialogDirectory()), tr(fileFilter));

    if (!fileName.isNull())
        viewModel_->saveDocument(fileName);
}

void MainWindow::displayError(const QString &error)
{
    QMessageBox::critical(this, tr("Error"), error, QMessageBox::StandardButton::Ok);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    switch (promptToSaveDirtyDocument()) {
    case Save:
        save();
    case DontSave:
        event->accept();
        break;
    case Cancel:
        event->ignore();
    }
}

MainWindow::SavePromptButton MainWindow::promptToSaveDirtyDocument()
{
    if (viewModel_->faceModel() == nullptr || !viewModel_->faceModel()->isModifiedSinceSave()) {
        return DontSave; // ignore this dialog and move on
    }

    QStringList buttons { tr("Save"), tr("Don't Save"), tr("Cancel") };
    auto ret = QMessageBox::information(this,
                                        "",
                                        tr("Do you want to save the changes you made? Your changes will be lost if you don't save them."),
                                        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    return static_cast<MainWindow::SavePromptButton>(ret);
}

void MainWindow::displayFace(f2b::font::face& face)
{
    if (faceWidget_ == nullptr) {
        faceWidget_ = new FaceWidget();
        faceWidget_->setShowsNonExportedItems(viewModel_->shouldShowNonExportedGlyphs());
        ui_->faceGraphicsView->scene()->addItem(faceWidget_);

        connect(faceWidget_, &FaceWidget::currentGlyphIndexChanged,
                this, &MainWindow::switchActiveGlyph);
        connect(faceWidget_, &FaceWidget::glyphExportedStateChanged,
                this, &MainWindow::setGlyphExported);
        connect(ui_->actionShow_non_exported_Glyphs, &QAction::toggled,
                [&](bool checked) {
            viewModel_->setShouldShowNonExportedGlyphs(checked);
            faceWidget_->setShowsNonExportedItems(checked);
            QApplication::processEvents();
            ui_->faceGraphicsView->setSceneRect(faceWidget_->rect());
        });
    }

    auto margins = viewModel_->faceModel()->originalFaceMargins();
    faceWidget_->load(face, margins);

    ui_->showNonExportedGlyphsCheckBox->setVisible(true);
    ui_->faceInfoSeparator->setVisible(true);

    auto faceInfo = viewModel_->faceModel()->faceInfo();
    updateFaceInfoLabel(faceInfo);
    updateDefaultFontName(faceInfo);
    ui_->faceInfoLabel->setVisible(true);

    auto glyph = viewModel_->faceModel()->activeGlyph();
    if (glyph.has_value()) {
        displayGlyph(glyph.value());
        faceWidget_->setCurrentGlyphIndex(viewModel_->faceModel()->activeGlyphIndex());
        viewModel_->setActiveGlyphIndex(viewModel_->faceModel()->activeGlyphIndex());
    } else if (auto g = glyphWidget_.get()) {
        ui_->glyphGraphicsView->scene()->removeItem(g);
        glyphWidget_.release();
    }
    updateResetActions();
}

void MainWindow::setGlyphExported(std::size_t index, bool isExported)
{
    pushUndoCommand(new Command(tr("Toggle Glyph Exported"), [&, index, isExported] {
        viewModel_->setGlyphExported(index, !isExported);
        auto faceModel = viewModel_->faceModel();
        updateFaceInfoLabel(faceModel->faceInfo());
        if (!faceWidget_->showsNonExportedItems()) {
            auto margins = faceModel->originalFaceMargins();
            faceWidget_->load(faceModel->face(), margins);
            faceWidget_->setCurrentGlyphIndex(index);
            glyphWidget_->load(faceModel->face().glyph_at(index), margins);
        } else {
            faceWidget_->updateGlyphInfo(index, {}, !isExported);
        }

    }, [&, index, isExported] {
        auto faceModel = viewModel_->faceModel();
        auto shouldUpdateCurrentIndex = !isExported && !faceWidget_->showsNonExportedItems();

        std::optional<std::size_t> nextIndex {};
        if (shouldUpdateCurrentIndex) {
            // Find index of the next exported item
            auto i = std::next(faceModel->face().exported_glyph_ids().find(index));
            if (i != faceModel->face().exported_glyph_ids().end()) {
                nextIndex = *i;
            }
        }

        viewModel_->setGlyphExported(index, isExported);
        updateFaceInfoLabel(faceModel->faceInfo());

        if (shouldUpdateCurrentIndex) {
            auto margins = faceModel->originalFaceMargins();
            faceWidget_->load(faceModel->face(), margins);
            faceWidget_->setCurrentGlyphIndex(nextIndex);
            if (nextIndex.has_value()) {
                glyphWidget_->load(faceModel->face().glyph_at(nextIndex.value()), margins);
            }
        } else {
            faceWidget_->updateGlyphInfo(index, {}, isExported);
        }
    }));
}

void MainWindow::updateFaceInfoLabel(const FaceInfo &faceInfo)
{
    QStringList lines;
    lines << faceInfo.fontName;
    lines << tr("Size (full): %1x%2px").arg(faceInfo.size.width).arg(faceInfo.size.height);
    lines << tr("Size (adjusted): %1x%2px").arg(faceInfo.sizeWithoutMargins.width).arg(faceInfo.sizeWithoutMargins.height);
    lines << QString("%1, %2").arg(tr("%n Glyph(s)", "", faceInfo.numberOfGlyphs),
                                   tr("%n to export", "", faceInfo.numberOfExportedGlyphs));
    ui_->faceInfoLabel->setText(lines.join("\n"));
}

void MainWindow::updateDefaultFontName(const FaceInfo &faceInfo)
{
    auto fontName = faceInfo.fontName.toLower();
    fontName.remove(',');
    fontName.replace(' ', '_');
    ui_->fontArrayNameEdit->setText(fontName);
}

void MainWindow::displayGlyph(const f2b::font::glyph& glyph)
{
    auto margins = viewModel_->faceModel()->originalFaceMargins();
    if (!glyphWidget_.get()) {
        glyphWidget_ = std::make_unique<GlyphWidget>(glyph, margins);
        ui_->glyphGraphicsView->scene()->addItem(glyphWidget_.get());

        connect(glyphWidget_.get(), &GlyphWidget::pixelsChanged,
                this, &MainWindow::editGlyph);
    } else {
        glyphWidget_->load(glyph, margins);
    }
    updateResetActions();
    ui_->glyphGraphicsView->fitInView(glyphWidget_->boundingRect(), Qt::KeepAspectRatio);
}

void MainWindow::editGlyph(const BatchPixelChange& change)
{
    auto currentIndex = viewModel_->faceModel()->activeGlyphIndex();
    if (currentIndex.has_value()) {

        auto applyChange = [&, currentIndex, change](BatchPixelChange::ChangeType type) -> std::function<void()> {
            return [&, currentIndex, change, type] {
                viewModel_->modifyGlyph(currentIndex.value(), change, type);
                updateResetActions();
                glyphWidget_->applyChange(change, type);
                faceWidget_->updateGlyphInfo(currentIndex.value(), viewModel_->faceModel()->activeGlyph().value());
                viewModel_->updateDocumentTitle();
            };
        };

        pushUndoCommand(new Command(tr("Edit Glyph"),
                                    applyChange(BatchPixelChange::ChangeType::Reverse),
                                    applyChange(BatchPixelChange::ChangeType::Normal)));
    }
}

void MainWindow::switchActiveGlyph(std::optional<std::size_t> newIndex)
{
    auto currentIndex = viewModel_->faceModel()->activeGlyphIndex();
    if (currentIndex == newIndex) {
        return;
    }

    if (currentIndex.has_value() && newIndex.has_value()) {

        if (!pendingSwitchGlyphCommand_) {
            pendingSwitchGlyphCommand_ = std::make_unique<SwitchActiveGlyphCommand>(faceWidget_, viewModel_.get(),
                                                                                    currentIndex.value(), newIndex.value());
        } else {
            pendingSwitchGlyphCommand_->setToIndex(newIndex.value());
        }
        pendingSwitchGlyphCommand_->redo();
    } else {
        faceWidget_->setCurrentGlyphIndex(newIndex);
        viewModel_->setActiveGlyphIndex(newIndex);
    }
}

void MainWindow::resetCurrentGlyph()
{
    f2b::font::glyph currentGlyphState { viewModel_->faceModel()->activeGlyph().value() };
    auto glyphIndex = viewModel_->faceModel()->activeGlyphIndex().value();

    pushUndoCommand(new Command(tr("Reset Glyph"), [&, currentGlyphState, glyphIndex] {
        viewModel_->modifyGlyph(glyphIndex, currentGlyphState);
        viewModel_->updateDocumentTitle();
        displayGlyph(viewModel_->faceModel()->activeGlyph().value());
        faceWidget_->updateGlyphInfo(glyphIndex, viewModel_->faceModel()->activeGlyph().value());
    }, [&, glyphIndex] {
        viewModel_->resetGlyph(glyphIndex);
        viewModel_->updateDocumentTitle();
        displayGlyph(viewModel_->faceModel()->activeGlyph().value());
        faceWidget_->updateGlyphInfo(glyphIndex, viewModel_->faceModel()->activeGlyph().value());
    }));
}

void MainWindow::resetFont()
{
    auto message = tr("Are you sure you want to reset all changes to the font? This operation cannot be undone.");
    auto result = QMessageBox::warning(this, tr("Reset Font"), message, QMessageBox::Reset, QMessageBox::Cancel);
    if (result == QMessageBox::Reset) {
        viewModel_->faceModel()->reset();
        viewModel_->updateDocumentTitle();
        undoStack_->clear();
        updateResetActions();
        displayFace(viewModel_->faceModel()->face());
    }
}

void MainWindow::updateResetActions()
{
    if (viewModel_->faceModel() == nullptr) {
        ui_->actionReset_Glyph->setEnabled(false);
        ui_->actionReset_Font->setEnabled(false);
    } else {
        auto currentIndex = viewModel_->faceModel()->activeGlyphIndex();
        if (currentIndex.has_value()) {
            ui_->actionReset_Glyph->setEnabled(viewModel_->faceModel()->isGlyphModified(currentIndex.value()));
        } else {
            ui_->actionReset_Glyph->setEnabled(false);
        }
        ui_->actionReset_Font->setEnabled(viewModel_->faceModel()->isModified());
    }
}

void MainWindow::debounceFontNameChanged(const QString &fontName)
{
    if (fontNameDebounceTimer_ == nullptr) {
        fontNameDebounceTimer_ = std::make_unique<QTimer>();
        fontNameDebounceTimer_->setInterval(300);
        fontNameDebounceTimer_->setSingleShot(true);
    } else {
        fontNameDebounceTimer_->stop();
        fontNameDebounceTimer_->disconnect();
    }

    connect(fontNameDebounceTimer_.get(), &QTimer::timeout, [&, fontName] {
        viewModel_->setFontArrayName(fontName);
    });

    fontNameDebounceTimer_->start();
}

void MainWindow::displaySourceCode()
{
    ui_->stackedWidget->setCurrentWidget(ui_->sourceCodeContainer);
    QElapsedTimer timer;
    timer.start();
    ui_->sourceCodeTextBrowser->setPlainText(viewModel_->sourceCode());
    qDebug() << "Displaying finished in" << timer.elapsed() << "ms";
}

void MainWindow::exportSourceCode()
{
    ui_->statusBar->clearMessage();

    QString directoryPath = viewModel_->lastSourceCodeDirectory();
    if (directoryPath.isNull()) {
        directoryPath = defaultDialogDirectory();
    }

    auto dialog = std::make_shared<QFileDialog>(this, tr("Save Source Code"), directoryPath);
    dialog->setAcceptMode(QFileDialog::AcceptSave);
    dialog->setFileMode(QFileDialog::AnyFile);

    connect(dialog.get(), &QFileDialog::finished, [=](int) {
        dialog->setParent(nullptr);
        auto files = dialog->selectedFiles();
        if (!files.isEmpty()) {
            auto filePath = files.first();
            if (!filePath.isNull()) {
                QFile output(filePath);
                if (output.open(QFile::WriteOnly | QFile::Truncate)) {
                    output.write(ui_->sourceCodeTextBrowser->document()->toPlainText().toUtf8());
                    output.close();
                    viewModel_->setLastSourceCodeDirectory(filePath);
                    ui_->statusBar->showMessage(tr("Source code successfully exported."), 5000);
                } else {
                    displayError(tr("Unable to write to file: ") + filePath);
                }
            }
        }
    });

    dialog->open();
}

void MainWindow::pushUndoCommand(QUndoCommand *command)
{
    bool shouldPushSwitchGlyphCommand = pendingSwitchGlyphCommand_ != nullptr;

    if (shouldPushSwitchGlyphCommand) {
        undoStack_->beginMacro(command->text());
        undoStack_->push(pendingSwitchGlyphCommand_.get());
        pendingSwitchGlyphCommand_.release();
    }

    undoStack_->push(command);

    if (shouldPushSwitchGlyphCommand) {
        undoStack_->endMacro();
    }
}
