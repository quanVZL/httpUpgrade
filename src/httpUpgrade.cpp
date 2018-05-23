#include "httpUpgrade.hpp"

UpgradeDownload::UpgradeDownload(QObject *parent) : QObject(parent){
    mNetworkAccessManager = std::unique_ptr<QNetworkAccessManager>(new QNetworkAccessManager);
    startLink();
}

UpgradeDownload::~UpgradeDownload() { }

void UpgradeDownload::startLink() {
    mUrl = QUrl(SERVERADDRESS);

    QString fileName = DOWNLOADPATH + mUrl.fileName();

    mFile = fileName + "_tmp";

    if (mBytesHasReceived <= 0) {
        removeFile(mFile);
    }

    QNetworkRequest request;
    request.setUrl(mUrl);

    mNetworkReply = std::unique_ptr<QNetworkReply>(mNetworkAccessManager->get(request));

    connect(mNetworkReply.get(), &QNetworkReply::downloadProgress, this, &UpgradeDownload::onDownloadProcess);
    connect(mNetworkReply.get(), &QNetworkReply::readyRead, this, &UpgradeDownload::onReadyRead);
    connect(mNetworkReply.get(), &QNetworkReply::finished, this, &UpgradeDownload::onFinished);
//    connect(mNetworkReply.get(), static_cast<void(QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, &UpgradeDownload::onError);
    connect(mNetworkReply.get(), SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
}

void UpgradeDownload::onDownloadProcess(qint64 byteReceived, qint64 byteTotal) {
    if (mBytesReceived != byteReceived) {
        mBytesReceived = byteReceived;
    }
    if (mBytesTotalReceived != byteTotal) {
        mBytesTotalReceived = byteTotal;
    }

    emit sendDownloadProcess(mBytesReceived, mBytesTotalReceived);
}

void UpgradeDownload::onReadyRead() {
    QFile file(mFile);

    if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
       file.write(mNetworkReply->readAll());
    }
    file.close();
}

void UpgradeDownload::onError(QNetworkReply::NetworkError code) {

    QString strError = mNetworkReply->errorString();
    qWarning() << "status code: " << code << strError;
    emit sendError(strError);
    closeDownload();
}

void UpgradeDownload::onFinished() {
    QVariant statusCode = mNetworkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    if (mNetworkReply->error() == QNetworkReply::NoError) {
        QFileInfo fileInfo(mFile);
        if (fileInfo.exists()) {
            int index = mFile.lastIndexOf("_tmp");
            QString realName = mFile.left(index);
            QFile::rename(mFile, realName);
        }
    }
    emit sendFinished(statusCode.toInt());
}

void UpgradeDownload::onStopWork() {
    mNetworkReply.release();
}

void UpgradeDownload::resetData() {
    mBytesHasReceived = 0;
    mBytesReceived = 0;
    mBytesTotalReceived = 0;
}

void UpgradeDownload::removeFile(const QString &file) {
    QFileInfo fileInfo(file);

    if (fileInfo.exists()) {
        QFile::remove(file);
    }
}

void UpgradeDownload::closeDownload() {
    onStopWork();
    resetData();
    removeFile(mFile);
}


