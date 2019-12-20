#include "codeeditor.h"
#include "functionfoldarea.h"

#include <QApplication>
#include <QColor>
#include <QColorDialog>
#include <QMenu>
#include <QMessageBox>
#include <QRegularExpression>
#include <QTextBlock>
#include <QTextBlockUserData>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextList>
#include <QTextListFormat>
#include <QTextStream>
#include <QListView>
#include <QListWidgetItem>

#include <QDebug>


class LineNumberArea : public QWidget
{

public:

    LineNumberArea(CodeEditor *parent) : QWidget(parent) {
        codeEditor = parent;
    }

    QSize sizeHint() const override ;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override {
        codeEditor->LineNumberAreaPaintEvent(event);
    }

private:
    CodeEditor *codeEditor;

};

void LineNumberArea::mousePressEvent(QMouseEvent *event){               //点击行号选择对应行
    QTextBlock block = codeEditor->getMousePositionBlock(event->y());
    QTextCursor cursor = codeEditor->textCursor();
    QTextBlock block_cur = cursor.block();
    //codeEditor->getInvisibleBlockNum(block.blockNumber());
    //qDebug()<<block.blockNumber();
    if(block.isValid()&&block_cur.isValid()){
        int num_1 = block.blockNumber();            //鼠标所在block位置
        int num_2 = block_cur.blockNumber();        //当前光标block位置
        //qDebug()<<num_1<<"\t"<<num_2<<endl;
        while(num_1 != num_2){
            if(num_2<num_1){
                //codeEditor->moveCursor(QTextCursor::NextBlock);
                //cursor = codeEditor->textCursor();
                //num_2 = cursor.blockNumber();
                cursor.movePosition(QTextCursor::NextBlock);
                ++num_2;
            }
            else{
                //codeEditor->moveCursor(QTextCursor::PreviousBlock);
                //cursor = codeEditor->textCursor();
                //num_2 = cursor.blockNumber();
                cursor.movePosition(QTextCursor::PreviousBlock);
                --num_2;
            }
        }

        //codeEditor->setTextCursor(cursor);
        //cursor = codeEditor->textCursor();
        cursor.movePosition(QTextCursor::StartOfBlock);
        cursor.movePosition(QTextCursor::EndOfBlock,QTextCursor::KeepAnchor);
        //cursor.select(QTextCursor::BlockUnderCursor);
        codeEditor->setTextCursor(cursor);

    }
}

QSize LineNumberArea::sizeHint() const{
    return QSize(codeEditor->LineNumberAreaWidth(),0);
}







CodeEditor::CodeEditor(QWidget *parent):QPlainTextEdit(parent),m_completer(nullptr)
{
    QStringList listwords = WordsfromFile(":/qm/qm/C.txt");
    QCompleter *c = new QCompleter(listwords,this);
    //QCompleter *c = new QCompleter(this);
    setCompleter(c);
    setLineNumberArea();

    highlightCurrentLine();
    m_MousePosition.setX(0);
    m_MousePosition.setY(0);

    isRepaint = false;
}

void CodeEditor::setCompleter(QCompleter *completer){
    if(m_completer){
        disconnect(m_completer,nullptr,this,nullptr);
    }

    m_completer = completer;

    if(!m_completer)
        return;

    m_completer->setWidget(this);

    //m_completer->setModel(modelFromFile(":/qm/qm/C.txt"));
    //m_completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);

    m_completer->setCompletionMode(QCompleter::PopupCompletion);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_completer->setWrapAround(false);
    m_completer->setMaxVisibleItems(3);
    m_completer->popup()->setStyleSheet("QAbstractItemView{background-color:#294662;"
                                        "color:#ffffff;"
                                        "border:1px solid #375C80;"
                                        "border-radius:6px;"            //圆角只出现轮廓，猜测与Completer的实现有关
                                        "padding:0px 0px 0px 0px;"
                                        "min-width:17px;"
                                        "font:14px \"Microsoft YaHei UI\";"
                                        "selection-color:#ffffff;"
                                        //"selection-border:solid"      //边界设定时其他属性会消失
                                        "selection-background-color:#375C80}"   //选择项的高亮，下方实现有误，返回时
                                        //"QListWidgetItem{background-color:#294000;border:none;color:#ffffff;font:bold;}"
                                        );
    //qDebug()<<m_completer->popup()->children();
    connect(m_completer, SIGNAL(activated(QString)), this, SLOT(onCompleterActivated(QString)));
}

void CodeEditor::setLineNumberArea(){
    lineNumberArea = new LineNumberArea(this);
    functionFoldArea = new FunctionFoldArea(this);
    //lineNumberArea->setVisible(false);                    //设置行号可见

    connect(this,SIGNAL(blockCountChanged(int)),this,SLOT(updateLineNumberAreaWidth(int)));
    connect(this,SIGNAL(cursorPositionChanged()),this,SLOT(highlightCurrentLine()));
    connect(this,SIGNAL(updateRequest(QRect,int)),this,SLOT(updateLineNumberArea(QRect,int)));
    connect(this,SIGNAL(updateRequest(QRect,int)),functionFoldArea,SLOT(update()));
    connect(this,SIGNAL(textChanged()),functionFoldArea,SLOT(repaint()));


    updateLineNumberAreaWidth(0);
}

