#pragma once
#include <QVariantAnimation>
#include <QLinearGradient>
#include <QMap>

class LinearColor
{
public:
	LinearColor();
	~LinearColor();

	enum GradientDirection
	{
		BottomToTop,
		TopToBottom
	};

	void setKeyColor(float percent, const QColor& color);
	QColor interpolatedColor(float percent);
	QLinearGradient linearGradient(GradientDirection dir = BottomToTop);

protected:
	QVariantAnimation m_varAni;
	QMap<float, QColor> m_keyColors;
};