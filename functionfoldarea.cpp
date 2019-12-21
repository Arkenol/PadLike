#include "functionfoldarea.h"
#include <QDebug>

void FunctionFoldArea::mouseMoveEvent(QMouseEvent *event){
    CodeEditor->m_MousePosition.setX(event->x());
    CodeEditor->m_MousePosition.setY(event->y());
    repaint();
}

void FunctionFoldArea::paintEvent(QPaintEvent *event){
    CodeEditor->FunctionFoldAreaPaintEvent(event);
}

void FunctionFoldArea::leaveEvent(QEvent *event){
    CodeEditor->m_MousePosition.setX(-1);
    repaint();
    event->ignore();
    //QWidget::leaveEvent(event);
}

void FunctionFoldArea::mousePressEvent(QMouseEvent *event){
    QTextBlock block = CodeEditor->getMousePositionBlock(event->y());
    //qDebug()<<block.blockNumber();

    if(block.isValid()){
        while (block.isVisible() == false && block.blockNumber()!=-1) {
            block = block.next();
        }

        if(block.blockNumber() == -1)
            return;

        if(block.next().isVisible())
            CodeEditor->FunctionFold(event->y());
        else
            CodeEditor->FunctionUnfold(event->y());
    }
}
