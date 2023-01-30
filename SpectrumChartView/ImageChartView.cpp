#include "ImageChartView.h"
#include <QValueAxis>
#include <QLinearGradient>
#include <QVariantAnimation>
#include <QPainter>
#include <QGraphicsLayout>
#include <QFontMetrics>
#include <QtMath>
#include <QToolTip>
#include <QSet>
#include <QDebug>

ImageChartView::ImageChartView(QWidget *parent)
	: QWidget(parent)
{
	m_chartView = new CommonChartView(this);
	m_chartView->setGrabbedWidget(this);

	m_legend = new SpectrumLegend(this);
	m_legendVisible = true;

	m_minValue = 0;
	m_maxValue = 1;

	m_imgRatioMode = Qt::IgnoreAspectRatio;
	m_imgRatio = 1;

	QMap<float, QColor> initLegendKeyColors = {
		{0,		QColor(62, 38, 168)},
		{0.3f,	QColor(45, 140, 243)},
		{0.5f,	QColor(20, 190, 184)},
		{0.82f, QColor(251, 187, 62)},
		{1,		QColor(249, 251, 21)}
	};
	setLegendKeyColors(initLegendKeyColors);

	m_imageWidget = new ChartImageWidget;
	m_imageWidget->setImageChartView(this);
	m_imageItem = new QGraphicsProxyWidget;
	m_imageItem->setWidget(m_imageWidget);
	m_chartView->scene()->addItem(m_imageItem);
	connect(m_imageWidget, &ChartImageWidget::hovered, this, [=](const QPoint& pos, bool show) {
		imageHovered(pos, show);
	});
	connect(m_imageWidget, &ChartImageWidget::resized, this, [=](const QSize& size) {
		imageResized(size);
	});

	QValueAxis* xAxis = m_chartView->xAxis();
	QValueAxis* yAxis = m_chartView->yAxis();

	// 注意轴上数字的变化也会改变坐标内区域
	connect(m_chartView->chart(), &QChart::geometryChanged, this, &ImageChartView::adjustImageGeometry);
	connect(xAxis, &QValueAxis::rangeChanged, this, &ImageChartView::adjustImageGeometry);
	connect(yAxis, &QValueAxis::rangeChanged, this, &ImageChartView::adjustImageGeometry);

	clear();
}

ImageChartView::~ImageChartView()
{
}

CommonChartView* ImageChartView::chartView()
{
	return m_chartView;
}

void ImageChartView::setLegendVisible(bool visible)
{
	m_legendVisible = visible;
}

void ImageChartView::setLegendKeyColors(const QMap<float, QColor>& colors)
{
	m_legendKeyColors = colors;
	for (auto it = colors.begin(); it != colors.end(); it++)
		m_linearColor.setKeyColor(it.key(), it.value());
	
	QLinearGradient grad = m_linearColor.linearGradient();
	m_legend->setColorGradient(grad);
}

