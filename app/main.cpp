#include "mainwindow.h"
#include "global.h"

#include <QApplication>
#include <QtGui>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QApplication::setOrganizationName(Global::organization_name);
    QApplication::setOrganizationDomain(Global::organization_domain);
    QApplication::setApplicationName(Global::application_name);
    QApplication::setApplicationVersion(QString::fromStdString(std::string(Global::application_version)));

    QTranslator myappTranslator;
    if (myappTranslator.load(":/l10n/fontedit_en.qm")) {
        a.installTranslator(&myappTranslator);
    }

    MainWindow w;
    w.show();
    return QApplication::exec();
}
