#include "mainwindow.h"
#include "highlighter.h"

#include <QDebug>


MainWindow::MainWindow():m_pFindDlg(new FindDialog(this,&m_TextEditor)),
    m_pReplaceDlg(new ReplaceDialog(this,&m_TextEditor)),
    m_pPageSetupDlg(new QPageSetupDialog(this))
{
    m_isTextChanged = false;
    m_filepath="";
    tab_size = 4;
}



MainWindow* MainWindow::NewInstance(){
    MainWindow* temp = new MainWindow();

    if( temp == nullptr || !temp->Initilize()){
        delete temp;
        temp = nullptr;
    }

    return temp;

}



bool MainWindow::Initilize(){
    bool status = true ;
    Configs config;

    status = status && initMenubar() ;
        //if(status=false)qDebug()<<false;
    status = status && initToolbar() ;
        //QToolBar* tb = toolBar();
        //qDebug()<<tb;
        //if(status==false)qDebug()<<false;
    status = status && initStatusBar();
        //if(status==false)qDebug()<<false;
    status = status && initMainEditor();
        //if(status==false)qDebug()<<false;

    setWindowTitle("untitle - PadLike");
    setWindowIcon(QIcon(":/Icons/icons/fileIcon.png"));
    setAcceptDrops(true);
    //setMinimumSize(400,250);

    QSettings setting(QCoreApplication::organizationName(),QCoreApplication::applicationName());
    m_recentFiles =  setting.value("recentFiles").toStringList();           //读取配置文件

    setCurrentFile("");
    updateRecentFileActions();

    if(status&&config.isValids()){                      //另一种读取配置文件的方式
        m_TextEditor.setFont(config.getFont());
        if(!config.isAutoWraps()){
            m_TextEditor.setLineWrapMode(QPlainTextEdit::NoWrap);
            findMenuBarAction("自动换行")->setChecked(false);
            findToolBarAction("自动换行")->setChecked(false);
        }
        if(!config.isToolBarVisibles()){
            toolBar()->setVisible(false);
            findMenuBarAction("工具栏")->setChecked(false);
            findToolBarAction("工具栏")->setChecked(false);
        }
        if(!config.isStatusBarVisibles()){
            statusBar()->setVisible(false);
            findMenuBarAction("状态栏")->setChecked(false);
            findToolBarAction("状态栏")->setChecked(false);
        }
        move(config.getWindowPoint());
        resize(config.getWindowSize());
    }
    return status;
}



bool MainWindow::initMenubar(){
    QMenuBar* m_menubar = menuBar();
    bool status = true;

    status = (m_menubar!=nullptr)?true:false;

    status = initFileMenu(m_menubar) &&
             initEditMenu(m_menubar) &&
             initFormatMenu(m_menubar) &&
             initViewMenu(m_menubar) &&
             initHelpMenu(m_menubar);

    return status;
}