QMap<float, QColor> ImageChartView::legendKeyColors()
{
	return m_legendKeyColors;
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

void ImageChartView::setTitles(const QString& chartTitleText, const QString& chartTitleUnit,
	const QString& xAxisTitleText, const QString& xAxisTitleUnit,
	const QString& yAxisTitleText, const QString& yAxisTitleUnit)
{
	m_chartView->chart()->setTitle(chartTitleText);
    m_chartView->xAxis()->setTitleText(makeTitle(xAxisTitleText, xAxisTitleUnit));
    m_chartView->yAxis()->setTitleText(makeTitle(yAxisTitleText, yAxisTitleUnit));
}

void ImageChartView::prepareXYAxes(const QVector<qreal>& xValues, const QVector<qreal>& yValues)
{
	if (!xValues.isEmpty())
	{
		// 角度只有一个值时，adjustSpecImage取坐标点会出问题，这里简陋修复一下
		m_xValues = xValues;
		QVector<qreal> adjustedValues = m_xValues;
		if (adjustedValues.size() == 1)
			adjustedValues << (adjustedValues.first() + 1);

		m_chartView->xAxis()->setRange(adjustedValues.first(), adjustedValues.back());
	}

	if (!yValues.isEmpty())
	{
		m_yValues = yValues;
		QVector<qreal> adjustedValues = m_yValues;
		if (adjustedValues.size() == 1)
			adjustedValues << (adjustedValues.first() + 1);

		m_chartView->yAxis()->setRange(adjustedValues.first(), adjustedValues.back());
	}
}

void ImageChartView::updateLegend()
{
	m_legend->setValueRange(m_minValue, m_maxValue);
}

void ImageChartView::clear()
{
	m_minValue = 0;
	m_maxValue = 1;
	repaintImage();
	updateLegend();
}

QSize ImageChartView::imageSize()
{
	return m_imageWidget->size();
}

void ImageChartView::setImageAspectRatioMode(Qt::AspectRatioMode mode)
{
	m_imgRatioMode = mode;
}

Qt::AspectRatioMode ImageChartView::imageAspectRatioMode()
{
	return m_imgRatioMode;
}

void ImageChartView::setImageAspectRatio(float ratio)
{
	m_imgRatio = ratio;
}

float ImageChartView::imageAspectRatio()
{
	return m_imgRatio;
}

/*static*/ qreal ImageChartView::keepPrecision(qreal num, int precision, KeepPrecisionMode mode /*= Round*/)
{
    if (precision <= 0 || qFuzzyIsNull(num))
        return num;

    // 不使用stringstream::precision，自己写效率反而高了很多
    qreal result = 0;
    qreal pivotTop = qPow(10, precision);
    qreal pivotBottom = qPow(10, precision - 1);
    qreal mul = 1;
    qreal tmp = num;
    for (int i = 0; i < 20; i++)	// 怕出现精度问题，进入死循环，所以控制一下循环次数
    {
        qreal absTmp = qFabs(tmp);
        if (absTmp >= pivotBottom && absTmp <= pivotTop)
        {
            if (mode == Round)
                tmp = qRound64(tmp);
            else if (mode == Ceil)
                tmp = qCeil(tmp);
            else if (mode == Floor)
                tmp = qFloor(tmp);

            result = tmp / mul;
            break;
        }
        else if (absTmp < pivotBottom)
        {
            tmp *= 10.0;
            mul *= 10.0;
        }
        else if (absTmp > pivotTop)
        {
            tmp /= 10.0;
            mul /= 10.0;
        }
    }

    return result;
}

const static int kLegendSpan = 15;
const static int kLegendWidth = 100;

// 根据可用尺寸，结合宽高比，算出合适的尺寸
QSize ImageChartView::evaluateSize(const QSize& availableSize)
{
	if (m_imgRatioMode != Qt::KeepAspectRatio)
		return availableSize;

	QChart* chart = m_chartView->chart();
	QValueAxis* xAxis = m_chartView->xAxis();
	QValueAxis* yAxis = m_chartView->yAxis();

	// 可用尺寸裁去内边距后，根据宽高比算出图像尺寸，再拼上内边距得到最终尺寸
	QPointF pt0 = chart->mapToPosition(QPointF(xAxis->min(), yAxis->min()));
	QPointF pt1 = chart->mapToPosition(QPointF(xAxis->max(), yAxis->max()));
	int leftPadding = pt0.x();
	int topPadding = pt1.y();
	int rightPadding = m_chartView->width() - pt1.x() + kLegendSpan + kLegendWidth;
	int bottomPadding = m_chartView->height() - pt0.y();

	QSize innerSize(availableSize.width() - leftPadding - rightPadding, 
		availableSize.height() - topPadding - bottomPadding);

	QSize adjustedSize = innerSize;
	if (1.0f * innerSize.width() / innerSize.height() > m_imgRatio)
	{
		// 可用尺寸宽高比大于所要求宽高长，例如要求一个瘦长的比例，保留高度，压缩宽度
		adjustedSize.setWidth(innerSize.height() * m_imgRatio);
	}
	else
	{
		// 可用尺寸宽高比小于所要求宽高长，例如要求一个横向较宽的比例，保留宽度，压缩高度
		adjustedSize.setHeight(innerSize.width() / m_imgRatio);
	}

	adjustedSize += QSize(leftPadding + rightPadding, topPadding + bottomPadding);

	return adjustedSize;
}

void ImageChartView::renderImage(QPainter* painter, const QRect& rect)
{
}

void ImageChartView::determineValueRange()
{
}

void ImageChartView::resizeEvent(QResizeEvent *event)
{
	if (m_legendVisible)
	{
		m_chartView->setGeometry(0, 0, width() - kLegendSpan - kLegendWidth, height());
		m_legend->setGeometry(m_chartView->width() + kLegendSpan, 0, kLegendWidth, height());
	}
	else
	{
		m_chartView->setGeometry(0, 0, event->size().width(), event->size().height());
	}

	QWidget::resizeEvent(event);
}

void ImageChartView::imageResized(const QSize& size)
{
}

void ImageChartView::imageHovered(const QPoint& pos, bool show)
{
}

void ImageChartView::drawSpectrumImage(QImage& image, const QVector<QVector<qreal>>& specValues, DrawSpectrumMode mode /*= BottomToTop*/)
{
	if (specValues.isEmpty())
		return;

	if (specValues[0].isEmpty())
		return;

	int xCount = specValues.size();
	int yCount = specValues[0].size();

	image = QImage(QSize(xCount, yCount), QImage::Format_RGB32);
	for (int x = 0; x < xCount; x++)
	{
		for (int y = 0; y < yCount; y++)
		{
			qreal val = specValues[x][y];
			if (std::isnan(val))
				continue;

			double percent = (val - m_minValue) / (m_maxValue - m_minValue);
			QColor interColor = m_linearColor.interpolatedColor(percent);

			if (mode == BottomToTop)
				image.setPixelColor(x, yCount - y - 1, interColor);
			else
				image.setPixelColor(x, y, interColor);
		}
	}
}

void ImageChartView::repaintImage()
{
	m_imageWidget->repaint();
}

void ImageChartView::adjustImageGeometry()
{
	QChart* chart = m_chartView->chart();
	QValueAxis* xAxis = m_chartView->xAxis();
	QValueAxis* yAxis = m_chartView->yAxis();

	QPointF pt0 = chart->mapToPosition(QPointF(xAxis->min(), yAxis->min()));
	QPointF pt1 = chart->mapToPosition(QPointF(xAxis->max(), yAxis->max()));
	QRectF rc = QRectF(pt0, pt1).normalized();
	m_imageItem->setGeometry(rc);

	if (m_legendVisible)
	{
		m_legend->setTopPadding((int)pt1.y());
		m_legend->setBottomPadding((int)(height() - pt0.y()));
	}
}

//////////////////////////////////////////////////////////////////////////
ChartImageWidget::ChartImageWidget(QWidget *parent /*= Q_NULLPTR*/)
	: QWidget(parent)
{
	m_imageChartView = nullptr;
	setMouseTracking(true);
	
	m_hoverTimer = new QTimer(this);
	m_hoverTimer->setSingleShot(true);
	m_hoverTimer->setInterval(200);
	connect(m_hoverTimer, &QTimer::timeout, this, &ChartImageWidget::onHoverTimeout);
}

ChartImageWidget::~ChartImageWidget()
{
}

void ChartImageWidget::setImageChartView(ImageChartView* chartView)
{
	m_imageChartView = chartView;
}

void ChartImageWidget::resizeEvent(QResizeEvent *event)
{
	emit resized(size());
	QWidget::resizeEvent(event);
}

void ChartImageWidget::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	if (m_imageChartView)
		m_imageChartView->renderImage(&painter, rect());

	QWidget::paintEvent(event);
}