QString CodeEditor::wordUnderCursor() const {
    /*
    //不断向左移动cursor，并选中字符，并查看选中的单词中是否含有空格——空格作为单词的分隔符
    QTextCursor cursor = textCursor();
    QString selectedString;
    while (cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, 1))
    {
        selectedString = cursor.selectedText();
        if (selectedString.startsWith(QString(" ")) || selectedString.startsWith(QChar(0x422029)))
        {
            break;
        }

    }
    if (selectedString.startsWith(QChar(0x422029)))     //换段符
    {
        selectedString.replace(0, 1, QChar(' '));
    }
    return selectedString.trimmed();
    */

    QTextCursor cursor = textCursor();
    cursor.select(QTextCursor::WordUnderCursor);
    return cursor.selectedText();
}

QAbstractItemModel* CodeEditor::modelFromFile(const QString &path){
    QFile file(path);
    if (!file.open(QFile::ReadOnly))
        return new QStringListModel(m_completer);

#ifndef QT_NO_CURSOR
    QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
#endif
    QStringList words;
    QStringList linewords;
    QRegExp reg("\\s+");

    while (!file.atEnd()) {
        QString line = file.readLine();
        if(line.startsWith("$$"))
            continue;
        linewords = line.split(reg);
        if (!line.isEmpty())
            words << linewords.at(0).trimmed();
    }
    //qDebug()<<words;

#ifndef QT_NO_CURSOR
    QGuiApplication::restoreOverrideCursor();
#endif
    return new QStringListModel(words,m_completer);
}

QStringList CodeEditor::WordsfromFile(const QString &path){
    QFile file(path);
    if(!file.open(QFile::ReadOnly)){
        QMessageBox::critical(nullptr,QString("读取错误"),QString("文件%1读取错误"),"确定");
        qDebug()<<"Open file " << path << "error" << __FUNCTION__;
    }
#ifndef QT_NO_CURSOR
    QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
#endif
    QStringList words;
    QStringList linewords;
    QRegExp reg("\\s+");

    while(!file.atEnd()){
        QString LineString = file.readLine();
        if(LineString.startsWith("$$"))
            continue;
        linewords = LineString.split(reg);
        //if(linewords.size()!=4)continue;
        QString word = linewords.at(0);
        words.append(word);
    }
#ifndef QT_NO_CURSOR
    QGuiApplication::restoreOverrideCursor();
#endif
    //qDebug() << words;
    return words;


}

void CodeEditor::onCompleterActivated(const QString &completion)
{
    /*
    QString completionPrefix = wordUnderCursor(),
            shouldInsertText = completion;
    QTextCursor cursor = textCursor();
    if (!completion.contains(completionPrefix))
    {
        // delete the previously typed.
        cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, completionPrefix.size());
        cursor.removeSelectedText();
        //cursor.clearSelection();
    }
    else
    {
        // 补全相应的字符
        shouldInsertText = shouldInsertText.replace(
            shouldInsertText.indexOf(completionPrefix), completionPrefix.size(), "");
    }
    cursor.insertText(shouldInsertText);*/

    //QString completionPre = wordUnderCursor();
    QTextCursor cursor = textCursor();
    QString completionPre = m_completer->completionPrefix();
    int extra = completion.length() - completionPre.length();
    cursor.movePosition(QTextCursor::Left);
    cursor.movePosition(QTextCursor::EndOfWord);
    cursor.insertText(completion.right(extra));
    setTextCursor(cursor);

}


int CodeEditor::LineNumberAreaWidth(){
    int digit = 1;
    int max = qMax(1,blockCount());

    while(max>=10){
        max/=10;
        ++digit;
    }

    int space = 6 + fontMetrics().horizontalAdvance(QLatin1Char('9'))*digit;

    return space;
}



void CodeEditor::updateLineNumberAreaWidth(int){            //width为行号宽度，20为折叠区域长度
    setViewportMargins(LineNumberAreaWidth()+20,0,0,0);
}



void CodeEditor::updateLineNumberArea(const QRect &rect,int dy){
    if(dy)
        lineNumberArea->scroll(0,dy);
    else
        lineNumberArea->update(0,rect.y(),lineNumberArea->width(),rect.height());

    if(rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}



void CodeEditor::resizeEvent(QResizeEvent *event){
    QPlainTextEdit::resizeEvent(event);
    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(),cr.top(),LineNumberAreaWidth(),cr.height()));
    functionFoldArea->setGeometry(QRect(cr.left()+LineNumberAreaWidth(),cr.top(),20,cr.height()));
}



