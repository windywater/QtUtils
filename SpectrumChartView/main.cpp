#include <QApplication>
#include "SpectrumChartView.h"

int main(int argc, char** argv)
{
    QApplication a(argc, argv);
    SpectrumChartView chartView;
    QVector<qreal> xValues{1, 2, 3};
    QVector<qreal> yValues{40, 50, 60};
    QVector<QVector<qreal>> values{
        { 1, 2, 3 },
        { 4, 5, 6 },
        { 10, 11, 12 }
    };
    chartView.setValues(xValues, yValues, values);

    chartView.resize(1200, 800);
    chartView.show();

    return a.exec();
}