void ChartImageWidget::mouseMoveEvent(QMouseEvent *event)
{
	m_curMousePos = event->pos();
	m_hoverTimer->start();
	emit hovered(m_curMousePos, false);

	QWidget::mouseMoveEvent(event);
}

void ChartImageWidget::leaveEvent(QEvent *event)
{
	m_hoverTimer->stop();
	emit hovered(m_curMousePos, false);

	QWidget::leaveEvent(event);
}

void ChartImageWidget::onHoverTimeout()
{
	// trick: 模态窗口中enter和leave事件进不来，可能和QGraphicsProxyWidget::hoverLeaveEvent实现中的事件分发有关，
	// 所以计时停止时判断鼠标是否悬停在图像上，若没有则视为已移出
	QPoint globalTl = mapToGlobal(QPoint(0, 0));
	QPoint globalBr = mapToGlobal(rect().bottomRight());
	QRect globalRect = QRect(globalTl, globalBr);
	if (globalRect.contains(QCursor::pos()))
		emit hovered(m_curMousePos, true);
	else
		emit hovered(m_curMousePos, false);
}

//////////////////////////////////////////////////////////////////////////
SpectrumLegend::SpectrumLegend(QWidget *parent)
	: QWidget(parent)
{
	m_topPadding = m_bottomPadding = 0;
}

