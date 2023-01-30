#include "CommonChartView.h"
#include <QChart>
#include <QGraphicsLayout>
#include <QValueAxis>
#include <QMenu>
#include <QClipboard>
#include <QApplication>
#include <QFileDialog>
#include <QToolTip>
#include <QCursor>
#include <QSignalBlocker>

CommonChartView::CommonChartView(QWidget *parent /*= Q_NULLPTR*/)
	: QChartView(parent)
{
	setRenderHint(QPainter::Antialiasing);

	m_customContextMenu = nullptr;
	m_grabbedWidget = this;
	QChart* chart = this->chart();
	chart->layout()->setContentsMargins(0, 0, 0, 0);
	chart->setMargins(QMargins(0, 0, 0, 0));
	chart->legend()->hide();

	initAxesAndSeries();
	adjustStyle();
}

CommonChartView::~CommonChartView()
{
}

static QString makeTitle(const QString& titleText, const QString& titleUnit)
{
    if (titleText.isEmpty() && titleUnit.isEmpty())
        return "";

    QString text;
    if (titleUnit.isEmpty())
        text = titleText;
    else
        text = QString("%1 [%2]").arg(titleText, titleUnit);

    return text;
}

void CommonChartView::setTitles(const QString& chartTitleText, const QString& chartTitleUnit,
	const QString& xAxisTitleText, const QString& xAxisTitleUnit,
	const QString& yAxisTitleText, const QString& yAxisTitleUnit)
{
	QChart* chart = this->chart();
	chart->setTitle(chartTitleText);
    xAxis()->setTitleText(makeTitle(xAxisTitleText, xAxisTitleUnit));
    yAxis()->setTitleText(makeTitle(yAxisTitleText, yAxisTitleUnit));
}

QValueAxis* CommonChartView::xAxis()
{
	QList<QAbstractAxis*> axes = chart()->axes(Qt::Horizontal);
	if (axes.isEmpty())
		return Q_NULLPTR;

	return qobject_cast<QValueAxis*>(axes.first());
}

QValueAxis* CommonChartView::yAxis()
{
	QList<QAbstractAxis*> axes = chart()->axes(Qt::Vertical);
	if (axes.isEmpty())
		return Q_NULLPTR;

	return qobject_cast<QValueAxis*>(axes.first());
}

void CommonChartView::initAxesAndSeries()
{
	QChart* chart = this->chart();
	QValueAxis* xAxis = new QValueAxis;
	QValueAxis* yAxis = new QValueAxis;
	chart->addAxis(xAxis, Qt::AlignBottom);
	chart->addAxis(yAxis, Qt::AlignLeft);
	xAxis->setTickCount(11);
	yAxis->setTickCount(11);

	m_lineSeries = new QLineSeries();
	m_scatterSeries = new QScatterSeries();
	connect(m_scatterSeries, &QScatterSeries::hovered, this, &CommonChartView::showHidePointTip);
	connect(m_scatterSeries, &QScatterSeries::clicked, this, &CommonChartView::pointClicked);

	QPen pen(QBrush(QColor(153, 202, 83)), 2);
	m_lineSeries->setPen(pen);
	m_scatterSeries->setColor(QColor(246, 166, 37));
	m_scatterSeries->setBorderColor(Qt::transparent);
	m_scatterSeries->setMarkerSize(10);

	chart->addSeries(m_lineSeries);
	chart->addSeries(m_scatterSeries);

	// ����ҪattachAxis��������������ʾ������
	m_lineSeries->attachAxis(xAxis);
	m_lineSeries->attachAxis(yAxis);
	m_scatterSeries->attachAxis(xAxis);
	m_scatterSeries->attachAxis(yAxis);
}

void CommonChartView::setLineSeriesVisible(bool visible)
{
	m_lineSeries->setVisible(visible);
}

void CommonChartView::setScatterSeriesVisible(bool visible)
{
	m_scatterSeries->setVisible(visible);
}

void CommonChartView::adjustStyle()
{
	// ͳһ����
	static const QString kFontFamily = "Microsoft Yahei";

	QChart* chart = this->chart();
	QFont font = chart->titleFont();
	font.setFamily(kFontFamily);
	chart->setTitleFont(font);

	QList<QAbstractAxis*> axes = chart->axes();
	for (QAbstractAxis* axis : axes)
	{
		font = axis->titleFont();
		font.setFamily(kFontFamily);
		axis->setTitleFont(font);

		font = axis->labelsFont();
		font.setFamily(kFontFamily);
		axis->setLabelsFont(font);
	}

	QLegend* legend = chart->legend();
	font = legend->font();
	font.setFamily(kFontFamily);
	legend->setFont(font);
}

void CommonChartView::clear()
{
	m_lineSeries->clear();
	m_scatterSeries->clear();
}

void CommonChartView::setGrabbedWidget(QWidget* widget)
{
	m_grabbedWidget = widget;
}

QWidget* CommonChartView::grabbedWidget()
{
	return m_grabbedWidget;
}

void CommonChartView::setCustomContextMenu(QMenu* menu)
{
	m_customContextMenu = menu;
}

QMenu* CommonChartView::customContextMenu()
{
	return m_customContextMenu;
}

void CommonChartView::appendPoint(qreal x, qreal y)
{
	m_lineSeries->append(x, y);
	m_scatterSeries->append(x, y);
}

