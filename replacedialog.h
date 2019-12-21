#ifndef REPLACEDIALOG_H
#define REPLACEDIALOG_H

#include "finddialog.h"

class ReplaceDialog : public FindDialog
{
    Q_OBJECT

protected:
    QLabel m_Replace;
    QLabel m_ReplaceEdit;
    QPushButton m_ReplaceBtn;
    QPushButton m_ReplaceAllBtn;

    void initWindow();
    void connectSlot();

public slots:
    void onReplaceClicked();
    void onReplaceAllClicked();

public:
    ReplaceDialog(QWidget *parent = nullptr, QPlainTextEdit *TextEditer = nullptr);
};

#endif // REPLACEDIALOG_H
