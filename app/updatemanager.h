#ifndef UPDATEMANAGER_H
#define UPDATEMANAGER_H

#include <QObject>
#include <QtNetwork>
#include <memory>

class UpdateManager : public QObject
{
    Q_OBJECT
public:

    struct Update {
        QString currentVersion;
        QString latestVersion;
        QString releaseNotes;
        QUrl webpageURL;
    };

    explicit UpdateManager(QObject *parent = nullptr);

    void checkForUpdatesIfNeeded();
    void checkForUpdates(bool verbose = false);

    bool shouldCheckAtStartup() const { return shouldCheckAtStartup_; }
    void setShouldCheckAtStartup(bool check);

signals:
    void updateAvailable(const Update& update);
    void updateNotAvailable();

private:
    void handleReply(QNetworkReply *reply);

    std::unique_ptr<QNetworkAccessManager> manager_;
    bool shouldCheckAtStartup_ { true };
    bool isChecking_ { false };
    QSettings settings_;
};

#endif // UPDATEMANAGER_H
