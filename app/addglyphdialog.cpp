#include "addglyphdialog.h"
#include "./ui_addglyphdialog.h"
#include "facewidget.h"

AddGlyphDialog::AddGlyphDialog(const Font::Face& face, QWidget *parent) :
    QDialog(parent),
    ui_ { new Ui::AddGlyphDialog }
{
    ui_->setupUi(this);

    faceScene_->setBackgroundBrush(QBrush(Qt::lightGray));
    faceWidget_ = new FaceWidget(7);

    ui_->faceGraphicsView->setScene(faceScene_.get());
    ui_->faceGraphicsView->scene()->addItem(faceWidget_);

    faceWidget_->load(face, Font::Margins {});
    connect(faceWidget_, &FaceWidget::currentGlyphIndexChanged, ui_->copyRadio, &QRadioButton::click);
    connect(faceWidget_, &FaceWidget::currentGlyphIndexChanged, [&, face](std::size_t index) {
        newGlyph_ = face.glyph_at(index);
    });
    connect(this, &AddGlyphDialog::accepted, [&]{
        emit glyphSelected(newGlyph_);
    });
}

AddGlyphDialog::~AddGlyphDialog()
{
    delete ui_;
}
