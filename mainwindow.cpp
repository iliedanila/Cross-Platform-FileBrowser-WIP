#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDir>
#include <QProgressDialog>
#include <QProcess>
#include <QInputDialog>
#include <QKeyEvent>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    CustomizeUI();
    Connect();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F1)
    {
        ui->leftBrowser->toggleDriveMenu();
    }
    else if (event->key() == Qt::Key_F2)
    {
        ui->rightBrowser->toggleDriveMenu();
    }
    else
    {
        QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::CustomizeUI()
{
    ui->viewButton->setEnabled(false);
    ui->editButton->setEnabled(false);
}

void MainWindow::Connect()
{
    bool connected = true;
    connected &= connect(
        ui->leftBrowser,
        SIGNAL(switchMe()),
        this,
        SLOT(switchToRightBrowser())) != Q_NULLPTR;

    connected &= connect(
        ui->rightBrowser,
        SIGNAL(switchMe()),
        this,
        SLOT(switchToLeftBrowser())) != Q_NULLPTR;

    connected &= connect(
        ui->leftDriveButton,
        SIGNAL(clicked()),
        ui->leftBrowser,
        SLOT(toggleDriveMenu())) != Q_NULLPTR;

    connected &= connect(
        ui->rightDriveButton,
        SIGNAL(clicked()),
        ui->rightBrowser,
        SLOT(toggleDriveMenu())) != Q_NULLPTR;

    connected &= connect(
        ui->copyButton,
        SIGNAL(clicked()),
        this,
        SLOT(handleCopy())) != Q_NULLPTR;

    connected &= connect(
        ui->leftBrowser,
        SIGNAL(copy()),
        this,
        SLOT(handleCopy())) != Q_NULLPTR;

    connected &= connect(
        ui->rightBrowser,
        SIGNAL(copy()),
        this,
        SLOT(handleCopy())) != Q_NULLPTR;

    connected &= connect(
        ui->moveButton,
        SIGNAL(clicked()),
        this,
        SLOT(handleMove())) != Q_NULLPTR;

    connected &= connect(
        ui->leftBrowser,
        SIGNAL(move()),
        this,
        SLOT(handleMove())) != Q_NULLPTR;

    connected &= connect(
        ui->rightBrowser,
        SIGNAL(move()),
        this,
        SLOT(handleMove())) != Q_NULLPTR;

    connected &= connect(
        ui->deleteButton,
        SIGNAL(clicked()),
        this,
        SLOT(handleDel())) != Q_NULLPTR;

    connected &= connect(
        ui->leftBrowser,
        SIGNAL(del()),
        this,
        SLOT(handleDel())) != Q_NULLPTR;

    connected &= connect(
        ui->rightBrowser,
        SIGNAL(del()),
        this,
        SLOT(handleDel())) != Q_NULLPTR;

    connected &= connect(
        ui->newFolderButton,
        SIGNAL(clicked()),
        this,
        SLOT(handleNewFolder())) != Q_NULLPTR;

    connected &= connect(
        ui->leftBrowser,
        SIGNAL(newFolder()),
        this,
        SLOT(handleNewFolder())) != Q_NULLPTR;

    connected &= connect(
        ui->rightBrowser,
        SIGNAL(newFolder()),
        this,
        SLOT(handleNewFolder())) != Q_NULLPTR;

    Q_ASSERT(connected);
}

void MainWindow::switchToLeftBrowser()
{
    ui->leftBrowser->setFocus();
}

void MainWindow::switchToRightBrowser()
{
    ui->rightBrowser->setFocus();
}

void MainWindow::handleCopy()
{
    QStringList filePaths;
    QString rootFolder;
    QString destination;
    if (ui->leftBrowser->hasFocus())
    {
        filePaths = ui->leftBrowser->getSelected();
        rootFolder = ui->leftBrowser->getRootPath();
        destination = ui->rightBrowser->getRootPath();
    }
    if (ui->rightBrowser->hasFocus())
    {
        filePaths = ui->rightBrowser->getSelected();
        rootFolder = ui->rightBrowser->getRootPath();
        destination = ui->leftBrowser->getRootPath();
    }

    if (filePaths.count())
    {
        FileOperation* copyOperation = new FileOperation(FileOperation::eCopy, rootFolder, filePaths, destination, this);

        QProgressDialog* dialog = new QProgressDialog("Copy files...", "Cancel", 0, 100, this);
        dialog->setWindowModality(Qt::NonModal);

        connect(copyOperation,
                SIGNAL(setProgress(int)),
                dialog,
                SLOT(setValue(int)));
        connect(copyOperation, SIGNAL(finished()), copyOperation, SLOT(deleteLater()));
        connect(copyOperation, SIGNAL(finished()), dialog, SLOT(close()));
        connect(dialog, SIGNAL(canceled()), copyOperation, SLOT(cancel()));

        copyOperation->start();
        dialog->show();
    }
}

void MainWindow::handleMove()
{
    QStringList filePaths;
    QString rootFolder;
    QString destination;
    if (ui->leftBrowser->hasFocus())
    {
        filePaths = ui->leftBrowser->getSelected();
        rootFolder = ui->leftBrowser->getRootPath();
        destination = ui->rightBrowser->getRootPath();
    }
    if (ui->rightBrowser->hasFocus())
    {
        filePaths = ui->rightBrowser->getSelected();
        rootFolder = ui->rightBrowser->getRootPath();
        destination = ui->leftBrowser->getRootPath();
    }

    if (filePaths.count())
    {
        FileOperation* moveOperation = new FileOperation(FileOperation::eMove, rootFolder, filePaths, destination, this);

        QProgressDialog* dialog = new QProgressDialog("Move files...", "Cancel", 0, 100, this);
        dialog->setWindowModality(Qt::NonModal);

        connect(moveOperation,
                SIGNAL(setProgress(int)),
                dialog,
                SLOT(setValue(int)));
        connect(moveOperation, SIGNAL(finished()), moveOperation, SLOT(deleteLater()));
        connect(moveOperation, SIGNAL(finished()), dialog, SLOT(close()));
        connect(dialog, SIGNAL(canceled()), moveOperation, SLOT(cancel()));

        moveOperation->start();
        dialog->show();
    }
}

void MainWindow::handleDel()
{
    QStringList filePaths;
    QString rootFolder;

    if (ui->leftBrowser->hasFocus())
    {
        filePaths = ui->leftBrowser->getSelected();
        rootFolder = ui->leftBrowser->getRootPath();
    }
    if (ui->rightBrowser->hasFocus())
    {
        filePaths = ui->rightBrowser->getSelected();
        rootFolder = ui->rightBrowser->getRootPath();
    }

    if (filePaths.count())
    {
        FileOperation* delOperation = new FileOperation(FileOperation::eDelete, rootFolder, filePaths, QString(), this);
        QProgressDialog* dialog = new QProgressDialog("Deleting files...", "Cancel", 0, 100, this);
        dialog->setWindowModality(Qt::NonModal);

        connect(delOperation,
                SIGNAL(setProgress(int)),
                dialog,
                SLOT(setValue(int)));
        connect(delOperation, SIGNAL(finished()), delOperation, SLOT(deleteLater()));
        connect(delOperation, SIGNAL(finished()), dialog, SLOT(close()));
        connect(dialog, SIGNAL(canceled()), delOperation, SLOT(cancel()));

        delOperation->start();
        dialog->show();

    }
}

void MainWindow::handleNewFolder()
{
    QString rootFolder;
    if (ui->leftBrowser->hasFocus())
    {
        rootFolder = ui->leftBrowser->getRootPath();
    }
    else
    {
        rootFolder = ui->rightBrowser->getRootPath();
    }

    bool ok;
    QString folderName = QInputDialog::getText(this, "New Folder Name", "Insert new folder name:",
                                               QLineEdit::Normal, QString(), &ok);
    if (ok && !folderName.isEmpty())
    {
        QDir currentFolder(rootFolder);
        currentFolder.mkdir(folderName);
    }
}
