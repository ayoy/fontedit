#include "updatehelper.h"
#include <QtNetwork>
#include <QJsonDocument>
#include <QApplication>
#include "semver.hpp"

namespace SettingsKey {
static const QString checkForUpdatesAtStartup = "main_window/check_for_updates_at_startup";
}

UpdateHelper::UpdateHelper(QObject *parent) : QObject(parent),
    manager_ { std::make_unique<QNetworkAccessManager>() }
{
    shouldCheckAtStartup_ = settings_.value(SettingsKey::checkForUpdatesAtStartup, true).toBool();
    manager_->setStrictTransportSecurityEnabled(true);
//    qDebug() << QSslSocket::supportsSsl() << QSslSocket::sslLibraryBuildVersionString() << QSslSocket::sslLibraryVersionString();
    connect(manager_.get(), &QNetworkAccessManager::finished, this, &UpdateHelper::handleReply);
}

void UpdateHelper::setShouldCheckAtStartup(bool check)
{
    shouldCheckAtStartup_ = check;
    settings_.setValue(SettingsKey::checkForUpdatesAtStartup, check);
}

void UpdateHelper::checkForUpdatesIfNeeded()
{
    if (shouldCheckAtStartup_) {
        checkForUpdates(false);
    }
}

void UpdateHelper::checkForUpdates(bool verbose)
{
    QNetworkRequest request(QUrl("https://api.github.com/repos/ayoy/fontedit/releases/latest"));
    request.setRawHeader("Accept", "application/vnd.github.v3+json");
    request.setAttribute(QNetworkRequest::User, verbose);
    manager_->get(request);
}

void UpdateHelper::handleReply(QNetworkReply *reply)
{
    auto verbose = reply->request().attribute(QNetworkRequest::User).toBool();

    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Error fetching latest version:" << reply->error();
        reply->deleteLater();
        return;
    }

    QJsonParseError error;
    auto replyJSON = QJsonDocument::fromJson(reply->readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug() << "Error parsing JSON:" << error.errorString();
        reply->deleteLater();
        return;
    }

    auto latestVersionString = replyJSON.object().value("tag_name").toString();

    if (latestVersionString.isNull()) {
        qDebug() << "Latest version unknown";
        reply->deleteLater();
        return;
    } else if (latestVersionString.startsWith('v')) {
        latestVersionString.remove(0, 1);
    }

    semver::version latestVersion { latestVersionString.toStdString() };
    semver::version currentVersion { QApplication::applicationVersion().toStdString() };

    if (latestVersion > currentVersion) {
        auto releaseNotes = replyJSON.object().value("body").toString();
        auto urlString = replyJSON.object().value("html_url").toString();
        qDebug() << "An update is available. Check out" << urlString << "for more info";
        emit updateAvailable({
                                 QApplication::applicationVersion(),
                                 latestVersionString,
                                 releaseNotes,
                                 QUrl(urlString)
                             });
    } else {
        qDebug() << "No update available";
        if (verbose)
            emit updateNotAvailable();
    }

    reply->deleteLater();
}
