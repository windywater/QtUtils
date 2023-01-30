#include "SpectrumChartView.h"
#include <QToolTip>
#include <QCursor>
#include <QtMath>

SpectrumChartView::SpectrumChartView(QWidget *parent /*= Q_NULLPTR*/)
	:ImageChartView(parent)
{
	clear();
}

SpectrumChartView::~SpectrumChartView()
{
}

void SpectrumChartView::setValues(const QVector<qreal>& xValues, const QVector<qreal>& yValues, const QVector<QVector<qreal>>& specValues)
{
	prepareXYAxes(xValues, yValues);

	m_specValues = specValues;
	determineValueRange();
	drawSpectrumImage(m_specImage, m_specValues);	// 自底向上绘制
	repaintImage();
}

void SpectrumChartView::clear()
{
	m_specImage = QImage();
	m_specValues.clear();
	ImageChartView::clear();
}

void SpectrumChartView::renderImage(QPainter* painter, const QRect& rect)
{
	painter->drawImage(rect, m_specImage);
}

static bool findRange(const QVector<qreal>& values, qreal& min, qreal& max)
{
    if (values.isEmpty())
        return false;

    min = std::numeric_limits<double>::max();
    max = -std::numeric_limits<double>::max();

    for (const auto& v : values)
    {
        min = qMin(min, v);
        max = qMax(max, v);
    }

    return true;
}

static bool findRange(const QVector<QVector<qreal>>& values, qreal& min, qreal& max)
{
    if (values.isEmpty())
        return false;

    min = std::numeric_limits<double>::max();
    max = -std::numeric_limits<double>::max();

    for (const auto& vec : values)
    {
        for (const auto& v : vec)
        {
            min = qMin(min, v);
            max = qMax(max, v);
        }
    }

    return true;
}

static void adjustRange(qreal& min, qreal& max)
{
    qreal oldMin = min;
    qreal oldMax = max;

    qreal grid = (max - min) / 10;
    grid = ImageChartView::keepPrecision(grid, 2);
    min = qFloor(min / grid) * grid;	// qFloor: <=的最大整数
    max = qCeil(max / grid) * grid;		// qCeil: >=的最小整数

    min = ImageChartView::keepPrecision(min, 2, ImageChartView::Floor);
    max = ImageChartView::keepPrecision(max, 2, ImageChartView::Ceil);

    // 防止相等造成比例尺绘制失败，(0.99927, 0.999882) -> (1, 1)
    if (qFuzzyCompare(min, max))
    {
        min -= 1;
        max += 1;
    }
}

void SpectrumChartView::determineValueRange()
{
    findRange(m_specValues, m_minValue, m_maxValue);
    adjustRange(m_minValue, m_maxValue);
	m_legend->setValueRange(m_minValue, m_maxValue);
}

void SpectrumChartView::imageResized(const QSize& size)
{
}

void SpectrumChartView::imageHovered(const QPoint& pos, bool show)
{
	if (m_specValues.isEmpty())
		return;

	if (show)
	{
		QSize imgSize = imageSize();
		int xIndex = (int)(1.0f * pos.x() / imgSize.width() * m_specValues.size());
		int yIndex = (int)((1 - 1.0f * pos.y() / imgSize.height()) * m_specValues[0].size());	// 自底向上，Y要反过来
		qreal value = m_specValues[xIndex][yIndex];
		
		QPoint tipPos = QCursor::pos() + QPoint(10, 20);
		QToolTip::showText(tipPos, QString::number(value), this, QRect(), 5000);
	}
	else
		QToolTip::hideText();
}
