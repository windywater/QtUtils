#include "Dialog.h"
#include "ui_Dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    m_delayTask = new DelayTask(this);
    m_delayTask->setDelayTime(1000);
    ui->contRadio->setChecked(true);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_contRadio_toggled(bool checked)
{
    if (checked)
        m_delayTask->setDelayMode(DelayTask::ContinuousDelay);
}

void Dialog::on_timeoutRadio_toggled(bool checked)
{
    if (checked)
        m_delayTask->setDelayMode(DelayTask::TimeoutDelay);
}

void Dialog::on_lineEdit_textChanged(const QString& text)
{
    (void)text;
    m_delayTask->doTask([=]() {
        ui->textLabel->setText(ui->lineEdit->text());
    });
}
