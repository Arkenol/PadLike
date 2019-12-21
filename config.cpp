#include "config.h"

#include <QApplication>
#include <QDataStream>
#include <QFile>

Configs::Configs(QObject* parent):QObject (parent){
    isValid = restore();
}

Configs::Configs(QFont font,QPoint Point,QSize size,bool isAW,bool isTB,bool isSB,QObject* parent):
    QObject (parent){               //QSetting 亦可保存一些设定

    isAutoWrap = isAW;
    isToolBarVisible = isTB;
    isStatusBarVisible = isSB;
    m_EditerFont = font;
    m_WindowsSize = size;
    m_WindowsPoints = Point;

    isValid = true;

}

bool Configs::store(){
    bool temp = true ;
    QFile file(QApplication::applicationDirPath() + "/pad_like.config");

    if(file.open(QIODevice::WriteOnly)){

        QDataStream out (&file);

        out << m_EditerFont;
        out << m_WindowsSize;
        out << m_WindowsPoints;
        out << isAutoWrap;
        out << isToolBarVisible;
        out << isStatusBarVisible;

        file.close();
    }
    else{
        temp = false;
    }

    return temp ;

}

bool Configs::restore(){
    bool temp = true ;
    QFile file(QApplication::applicationDirPath() + "/pad_like.config");

    if(file.open(QIODevice::ReadOnly)){

        QDataStream in(&file);

        in >> m_EditerFont;
        in >> m_WindowsSize;
        in >> m_WindowsPoints;
        in >> isAutoWrap;
        in >> isToolBarVisible;
        in >> isStatusBarVisible;

        file.close();
    }
    else{
        temp = false;
    }

    return temp ;

}

QFont Configs::getFont(){
    return m_EditerFont;
}

QPoint Configs::getWindowPoint(){
    return m_WindowsPoints;
}

QSize Configs::getWindowSize(){
    return m_WindowsSize;
}

bool Configs::isAutoWraps(){
    return isAutoWrap;
}

bool Configs::isValids(){
    return isValid;
}

bool Configs::isToolBarVisibles(){
    return isToolBarVisible;
}

bool Configs::isStatusBarVisibles(){
    return isStatusBarVisible;
}