bool MainWindow::initFileMenu(QMenuBar* m){
    QMenu* menu = new QMenu(tr("文件(&F)"),m);

    bool status;
    status = (menu!=nullptr)?true:false;

    if(status){
        QAction* action = nullptr;

        status = status && initAction(action,menu,tr("新建(&N)"),Qt::CTRL+Qt::Key_N);
        if(status){
            connect(action,SIGNAL(triggered()),this,SLOT(onFileNew()));
            menu->addAction(action);
        }

        status = status && initAction(action,menu,tr("新窗口(&W)"),Qt::CTRL+Qt::SHIFT+Qt::Key_N);
        if(status){
            connect(action,SIGNAL(triggered()),this,SLOT(onOpenNewWindow()));
            menu->addAction(action);
        }

        status = status && initAction(action,menu,tr("打开(&O)"),Qt::CTRL+Qt::Key_O);
        if(status){
            connect(action,SIGNAL(triggered()),this,SLOT(onFileOpen()));
            menu->addAction(action);
        }

        status = status && initAction(action,menu,tr("保存(&S)"),Qt::CTRL+Qt::Key_S);
        if(status){
            connect(action,SIGNAL(triggered()),this,SLOT(onFileSave()));
            menu->addAction(action);
        }

        status = status && initAction(action,menu,tr("另存为(&A)"),Qt::CTRL+Qt::Key_A);
        if(status){
            connect(action,SIGNAL(triggered()),this,SLOT(onFileSaveAs()));
            menu->addAction(action);
        }

        SeparatorAction = menu->addSeparator();

        for(int i = 0;i<MAX_RecentFiles;i++){
            status = status && initAction(RecentFilesAct[i],menu,"",0);
            if(!status)break;
            RecentFilesAct[i]->setVisible(false);
            connect(RecentFilesAct[i],SIGNAL(triggered()),this,SLOT(openRecentFile()));
            menu->addAction(RecentFilesAct[i]);
        }

        menu->addSeparator();

        status = status && initAction(action,menu,tr("页面设置(&U)"),Qt::CTRL+Qt::Key_U);
        if(status){
            connect(action,SIGNAL(triggered()),this,SLOT(onFilePageSetup()));
            menu->addAction(action);
        }

        status = status && initAction(action,menu,tr("打印(&P)"),Qt::CTRL+Qt::Key_P);
        if(status){
            connect(action,SIGNAL(triggered()),this,SLOT(onFilePrint()));
            menu->addAction(action);
        }

        status = status && initAction(action,menu,tr("导出至PDF..."),0);
        if(status){
            connect(action,SIGNAL(triggered()),this,SLOT(onPrintFilePDF()));
            action->setIcon(QIcon(":/Icons/icons/exportpdf.png"));
            menu->addAction(action);
        }

        menu->addSeparator();

        status = status && initAction(action,menu,tr("退出(&X)"),0);
        if(status){
            connect(action,SIGNAL(triggered()),this,SLOT(close()));
            menu->addAction(action);
        }
    }
    if(status){
        m->addMenu(menu);
    }
    else{
        delete menu;
    }
    return status;
}



bool MainWindow::initEditMenu(QMenuBar* m){
    QMenu* menu = new QMenu(tr("编辑(&E)"),m);

    bool status;
    status = (menu!=nullptr)?true:false;

    if(status){
        QAction* action = nullptr;

        status = status && initAction(action,menu,tr("撤销(&N)"),Qt::CTRL+Qt::Key_Z);
        if(status){
            connect(action,SIGNAL(triggered()),&m_TextEditor,SLOT(undo()));
            action->setEnabled(false);
            menu->addAction(action);
        }

        status = status && initAction(action,menu,tr("剪切(&T)"),Qt::CTRL+Qt::Key_X);
        if(status){
            connect(action,SIGNAL(triggered()),&m_TextEditor,SLOT(cut()));
            action->setEnabled(false);
            menu->addAction(action);
        }

        status = status && initAction(action,menu,tr("复制(&C)"),Qt::CTRL+Qt::Key_C);
        if(status){
            connect(action,SIGNAL(triggered()),&m_TextEditor,SLOT(copy()));
            action->setEnabled(false);
            menu->addAction(action);
        }

        status = status && initAction(action,menu,tr("粘贴(&V)"),Qt::CTRL+Qt::Key_V);
        if(status){
            connect(action,SIGNAL(triggered()),&m_TextEditor,SLOT(paste()));
            menu->addAction(action);
        }

        status = status && initAction(action,menu,tr("删除(&L)"),Qt::Key_Delete);
        if(status){
            connect(action,SIGNAL(triggered()),this,SLOT(onEditDelete()));
            menu->addAction(action);
        }

        menu->addSeparator();

        status = status && initAction(action,menu,tr("开启/关闭单行注释"),Qt::CTRL+Qt::Key_M);
        if(status){
            connect(action,SIGNAL(triggered()),&m_TextEditor,SLOT(quickComment()));
            menu->addAction(action);
        }

        menu->addSeparator();


        status = status && initAction(action,menu,tr("查找(&F).."),Qt::CTRL+Qt::Key_F);
        if(status){
            connect(action,SIGNAL(triggered()),this,SLOT(onEditFind()));
            menu->addAction(action);
        }

        status = status && initAction(action,menu,tr("查找下一个(&N)"),Qt::CTRL+Qt::Key_F3);
        if(status){
            connect(action,SIGNAL(triggered()),this,SLOT(onEditFindNext()));
            menu->addAction(action);
        }

        status = status && initAction(action,menu,tr("替换(&R)"),Qt::CTRL+Qt::Key_H);
        if(status){
            connect(action,SIGNAL(triggered()),this,SLOT(onEditReplace()));
            menu->addAction(action);
        }

        status = status && initAction(action,menu,tr("转到(&G)"),Qt::CTRL+Qt::Key_G);
        if(status){
            connect(action,SIGNAL(triggered()),this,SLOT(onEditGoto()));
            menu->addAction(action);
        }

        menu->addSeparator();

        status = status && initAction(action,menu,tr("全选(&A)"),Qt::CTRL+Qt::Key_A);
        if(status){
            connect(action,SIGNAL(triggered()),this,SLOT(onEditSelectAll()));
            menu->addAction(action);
        }

        status = status && initAction(action,menu,tr("时间/日期(&D)"),Qt::CTRL+Qt::Key_F5);
        if(status){
            connect(action,SIGNAL(triggered()),this,SLOT(onEditDate()));
            menu->addAction(action);
        }

        menu->addSeparator();

    }
    if(status){
        m->addMenu(menu);
    }
    else{
        delete menu;
    }
    return status;
}



