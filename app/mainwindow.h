#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QUndoStack>
#include <QTimer>

#include "mainwindowmodel.h"
#include "updatehelper.h"
#include "facewidget.h"
#include "glyphwidget.h"
#include "batchpixelchange.h"
#include "command.h"

#include <memory>

namespace Ui {
class MainWindow;
}

class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow();

protected:
    virtual void closeEvent(QCloseEvent *event) override;

private slots:
    void displayFace(f2b::font::face& face);

private:
    void connectUpdateHelper();
    void connectUIInputs();
    void connectViewModelOutputs();
    void initUI();
    void setupActions();

    void showUpdateDialog(std::optional<UpdateHelper::Update> update);

    void showAboutDialog();
    void showFontDialog();
    void showOpenDocumentDialog();
    void showCloseDocumentDialogIfNeeded();
    void showAddGlyphDialog();
    void showDeleteGlyphDialog();
    void save();
    void saveAs();
    void resetCurrentGlyph();
    void resetFont();
    void displayGlyph(const f2b::font::glyph& glyph);
    void updateUI(UIState uiState);
    void editGlyph(const BatchPixelChange& change);
    void switchActiveGlyph(std::optional<std::size_t> newIndex);
    void setGlyphExported(std::size_t index, bool isExported);
    void updateResetActions();
    void updateFaceInfoLabel(const FaceInfo& faceInfo);
    void updateDefaultFontName(const FaceInfo& faceInfo);

    void displaySourceCode();
    void exportSourceCode();
    void closeCurrentDocument();
    void displayError(const QString& error);
    void pushUndoCommand(QUndoCommand *command);

    void debounceFontNameChanged(const QString& fontName);

    QString defaultDialogDirectory() const;

    enum SavePromptButton {
        Save,
        DontSave,
        Cancel
    };

    SavePromptButton promptToSaveDirtyDocument();

    Ui::MainWindow *ui_;

    std::unique_ptr<GlyphWidget> glyphWidget_ {};
    FaceWidget *faceWidget_ { nullptr };
    QLabel *statusLabel_;
    std::unique_ptr<UpdateHelper> updateHelper_ { std::make_unique<UpdateHelper>() };
    std::unique_ptr<MainWindowModel> viewModel_ { std::make_unique<MainWindowModel>() };
    std::unique_ptr<QGraphicsScene> faceScene_ { std::make_unique<QGraphicsScene>() };
    std::unique_ptr<QUndoStack> undoStack_ { std::make_unique<QUndoStack>() };
    std::unique_ptr<QTimer> fontNameDebounceTimer_ {};

    std::unique_ptr<SwitchActiveGlyphCommand> pendingSwitchGlyphCommand_ {};
};

#endif // MAINWINDOW_H