void CodeEditor::keyPressEvent(QKeyEvent *event){

    if(event->key() == Qt::Key_Insert){                     //重写模式
        setOverwriteMode(!overwriteMode());
        //emit overwriteModeChanged();               //为标签提供信息,使用如下
        return;
    }    

    /*
        connect(&CodeEditor, &CodeEditor::overwriteModeChanged, [=](){
               if(CodeEditor->overwriteMode())
               {
                   insertModeLabel->setText("OVR");
               }
               else
               {
                   insertModeLabel->setText("INS");
               }
            })

    */

    if(event->key() == Qt::Key_M && event->modifiers() == Qt::AltModifier){
        quickComment();                 // 快速注释 or 反注释
        return;
    }

    if (m_completer)
    {
        if (m_completer->popup()->isVisible())
        {
            switch(event->key())
            {
                case Qt::Key_Escape:
                case Qt::Key_Enter:
                case Qt::Key_Return:
                case Qt::Key_Tab:
                case Qt::Key_Backtab:
                    event->ignore();
                    return;
                default:
                    break;
            }
        }
        bool CompleteShortCut = ((event->modifiers() & Qt::ControlModifier) && event->key() == Qt::Key_0);          //ctrl+0 快捷键
        if(!CompleteShortCut)
            QPlainTextEdit::keyPressEvent(event);
        const bool Ctrl_or_Shift = event->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
        if(Ctrl_or_Shift && event->text().isEmpty())
            return;
        static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
        bool hasModifier = (event->modifiers() != Qt::NoModifier) && !Ctrl_or_Shift;
        QString completionPrefix = wordUnderCursor();

        if (!CompleteShortCut && (hasModifier || event->text().isEmpty()|| completionPrefix.length() < 2            //前两个字符匹配时方显示complete
                          || eow.contains(event->text().right(1)))) {
            dynamic_cast<QListView*>(m_completer->popup())->hide();
            return;
        }

        if (completionPrefix != m_completer->completionPrefix()) {
            m_completer->setCompletionPrefix(completionPrefix);             // 通过设置QCompleter的前缀，来让Completer寻找关键词
            dynamic_cast<QListView*>(m_completer->popup())->setCurrentIndex(m_completer->completionModel()->index(0, 0));             //定位到第一个选项
        }
        QRect cr = cursorRect();
        cr.setWidth(dynamic_cast<QListView*>(m_completer->popup())->sizeHintForColumn(0)
                    + dynamic_cast<QListView*>(m_completer->popup())->verticalScrollBar()->sizeHint().width());
        m_completer->complete(cr);
        //m_completer->complete(QRect(cr.left(),cr.top(),60,15));
        //QAbstractItemView *a=m_completer->popup();
        //QObjectList b = a->children();
        //qDebug()<<b;
        return;
    }

        /*
        QPlainTextEdit::keyPressEvent(event);
        completerPrefix = wordUnderCursor();
        QRect cr = cursorRect();
        cr.setWidth(m_completer->popup()->sizeHintForColumn(0) + m_completer->popup()->verticalScrollBar()->sizeHint().width());
        static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word

        if (completerPrefix == "" || completerPrefix.length()<2 || eow.contains(event->text().right(1)) || event->text().isEmpty())            //
        {
            m_completer->popup()->hide();
            return;
        }

        if(completerPrefix!=m_completer->completionPrefix()){
            m_completer->setCompletionPrefix(completerPrefix); // 通过设置QCompleter的前缀，来让Completer寻找关键词
            m_completer->popup()->setCurrentIndex(m_completer->completionModel()->index(0, 0));  //定位到第一个选项
        }

        //qDebug() << "completerPrefix:" << completerPrefix << " match_count:" << m_completer->completionCount() << " completionColumn:"<<m_completer->completionColumn();
        //if (m_completer->completionCount() > 0)           //使用这个，会使补全列表，在前缀除space不匹配时，popup依然存在，显示一个空的列表
        //{
            //m_completer->complete(QRect(cr.left(), cr.top(), 60, 15));
            m_completer->complete(cr);
        //}
        return;
    }*/

    QPlainTextEdit::keyPressEvent(event);
}



