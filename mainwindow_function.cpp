#include "mainwindow.h"

#include <QByteArray>
#include <QDateTime>
#include <QDesktopServices>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QFont>
#include <QFontDialog>
#include <QInputDialog>
#include <QKeyEvent>
#include <QList>
#include <QMap>
#include <QMessageBox>
#include <QMimeData>
#include <QObjectList>
#include <QPlainTextEdit>
#include <QProcess>
#include <QTextCodec>
#include <QTextCursor>
#include <QTextStream>
#include <QUrl>

#include <QDebug>



QString MainWindow::showFileDialog(QFileDialog::AcceptMode mode, QString title, QString icon){

    QFileDialog fd(this);
    QStringList filters;
    QMap<QString,QString> m;

    const char *filter[][2]={
        {"文本文档(*.txt)",".txt"},
        {"C文件(*.c)",".c"},
        {"C++文件(*.cpp)",".cpp"},
        {"C头文件(*.h)",".h"},
        {"所有文档(*.*)",".*"},
        {nullptr,nullptr}
    };

    QString temp="";

    for(int i=0;filter[i][0]!=nullptr;++i){
        filters.append(filter[i][0]);
        m.insert(filter[i][0],filter[i][1]);
    }

    fd.setWindowTitle(title);
    fd.setWindowIcon(QIcon(icon));
    fd.setAcceptMode(QFileDialog::AcceptOpen);
    fd.setNameFilters(filters);

    if(mode == QFileDialog::AcceptOpen){
        fd.setFileMode(QFileDialog::ExistingFile);              //点击OK之后，返回的值，此处为单个文件
    }

    if(fd.exec() == QFileDialog::Accepted){
        temp = fd.selectedFiles()[0];               //返回选择文件

        if(mode == QFileDialog::AcceptSave){
            QString postfix = m[fd.selectedNameFilter()];               //使用QMap添加后缀
            if((postfix != ".*") && !temp.endsWith(postfix)){
                temp = temp + postfix;
            }
        }
    }

    return temp ;

}



void MainWindow::showErrorMessage(QString message){

    QMessageBox msg(this);

    msg.setWindowTitle("Padlike");
    msg.setWindowFlag(Qt::Drawer);
    msg.setIcon(QMessageBox::Critical);
    msg.setText(message);
    msg.setStandardButtons(QMessageBox::Ok);
    msg.setButtonText(QMessageBox::Ok,tr("确定"));

    msg.exec();

}



int MainWindow::showQueryMessage(QString message){

    QMessageBox msg(this);

    msg.setIcon(QMessageBox::Question);
    msg.setWindowTitle("Padlike");
    msg.setWindowFlag(Qt::Drawer);
    msg.setText(message);
    msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msg.setButtonText(QMessageBox::Yes,tr("是"));
    msg.setButtonText(QMessageBox::No,tr("否"));
    msg.setButtonText(QMessageBox::Cancel,tr("取消"));

    return msg.exec();

}



void MainWindow::preEditChange(){

    if(m_isTextChanged){
        QString path = (m_filepath !="") ? m_filepath : "无标题" ;
        int cases = showQueryMessage(QString("是否将更改保存到")+"\n"+"\""+path+"\"?");

        switch(cases){
            case QMessageBox::Yes:
                saveCurrentData(tr("保存"),m_filepath);
                break;
            case QMessageBox::No:
                m_isTextChanged = false;
                break;
            case QMessageBox::Cancel:
                break;
        }
    }
}



