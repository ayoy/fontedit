#include "mainwindow.h"

#include <QApplication>
#include <QtGui>
#include <QtCore>
#include "f2b.h"
#include <sstream>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    QFont font("Menlo", 40, QFont::Bold);
//    font.setStyleStrategy(QFont::StyleStrategy::NoAntialias);
//    QString text = " !\"#$%&'()*+,-./0123456789:;<=>?\n"
//                   "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_\n"
//                   "`abcdefghijklmnopqrstuvwxyz{|}~ ";
//    QFontMetrics fm(font);
//    qDebug() << font
//             << fm.height()
//             << fm.maxWidth();

//    QImage image(fm.maxWidth() * 32, fm.height() * 3, QImage::Format::Format_Mono);

//    QPainter p(&image);
//    p.setFont(font);
//    p.fillRect(QRect(QPoint(), image.size()), QColor(Qt::color0));
//    p.setPen(QPen(Qt::color1));
//    p.drawText(image.rect(), 0, " !\"#$%&'()*+,-./0123456789:;<=>?\n"
//               "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_\n"
//               "`abcdefghijklmnopqrstuvwxyz{|}~ ");
//    p.end();

//    QByteArray bytearray;
//    QBuffer buffer(&bytearray);
//    image.save(&buffer, "BMP");

//    std::istringstream is(bytearray.toStdString());
//    FontFace face(is);

//    qDebug() << bytearray;

    MainWindow w;
    w.show();
    return QApplication::exec();
}
