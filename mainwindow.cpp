#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    initData();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::initData() {
    ui->progressBar->setValue(0);
    ui->pushButton->setEnabled(false);
    ui->fileSizeValue->setText(QStringLiteral("0 K"));
    mDownloadEvent = std::unique_ptr<UpgradeDownload>(new UpgradeDownload);

    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::exit);

    connect(mDownloadEvent.get(), &UpgradeDownload::sendDownloadProcess, this, &MainWindow::onDownloadProcess);
    connect(mDownloadEvent.get(), &UpgradeDownload::sendFinished, this, &MainWindow::onFinished);
    connect(mDownloadEvent.get(), &UpgradeDownload::sendError, this, &MainWindow::onError);
}

void MainWindow::onDownloadProcess(qint64 bytesReceived, qint64 bytesTotal) {
    ui->progressBar->setValue(bytesReceived);
    ui->fileReceicedSize->setText(unitsConversion(bytesReceived));

    if (mFileSize != bytesTotal) {
        mFileSize = bytesTotal;
        ui->progressBar->setMaximum(bytesTotal);
        ui->fileSizeValue->setText(unitsConversion(mFileSize));
    }
}

void MainWindow::onFinished(int status) {
    if (status >= 200 && status <= 206) {
        ui->pushButton->setEnabled(true);
    } else {
        mErrorInfo += QStringLiteral("Http Status Code is :") + status + "\n";
    }
}

void MainWindow::onError(QString str) {
    mErrorInfo += str + "\n";
    showMessageBox(QMessageBox::Warning, "Warn", mErrorInfo, QMessageBox::Ok);
}

void MainWindow::exit() {
    QCoreApplication::quit();
}

QString MainWindow::unitsConversion(int bytes) {
    QString unit = "KB";
    if (bytes <= 0) {
        bytes = 0;
        unit = "B";
    } else if (bytes < UNIT_KB) {
        bytes = 1;
        unit = "KB";
    } else if (bytes < UNIT_MB) {
        bytes /= UNIT_KB;
        unit = "KB";
    } else {
        bytes /= UNIT_MB;
        unit = "MB";
    }

    return QString("%1 %2").arg(bytes).arg(unit);
}

void MainWindow::showMessageBox(QMessageBox::Icon icon,
                                const QString &title,
                                const QString &text,
                                QMessageBox::StandardButton buttons) {
    QMessageBox msgBox(icon, title, text, buttons);
    msgBox.exec();
}