void MainWindow::openFileEditor(QString path){

    if(path!=""){

        QFile file(path);

        if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
            QTextStream in(&file);

            setCurrentFile(path);

            in.setCodec("UTF-8");

            m_TextEditor.setPlainText(in.readAll());

            /*
            QByteArray b = in.readAll().toLocal8Bit();
            QByteArray a = file.readAll();

            if(a.contains(QByteArray("\r\n")))
                qDebug()<<11111;
                //LineEndformat = EndofLine::Windows;
            else if(a.contains('\r'))
                qDebug()<<22222;
                //LineEndformat = EndofLine::Macs;
            else if(a.contains('\n'))
                qDebug()<<33333;
                //LineEndformat = EndofLine::Unixs;
            else
                qDebug()<<44444;
                //LineEndformat = EndofLine::Defaults;
            */


            statusBar()->showMessage(tr("文件加载中..."),2000);

            file.close();

            m_filepath = path;
            m_isTextChanged = false;

            setWindowTitle(m_filepath+" - Padlike");

        }

        else{
            showErrorMessage(QString("打开文件失败!") + "\n" + "\n" + "\"" + path + "\"");
        }
    }
}



void MainWindow::OpenFile(QString path){

    preEditChange();

    if(!m_isTextChanged){
        openFileEditor(path);
    }

    //updateEndofLineLable();

}



void MainWindow::onFileOpen(){

    preEditChange();

    if(!m_isTextChanged){
        QString path = showFileDialog(QFileDialog::AcceptOpen,"打开",":");
        openFileEditor(path);
    }

    //updateEndofLineLable();

}



QString MainWindow::saveCurrentData(QString title, QString path){
    QString temp = path;

    if(temp == ""){
        temp = showFileDialog(QFileDialog::AcceptSave, title, "");
    }

    if(temp != ""){
        QFile file(temp);

        if(file.open(QIODevice::WriteOnly|QIODevice::Text)){
            QTextStream out(&file);

            setCurrentFile(temp);

            out << m_TextEditor.toPlainText();

            statusBar()->showMessage(tr("文件保存中..."),2000);

            file.close();

            setWindowTitle(temp + " - Padlike");

            m_isTextChanged = false;
        }
        else{
            showErrorMessage(QString("保存文件失败!") + "\n" + "\n" + "\"" + path + "\"");
        }
    }

    return temp ;

}



void MainWindow::onFileSave(){

    QString path = saveCurrentData("保存",m_filepath);

    if(path != ""){
        m_filepath = path;
    }

}



void MainWindow::onFileSaveAs(){
    QString path = saveCurrentData("另存为");

    if(path!=""){
        m_filepath = path ;
    }

}



void MainWindow::onFileNew(){

    preEditChange();

    if(!m_isTextChanged){
        m_TextEditor.clear();

        m_isTextChanged = false ;

        m_filepath = "";

        setWindowTitle("untitle - Padlike");
    }
}



void MainWindow::onFilePageSetup(){

#ifndef QT_NO_PRINTER

    m_pPageSetupDlg->exec();

#endif

}



void MainWindow::onFilePrint(){

#ifndef QT_NO_PRINTER

    QPrintDialog dlg(this);

    dlg.setWindowTitle("打印");

    if(dlg.exec() == QPrintDialog::Accepted){
        QPrinter *p = dlg.printer();
        p->setPageLayout(m_pPageSetupDlg->printer()->pageLayout());
        m_TextEditor.document()->print(p);
    }

#endif

}



void MainWindow::onPrintFilePDF()
{

#ifndef QT_NO_PRINTER

    QFileDialog fileDialog(this, tr("导出PDF文件..."));

    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setMimeTypeFilters(QStringList("application/pdf"));
    fileDialog.setDefaultSuffix("pdf");

    if (fileDialog.exec() != QDialog::Accepted)
        return;

    QString fileName = fileDialog.selectedFiles().first();

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    m_TextEditor.document()->print(&printer);

    statusBar()->showMessage(tr("Exported \"%1\"")
                             .arg(QDir::toNativeSeparators(fileName)));

#endif

}