bool MainWindow::initFormatMenu(QMenuBar *m){
    QMenu* menu = new QMenu(tr("格式(&O)"),m);

    bool status;
    status = (menu!=nullptr)?true:false;
    if(status){
        QAction* action = nullptr;

        status = status && initAction(action,menu,tr("自动换行(&W)"),0);
        if(status){
            action->setCheckable(true);
            action->setChecked(true);
            connect(action, SIGNAL(triggered()), this, SLOT(FormatWrap()));
            menu->addAction(action);
        }

        status = status && initAction(action,menu,tr("字体...(&F)"),0);
        if(status){
            connect(action,SIGNAL(triggered()),this,SLOT(FormatFont()));
            menu->addAction(action);
        }

        QMenu *sub_menu = menu->addMenu(tr("缩进"));
        if(sub_menu == nullptr){
            status = status && false;
        }

            if(status){

                status = status && initAction(action,sub_menu,"Tab:2",0);
                if(status){
                    action->setCheckable(true);
                    action->setChecked(false);
                    connect(action, SIGNAL(triggered()), this, SLOT(Tabsize_2()));
                    sub_menu->addAction(action);
                    Tab_2 = action;
                }

                status = status && initAction(action,sub_menu,"Tab:4",0);
                if(status){
                    action->setCheckable(true);
                    action->setChecked(true);
                    connect(action, SIGNAL(triggered()), this, SLOT(Tabsize_4()));
                    sub_menu->addAction(action);
                    Tab_4 = action;
                }

                status = status && initAction(action,sub_menu,"Tab:6",0);
                if(status){
                    action->setCheckable(true);
                    action->setChecked(false);
                    connect(action, SIGNAL(triggered()), this, SLOT(Tabsize_6()));
                    sub_menu->addAction(action);
                    Tab_6 = action;
                }

                status = status && initAction(action,sub_menu,"Tab:8",0);
                if(status){
                    action->setCheckable(true);
                    action->setChecked(false);
                    connect(action, SIGNAL(triggered()), this, SLOT(Tabsize_8()));
                    sub_menu->addAction(action);
                    Tab_8 = action;
                }
            }
    }
    if(status){
        m->addMenu(menu);
    }
    else{
        delete menu;
    }
    return status;
}



bool MainWindow::initViewMenu(QMenuBar *m){
    QMenu* menu = new QMenu(tr("查看(&V)"),m);

    bool status;
    status = (menu!=nullptr)?true:false;
    if(status){
        QAction* action = nullptr;

        status = status && initAction(action,menu,tr("工具栏(&T)"),0);
        if(status){
            action->setCheckable(true);
            action->setChecked(true);
            connect(action, SIGNAL(triggered()), this, SLOT(onViewToolBar()));
            menu->addAction(action);
        }

        status = status && initAction(action,menu,tr("状态栏(&S)"),0);
        if(status){
            action->setCheckable(true);
            action->setChecked(true);
            connect(action,SIGNAL(triggered()),this,SLOT(onViewStatusBar()));
            menu->addAction(action);
        }

        menu->addSeparator();

        status = status && initAction(action,menu,tr("全屏(&F)"),Qt::Key_F12);
        if(status){
            action->setCheckable(false);
            connect(action,SIGNAL(triggered()),this,SLOT(onFullScreen()));
            menu->addAction(action);
        }
    }
    if(status){
        m->addMenu(menu);
    }
    else{
        delete menu;
    }
    return status;
}