void CommonChartView::plot(const QVector<qreal>& xValues, const QVector<qreal>& yValues)
{
	if (xValues.size() != yValues.size())
		return;

	clear();

    // TODO: ��Ӵ������ݻ�������Կ��٣���ʱ����series�źŷ��Ϳ��Խ��
	QSignalBlocker b1(m_lineSeries);
	QSignalBlocker b2(m_scatterSeries);

	for (int i = 0; i < xValues.size(); i++)
		appendPoint(xValues.at(i), yValues.at(i));

	fitAxesRange();
	setDefaultXAxisRange(xAxis()->min(), xAxis()->max());
	setDefaultYAxisRange(yAxis()->min(), yAxis()->max());

	emit plotDone();
}

QVector<QPointF> CommonChartView::points()
{
	return m_lineSeries->pointsVector();
}

QVector<qreal> CommonChartView::xValues()
{
	QVector<qreal> values;
	QVector<QPointF> pts = m_lineSeries->pointsVector();
	for (const auto& pt : pts)
		values << pt.x();

	return values;
}

QVector<qreal> CommonChartView::yValues()
{
	QVector<qreal> values;
	QVector<QPointF> pts = m_lineSeries->pointsVector();
	for (const auto& pt : pts)
		values << pt.y();

	return values;
}

void CommonChartView::showHidePointTip(const QPointF& point, bool state)
{
	QScatterSeries* series = qobject_cast<QScatterSeries*>(sender());

	QString tip;
	QString seriesName = series->name();
	if (seriesName.isEmpty())
		tip = QString("(%1, %2)").arg(point.x()).arg(point.y());
	else // �����������ʾ
		tip = QString("%1 (%2, %3)").arg(seriesName).arg(point.x()).arg(point.y());

	if (state)
	{
		QPoint pos = QCursor::pos() + QPoint(10, 20);
		QToolTip::showText(pos, tip, this, QRect(), 5000);
	}
	else
		QToolTip::hideText();
}

void CommonChartView::fitAxesRange()
{
	QMap<QAbstractAxis*, QVector<QPointF>> axisPointsMap;

	// �ҵ�ÿ���������������ĵ�
	QChart* chart = this->chart();
	QList<QAbstractSeries*> series = chart->series();
	for (QAbstractSeries* ser : series)
	{
		QXYSeries* xySer = qobject_cast<QXYSeries*>(ser);
		QList<QAbstractAxis*> attAxes = xySer->attachedAxes();
		for (QAbstractAxis* axis : attAxes)
			axisPointsMap[axis].append(xySer->pointsVector());
	}

	// ���ݺ������������㣬�ҵ���Χ
	for (QAbstractAxis* axis : axisPointsMap.keys())
	{
		QValueAxis* vAxis = qobject_cast<QValueAxis*>(axis);
		const QVector<QPointF>& points = axisPointsMap[vAxis];

		qreal min = std::numeric_limits<double>::max();
		qreal max = -std::numeric_limits<double>::max();
		if (vAxis->orientation() == Qt::Horizontal)
		{
			for (const QPointF& pt : points)
			{
				min = qMin(pt.x(), min);
				max = qMax(pt.x(), max);
			}
		}
		else
		{
			for (const QPointF& pt : points)
			{
				min = qMin(pt.y(), min);
				max = qMax(pt.y(), max);
			}
		}

		// ����min==max�������ע�⣺��qFuzzyCompare(min, max)���ɿ�
		if (qFuzzyIsNull(max-min))
			min -= 1, max += 1;

		// ����Ƕ�̬�̶ȣ���ֹ��̫��̶�����ɿ���
		if (vAxis->tickType() == QValueAxis::TicksDynamic)
		{
			qreal oldInterval = vAxis->tickInterval();
			qreal sugInterval = (max - min) / 10.0;
			if (oldInterval < sugInterval)
				vAxis->setTickInterval((max - min) / 10);
		}

		vAxis->setRange(min, max);
	}
}

void CommonChartView::setDefaultXAxisRange(qreal min, qreal max)
{
	m_defaultXAxisRange = qMakePair(min, max);
}

void CommonChartView::setDefaultYAxisRange(qreal min, qreal max)
{
	m_defaultYAxisRange = qMakePair(min, max);
}

void CommonChartView::contextMenuEvent(QContextMenuEvent* event)
{
	// ����ʹ���Զ���˵�
	if (m_customContextMenu)
	{
		m_customContextMenu->exec(event->globalPos());
		return;
	}

	QMenu menu(this);
	QAction* copyImgAct = new QAction(tr("Copy image to clipboard"), &menu);
	QAction* saveImgAct = new QAction(tr("Save image to file"), &menu);

	menu.addAction(copyImgAct);
	menu.addAction(saveImgAct);

	// ͼ�񱣴浽������
	connect(copyImgAct, &QAction::triggered, this, [=]() {
		if (!m_grabbedWidget)
			return;

		QPixmap pic = m_grabbedWidget->grab();
		QClipboard* clipboard = QApplication::clipboard();
		clipboard->setPixmap(pic);
	});

	// ͼ�񱣴��ļ�
	connect(saveImgAct, &QAction::triggered, this, [=]() {
		if (!m_grabbedWidget)
			return;

		QPixmap pic = m_grabbedWidget->grab();
		QString file = QFileDialog::getSaveFileName(this, "", "", tr("Image file (*.png *.jpg *.jpeg *.bmp);;All files (*.*)"));
		if (file.isEmpty())
			return;

		pic.save(file);
	});

	menu.exec(event->globalPos());

	QChartView::contextMenuEvent(event);
}

bool CommonChartView::viewportEvent(QEvent* event)
{
	// Ҫ����Ĭ�ϵ�ToolTip�¼��������̣�������������ͣ��ʱ����QGraphicsSceneHelpEvent��
	// ��ʾtool tip����ʹΪ�գ��������Ļ��Զ����QToolTip::showText�ܿ����ʧ
	if (event->type() == QEvent::ToolTip)
		return true;

	return QChartView::viewportEvent(event);
}
