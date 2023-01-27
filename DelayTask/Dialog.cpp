#include "Dialog.h"
#include "ui_Dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    m_delayTask = new DelayTask(DelayTask::Prevent, this);
    m_delayTask->setDelayTime(300);

}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_lineEdit_textChanged(const QString& text)
{
    m_delayTask->doTask([=]() {
        ui->textLabel->setText(ui->lineEdit->text());
    });
}
