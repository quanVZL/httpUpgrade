#pragma once

#include <QObject>
#include <QFile>
#include <QFileInfo>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include "memory"

#define SERVERADDRESS "http://yoctodata.autoio.org/projects/Fast/1.2-fast-20180509140334/upgrade.squashfs"
#define DOWNLOADPATH "/home/quanwei/"

class UpgradeDownload : public QObject {
    Q_OBJECT
public:
    UpgradeDownload(QObject *parent = nullptr);
    ~UpgradeDownload();

public slots:
    void onError(QNetworkReply::NetworkError code);

signals:
    void sendDownloadProcess(qint64, qint64);
    void sendFinished(int);
    void sendError(QString errorInfo);

private:
    void startLink();
    void onDownloadProcess(qint64 byteReceived, qint64 byteTotal);
    void onReadyRead();
    void onFinished();
    void onStopWork();
    void resetData();
    void removeFile(const QString &file);
    void closeDownload();

private:
    QUrl mUrl = QUrl("");
    QString mFile;

    int mBytesReceived = 0;
    int mBytesHasReceived = 0;
    int mBytesTotalReceived = 0;

    std::unique_ptr<QNetworkAccessManager> mNetworkAccessManager = nullptr;
    std::unique_ptr<QNetworkReply> mNetworkReply = nullptr;
};
