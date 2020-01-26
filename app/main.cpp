#include "mainwindow.h"

#include <QApplication>
#include <QtGui>
#include <QtCore>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    QFont font("Menlo", 40, QFont::Bold);
//    font.setStyleStrategy(QFont::StyleStrategy::NoAntialias);
//    QFontMetrics fm(font);
//    qDebug() << font
//             << fm.height()
//             << fm.maxWidth();

//    QImage image(fm.maxWidth() * 32, fm.height() * 3, QImage::Format::Format_RGB32);

//    QPainter p(&image);
//    p.setFont(font);
//    p.fillRect(QRect(QPoint(), image.size()), QColor(Qt::white));
//    p.setPen(QPen(Qt::black));
//    p.drawText(image.rect(), 0, " !\"#$%&'()*+,-./0123456789:;<=>?\n"
//               "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_\n"
//               "`abcdefghijklmnopqrstuvwxyz{|}~ ");
//    p.end();
//    image.save("hej.png");

    MainWindow w;
    w.show();
    return QApplication::exec();
}
