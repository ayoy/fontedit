#ifndef MAINWINDOWMODEL_H
#define MAINWINDOWMODEL_H

#include "fontfaceviewmodel.h"
#include <f2b.h>
#include <memory>
#include <functional>
#include <optional>
#include <bitset>
#include <variant>
#include <vector>
#include <mutex>

#include <QMap>
#include <QSettings>

struct UIState {
    enum InterfaceAction {
        ActionAddGlyph = 0,
        ActionDeleteGlyph,
        ActionSave,
        ActionClose,
        ActionCopy,
        ActionPaste,
        ActionPrint,
        ActionExport,
        ActionTabEdit,
        ActionTabCode,
        ActionCount,
        ActionFirst = ActionAddGlyph
    };

    enum UserAction {
        UserIdle = 0,
        UserLoadedDocument,
        UserLoadedGlyph,
    };

    enum Message {
        MessageIdle = 0,
        MessageLoadedFace,
        MessageLoadedGlyph
    };

    enum Tab {
        TabEdit,
        TabCode
    };

    std::bitset<ActionCount> actions;
    UserAction lastUserAction;
    Message statusBarMessage;
    Tab selectedTab;
};


class MainWindowModel: public QObject
{
    Q_OBJECT

public:

    using InputEvent = std::variant<UIState::InterfaceAction,UIState::UserAction>;

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

    const std::vector<std::pair<SourceCode::Indentation, QString>>& indentationStyles() const {
        return indentationStyles_;
    }

    QString indentationStyleCaption() const;

    void registerInputEvent(InputEvent e);

    const std::optional<QString>& currentDocumentPath() const {
        return documentPath_;
    }

    QString documentTitle() const { return documentTitle_; }
    void updateDocumentTitle();

    void setFontArrayName(const QString& fontArrayName) {
        if (fontArrayName_ != fontArrayName) {
            fontArrayName_ = fontArrayName;
            reloadSourceCode();
        }
    }

    QString lastVisitedDirectory() const;

    QString lastSourceCodeDirectory() const;
    void setLastSourceCodeDirectory(const QString& path);

    void resetGlyph(std::size_t index);
    void modifyGlyph(std::size_t index, const Font::Glyph &new_glyph);
    void modifyGlyph(std::size_t index,
                     const BatchPixelChange &change,
                     BatchPixelChange::ChangeType changeType);
    void appendGlyph(Font::Glyph glyph);
    void deleteGlyph(std::size_t index);

    const QString& sourceCode() {
        std::scoped_lock { sourceCodeMutex_ };
        return sourceCode_;
    }

public slots:
    void importFont(const QFont& font);

    void openDocument(const QString& fileName);
    void saveDocument(const QString& fileName);
    void closeCurrentDocument();

    void setActiveGlyphIndex(std::size_t index);
    void setInvertBits(bool enabled);
    void setMSBEnabled(bool enabled);
    void setIncludeLineSpacing(bool enabled);
    void setOutputFormat(const QString &format); // human-readable
    void setIndentation(const QString &indentationLabel); // human-readable

signals:
    void uiStateChanged(UIState state) const;
    void faceLoaded(Font::Face& face) const;
    void activeGlyphChanged(const Font::Glyph& glyph) const;
    void sourceCodeUpdating() const;
    void sourceCodeChanged() const;
    void runnableFinished() const;
    void documentTitleChanged(const QString& title);
    void documentClosed();
    void documentError(const QString& error);

private:
    void reloadSourceCode();
    void setDocumentPath(const std::optional<QString>& path);
    void setLastVisitedDirectory(const QString& path);
    void openDocument(const QString& fileName, bool failSilently);

    UIState uiState_ {};
    std::unique_ptr<FontFaceViewModel> fontFaceViewModel_;
    std::optional<QString> documentPath_;
    QString documentTitle_;
    QString fontArrayName_;
    SourceCodeOptions sourceCodeOptions_;

    QString sourceCode_;
    std::mutex sourceCodeMutex_;

    QMap<QString, QString> formats_; // identifier <-> human-readable
    QString currentFormat_; // identifier
    std::vector<std::pair<SourceCode::Indentation, QString>> indentationStyles_;
    QSettings settings_;
};

#endif // MAINWINDOWMODEL_H
