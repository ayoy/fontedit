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

    face_scene_->setBackgroundBrush(QBrush(Qt::lightGray));
    ui->faceGraphicsView->setScene(face_scene_.get());


    QFont font("Monaco", 24, QFont::Bold);
    font.setStyleStrategy(QFont::StyleStrategy::NoAntialias);

    connect(ui->actionNew, &QAction::triggered, [=] () {
        qDebug() << "new";
    });

    connect(ui->actionImport_Font, &QAction::triggered, [&] () {
        bool ok;
        QFont f = QFontDialog::getFont(&ok, QFont(), this, tr("Select Font"), QFontDialog::MonospacedFonts);
        qDebug() << "selected font:" << f << "ok?" << ok;
        FontFaceViewModel viewModel { f };
        setupViewModel(std::move(viewModel));
    });
}

void MainWindow::setupViewModel(FontFaceViewModel &&viewModel)
{
    view_model_ = viewModel;
    view_model_.value().set_active_glyph_index(8);

    displayFace(view_model_.value().face());

    if (view_model_.value().active_glyph() != nullptr) {
        displayGlyph(*view_model_.value().active_glyph());
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::displayFace(const Font::Face &face)
{
    if (face_widget_ == nullptr) {
        face_widget_ = new FaceWidget();
        ui->faceGraphicsView->scene()->addItem(face_widget_);
    }

    face_widget_->load(face);
}

void MainWindow::displayGlyph(const Font::Glyph &glyph)
{
    if (glyph_widget_ == nullptr) {
        glyph_widget_ = std::make_unique<GlyphWidget>(pixel_size);
        ui->glyphGraphicsView->scene()->addItem(glyph_widget_.get());

        connect(glyph_widget_.get(), &GlyphWidget::pixelChanged,
                [&] (Font::Point p, bool is_selected) {
            view_model_->active_glyph()->set_pixel_set(p, is_selected);
        });
    }

    glyph_widget_->load(glyph);
    ui->glyphGraphicsView->fitInView(glyph_widget_->rect(), Qt::KeepAspectRatio);
}
