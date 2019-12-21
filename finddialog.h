#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QWidget>
#include <QDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QGroupBox>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPointer>
#include <QMessageBox>
#include <QEvent>
#include <QTextCursor>


class FindDialog: public QDialog
{
    Q_OBJECT

protected:
    QGroupBox m_RadioBox;
    QGridLayout m_glayout;
    QHBoxLayout m_hlayout;

    QLabel m_find;
    QLineEdit m_findedit;
    QPushButton m_findbtn;
    QPushButton m_cancelbtn;
    QCheckBox m_matchbox;
    QRadioButton m_upforward;
    QRadioButton m_downforward;

    QPointer<QPlainTextEdit> m_textediter;

    void initWindow();
    void connectSlot();

public slots:
    void onFindClicked();
    void onCancelClicked();

public:
    FindDialog(QWidget* parent = nullptr,QPlainTextEdit* TextEditer = nullptr);
    void setPlainTextEdit(QPlainTextEdit *TextEditer);
    QPlainTextEdit* getPlainTextEdit();
    bool event(QEvent *event);
    ~FindDialog();
};

#endif // FINDDIALOG_H
