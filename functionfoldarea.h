#ifndef FUNCTIONFOLDAREA_H
#define FUNCTIONFOLDAREA_H

#include <QEvent>
#include <QWidget>
#include <QTextBlock>

#include "codeeditor.h"

class FunctionFoldArea : public QWidget
{
    Q_OBJECT
public:
    FunctionFoldArea(CodeEditor *parent):QWidget(parent){
        CodeEditor = parent;
        setMouseTracking(true);                     //鼠标移动即mousemove事件一直触发
    }

    void mouseMoveEvent(QMouseEvent *event)override;

protected:
    void paintEvent(QPaintEvent *event) override;

    void leaveEvent(QEvent *event)override ;

    void mousePressEvent(QMouseEvent *event)override;

private:
    CodeEditor *CodeEditor;

};
#endif // FUNCTIONFOLDAREA_H
