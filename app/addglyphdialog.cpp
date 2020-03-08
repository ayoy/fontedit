#include "addglyphdialog.h"
#include "./ui_addglyphdialog.h"
#include "facewidget.h"

AddGlyphDialog::AddGlyphDialog(const FontFaceViewModel& faceViewModel, QWidget *parent) :
    QDialog(parent),
    ui_ { new Ui::AddGlyphDialog }
{
    ui_->setupUi(this);

    faceScene_->setBackgroundBrush(QBrush(Qt::lightGray));
    faceWidget_ = new FaceWidget(7);

    ui_->titleLabel->setText(tr("Add a new Glyph to %1").arg(faceViewModel.faceInfo().fontName));

    ui_->faceGraphicsView->setScene(faceScene_.get());
    ui_->faceGraphicsView->scene()->addItem(faceWidget_);

    faceWidget_->load(faceViewModel.face(), Font::Margins {});
    connect(faceWidget_, &FaceWidget::currentGlyphIndexChanged, ui_->copyRadio, &QRadioButton::click);
    connect(faceWidget_, &FaceWidget::currentGlyphIndexChanged, [&, faceViewModel](std::size_t index) {
        newGlyph_ = faceViewModel.face().glyph_at(index);
    });
    connect(this, &AddGlyphDialog::accepted, [&]{
        emit glyphSelected(newGlyph_);
    });

    if (faceViewModel.font().has_value()) {
        ui_->characterRadio->setEnabled(true);
        ui_->characterErrorLabel->setVisible(false);
        ui_->characterLineEdit->setVisible(true);
    } else {
        ui_->characterRadio->setEnabled(false);
        ui_->characterErrorLabel->setVisible(true);
        ui_->characterLineEdit->setVisible(false);
    }
}

AddGlyphDialog::~AddGlyphDialog()
{
    delete ui_;
}