void CodeEditor::paintEvent(QPaintEvent *event){
    QPlainTextEdit::paintEvent(event);

    QTextBlock block = firstVisibleBlock();
    while(block.isVisible() == false && block.blockNumber() != -1)
    {
        block = block.next();
    }
    if(block.blockNumber() == -1) return;
    int blockHeight = static_cast<int>(blockBoundingRect(block).height());
    int visibleBlockNum = rect().height()/blockHeight+1;
    int blockCounter = 1;
    QPainter painter(viewport());
    painter.setPen(Qt::black);
    //painter.setPen(cpadPalette.defaultGroup[9]);
    int littleChange = firstVisibleBlock().blockNumber() == 0 ? 4 : 0;
    while(block.blockNumber() != -1 && blockCounter <= visibleBlockNum)
    {
        if(block.isVisible() == false)
        {
            if(block.previous().text().indexOf('{') == -1)
                painter.drawText(QRect(block.previous().text().length()*fontMetrics().horizontalAdvance('a')+30,
                                       static_cast<int>(blockBoundingGeometry(block.previous()).top()+littleChange),
                                       fontMetrics().horizontalAdvance('a')*5,fontMetrics().height()),
                                 Qt::AlignCenter,QString("{...}"));
            else painter.drawText(QRect(block.previous().text().length()*fontMetrics().horizontalAdvance('a')+30,
                                        static_cast<int>(blockBoundingGeometry(block.previous()).top()+littleChange),
                                        fontMetrics().horizontalAdvance('a')*5,fontMetrics().height()),
                                  Qt::AlignCenter,QString("...}"));
            //painter.drawRect(QRect(block.previous().text().length()*fontMetrics().horizontalAdvance('a')+25,
                             //static_cast<int>(blockBoundingGeometry(block.previous()).top()+littleChange),
                             //fontMetrics().horizontalAdvance('a')*5+10,fontMetrics().height()));
            painter.drawRoundedRect(QRect(block.previous().text().length()*fontMetrics().horizontalAdvance('a')+25,
                                   static_cast<int>(blockBoundingGeometry(block.previous()).top()+littleChange),
                                   fontMetrics().horizontalAdvance('a')*5+10,fontMetrics().height()),50,20,Qt::RelativeSize);
            while(block.blockNumber() != -1 && block.isVisible() == false)
            {
                block = block.next();
            }
        }
        else block = block.next();
        if(block.blockNumber() != -1 && block.isVisible()) blockCounter++;
    }

    if(isRepaint)
    {
        isRepaint = false;

        QBrush brush(QColor(200,230,255));
        brush.setStyle(Qt::Dense4Pattern);
        QPainter painter(viewport());

        QTextBlock block = getMousePositionBlock(m_MousePosition.y());
        //int blockHeight = (int)blockBoundingRect(block).height();
        /*int blockNum = mousePosition.y()/blockHeight+1;

        if(blockNum <= blockCount())
        {
            for(int i = 1 ; i < blockNum ; i++)
            {
                if(block.blockNumber() == -1) return;
                if(block.isVisible() == false) i--;
                block = block.next();
            }

            while(block.isVisible() == false && block.blockNumber() != -1)
            {
                block = block.next();
            }
        }
        else return;*/
        if(block.blockNumber() == -1) return;
        if(JudgeLeftBracket(block)) block = block.next();

        QTextBlock leftBracketBlock,rightBracketBlock;
        if(MatchForFolding(block,leftBracketBlock,rightBracketBlock))
        {
            int littleChange = firstVisibleBlock().blockNumber() == 0 ? 4 : 0;
            int bottomBlockPosition = static_cast<int>(blockBoundingGeometry(leftBracketBlock).bottom())+
                                      (rightBracketBlock.blockNumber()-leftBracketBlock.blockNumber())*
                                      static_cast<int>(blockBoundingGeometry(leftBracketBlock).height());
            painter.fillRect(QRect(0,littleChange,event->rect().width(),
                                   static_cast<int>(blockBoundingGeometry(leftBracketBlock).top())),brush);
            painter.fillRect(QRect(0,bottomBlockPosition+littleChange,
                                   event->rect().width(),
                                   event->rect().height()-bottomBlockPosition),
                             brush);
            painter.fillRect(QRect(0,static_cast<int>(blockBoundingGeometry(leftBracketBlock).top())+littleChange,5,
                                   bottomBlockPosition-
                                   static_cast<int>(blockBoundingGeometry(leftBracketBlock).top())),brush);
            painter.fillRect(QRect(event->rect().width()-5,
                                   static_cast<int>(blockBoundingGeometry(leftBracketBlock).top())+littleChange,5,
                                   bottomBlockPosition-
                                   static_cast<int>(blockBoundingGeometry(leftBracketBlock).top())),brush);
            this->hide();
            this->show();
        }
        else return;
    }
}



void CodeEditor::onEditDelete(){
    QTextCursor cursor = textCursor();
    if(cursor.hasSelection()){
        cursor.removeSelectedText();
    }
    else
        return;

    //QKeyEvent KeyPress(QEvent::KeyPress,Qt::Key_Delete,Qt::NoModifier);
    //QKeyEvent keyRelease(QEvent::KeyRelease, Qt::Key_Delete, Qt::NoModifier);

    //QApplication::sendEvent(this,&KeyPress);
    //QApplication::sendEvent(this,&keyRelease);

}