void MainWindow::onTextChanged(){
    if(!m_isTextChanged){
        QString title = windowTitle();
        if(m_filepath==""){
            title = "untitle* - Padlike";
            setWindowTitle(title);
        }
        else
            setWindowTitle(m_filepath+"* - Padlike");
    }

    m_isTextChanged = true ;

    m_Status.setText("  length: "+QString::number(m_TextEditor.toPlainText().length())+"  lines："+QString::number(m_TextEditor.document()->lineCount())+"  ");


}



void MainWindow::onEditDelete(){

    QKeyEvent KeyPress(QEvent::KeyPress,Qt::Key_Delete,Qt::NoModifier);
    QKeyEvent keyRelease(QEvent::KeyRelease, Qt::Key_Delete, Qt::NoModifier);

    QApplication::sendEvent(&m_TextEditor,&KeyPress);
    QApplication::sendEvent(&m_TextEditor,&keyRelease);

}



void MainWindow::onEditFind(){
    m_pFindDlg->show();
}



void MainWindow::onEditFindNext(){
    m_pFindDlg->onFindClicked();
}



void MainWindow::onEditReplace(){
    m_pReplaceDlg->show();
}



void MainWindow::onEditGoto(){
    bool checked = false;
    int ln = QInputDialog::getInt(this,tr("转到"),tr("行号"),1,1,m_TextEditor.document()->lineCount(),1,&checked,Qt::WindowCloseButtonHint|Qt::Drawer);

    if(checked){
        QString text = m_TextEditor.toPlainText();
        QTextCursor cur = m_TextEditor.textCursor();
        int pos = 0;
        int next = -1;

        for(int i=0;i<ln;++i){
            pos = next + 1 ;
            next = text . indexOf('\n',pos);
        }

        cur.setPosition(pos);

        m_TextEditor.setTextCursor(cur);

    }
}



void MainWindow::onEditSelectAll(){
    QTextCursor cur = m_TextEditor.textCursor();
    int length = m_TextEditor.toPlainText().length();

    cur.setPosition(0);
    cur.setPosition(length , QTextCursor :: KeepAnchor);

    m_TextEditor.setTextCursor(cur);

}



void MainWindow::onEditDate(){
    m_TextEditor.insertPlainText("[ "+QDateTime().currentDateTime().toString("hh:mm:ss yyyy/MM/dd")+" ]");
}



void MainWindow::FormatFont(){

    bool checked = false;

    QFont font = QFontDialog::getFont(&checked,m_TextEditor.font(),this,"字体");

    if(checked){
        m_TextEditor.setFont(font);
        QFontMetrics i(font);
        m_TextEditor.setTabStopDistance(i.horizontalAdvance('a') * tab_size);
    }

}



void MainWindow::FormatWrap(){
    QPlainTextEdit::LineWrapMode mode = m_TextEditor.lineWrapMode();

    if(mode == QPlainTextEdit::NoWrap){

        m_TextEditor.setLineWrapMode(QPlainTextEdit::WidgetWidth);

        findMenuBarAction("自动换行")->setChecked(true);
        findToolBarAction("自动换行")->setChecked(true);

    }
    else{

        m_TextEditor.setLineWrapMode(QPlainTextEdit::NoWrap);

        findMenuBarAction("自动换行")->setChecked(false);
        findToolBarAction("自动换行")->setChecked(false);

    }
}



void MainWindow::onViewToolBar(){

    QToolBar *toolbar = toolBar();

    bool isTbVisible = toolbar->isVisible();

    toolbar->setVisible(!isTbVisible);

    findMenuBarAction("工具栏")->setChecked(!isTbVisible);
    findToolBarAction("工具栏")->setChecked(!isTbVisible);

}



void MainWindow::onViewStatusBar(){

    QStatusBar *statusbar = statusBar();

    bool isSbVisible = statusbar->isVisible();

    statusbar->setVisible(!isSbVisible);

    findMenuBarAction("状态栏")->setChecked(!isSbVisible);
    findToolBarAction("状态栏")->setChecked(!isSbVisible);

}



