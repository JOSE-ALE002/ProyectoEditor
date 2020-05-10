#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
#include <QFileInfo>
#include <QSettings>
#include <QDesktopServices>
#include <QUrl>
#include <QApplication>
#include <QDesktopWidget>
#include <QSessionManager>
#include <qtextedit.h>
#include <QFontDialog>
#include <QFont>
#include <QPlainTextEdit>
#include <QColorDialog>
#include <QTextStream>
#include <qtextedit.h>
#include <QFontDialog>
#include <QFontDatabase>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setCentralWidget(ui->textEdit);
    createStatusBar();

    readSettings();

    connect(ui->textEdit->document(), &QTextDocument::contentsChanged,
            this, &MainWindow::documentWasModified);

#ifndef QT_NO_SESSIONMANAGER
    QGuiApplication::setFallbackSessionManagementEnabled(false);
    connect(qApp, &QGuiApplication::commitDataRequest,
            this, &MainWindow::commitData);
#endif

    setCurrentFile(QString());
    setUnifiedTitleAndToolBarOnMac(true);
}

bool MainWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName),
                                  file.errorString()));
        return false;
    }

    QTextStream out(&file);
#ifndef QT_NO_CURSOR
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    out << ui->textEdit->toPlainText();
#ifndef QT_NO_CURSOR
    QGuiApplication::restoreOverrideCursor();
#endif

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    ui->textEdit->document()->setModified(false);
    setWindowModified(false);

    QString shownName = curFile;
    if (curFile.isEmpty())
        shownName = "untitled.txt";
    setWindowFilePath(shownName);
}

void MainWindow::newFile()
{
    if (maybeSave()) {
        ui->textEdit->clear();
        setCurrentFile(QString());
    }
}


void MainWindow::open()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(
                    this, tr("Open File"),
                    "/home",
                    tr("Text files (*.txt);;Images (*.png *.xpm *.jpg);;XML files (*.xml);; ODT files (*.odt)"));
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}

void MainWindow::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }

    QTextStream in(&file);
#ifndef QT_NO_CURSOR
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    ui->textEdit->setPlainText(in.readAll());
#ifndef QT_NO_CURSOR
    QGuiApplication::restoreOverrideCursor();
#endif

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);
}

bool MainWindow::save()
{
    if (curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool MainWindow::saveAs()
{
    QFileDialog dialog(
                this, tr("Save File"),
                "/home/joseibanez/Documentos/Example.txt",
                tr("Text files (*.txt);;Images (*.png *.xpm *.jpg);;XML files (*.xml);; ODT files (*.odt)"));
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    if (dialog.exec() != QDialog::Accepted)
        return false;
    return saveFile(dialog.selectedFiles().first());
}


bool MainWindow::maybeSave()
{
    if (!ui->textEdit->document()->isModified())
        return true;
    const QMessageBox::StandardButton ret
        = QMessageBox::warning(this, tr("Application"),
                               tr("The document has been modified.\n"
                                  "Do you want to save your changes?"),
                               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    switch (ret) {
    case QMessageBox::Save:
        return save();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }
    return true;
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    if(maybeSave()){
        //writeSetting();
        event->accept();

    }else{

        event->ignore();
    }
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

#ifndef QT_NO_SESSIONMANAGER
void MainWindow::commitData(QSessionManager &manager)
{
    if (manager.allowsInteraction()) {
        if (!maybeSave())
            manager.cancel();
    } else {
        // Non-interactive: save without asking
        if (ui->textEdit->document()->isModified())
            save();
    }
}
#endif

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }
}


void MainWindow::writeSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("geometry", saveGeometry());
}

void MainWindow::documentWasModified()
{
    setWindowModified(ui->textEdit->document()->isModified());
}

void MainWindow::on_Editor_action_New_triggered()
{
    newFile();
}

void MainWindow::on_Editor_action_Open_triggered()
{
    open();
}

void MainWindow::on_Editor_action_Save_triggered()
{
    save();
}

void MainWindow::on_Editor_action_Save_As_triggered()
{
    saveAs();
}

void MainWindow::on_Editor_action_Search_Article_triggered()
{
    QString link = "http://constitution.org/cons/elsalvad.htm";
    QDesktopServices::openUrl(QUrl(link));
}

void MainWindow::on_Editor_action_Left_Align_triggered()
{
    ui->textEdit->setAlignment(Qt::AlignLeft);
}

void MainWindow::on_Editor_actionRight_Align_triggered()
{
    ui->textEdit->setAlignment(Qt::AlignRight);
}

void MainWindow::on_Editor_action_Center_Align_triggered()
{
     ui->textEdit->setAlignment(Qt::AlignCenter);
}

void MainWindow::on_action_Justify_Align_triggered()
{
    ui->textEdit->setAlignment(Qt::AlignJustify);
}
void MainWindow::on_Editor_action_Fonts_triggered()
{
    bool ok;
    QFont fontStyle = QFontDialog::getFont(&ok,this);

    if(ok){
            QTextCharFormat format;
            format.setFont(fontStyle);
            QTextCursor txtCursor(ui->textEdit->textCursor());
            txtCursor.setCharFormat(format);
            txtCursor.clearSelection();
            ui->textEdit->setTextCursor(txtCursor);
    }
    else{
            return;
    }
}

void MainWindow::on_Editor_action_Font_Colours_triggered()
{
    QColor txtColour = QColorDialog::getColor();
    ui->textEdit->setTextColor(txtColour);
}

void MainWindow::on_Editor_action_About_Editor_triggered()
{
    QMessageBox::about(this,"About TextEditor"," Creado por:\n\n * Jose Alejandro Ibañez Martinez (IM18002)\n\n * Jose Adolfo Gusman Solis (GS18030)");
}

void MainWindow::on_Editor_action_About_Qt_triggered()
{
    QMessageBox::aboutQt(this,"About Qt Creator.");
}

void MainWindow::on_Editor_actionMarriage_triggered()
{
    //ui->plainTextEdit->setPlainText("");
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(
                    this, tr("Open File"),
                    "Plantillas/Matrimonio Civil.txt",
                    tr("Text files (*.txt);;Images (*.png *.xpm *.jpg);;XML files (*.xml);; ODT files (*.odt)"));
        if (!fileName.isEmpty())
            loadFile(fileName);
    }

}

void MainWindow::on_actionCompraventa_de_Inmueble_triggered()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(
                    this, tr("Open File"),
                    "Plantillas/Compraventa de Inmueble.txt",
                    tr("Text files (*.txt);;Images (*.png *.xpm *.jpg);;XML files (*.xml);; ODT files (*.odt)"));
        if (!fileName.isEmpty())
            loadFile(fileName);
    }

}

void MainWindow::on_action_Declaracion_Juramentada_triggered()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(
                    this, tr("Open File"),
                    "Plantillas/Declaracion Juramentada.txt",
                    tr("Text files (*.txt);;Images (*.png *.xpm *.jpg);;XML files (*.xml);; ODT files (*.odt)"));
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}

void MainWindow::on_action_Show_all_Template_triggered()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(
                    this, tr("Open File"),
                    "Plantillas",
                    tr("Text files (*.txt);;Images (*.png *.xpm *.jpg);;XML files (*.xml);; ODT files (*.odt)"));
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}
