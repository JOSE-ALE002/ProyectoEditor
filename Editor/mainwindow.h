#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSessionManager>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);


private slots:

    void closeEvent(QCloseEvent *event);

    void on_Editor_action_New_triggered();

    void on_Editor_action_Open_triggered();

    void on_Editor_action_Save_triggered();

    void on_Editor_action_Save_As_triggered();

    void on_Editor_action_Search_Article_triggered();

    void on_Editor_actionMarriage_triggered();

    void on_Editor_action_Left_Align_triggered();

    void on_Editor_actionRight_Align_triggered();

    void on_Editor_action_Center_Align_triggered();

    void on_Editor_action_Fonts_triggered();

    void on_Editor_action_Font_Colours_triggered();

    void on_Editor_action_About_Editor_triggered();

    void on_Editor_action_About_Qt_triggered();

    void on_action_Justify_Align_triggered();

    void on_actionCompraventa_de_Inmueble_triggered();

    void on_action_Declaracion_Juramentada_triggered();

    void on_action_Show_all_Template_triggered();

private:
    Ui::MainWindow *ui;
    bool saveFile(const QString &fileName);
    void loadFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    void documentWasModified();
#ifndef QT_NO_SESSIONMANAGER
    void commitData(QSessionManager &);
#endif
    void createStatusBar();
    void readSettings();
    void writeSettings();
    void newFile();
    void open();
    bool save();
    bool saveAs();
    bool maybeSave();

    QString strippedName(const QString &fullFileName);

    QString curFile;
};

#endif