bool MainWindow::initHelpMenu(QMenuBar *m){
    QMenu* menu = new QMenu(tr("帮助(&H)"),m);

    bool status;
    status = (menu!=nullptr)?true:false;
    if(status){
        QAction* action = nullptr;

        status = status && initAction(action,menu,tr("查看帮助(&H)"),0);
        if(status){
            connect(action, SIGNAL(triggered()), this, SLOT(onHelpManual()));
            menu->addAction(action);
        }

        status = status && initAction(action,menu,tr("关于记事本(&A)"),0);
        if(status){
            connect(action,SIGNAL(triggered()),this,SLOT(onHelpAbout()));
            menu->addAction(action);
        }
    }
    if(status){
        m->addMenu(menu);
    }
    else{
        delete menu;
    }
    return status;
}



bool MainWindow::initAction(QAction* &action,QWidget* parent,QString text,int key_shortcut){
    bool status = true;
    action = new QAction(text,parent);

    if(action!=nullptr){
        action->setShortcut(QKeySequence(key_shortcut));
    }
    else{
        status = false;
    }

    return status ;
}



bool MainWindow::initToolbar(){
    //QToolBar* toolbar = new QToolBar("工具栏",this);
    QToolBar* toolbar = addToolBar(tr("工具栏"));
        //if(toolbar == nullptr)qDebug()<<false;
    bool status = true;
        //qDebug()<<toolBar();

    toolbar->setIconSize(QSize(16,16));
    toolbar->setFloatable(false);
    toolbar->setMovable(false);

    bool s1 = initFileToolbar(toolbar);
    toolbar->addSeparator();

    bool s2 = initEditToolbar(toolbar);
    toolbar->addSeparator();

    bool s3 = initFormatToolbar(toolbar);
    toolbar->addSeparator();

    bool s4 = initViewToolbar(toolbar);
    toolbar->addSeparator();

    status = status &&
            s1 &&
            s2 &&
            s3 &&
            s4;
    return status;
}



bool MainWindow::initFileToolbar(QToolBar* toolbar){
    QAction* action = nullptr;
    bool status = true ;

    status = status && initAction(action,toolbar,tr("新建"),":/Icons/icons/new.png");
    if(status){
        connect(action,SIGNAL(triggered()),this,SLOT(onFileNew()));
        toolbar->addAction(action);
    }

    status = status && initAction(action,toolbar,tr("打开"),":/Icons/icons/open.png");
    if(status){
        connect(action,SIGNAL(triggered()),this,SLOT(onFileOpen()));
        toolbar->addAction(action);
    }

    status = status && initAction(action,toolbar,tr("保存"),":/Icons/icons/save.png");
    if(status){
        connect(action,SIGNAL(triggered()),this,SLOT(onFileSave()));
        toolbar->addAction(action);
    }

    status = status && initAction(action,toolbar,tr("另存为"),":/Icons/icons/saveas.png");
    if(status){
        connect(action,SIGNAL(triggered()),this,SLOT(onFileSaveAs()));
        toolbar->addAction(action);
    }

    status = status && initAction(action,toolbar,tr("打印"),":/Icons/icons/print.png");
    if(status){
        connect(action,SIGNAL(triggered()),this,SLOT(onFilePrint()));
        toolbar->addAction(action);
    }

    status = status && initAction(action,toolbar,tr("输出至PDF..."),":/Icons/icons/exportpdf.png");
    if(status){
        connect(action,SIGNAL(triggered()),this,SLOT(onPrintFilePDF()));
        toolbar->addAction(action);
    }
    return status;
}



