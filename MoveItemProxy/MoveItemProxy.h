#pragma once

#include <QItemDelegate>
#include <QListWidget>

class MoveItemProxy : public QItemDelegate
{
	Q_OBJECT

public:
	MoveItemProxy(QObject *parent);
	~MoveItemProxy();

	void attachListWidget(QListWidget* listWidget);
	void moveItem(int oldRow, int newRow);
	void reverse();

protected:
	virtual void destroyEditor(QWidget *editor, const QModelIndex &index) const;

protected:
	QListWidget* m_listWidget;
};