void CodeEditor::contextMenuEvent(QContextMenuEvent *event){                //重载右键菜单
    QMenu* menu = new QMenu(this);
    QAction *action;
    const bool showTextSelectionActions = Qt::TextEditable | Qt::TextSelectableByKeyboard | Qt::TextSelectableByMouse;

    if(Qt::TextEditable!=0){
        action = menu->addAction(tr("撤销"),this,SLOT(undo()));
        action->setEnabled(this->document()->isUndoAvailable());
        action->setObjectName(QStringLiteral("edit-undo"));
        setActionIcon(action,QStringLiteral("edit-undo"));

        action = menu->addAction(tr("恢复"),this,SLOT(undo()));
        action->setEnabled(this->document()->isRedoAvailable());
        action->setShortcut(Qt::CTRL+Qt::Key_Y);
        action->setObjectName(QStringLiteral("edit-redo"));
        setActionIcon(action,QStringLiteral("edit-redo"));

        menu->addSeparator();

#ifndef QT_NO_CLIPBOARD

        action = menu->addAction(tr("剪切"),this,SLOT(cut()));
        action->setEnabled(this->textCursor().hasSelection());
        action->setObjectName(QStringLiteral("edit-cut"));
        setActionIcon(action,QStringLiteral("edit-cut"));

#endif
        }

#ifndef QT_NO_CLIPBOARD

        if(showTextSelectionActions){
            action = menu->addAction(tr("复制"),this,SLOT(copy()));
            action->setEnabled(this->textCursor().hasSelection());
            action->setObjectName(QStringLiteral("edit-copy"));
            setActionIcon(action,QStringLiteral("edit-copy"));
        }
#endif


        if(Qt::TextEditable!=0){

#ifndef QT_NO_CLIPBOARD

            action = menu->addAction(tr("粘贴"),this,SLOT(paste()));
            action->setEnabled(canPaste());
            action->setObjectName(QStringLiteral("edit-paste"));
            setActionIcon(action, QStringLiteral("edit-paste"));

#endif

            action = menu->addAction(tr("删除"),this,SLOT(onEditDelete()));
            action->setEnabled(this->textCursor().hasSelection());
            action->setObjectName(QStringLiteral("edit-delete"));
            setActionIcon(action, QStringLiteral("edit-delete"));
        }

        if(showTextSelectionActions){
            menu->addSeparator();
            action = menu->addAction(tr("全选"),this,SLOT(selectAll()));
            action->setEnabled(!this->document()->isEmpty());
            action->setObjectName(QStringLiteral("select-all"));
        }


    menu->exec(event->globalPos());

    //QPlainTextEdit::contextMenuEvent(event);

}



void CodeEditor::highlightCurrentLine(){
    QList<QTextEdit::ExtraSelection> extraSelections;           //专门为额外text提供format的结构体
    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::yellow).lighter(160);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}



void CodeEditor::LineNumberAreaPaintEvent(QPaintEvent *event)           //行号绘制
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = static_cast<int>(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + static_cast<int>(blockBoundingRect(block).height());
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);

            if(painter.font().bold()){
                QFont a = painter.font();
                a.setBold(false);
                painter.setFont(a);
            }
            QTextCursor cursor = textCursor();
            if(cursor.blockNumber()+1==number.toInt()){             //选中行号加粗
                QFont a = painter.font();
                a.setBold(true);
                painter.setFont(a);
            }

            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignCenter, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + static_cast<int>(blockBoundingRect(block).height());
        ++blockNumber;
    }
}



void CodeEditor::quickComment(){                //快速单行注释
    QTextCursor t_cur(textCursor());
    QString line(t_cur.block().text());
    QRegExp reg("\\s*//");

    if(reg.indexIn(line)==0){
        int count = line.indexOf("//");
        line.remove(count,2);
        t_cur.movePosition(QTextCursor::StartOfBlock);
        t_cur.movePosition(QTextCursor::EndOfBlock,QTextCursor::KeepAnchor);
        t_cur.insertText(line);
    }

    else{
        int length = line.length();
        int count = 0;
        for (;count<length;++count) {
            if(line[count]!=' ')
                break;
        }
        line.insert(count,"//");
        t_cur.movePosition(QTextCursor::StartOfBlock);
        t_cur.movePosition(QTextCursor::EndOfBlock,QTextCursor::KeepAnchor);
        t_cur.insertText(line);
    }
}



int CodeEditor::getInvisibleBlockNum(int num){      //获取某个block之前的不可见block数量
    QTextBlock block = firstVisibleBlock();
    int count = 0;
    for(int i = 1;i<=num;++i){
        if(block.blockNumber() == -1)
            break;
        if(block.isVisible() == false){
            i--;
            count++;
        }
        block = block.next();
    }
    return count;
}