void MainWindow::onFullScreen(){
    QToolBar *toolbar = toolBar();
    QStatusBar *statusbar = statusBar();
    bool isTbVisible = toolbar->isVisible();
    bool isSbVisible = statusbar->isVisible();
    if(isFullScreen()){
        toolbar->setVisible(true);
        statusbar->setVisible(true);
        findMenuBarAction("工具栏")->setChecked(!isTbVisible);
        findToolBarAction("工具栏")->setChecked(!isTbVisible);
        findMenuBarAction("状态栏")->setChecked(!isSbVisible);
        findToolBarAction("状态栏")->setChecked(!isSbVisible);
        this->setWindowState(Qt::WindowMaximized);
    }
    else{
        if(isTbVisible){
            toolbar->setVisible(false);
            findMenuBarAction("工具栏")->setChecked(!isTbVisible);
            findToolBarAction("工具栏")->setChecked(!isTbVisible);
        }
        if(isSbVisible){
            statusbar->setVisible(false);
            findMenuBarAction("状态栏")->setChecked(!isSbVisible);
            findToolBarAction("状态栏")->setChecked(!isSbVisible);
        }
        this->setWindowState(Qt::WindowFullScreen);
    }
}



void MainWindow::onHelpManual(){
    QDesktopServices::openUrl(QUrl("https://www.github.com/Arkenol/Padlike"));
}



void MainWindow::onHelpAbout(){
    HelpDiglog(this).exec();
}



void MainWindow::onCopyAvailable(bool isTrue){
    findMenuBarAction("复制")->setEnabled(isTrue);
    findToolBarAction("复制")->setEnabled(isTrue);
    findMenuBarAction("剪切")->setEnabled(isTrue);
    findToolBarAction("剪切")->setEnabled(isTrue);
}



void MainWindow::onRedoAvailable(bool isTrue){
    findToolBarAction("恢复")->setEnabled(isTrue);
}



void MainWindow::onUndoAvailable(bool isTrue){
    findMenuBarAction("撤销")->setEnabled(isTrue);
    findToolBarAction("撤销")->setEnabled(isTrue);
}



void MainWindow::onCursorPositionChanged(){
    int col = 0;
    int ln = 0;
    int flg = -1;
    int pos = m_TextEditor.textCursor().position();
    QString text = m_TextEditor.toPlainText();

    for(int i=0;i<pos;++i){
        if(text[i]=="\n"){
            ln++;
            flg=i;
        }
    }

    flg++;

    col = pos-flg;

    m_CursorStatus.setText("  Ln: " + QString::number(ln + 1) + "  Col: " + QString::number(col + 1)+"  ");

}



void MainWindow::closeEvent(QCloseEvent *event){

    preEditChange();

    if(!m_isTextChanged){
        QFont font = m_TextEditor.font();
        bool isWrap = (m_TextEditor.lineWrapMode() == QPlainTextEdit::WidgetWidth);
        bool ToolBarVisible = (findMenuBarAction("工具栏")->isCheckable() && findToolBarAction("工具栏")->isChecked());
        bool StatusBarVisible = (findMenuBarAction("状态栏")->isCheckable() && findToolBarAction("状态栏")->isChecked());

        Configs config(font,pos(),size(),isWrap,ToolBarVisible,StatusBarVisible,this);

        config.store();

        QSettings setting(QCoreApplication::organizationName(),QCoreApplication::applicationName());
        setting.setValue("recentFiles",m_recentFiles);

        QMainWindow::closeEvent(event);

    }

    else{
        event->ignore();
    }
}



void MainWindow::dragEnterEvent(QDragEnterEvent *event){

    if(event->mimeData()->hasUrls()){
        event->acceptProposedAction();
    }

    else{
        event->ignore();
    }
}


