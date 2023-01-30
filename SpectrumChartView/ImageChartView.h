#pragma once
#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QLegend>
#include <QGraphicsProxyWidget>
#include <QTimer>
#include <QMap>
#include "LinearColor.h"
#include "CommonChartView.h"

class ChartImageWidget;
class SpectrumLegend;

class ImageChartView : public QWidget
{
	Q_OBJECT
public:
	ImageChartView(QWidget *parent);
	~ImageChartView();

	CommonChartView* chartView();
	void setLegendVisible(bool visible);
	void setLegendKeyColors(const QMap<float, QColor>& colors);
	QMap<float, QColor> legendKeyColors();
	void setTitles(const QString& chartTitleText, const QString& chartTitleUnit,
		const QString& xAxisTitleText, const QString& xAxisTitleUnit,
		const QString& yAxisTitleText, const QString& yAxisTitleUnit);
	virtual void clear();

	QSize imageSize();
	void setImageAspectRatioMode(Qt::AspectRatioMode mode);
	Qt::AspectRatioMode imageAspectRatioMode();
	void setImageAspectRatio(float ratio);
	float imageAspectRatio();
	QSize evaluateSize(const QSize& availableSize);

    enum KeepPrecisionMode
    {
        Round,
        Ceil,
        Floor
    };
    static qreal keepPrecision(qreal num, int precision, KeepPrecisionMode mode = Round);

protected:
	void prepareXYAxes(const QVector<qreal>& xValues, const QVector<qreal>& yValues);
	void updateLegend();

	virtual void resizeEvent(QResizeEvent *event);
	virtual void renderImage(QPainter* painter, const QRect& rect);
	virtual void determineValueRange();
	virtual void imageResized(const QSize& size);
	virtual void imageHovered(const QPoint& pos, bool show);

	enum DrawSpectrumMode
	{
		BottomToTop,
		TopToBottom
	};
	void drawSpectrumImage(QImage& image, const QVector<QVector<qreal>>& specValues, DrawSpectrumMode mode = BottomToTop);
	void repaintImage();

protected Q_SLOTS:
	void adjustImageGeometry();

Q_SIGNALS:
	void imageHoverd(const QPoint& pos, bool show);

protected:
	CommonChartView* m_chartView;
	LinearColor m_linearColor;
	QMap<float, QColor> m_legendKeyColors;
	ChartImageWidget* m_imageWidget;
	SpectrumLegend* m_legend;
	bool m_legendVisible;
	QGraphicsProxyWidget* m_imageItem;
	QVector<qreal> m_xValues;
	QVector<qreal> m_yValues;
	qreal m_minValue;
	qreal m_maxValue;
	Qt::AspectRatioMode m_imgRatioMode;
	float m_imgRatio;

	friend class ChartImageWidget;
};

class ChartImageWidget : public QWidget
{
	Q_OBJECT
public:
	ChartImageWidget(QWidget *parent = Q_NULLPTR);
	~ChartImageWidget();
	void setImageChartView(ImageChartView* chartView);

protected:
	virtual void resizeEvent(QResizeEvent *event);
	virtual void paintEvent(QPaintEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void leaveEvent(QEvent *event);

protected Q_SLOTS:
	void onHoverTimeout();

Q_SIGNALS:
	void resized(const QSize& size);
	void hovered(const QPoint& pos, bool show);

protected:
	ImageChartView* m_imageChartView;
	QTimer* m_hoverTimer;
	QPoint m_curMousePos;
};

class SpectrumLegend : public QWidget
{
	Q_OBJECT
public:
	SpectrumLegend(QWidget *parent);
	~SpectrumLegend();

	void setValueRange(qreal min, qreal max);
	void setColorGradient(const QGradient& grad);
	void setTopPadding(int padding);
	void setBottomPadding(int padding);

protected:
	virtual void paintEvent(QPaintEvent *event);
	int valueToY(qreal value);

protected:
	qreal m_min;
	qreal m_max;
	QGradient m_colorGrad;
	int m_topPadding;
	int m_bottomPadding;
};