QTextBlock CodeEditor::getMousePositionBlock(int position_Y){           //根据坐标y获取鼠标所在block
    QTextBlock block;
    block = firstVisibleBlock();

    while (block.blockNumber()!= -1  &&  block.isVisible() == false) {
        block = block.next();
    }

    if(block.blockNumber() == -1)
        return block;

    int block_height = static_cast<int>(blockBoundingRect(block).height());
    int block_num = position_Y/block_height + 1;

    if(block_num <= blockCount()){
        for(int i =1;i<block_num;++i){
            if(block.blockNumber() == -1)
                return block;
            if(block.isVisible() == false)
                --i;
            block = block.next();
        }

        return block;

    }

    else
        return block;           //返回开头

}

bool CodeEditor::JudgeLeftBracket(QTextBlock block){
    if(block.next().blockNumber() == -1)
        return false;
    block = block.next();
    for(int i=0;i<block.text().length();++i){
        if(block.text().at(i) != QChar(' ') && block.text().at(i) != QChar('{'))
            return false;
        else if(block.text().at(i) == QChar('{'))
            return true;
    }
    return false;
}

bool CodeEditor::MatchForFolding(QTextBlock &currentBlock,QTextBlock &leftBlock,QTextBlock rightBlock){
    if(!currentBlock.isValid())
        return false;
    QTextBlock current = currentBlock;
    bool flag = false;
    int count = 0;
    //find {
    while(true){
        TextBlockData *data = static_cast<TextBlockData*>(current.userData());
        if(data){
            QVector<ParenthesisInfo*>infos = data->parentheses();

            for(int i =0;i<infos.size();++i){
                ParenthesisInfo *info = infos.at(i);

                if(info->character == '{'){

                    if(count == 0){
                        leftBlock = current;
                        flag = true;
                        break;
                    }

                    else
                        --count;
                }

                else if(info->character == '}')
                    ++count;
            }

            if(flag == true)
                break;

            else{
                current = current.previous();
                if(!current.isValid())
                    break;
            }
        }

        else{
            current = current.previous();
            if(!current.isValid())
                break;
        }
    }
    if(flag == false)
        return false;
    flag = false;
    current = currentBlock;
    count = 0;
    while(true){
        TextBlockData *data = static_cast<TextBlockData*>(current.userData());
        if(data){
            QVector<ParenthesisInfo*>infos = data->parentheses();

            for(int i =0;i<infos.size();++i){
                ParenthesisInfo *info = infos.at(i);
                if(info->character == '{' && current.blockNumber()!=currentBlock.blockNumber())
                    ++count;
                if(info->character == '}'){
                    if(count == 0){
                        rightBlock = current;
                        flag = true;
                        break;
                    }
                    else
                        --count;
                }
            }

            if(flag == true)
                break;

            else
            {
                current = current.next();
                if(!current.isValid())
                    break;
            }
        }

        else{
            current = current.next();
            if(!current.isValid())
                break;
        }
    }

    if(flag == false)
        return false;

    return true;

}

void CodeEditor::FunctionFold(int y){
    QTextBlock blk_cur = getMousePositionBlock(y);
    QTextBlock blk_top,blk_bottom;

    if(JudgeLeftBracket(blk_cur))
        blk_cur = blk_cur.next();

    if(MatchForFolding(blk_cur,blk_top,blk_bottom)){
        bool isCursorMove = true ;

        if(blk_top.blockNumber() != 0 && JudgeLeftBracket(blk_top.previous()))
            blk_top = blk_top.previous();

        if(isCursorMove){
            QTextCursor cursor = textCursor();
            QTextBlock block = cursor.block();
            do{
                if(block.blockNumber()<blk_top.blockNumber()){
                    block = block.next();
                    moveCursor(QTextCursor::NextBlock);
                }
                else{
                    block = block.previous();
                    moveCursor(QTextCursor::PreviousBlock);
                }
            }while(block.blockNumber()!=blk_top.blockNumber() && textCursor().block().blockNumber()!=0);
        }

            if(blk_top.blockNumber() == 0 || JudgeLeftBracket(blk_top.previous()) == false)
                blk_top = blk_top.next();
            if(blk_top.blockNumber() == -1)
                return ;

            QTextBlock block = blk_top;
            do{
                block.setVisible(false);
                block = block.next();
                block.setVisible(false);
            }while(block.blockNumber()!=-1 && block.blockNumber()<blk_bottom.blockNumber());

            this->hide();
            this->show();
    }
}

void CodeEditor::FunctionUnfold(int y){
    QTextBlock block = getMousePositionBlock(y);

    if(block.isValid()){
        while(block.isVisible()==false && block.blockNumber()!=-1){
            block = block.next();
        }

        block = block.next();

        while(block.isVisible()==false && block.blockNumber()!=-1){
            block.setVisible(true);
            block = block.next();
        }

        this->hide();
        this->show();
    }
}