void MainWindow::dropEvent(QDropEvent *event){

    if(event->mimeData()->hasUrls()){
        QList<QUrl> list = event->mimeData()->urls();
        QString path = list[0].toLocalFile();
        QFileInfo file(path);

        if(file.isFile()){

            preEditChange();

            if(!m_isTextChanged){
                openFileEditor(path);
            }
        }

        else{
            showErrorMessage(QString("对 ")+"\""+path+"\"的访问被拒绝");
        }
    }

    else{
        event->ignore();
    }
}



void MainWindow::keyPressEvent(QKeyEvent *event){

    if(event->key() == Qt::Key_Plus && event->modifiers() == Qt::ControlModifier){          //ctrl + '+'  : 放大
        (&m_TextEditor)->zoomIn();                  //不同于其他软件，Qt缩放只改变字体大小
    }

    else if(event->key() == Qt::Key_Minus && event->modifiers() == Qt::ControlModifier){        //ctrl + '-' 缩小
        (&m_TextEditor)->zoomOut();
    }

    else{
        QMainWindow::keyPressEvent(event);
    }
}



void MainWindow::wheelEvent(QWheelEvent *event){
    //滚轮缩放，按ctrl时实现
    if(event->modifiers() == Qt::ControlModifier){

        if(event->delta()>0){            
            (&m_TextEditor)->zoomIn();

        }

        else if(event->delta() < 0){            
            (&m_TextEditor)->zoomOut();
        }
    }
    else
        QMainWindow::wheelEvent(event);
}



/*void MainWindow::contextMenuEvent(QContextMenuEvent *event){          //在CodeEditor实现，主窗口无视
    QMenu *menu = new QMenu(this);
    menu->addAction(QIcon(""),"剪切",&m_TextEditor,SLOT(cut()));
    menu->exec(event->globalPos());
}*/



QAction* MainWindow::findMenuBarAction(QString text){           //寻找菜单栏

    QAction *temp = nullptr;
    const QObjectList &list = menuBar()->children();

    for(int i = 0;i<list.count();i++){
        QMenu *menu = dynamic_cast<QMenu*>(list[i]);

        if(menu != nullptr){
            QList<QAction*> actions = menu->actions();

            for(int j=0;j<actions.count();++j){
                if(actions[j]->text().startsWith(text)){
                    temp = actions[j];
                    break;
                }
            }
        }
    }
    return temp;
}



QAction* MainWindow::findToolBarAction(QString text){           //寻找工具栏

    QAction *temp = nullptr;

    QToolBar* toolbar = toolBar();

    QList<QAction*> actions =toolbar->actions();

    for(int i = 0;i<actions.count();++i){
        if(actions[i]->toolTip().startsWith(text)){
            temp = actions[i];
            break;
        }
    }

    return temp;

}


/*QAction* MainWindow::findMenuBarActionSub(QString text){      //寻找子菜单中的动作，bug

    QAction *temp = nullptr;
    const QObjectList &list = menuBar()->children();

    for(int i = 0;i<list.count();i++){
        QMenu *menu = dynamic_cast<QMenu*>(list[i]);
        const QObjectList &list2 = menu->children();

        for(int k =0;k<list2.count();++k){
            QMenu *submenu = dynamic_cast<QMenu*>(list2[i]);

            if(submenu != nullptr){
                QList<QAction*> actions = menu->actions();
                for(int j=0;j<actions.count();++j){
                    if(actions[j]->text().startsWith(text)){
                        temp = actions[j];
                        break;
                    }
                }
            }
        }
    }
    return temp;
}
*/


void MainWindow::setTabsize(int n){
    tab_size = n;
    QFontMetrics metric(m_TextEditor.font());       //设置tab宽度
    m_TextEditor.setTabStopDistance(tab_size * metric.horizontalAdvance(' '));
}



