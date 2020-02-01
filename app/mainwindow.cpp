#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "f2b.h"
#include "facewidget.h"
#include "fontfaceimporter.h"
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

    auto importer = FontFaceImporter();
    auto face = importer.import_face(font);

    displayGlyph(face['}']);

    connect(ui->actionNew, &QAction::triggered, [=] () {
        qDebug() << "new";
    });

    connect(ui->actionImport_Font, &QAction::triggered, [=] () {
        bool ok;
        QFont f = QFontDialog::getFont(&ok, QFont(), this, tr("Select Font"), QFontDialog::MonospacedFonts);
        qDebug() << "new" << f;
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::displayGlyph(const Font::Glyph &glyph)
{
    if (glyphWidget_ == nullptr) {
        glyphWidget_ = new GlyphWidget(pixel_size);
        ui->glyphGraphicsView->scene()->addItem(glyphWidget_);

        QObject::connect(glyphWidget_, SIGNAL(pixelChanged(Font::Point, bool)),
                         this, SLOT(updatePixel(Font::Point, bool)));
    }

    glyphWidget_->loadGlyph(glyph);
    glyph_ = glyph;
}

void MainWindow::updatePixel(Font::Point pos, bool isSelected)
{
    glyph_->pixels[pos.offset(glyph_->size())] = isSelected;
    for (auto b : glyph_->pixels) {
        std::cout << b;
    }
    std::cout << std::endl;
}
