#pragma once
#include "ImageChartView.h"

class SpectrumChartView : public ImageChartView
{
	Q_OBJECT
public:
	SpectrumChartView(QWidget *parent = Q_NULLPTR);
	~SpectrumChartView();

	virtual void clear();
	void setValues(const QVector<qreal>& xValues, const QVector<qreal>& yValues, const QVector<QVector<qreal>>& specValues);

protected:
	virtual void renderImage(QPainter* painter, const QRect& rect);
	virtual void determineValueRange();
	virtual void imageResized(const QSize& size);
	virtual void imageHovered(const QPoint& pos, bool show);

protected:
	QVector<QVector<qreal>> m_specValues;
	QImage m_specImage;
};
