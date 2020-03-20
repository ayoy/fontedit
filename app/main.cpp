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
    QApplication::setApplicationVersion(Global::application_version);

    QTranslator myappTranslator;
    myappTranslator.load(":/l10n/fontedit_en.qm");
    a.installTranslator(&myappTranslator);

    a.setAttribute(Qt::AA_UseHighDpiPixmaps);

    MainWindow w;
    w.show();
    return QApplication::exec();
}
