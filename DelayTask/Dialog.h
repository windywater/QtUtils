#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "DelayTask.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

private Q_SLOTS:
    void on_lineEdit_textChanged(const QString& text);

private:
    Ui::Dialog *ui;
    DelayTask* m_delayTask;
};

#endif // DIALOG_H
