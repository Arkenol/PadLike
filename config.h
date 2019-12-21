#ifndef CONFIGS_H
#define CONFIGS_H

#include <QFont>
#include <QSize>
#include <QPoint>
#include <QObject>


class Configs: public QObject
{
    Q_OBJECT

private:
    QFont m_EditerFont;
    QSize m_WindowsSize;
    QPoint m_WindowsPoints;

    bool isToolBarVisible;
    bool isStatusBarVisible;
    bool isAutoWrap;
    bool isValid;

    bool restore();

public:
    explicit Configs(QObject *parent = nullptr);
    explicit Configs(QFont font,QPoint Point,QSize size,bool isAutoWrap,bool isToolBarVisible,bool isStatusBarVisible,QObject* parent = nullptr);

    bool store();

    QFont getFont();
    QSize getWindowSize();
    QPoint getWindowPoint();

    bool isAutoWraps();
    bool isValids();
    bool isToolBarVisibles();
    bool isStatusBarVisibles();
};

#endif // CONFIGS_H