void MainWindow::Tabsize_2(){
    setTabsize(2);
    m_Tablabel.setText("Tab_Size: " + QString::number(tab_size));
    Tab_2->setChecked(true);
    Tab_4->setChecked(false);
    Tab_6->setChecked(false);
    Tab_8->setChecked(false);
    //findMenuBarActionSub("Tab:2")->setCheck(true);
    //findMenuBarActionSub("Tab:4")->setCheck(false);
    //findMenuBarActionSub("Tab:6")->setCheck(false);
    //findMenuBarActionSub("Tab:8")->setCheck(false);
}



void MainWindow::Tabsize_4(){
    setTabsize(4);
    m_Tablabel.setText("Tab_Size: " + QString::number(tab_size));
    Tab_2->setChecked(false);
    Tab_4->setChecked(true);
    Tab_6->setChecked(false);
    Tab_8->setChecked(false);
}



void MainWindow::Tabsize_6(){
    setTabsize(6);
    m_Tablabel.setText("Tab_Size: " + QString::number(tab_size));
    Tab_2->setChecked(false);
    Tab_4->setChecked(false);
    Tab_6->setChecked(true);
    Tab_8->setChecked(false);
}



void MainWindow::Tabsize_8(){
    setTabsize(8);
    m_Tablabel.setText("Tab_Size: " + QString::number(tab_size));
    Tab_2->setChecked(false);
    Tab_4->setChecked(false);
    Tab_6->setChecked(false);
    Tab_8->setChecked(true);
}



void MainWindow::updateRecentFileActions(){                     //更新最新修改文件列表
    QMutableStringListIterator it(m_recentFiles);
    while(it.hasNext()){
        if(!QFile::exists(it.next())){
            it.remove();
        }
    }

    for(int i = 0;i<MAX_RecentFiles;++i){
        if(i<m_recentFiles.count()){
            QString text = tr("%1: %2")
                    .arg(i + 1)
                    .arg(m_recentFiles[i]);
            RecentFilesAct[i]->setText(text);
            RecentFilesAct[i]->setData(m_recentFiles[i]);
            RecentFilesAct[i]->setVisible(true);
        }
        else{
            RecentFilesAct[i]->setVisible(false);
        }

        SeparatorAction->setVisible(!m_recentFiles.isEmpty());
    }
}



void MainWindow::openRecentFile(){              //最近打开的文件对应槽函数
    preEditChange();

    QAction *action = qobject_cast<QAction*>(sender());            //发送信号的对象
    if(action){
        OpenFile(action->data().toString());
    }

    //updateEndofLineLable();

}



void MainWindow::setCurrentFile(const QString &fileName){           //现在打开的文件
    QString curFile = fileName;

    if(fileName.isEmpty())
    {
        //curFile.append("new");
    }

    if(!curFile.isEmpty())
    {
        //qDebug()<<curFile;
        curFile.replace('/', '\\');             //   '/'和'\'没啥区别，都能打开对应文件，此处调整显示格式与其他软件显示一致
        //qDebug()<<curFile;
        m_recentFiles.removeAll(curFile);           //清除重复
        m_recentFiles.prepend(curFile);             // 头插
        updateRecentFileActions();
    }
}



void MainWindow::onOpenNewWindow(){

    static QProcess process(this);

    QString dir = QCoreApplication::applicationDirPath();       //QDir::currentPath返回环境运行目录
    QString path = dir + "/" + QCoreApplication::applicationName() + ".exe";
    //qDebug() << path;
    process.startDetached(path);
    //QCoreApplication app;

}


/*
void MainWindow::updateEndofLineLable(){

    EndofLine b = getLineEndformat();

    if(b==Windows)
        m_EndofLineLabel->setText(tr("Windows (CRLF)"));
    else if(b==Macs)
        m_EndofLineLabel->setText(tr("Macintosh (CR)"));
    else if(b==Unixs)
        m_EndofLineLabel->setText(tr("Unix (LF)"));
    else
        m_EndofLineLabel->setText(tr("Unix (LF)"));

    qDebug()<<b;
}
*/
