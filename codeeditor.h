#ifndef CodeEditor_H
#define CodeEditor_H

#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QAction>
#include <QCompleter>
#include <QEvent>
#include <QIcon>
#include <QPainter>
#include <QPlainTextEdit>
#include <QPoint>
#include <QScrollBar>
#include <QStringListModel>

#include <textblockdata.h>


class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    CodeEditor(QWidget *parent = nullptr);

    QTextBlock getMousePositionBlock(int position_Y);
    void FunctionFold(int y);
    void FunctionUnfold(int y);

    void LineNumberAreaPaintEvent(QPaintEvent *event);
    void FunctionFoldAreaPaintEvent(QPaintEvent *event);
    int LineNumberAreaWidth();

    static void inline setActionIcon(QAction *action,const QString &name){
        const QIcon icon = QIcon::fromTheme(name);
        if(!icon.isNull()){
            action->setIcon(icon);
        }
    }

    QPoint m_MousePosition;


signals:
    //void overwriteModeChanged();


protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;


private slots:
    void highlightCurrentLine();
    void updateLineNumberAreaWidth(int);
    void updateLineNumberArea(const QRect &,int);

    void quickComment();
    void onEditDelete();
    void onCompleterActivated(const QString &completion);


private:
    bool isRepaint ;
    QCompleter *m_completer;
    QWidget *lineNumberArea;
    QWidget *functionFoldArea;

    QString completerPrefix;

    void setCompleter(QCompleter *completer);
    void setLineNumberArea();


    bool JudgeLeftBracket(QTextBlock block);
    bool MatchForFolding(QTextBlock currentBlock,QTextBlock &leftBlock,QTextBlock &rightBlock);

    QAbstractItemModel *modelFromFile(const QString &path);
    QStringList WordsfromFile(const QString &path);
    QString wordUnderCursor() const;

public:
    int getInvisibleBlockNum(int num);
};

#endif // CodeEditor_H
