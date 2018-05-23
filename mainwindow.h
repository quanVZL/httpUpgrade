#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include "src/httpUpgrade.hpp"
#include "memory"

#define UNIT_KB 1024        //KB
#define UNIT_MB 1024*1024   //MB

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void initData();
    void onDownloadProcess(qint64 bytesReceived, qint64 bytesTotal);
    void onFinished(int status);
    void onError(QString str);

    void exit();
    QString unitsConversion(int bytes);

    void showMessageBox(QMessageBox::Icon icon,
                        const QString &title,
                        const QString &text,
                        QMessageBox::StandardButton buttons);

private:
    Ui::MainWindow *ui;
    int mFileSize = 0;
    QString mErrorInfo = "";
    std::unique_ptr<UpgradeDownload> mDownloadEvent = nullptr;
};

#endif // MAINWINDOW_H
