#ifndef HELPDIGLOG_H
#define HELPDIGLOG_H

#include <QDialog>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>


class HelpDiglog : public QDialog
{
    Q_OBJECT

private:
    QLabel m_Logo;
    QLabel m_Version;
    QLabel m_Author;
    QLabel m_Authorurl;
    QLabel m_Homepage;
    QLabel m_Homeurl;
    QLabel m_Mail;
    QLabel m_Mailurl;

    QPlainTextEdit m_Editer;
    QPushButton m_btn;

    void initWindow();
    void connectSlot();

public slots:
    void m_btnClicked();

public:
    HelpDiglog(QWidget* parent);
};

#endif // HELPDIGLOG_H