bool MainWindow::initEditToolbar(QToolBar *toolbar){
    QAction* action = nullptr;
    bool status = true ;
    status = status && initAction(action,toolbar,tr("撤销"),":/Icons/icons/undo.png");
    if(status){
        connect(action,SIGNAL(triggered()),&m_TextEditor,SLOT(undo()));
        action->setEnabled(false);
        toolbar->addAction(action);
    }

    status = status && initAction(action,toolbar,tr("恢复"),":/Icons/icons/redo.png");
    if(status){
        connect(action,SIGNAL(triggered()),&m_TextEditor,SLOT(redo()));
        action->setEnabled(false);
        toolbar->addAction(action);
    }

    status = status && initAction(action,toolbar,tr("剪切"),":/Icons/icons/cut.png");
    if(status){
        connect(action,SIGNAL(triggered()),&m_TextEditor,SLOT(cut()));
        action->setEnabled(false);
        toolbar->addAction(action);
    }

    status = status && initAction(action,toolbar,tr("复制"),":/Icons/icons/copy.png");
    if(status){
        connect(action,SIGNAL(triggered()),&m_TextEditor,SLOT(copy()));
        action->setEnabled(false);
        toolbar->addAction(action);
    }

    status = status && initAction(action,toolbar,tr("粘贴"),":Icons/icons/paste.png");
    if(status){
        connect(action,SIGNAL(triggered()),&m_TextEditor,SLOT(paste()));
        toolbar->addAction(action);
    }

    status = status && initAction(action,toolbar,tr("查找"),":Icons/icons/find.png");
    if(status){
        connect(action,SIGNAL(triggered()),this,SLOT(onEditFind()));
        toolbar->addAction(action);
    }

    status = status && initAction(action,toolbar,tr("替换"),":/Icons/icons/replace.png");
    if(status){
        connect(action,SIGNAL(triggered()),this,SLOT(onEditReplace()));
        toolbar->addAction(action);
    }

    status = status && initAction(action,toolbar,tr("转到"),":/Icons/icons/gotocell.png");
    if(status){
        connect(action,SIGNAL(triggered()),this,SLOT(onEditGoto()));
        toolbar->addAction(action);
    }

    return status;
}



bool MainWindow::initFormatToolbar(QToolBar *toolbar){
    QAction* action = nullptr;
    bool status = true ;

    status = status && initAction(action,toolbar,tr("自动换行"),":/Icons/icons/wrap.png");
    if(status){
        action->setCheckable(true);
        action->setChecked(true);
        connect(action,SIGNAL(triggered()),this,SLOT(FormatWrap()));
        toolbar->addAction(action);
    }

    status = status && initAction(action,toolbar,tr("字体"),":/Icons/icons/font.png");
    if(status){
        connect(action,SIGNAL(triggered()),this,SLOT(FormatFont()));
        toolbar->addAction(action);
    }
    return status;
}



bool MainWindow::initViewToolbar(QToolBar *toolbar){
    QAction* action = nullptr;
    bool status = true ;

    status = status && initAction(action,toolbar,tr("工具栏"),":/Icons/icons/tool.png");
    if(status){
        action->setCheckable(true);
        action->setChecked(true);
        connect(action,SIGNAL(triggered()),this,SLOT(onViewToolBar()));
        toolbar->addAction(action);
    }

    status = status && initAction(action,toolbar,tr("状态栏"),":/Icons/icons/status.png");
    if(status){
        action->setCheckable(true);
        action->setChecked(true);
        connect(action,SIGNAL(triggered()),this,SLOT(onViewStatusBar()));
        toolbar->addAction(action);
    }
    return status;
}



bool MainWindow::initAction(QAction* &action,QWidget* parent,QString tiptext,QString icon){
    bool status = true;

    action = new QAction("",parent);

    if(action!=nullptr){
        action -> setToolTip(tiptext);
        action -> setIcon(QIcon(icon));
    }
    else{
        status = false;
    }
    return status;
}



