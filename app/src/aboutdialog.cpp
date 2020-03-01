#include "aboutdialog.h"
#include "./ui_aboutdialog.h"
#include <QIcon>
#include <QDesktopServices>
#include <QUrl>

static constexpr auto about = R"(<html>
             <head/>
             <body>
                <p><span style="font-size:18pt;"><b>FontEdit </b></span> v##version## build ##build##</p>
                <p>
                    Copyright ##year## Dominik Kapusta<br/>
                    <a href="https://kapusta.cc">https://kapusta.cc</a>
                    •
                    <a href="https://twitter.com/ayoy">@ayoy</a>
                </p>
                <p align="center">
                    <br/>
                    <a href="https://github.com/ayoy/fontedit">Get the Source Code</a>
                    •
                    <a href="https://github.com/ayoy/fontedit/issues">Report a Bug</a>
                    <br/>
                </p>
                <p>This program is distributed under the terms of
                    <a href="https://www.gnu.org/licenses/gpl-3.0.en.html">
                    General Public License v3</a>.
                </p>
                <p>Icons made by <a href="https://www.flaticon.com/authors/smashicons">Smashicons</a> and
                    <a href="https://www.flaticon.com/authors/freepik">Freepik</a>
                    from <a href="https://www.flaticon.com/">www.flaticon.com</a>.
                </p>
             </body>
             </html>)";

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    QIcon icon(":/icon/assets/icon/fontedit96.png");
    ui->iconLabel->setPixmap(icon.pixmap(ui->iconLabel->size()));
    auto text = QString::fromStdString({about});
    text.replace("##version##", VERSION);
    text.replace("##build##", BUILD);
    text.replace("##year##", YEAR);

    connect(ui->htmlLabel, &QLabel::linkActivated, [&](const QString& link) {
        QDesktopServices::openUrl(QUrl(link));
    });

    ui->htmlLabel->setText(text);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
