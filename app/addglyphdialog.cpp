#include "addglyphdialog.h"
#include "./ui_addglyphdialog.h"
#include "facewidget.h"
#include "qfontfacereader.h"

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

    faceWidget_->load(faceViewModel.face(), f2b::font::margins {});
    connect(faceWidget_, &FaceWidget::currentGlyphIndexChanged, ui_->copyRadio, &QRadioButton::click);
    connect(faceWidget_, &FaceWidget::currentGlyphIndexChanged, [&, faceViewModel](std::optional<std::size_t> index) {
        if (index.has_value())
            newGlyph_ = faceViewModel.face().glyph_at(index.value());
    });
    connect(ui_->buttonBox, &QDialogButtonBox::accepted, [&, faceViewModel] {
        if (ui_->emptyRadio->isChecked()) {
            newGlyph_ = f2b::font::glyph { faceViewModel.face().glyph_size() };
        } else if (ui_->characterRadio->isChecked()) {
            QFontFaceReader adapter {
                faceViewModel.font().value(),
                ui_->characterLineEdit->text().toStdString(),
                faceViewModel.face().glyph_size()
            };
            newGlyph_ = f2b::font::face(adapter).glyph_at(0);
        }
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