void CodeEditor::FunctionFoldAreaPaintEvent(QPaintEvent *event){
    QPainter painter(functionFoldArea);
    painter.fillRect(event->rect(),Qt::lightGray);

    if(m_MousePosition.x()>0 && m_MousePosition.x()<20){

        QTextBlock block = getMousePositionBlock(m_MousePosition.y());

        if(block.blockNumber()!=-1){

            QTextBlock topBlock,bottomBlock;

            if(JudgeLeftBracket(block))
                block = block.next();

            if(MatchForFolding(block,topBlock,bottomBlock)){
                isRepaint = true;
                if(topBlock.blockNumber() != 0 && JudgeLeftBracket(topBlock.previous())){
                    topBlock = topBlock.previous();
                }
                QLinearGradient linearG(QPointF(1,static_cast<int>(blockBoundingGeometry(topBlock).top())),
                                            QPointF(19,static_cast<int>(blockBoundingGeometry(topBlock).top())));
                linearG.setColorAt(0,QColor(98,153,210));
                linearG.setColorAt(1,QColor(98,153,210).lighter(120));
                QBrush brush(linearG);
                int bottomBlockPosition = static_cast<int>(blockBoundingGeometry(topBlock).bottom())+
                            (bottomBlock.blockNumber()-topBlock.blockNumber())*
                            static_cast<int>(blockBoundingGeometry(topBlock).height());
                painter.fillRect(QRect(1,static_cast<int>(blockBoundingGeometry(topBlock).top()),18,
                                   bottomBlockPosition-static_cast<int>(blockBoundingGeometry(topBlock).top())),
                                   brush);
            }
        }
    }

    QTextBlock block = firstVisibleBlock();

    while(block.isVisible()==false && block.blockNumber()!=-1){
        block = block.next();
    }

    if(block.blockNumber() == -1)
        return;

    int blockheight = static_cast<int>(blockBoundingRect(block).height());
    int visibleBlockNum = rect().height()/blockheight + 1;
    int blocknum = 1;

    while(block.blockNumber()!=-1 && blocknum<=visibleBlockNum){

        QTextBlock topBlock,bottomBlock;

        if(MatchForFolding(block,topBlock,bottomBlock)&&
           topBlock.blockNumber()>=firstVisibleBlock().blockNumber()){

            int foldPosition;

            if(topBlock.blockNumber() != 0 && topBlock.previous().isVisible() == false)
            {
                block = block.next();
                if(block.blockNumber() != -1 && block.isVisible())
                    ++blocknum;
                continue;
            }

            if(topBlock.blockNumber() != 0 && JudgeLeftBracket(topBlock.previous())){
                topBlock = topBlock.previous();
            }

            foldPosition = static_cast<int>(blockBoundingGeometry(topBlock).top());
            painter.setPen(Qt::black);
            int littleChange = firstVisibleBlock().blockNumber() == 0 ? 4 : 0;

            if(topBlock.next().isVisible()){
                QPointF points1[3] = {
                    QPointF(12,foldPosition+littleChange+5),
                    QPointF(6,foldPosition+littleChange+11),
                    QPointF(12,foldPosition+littleChange+11)
                };
                painter.drawPolygon(points1,3);
                QPointF points2[3] = {
                    QPointF(11,foldPosition+littleChange+6),
                    QPointF(7,foldPosition+littleChange+10),
                    QPointF(11,foldPosition+littleChange+10)
                };
                painter.drawPolygon(points2,3);
                QPointF points3[3] = {
                    QPointF(10,foldPosition+littleChange+7),
                    QPointF(8,foldPosition+littleChange+9),
                    QPointF(10,foldPosition+littleChange+9)
                };
                painter.drawPolygon(points3,3);
            }
            else{
                QPointF points[3] = {
                    QPointF(7,foldPosition+littleChange+4),
                    QPointF(7,foldPosition+littleChange+12),
                    QPointF(11,foldPosition+littleChange+8)
                };
                painter.drawPolygon(points,3);
            }
        }
        block = block.next();
        if(block.blockNumber()!=-1 && block.isVisible())
            ++blocknum;
    }
}


