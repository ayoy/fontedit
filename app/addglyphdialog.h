#ifndef ADDGLYPHDIALOG_H
#define ADDGLYPHDIALOG_H

#include <QDialog>
#include <QGraphicsScene>
#include <memory>
#include <optional>
#include "f2b.h"

namespace Ui {
class AddGlyphDialog;
}

class FaceWidget;

class AddGlyphDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddGlyphDialog(const Font::Face& face, QWidget *parent = nullptr);
    ~AddGlyphDialog();

signals:
    void glyphSelected(const std::optional<Font::Glyph>& glyph);

private:
    Ui::AddGlyphDialog *ui_;
    FaceWidget *faceWidget_ { nullptr };
    std::unique_ptr<QGraphicsScene> faceScene_ { std::make_unique<QGraphicsScene>() };

    std::optional<Font::Glyph> newGlyph_ {};
};

#endif // ADDGLYPHDIALOG_H
