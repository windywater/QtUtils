#include "LinearColor.h"

const static int kAniDuration = 100;

LinearColor::LinearColor()
{
	m_varAni.setEasingCurve(QEasingCurve::Linear);
	m_varAni.setDuration(kAniDuration);
}

LinearColor::~LinearColor()
{

}

void LinearColor::setKeyColor(float percent, const QColor& color)
{
	m_keyColors[percent] = color;
	m_varAni.setKeyValueAt(percent, color);
}

QColor LinearColor::interpolatedColor(float percent)
{
	if (percent < 0 || percent > 1)
		return QColor();

	m_varAni.setCurrentTime(kAniDuration * percent);
	QColor interColor = m_varAni.currentValue().value<QColor>();

	return interColor;
}

QLinearGradient LinearColor::linearGradient(GradientDirection dir /*= BottomToTop*/)
{
	QLinearGradient grad;
	grad.setCoordinateMode(QGradient::ObjectBoundingMode);

	QList<float> keys = m_keyColors.keys();
	for (float percent : keys)
		grad.setColorAt(percent, m_keyColors.value(percent));

	if (dir == BottomToTop)
	{
		grad.setStart(0, 1);
		grad.setFinalStop(0, 0);
	}
	else if (dir == TopToBottom)
	{
		grad.setStart(0, 0);
		grad.setFinalStop(0, 1);
	}

	return grad;
}