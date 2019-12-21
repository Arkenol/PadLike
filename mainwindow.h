#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QApplication>
#include <QFileDialog>
#include <QFontMetrics>
#include <QLabel>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QPageSetupDialog>
#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QToolBar>
#include <QSettings>
#include <QStatusBar>
#include <QString>
#include <QToolBar>

#include "codeeditor.h"
#include "config.h"
#include "finddialog.h"
#include "replacedialog.h"
#include "helpdialog.h"

#define MAX_RecentFiles 5

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    CodeEditor m_TextEditor;
    QLabel m_CursorStatus;
    QLabel m_Status;
    QLabel m_Tablabel;
    QLabel *m_ShowLabel;
    QLabel *m_EndofLineLabel;
    QString m_filepath;
    bool m_isTextChanged;

    /*
    enum EndofLine{
        Windows,Macs,Unixs,Defaults
    };
    EndofLine LineEndformat = EndofLine::Windows;
    EndofLine getLineEndformat(){
        return LineEndformat;
    }
    */

    QAction *Tab_2;
    QAction *Tab_4;
    QAction *Tab_6;
    QAction *Tab_8;

    QSharedPointer<FindDialog> m_pFindDlg;
    QSharedPointer<ReplaceDialog> m_pReplaceDlg;
    QSharedPointer<QPageSetupDialog> m_pPageSetupDlg;

    MainWindow();
    MainWindow(const MainWindow&);
    MainWindow& operator = (const MainWindow);

    bool Initilize();

    bool initMenubar();
    bool initToolbar();
    bool initStatusBar();

    bool initMainEditor();

    bool initFileToolbar(QToolBar* toolbar);
    bool initEditToolbar(QToolBar* toolbar);
    bool initFormatToolbar(QToolBar* toolbar);
    bool initViewToolbar(QToolBar* toolbar);

    bool initFileMenu(QMenuBar* m);
    bool initEditMenu(QMenuBar* m);
    bool initFormatMenu(QMenuBar* m);
    bool initViewMenu(QMenuBar* m);
    bool initHelpMenu(QMenuBar* m);

    bool initAction(QAction* &action,QWidget* parent,QString text,int key_shortcut);
    bool initAction(QAction* &action,QWidget* parent,QString name,QString icon);

    QString showFileDialog(QFileDialog::AcceptMode mode,QString title,QString icon);
    void showErrorMessage(QString message);
    int showQueryMessage(QString message);
    QString saveCurrentData(QString title = "",QString path = "");
    void preEditChange();
    void openFileEditor(QString path);

    QAction* findToolBarAction(QString text);
    QAction* findMenuBarAction(QString text);
  //QAction* findMenuBarActionSub(QString text);

    QToolBar *toolBar();

    QStringList m_recentFiles ;

    QAction *RecentFilesAct[MAX_RecentFiles];
    QAction *SeparatorAction;
    void updateRecentFileActions();
    void setCurrentFile(const QString &fileName);

    //void updateEndofLineLable();


protected:
    void closeEvent(QCloseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
    //bool event(QEvent *event);
    //void contextMenuEvent(QContextMenuEvent *event);

private slots:
    void openRecentFile();

    void onFileNew();
    void onFileOpen();
    void onFileSave();
    void onFileSaveAs();
    void onFilePageSetup();
    void onFilePrint();
    void onPrintFilePDF();

    void onTextChanged();
    void onEditDelete();
    void onEditFind();
    void onEditFindNext();
    void onEditReplace();
    void onEditGoto();
    void onEditSelectAll();
    void onEditDate();

    void FormatFont();
    void FormatWrap();

    void onViewToolBar();
    void onViewStatusBar();
    void onFullScreen();
    void onHelpManual();
    void onHelpAbout();

    void onCopyAvailable(bool isTrue);
    void onRedoAvailable(bool isTrue);
    void onUndoAvailable(bool isTrue);

    void onCursorPositionChanged();

    void setTabsize(int n);
    void Tabsize_2();
    void Tabsize_4();
    void Tabsize_6();
    void Tabsize_8();

    void onOpenNewWindow();

public:
    static MainWindow *NewInstance();
    void OpenFile(QString path);
    ~MainWindow();
    int tab_size;
};

#endif // MAINWINDOW_H
