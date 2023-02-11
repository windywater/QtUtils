#include "MoveItemProxy.h"

MoveItemProxy::MoveItemProxy(QObject *parent)
	: QItemDelegate(parent)
{
	m_listWidget = Q_NULLPTR;
}

MoveItemProxy::~MoveItemProxy()
{
}

void MoveItemProxy::attachListWidget(QListWidget* listWidget)
{
	m_listWidget = listWidget;
}

void MoveItemProxy::moveItem(int oldRow, int newRow)
{
	if (!m_listWidget)
		return;

	int rowCount = m_listWidget->count();
	if (oldRow < 0 || oldRow >= rowCount || newRow < 0 || newRow >= rowCount)
		return;

	if (oldRow == newRow)
		return;

	QListWidgetItem* item = m_listWidget->item(oldRow);
	QWidget* itemWidget = m_listWidget->itemWidget(item);

	QAbstractItemDelegate* oldDlgt = m_listWidget->itemDelegate();
	m_listWidget->setItemDelegate(this);
	m_listWidget->takeItem(oldRow);
	m_listWidget->setItemDelegate(oldDlgt);

	m_listWidget->insertItem(newRow, item);
	m_listWidget->setItemWidget(item, itemWidget);

	m_listWidget->setCurrentItem(item);
}

void MoveItemProxy::reverse()
{
	if (!m_listWidget)
		return;

	if (m_listWidget->count() <= 1)
		return;

	QList<QPair<QListWidgetItem*, QWidget*>> items;
	for (int i = 0; i < m_listWidget->count(); i++)
	{
		QListWidgetItem* item = m_listWidget->item(i);
		QWidget* widget = m_listWidget->itemWidget(item);
		items << qMakePair(item, widget);
	}

	QAbstractItemDelegate* oldDlgt = m_listWidget->itemDelegate();
	m_listWidget->setItemDelegate(this);

	while (m_listWidget->count() != 0)
	{
		QListWidgetItem* item = m_listWidget->item(0);
		QWidget* widget = m_listWidget->itemWidget(item);
		items.push_front(qMakePair(item, widget));
		m_listWidget->takeItem(0);
	}

	m_listWidget->setItemDelegate(oldDlgt);

	for (const auto& pair : items)
	{
		m_listWidget->addItem(pair.first);
		m_listWidget->setItemWidget(pair.first, pair.second);
	}
}

void MoveItemProxy::destroyEditor(QWidget *editor, const QModelIndex &index) const
{
	// takeItem ±≤ª»√editorœ˙ªŸ
}

