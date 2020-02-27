#ifndef MAINWINDOWMODEL_H
#define MAINWINDOWMODEL_H

#include "fontfaceviewmodel.h"
#include <f2b.h>
#include <memory>
#include <functional>
#include <optional>
#include <bitset>
#include <variant>

#include <QMap>
#include <QSettings>

class MainWindowModel: public QObject
{
    Q_OBJECT

public:
    enum InterfaceAction {
        ActionImportFont = 0,
        ActionOpen,
        ActionAddGlyph,
        ActionSave,
        ActionCopy,
        ActionPaste,
        ActionUndo,
        ActionRedo,
//        ActionResetGlyph,
//        ActionResetFont,
        ActionPrint,
        ActionExport,
        ActionTabCode,
        ActionCount,
        ActionFirst = ActionImportFont
    };

    enum UserAction {
        UserIdle = 0,
        UserLoadedFace,
        UserLoadedGlyph,
        UserEditedGlyph
    };

    using UIState = std::bitset<ActionCount>;
    using InputEvent = std::variant<InterfaceAction,UserAction>;

    explicit MainWindowModel(QObject *parent = nullptr);
    void restoreSession();

    FontFaceViewModel* faceModel() const {
        return fontFaceViewModel_.get();
    }

    const UIState& uiState() const { return uiState_; }

    Qt::CheckState invertBits() const {
        return sourceCodeOptions_.invert_bits ? Qt::Checked : Qt::Unchecked;
    }

    Qt::CheckState msbEnabled() const {
        return sourceCodeOptions_.bit_numbering == SourceCodeOptions::BitNumbering::MSB
                ? Qt::Checked : Qt::Unchecked;
    }

    Qt::CheckState includeLineSpacing() const {
        return sourceCodeOptions_.include_line_spacing ? Qt::Checked : Qt::Unchecked;
    }

    const QMap<QString,QString>& outputFormats() const {
        return formats_;
    }

    QString outputFormat() const {
        return formats_.value(currentFormat_, formats_.first());
    }

    void registerInputEvent(InputEvent e);

    const std::optional<QString>& currentDocumentPath() const {
        return documentPath_;
    }

    QString documentTitle() const { return documentTitle_; }
    void updateDocumentTitle();

public slots:
    void importFont(const QFont& font);
    void loadFace(const QString& fileName);
    void saveFace(const QString& fileName);
    void closeCurrentDocument();
    void setActiveGlyphIndex(std::size_t index);
    void prepareSourceCodeTab();
    void setInvertBits(bool enabled);
    void setMSBEnabled(bool enabled);
    void setIncludeLineSpacing(bool enabled);
    void setOutputFormat(const QString &format); // human-readable

signals:
    void uiStateChanged(UIState state) const;
    void faceLoaded(const Font::Face& face) const;
    void activeGlyphChanged(const Font::Glyph& glyph) const;
    void sourceCodeUpdating() const;
    void sourceCodeChanged(const QString& sourceCode) const;
    void runnableFinished(const QString& result) const;
    void documentTitleChanged(const QString& title);
    void documentClosed();

private:
    void reloadSourceCode();
    void updateDocumentPath(const std::optional<QString>& path);

    UIState uiState_ { 1<<ActionImportFont };
    std::unique_ptr<FontFaceViewModel> fontFaceViewModel_;
    std::optional<QString> documentPath_;
    QString documentTitle_;
    SourceCodeOptions sourceCodeOptions_;

    QMap<QString, QString> formats_; // identifier <-> human-readable
    QString currentFormat_; // identifier
    QSettings settings_;
};

#endif // MAINWINDOWMODEL_H
