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

    QFont font("Monaco", 24, QFont::Bold);
    font.setStyleStrategy(QFont::StyleStrategy::NoAntialias);

    connect(ui->actionNew, &QAction::triggered, [=] () {
        qDebug() << "new";
    });

    connect(ui->actionImport_Font, &QAction::triggered, [&] () {
        bool ok;
        QFont f = QFontDialog::getFont(&ok, QFont(), this, tr("Select Font"), QFontDialog::MonospacedFonts);
        qDebug() << "new" << f;
        FontFaceViewModel viewModel { f };
        setupViewModel(std::move(viewModel));
    });
}

void MainWindow::setupViewModel(FontFaceViewModel &&viewModel)
{
    viewModel_ = viewModel;
    viewModel_.value().set_active_glyph_index(8);

    if (viewModel_.value().active_glyph() != nullptr) {
        displayGlyph(*viewModel_.value().active_glyph());
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::displayFace(const Font::Face &face)
{
    if (faceWidget_ == nullptr) {
        faceWidget_ = std::make_unique<FaceWidget>();
        ui->faceGraphicsView->scene()->addItem(faceWidget_.get());
    }

    faceWidget_->load(face);
}

void MainWindow::displayGlyph(const Font::Glyph &glyph)
{
    if (glyphWidget_ == nullptr) {
        glyphWidget_ = std::make_unique<GlyphWidget>(pixel_size);
        ui->glyphGraphicsView->scene()->addItem(glyphWidget_.get());

        connect(glyphWidget_.get(), &GlyphWidget::pixelChanged,
                [&] (Font::Point p, bool is_selected) {
            viewModel_->active_glyph()->set_pixel_set(p, is_selected);
        });
    }

    glyphWidget_->load(glyph);
}
