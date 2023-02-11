#include "Dialog.h"
#include "ui_Dialog.h"
#include <QLabel>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QLineEdit>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    QLabel* widget1 = new QLabel(tr("Text item"), this);
    QListWidgetItem* item1 = new QListWidgetItem;
    ui->listWidget->addItem(item1);
    ui->listWidget->setItemWidget(item1, widget1);

    QWidget* widget2 = new QWidget(this);
    QPushButton* btn = new QPushButton(tr("Button item"), this);
    QHBoxLayout* hbox = new QHBoxLayout(widget2);
    hbox->setContentsMargins(5, 0, 5, 0);
    hbox->addWidget(btn);
    hbox->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    QListWidgetItem* item2 = new QListWidgetItem;
    ui->listWidget->addItem(item2);
    item2->setSizeHint(QSize(100, 40));
    ui->listWidget->setItemWidget(item2, widget2);

    QLineEdit* widget3 = new QLineEdit(this);
    widget3->setMaximumWidth(200);
    QListWidgetItem* item3 = new QListWidgetItem;
    ui->listWidget->addItem(item3);
    item2->setSizeHint(QSize(100, 40));
    ui->listWidget->setItemWidget(item3, widget3);

    m_proxy = new MoveItemProxy(this);
    m_proxy->attachListWidget(ui->listWidget);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_moveUpBtn_clicked()
{
    QListWidgetItem* curItem = ui->listWidget->currentItem();
    if (!curItem)
        return;

    int row = ui->listWidget->row(curItem);
    m_proxy->moveItem(row, row - 1);
}

void Dialog::on_moveDownBtn_clicked()
{
    QListWidgetItem* curItem = ui->listWidget->currentItem();
    if (!curItem)
        return;

    int row = ui->listWidget->row(curItem);
    m_proxy->moveItem(row, row + 1);
}

void Dialog::on_reverseBtn_clicked()
{
    m_proxy->reverse();
}

