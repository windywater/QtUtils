#pragma once
#include <QChartView>
#include <QValueAxis>
#include <QMenu>
#include <QLineSeries>
#include <QScatterSeries>

QT_CHARTS_USE_NAMESPACE

class CommonChartView : public QChartView
{
	Q_OBJECT

public:
	CommonChartView(QWidget *parent = Q_NULLPTR);
	~CommonChartView();

	void setTitles(const QString& chartTitleText, const QString& chartTitleUnit, 
		const QString& xAxisTitleText, const QString& xAxisTitleUnit,
		const QString& yAxisTitleText, const QString& yAxisTitleUnit);

	QValueAxis* xAxis();
	QValueAxis* yAxis();

	virtual void clear();
	void appendPoint(qreal x, qreal y);
	virtual void plot(const QVector<qreal>& xValues, const QVector<qreal>& yValues);
	QVector<QPointF> points();
	QVector<qreal> xValues();
	QVector<qreal> yValues();

	void setLineSeriesVisible(bool visible);
	void setScatterSeriesVisible(bool visible);
	void adjustStyle();
	void fitAxesRange();	// 将所有坐标轴的范围调整为合适的值

	void setGrabbedWidget(QWidget* widget);
	QWidget* grabbedWidget();

	void setCustomContextMenu(QMenu* menu);
	QMenu* customContextMenu();

protected:
	void initAxesAndSeries();
	virtual void contextMenuEvent(QContextMenuEvent* event);
	virtual bool viewportEvent(QEvent* event);

	void setDefaultXAxisRange(qreal min, qreal max);
	void setDefaultYAxisRange(qreal min, qreal max);

Q_SIGNALS:
	void pointClicked(const QPointF &point);
	void plotDone();

public Q_SLOTS:
	void showHidePointTip(const QPointF& point, bool state);

protected:
	QLineSeries* m_lineSeries;
	QScatterSeries* m_scatterSeries;
	QWidget* m_grabbedWidget;	// 保存图片时指定的widget
	QMenu* m_customContextMenu;
	QPair<qreal, qreal> m_defaultXAxisRange;
	QPair<qreal, qreal> m_defaultYAxisRange;
};