bool MainWindow::initStatusBar(){
    QStatusBar* statusbar = statusBar();
    m_EndofLineLabel = new QLabel(tr(" Windows (CRLF) "));
    m_ShowLabel = new QLabel(tr(" Arkenol "));
    bool status = true ;

    if(m_ShowLabel!=nullptr){
        m_ShowLabel->setAlignment(Qt::AlignRight);
        m_ShowLabel->setMinimumSize(m_ShowLabel->sizeHint());
        //m_ShowLabel->setMargin(1);              //设置 页边距
        m_ShowLabel->setIndent(50);             //设置间距
        statusbar->addWidget(m_ShowLabel);

        statusbar->addPermanentWidget(new QLabel());

        m_Status.setMinimumSize(m_Status.sizeHint());
        m_Status.setAlignment(Qt::AlignCenter);
        m_Status.setText("  Length: "+QString::number(0)+"  Line: "+QString::number(1) + "  ");
        statusbar->addPermanentWidget(&m_Status);

        m_CursorStatus.setMinimumSize(m_CursorStatus.sizeHint());
        m_CursorStatus.setAlignment(Qt::AlignCenter);
        m_CursorStatus.setText("  Ln: "+QString::number(0)+"  Col: "+QString::number(1) + "  ");
        statusbar->addPermanentWidget(&m_CursorStatus);

        m_Tablabel.setMinimumSize(m_Tablabel.sizeHint());
        m_Tablabel.setAlignment(Qt::AlignCenter);
        m_Tablabel.setText("  Tab_Size: " + QString::number(tab_size) + "  ");
        statusbar->addPermanentWidget(&m_Tablabel);

        /*
        EndofLine b = getLineEndformat();
        if(b==Windows)
            m_EndofLineLabel->setText(tr("Windows (CRLF)"));
        else if(b==Macs)
            m_EndofLineLabel->setText(tr("Macintosh (CR)"));
        else if(b==Unixs)
            m_EndofLineLabel->setText(tr("Unix (LF)"));
        else
            m_EndofLineLabel->setText(tr("Unix (LF)"));
        */

        m_EndofLineLabel->setMinimumSize(m_EndofLineLabel->sizeHint());
        m_EndofLineLabel->setAlignment(Qt::AlignCenter);
        statusbar->addPermanentWidget(m_EndofLineLabel);
    }
    else{
        status = false;
    }

    return status;

}



bool MainWindow::initMainEditor(){
    bool status = true;

    QPalette p = m_TextEditor.palette();
    p.setColor(QPalette::Inactive,QPalette::Highlight,p.color(QPalette::Active,QPalette::Highlight));
    p.setColor(QPalette::Inactive,QPalette::HighlightedText,p.color(QPalette::Active,QPalette::HighlightedText));
    m_TextEditor.setPalette(p);

    m_TextEditor.setParent(this);
    m_TextEditor.setAcceptDrops(false);
    setCentralWidget(&m_TextEditor);

    QFont f = QFont("Microsoft YaHei UI",10);
    //f.setLetterSpacing(QFont::PercentageSpacing,QString("100").toDouble());           //字符间距
    m_TextEditor.setFont(f);

    HighLighter *h = new HighLighter();             //设置高亮，可以转换
    h->setDocument(m_TextEditor.document());

    QFontMetrics metric(m_TextEditor.font());       //设置tab宽度
    m_TextEditor.setTabStopDistance(tab_size * metric.horizontalAdvance(' '));      //其他目标：不同文字数字，大小和间距应一致，QT自带API无法实现，应重写paint函数实现，待续

    connect(&m_TextEditor, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
    connect(&m_TextEditor, SIGNAL(copyAvailable(bool)), this, SLOT(onCopyAvailable(bool)));
    connect(&m_TextEditor, SIGNAL(redoAvailable(bool)), this, SLOT(onRedoAvailable(bool)));
    connect(&m_TextEditor, SIGNAL(undoAvailable(bool)), this, SLOT(onUndoAvailable(bool)));
    connect(&m_TextEditor, SIGNAL(cursorPositionChanged()), this, SLOT(onCursorPositionChanged()));

    return status;

}



QToolBar* MainWindow::toolBar(){
    QToolBar *temp = nullptr;
    const QObjectList &list = children();

    for(int i =0 ; i< list.count();++i){
        QToolBar* toolBar = dynamic_cast<QToolBar*>(list[i]);

        if(toolBar != nullptr){
            temp = toolBar;
            break;
        }
    }

    return temp;

}



MainWindow::~MainWindow()
{

}
