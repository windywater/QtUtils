#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "MoveItemProxy.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

private slots:
    void on_moveUpBtn_clicked();
    void on_moveDownBtn_clicked();
    void on_reverseBtn_clicked();

private:
    Ui::Dialog *ui;
    MoveItemProxy* m_proxy;
};

#endif // DIALOG_H