SpectrumLegend::~SpectrumLegend()
{

}

void SpectrumLegend::setValueRange(qreal min, qreal max)
{
	m_min = min;
	m_max = max;
	if (qFuzzyCompare(m_min, m_max))	// 防止出现相等情况
		m_max += 1;

	repaint();
}

void SpectrumLegend::setColorGradient(const QGradient& grad)
{
	m_colorGrad = grad;
	repaint();
}

void SpectrumLegend::setTopPadding(int padding)
{
	m_topPadding = padding;
}

void SpectrumLegend::setBottomPadding(int padding)
{
	m_bottomPadding = padding;
}


static QVector<qreal> determineScales(qreal min, qreal max)
{
	QVector<qreal> scales;
	QSet<qreal> scaleSet;

	const int kScaleCount = 6;
	qreal grid = (max - min) / (kScaleCount - 1);
	for (int i = 0; i < kScaleCount; i++)
	{
		qreal scale = min + i * grid;
        scale = ImageChartView::keepPrecision(scale, 2);
		scales << scale;
		scaleSet.insert(scale);
	}

	if (scaleSet.size() < kScaleCount)
	{
		scales.clear();
		for (int i = 0; i < kScaleCount; i++)
		{
			qreal scale = min + i * grid;
            scale = ImageChartView::keepPrecision(scale, 3);
			scales << scale;
		}
	}

	return scales;
}

int SpectrumLegend::valueToY(qreal value)
{
	int validHeight = height() - m_topPadding - m_bottomPadding;
	int y = validHeight / (m_max - m_min) * (m_max - value) + m_topPadding;

	return y;
}

void SpectrumLegend::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);

	QRect barRect = QRect(0, m_topPadding, 30, height() - m_topPadding - m_bottomPadding);
	QBrush br(m_colorGrad);
	painter.fillRect(barRect, br);
	painter.drawRect(barRect);

	QVector<qreal> scales = determineScales(m_min, m_max);
	for (qreal scale : scales)
	{
		int y = valueToY(scale);
		int x1 = barRect.right();
		int x2 = x1 - 3;
		painter.drawLine(x1, y, x2, y);

		QFontMetrics fm = painter.fontMetrics();
		QRect bounding = fm.boundingRect(QString::number(scale));

		QString scaleText = QString::number(scale);
		painter.drawText(QRect(x1 + 5, y - bounding.height() / 2, 80, bounding.height()), Qt::AlignLeft, QString::number(scale));
	}

	QWidget::paintEvent(event);
}