/*
void CodeEditor::isSave(){
    if(this->document()->isModified()){
        int ret = QMessageBox::warning(this,tr("警告"),tr("文档没有保存是否需要保存"),QMessageBox::Save|QMessageBox::Cancel,QMessage::Save);
        if(ret == QMessage::Save)
        {
            doSave();
        }
    }
}

void CodeEditor::doOpen(){
    QString file = QFileDialog::getOpenFileName(this);
    if(file.isEmpty()){
        isSave();
        if(openFile(file))
            bool ok = true;
    }
}

void CodeEditor::doClose(){
    if(this->document()->isModified())
        isSave();
    close();
}

bool CodeEditor::openFile(QString &file){
    QFile f(file);
    if(!f.open(QFile::ReadOnly|QFile:Text)){
        QMessageBox::warning(this,tr("打开文件"),tr("打开文件%1失败"\n%2).arg(file).arg(f.errorString()));
        return false;
    }
    QTextStream stream(&f);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    this->setPlainText(stream.readAll());
    QApplication::restoreOverrideCursor();
    //setCurrentFile(file);       //参考tabwidget标签，多文档时使用，待续
    connect(this,SIGNAL(textChanged()),this,SLOT(setDocumentmodified()));
    //this->showMaximized();
    return true;
}

void CodeEditor::doSave(){
    if(无标题)
        doSaveas();
    else
        saveFile(当前文件);
}

void CodeEditor::doSaveas(){
    QString file = QFileDialog::getSaveFileName(this,tr("另存为"),currentfile);
    if(!file.isEmpty()){
        saveFile(file);
    }
}

bool CodeEditor::saveFile(QString &file){
    QString filepath = m_filepath;          //使用时参考mainwindow.h,m_filepath
    QString filename = QString(%1/%2.txt).arg(filepath).arg(file);

    QFile f(filename);
    if(!f.open(QFile::WriteOnly|QFile:Text)){
        QMessageBox::warning(this,tr("保存文件"),tr("保存文件%1失败"\n%2).arg(file).arg(f.errorString()));
        return false;
    }
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QTextStream stream(&f);
    stream<<this->toPlainText();
    //setCurrentFile(file);
    QApplication::restoreOverrideCursor();
    return true;
}


void CodeEditor::setTextFamily(const QString &f){               //设置字体样式
    QTextCharFormat fmt;
    fmt.setFontFamily(f);
    mergeCurrentCharFormat(fmt);
}

void CodeEditor::setTextFontBold(bool ok){                      //设置粗体
    QTextCharFormat fmt ;
    if(ok)
        fmt.setFontWeight(QFont::Bold);
    else
        fmt.setFontWeight(QFont::Normal);
    mergeCurrentCharFormat(fmt);
}

void CodeEditor::setFontSize(QString num){              //字体大小
    QTextCharFormat fmt;
    fmt.setFontPointSize(num.toFloat());
    mergeCurrentCharFormat(fmt);
}

void CodeEditor::setTextItalic(bool ok){                //斜体
    QTextCharFormat fmt ;
    fmt.setFontItalic(ok);
    mergeCurrentCharFormat(fmt);
}

void CodeEditor::setUnderLine(bool ok){                 //下划线
    QTextCharFormat fmt;
    fmt.setFontUnderline(ok);
    mergeCurrentCharFormat(fmt);
}

void CodeEditor::setWrapMode(bool ok){
    if(ok)
        setLineWrapMode(QPlainTextEdit::WidgetWidth);
    else
        setLineWrapMode(QPlainTextEdit::NoWrap);
}

void CodeEditor::setFontColor(){                        //字体颜色
    QColor color = QColorDialog::getColor(Qt::red,this);
    if(color.isValid()){
        QTextCharFormat fmt ;
        fmt.setForeground(color);
        mergeCurrentCharFormat(fmt);
    }
}

void CodeEditor::setTextStyle(int StyleIndex){                  //文本列表排序，style为采用哪种方式，indent为设置的缩进值
    QTextCursor cursor = this->textCursor();
    if(StyleIndex != 0){
        QTextListFormat::Style style = QTextListFormat::ListDisc;
        switch(StyleIndex){
            default:
            case 1:
                style = QTextListFormat::ListDisc;
                break;
            case 2:
                style = QTextListFormat::ListCircle;
                break;
            case 3:
                style = QTextListFormat::ListSquare;
                break;
            case 4:
                style = QTextListFormat::ListDecimal;
                break;
            case 5:
                style = QTextListFormat::ListLowerAlpha;
                break;
            case 6:
                style = QTextListFormat::ListUpperAlpha;
                break;
            case 7:
                style = QTextListFormat::ListLowerRoman;
                break;
            case 8:
                style = QTextListFormat::ListUpperRoman;
                break;
        }
        cursor.beginEditBlock();
        QTextBlockFormat blockfmt = cursor.blockFormat();
        QTextListFormat listfmt;
        if(cursor.currentList()){
            listfmt = cursor.currentList()->format();
        }
        else{
            listfmt.setIndent(blockfmt.indent()+2);
            blockfmt.setIndent(0);
            cursor.setBlockFormat(blockfmt);
        }
        listfmt.setStyle(style);
        cursor.createList(listfmt);
        cursor.endEditBlock();
    }
    else{
        QTextBlockFormat blfmt;
        blfmt.setObjectIndex(-1);           //设置不可用
        cursor.mergeBlockFormat(blfmt);
    }
}

void CodeEditor::setAlignment(QAction * action){                       //设置对齐
       if ( action == pLeftAction )
              this->setAlignment( Qt::AlignLeft );
       if ( action == pCenterAction )
              this->setAlignment( Qt::AlignCenter );
       if ( action == pJustifyAction )
              this->setAlignment( Qt::AlignJustify );
       if ( action == pRightAction )
              this->setAlignment( Qt::AlignRight );
}
*/
