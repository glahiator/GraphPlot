#ifndef UNIQUEGRAPH_H
#define UNIQUEGRAPH_H
/*
 * Для построения графиков, в котором строятся на одном
 * чарте 2 линейные серии. *
 * */

#include <QObject>
#include <QtCharts>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

class UniqueGraph : public QObject
{
    Q_OBJECT
public:
    explicit UniqueGraph( QString _title, QObject *parent = nullptr);
    void Configure( QString _yAxisText, QPoint _yRange, int _yTickCount);
    void SetRange(QPoint _yRange);
    void SetTickCount(int _yTick);
    void ChartIncrement_if(bool _isLeft, qreal _leftVal, bool _isRight, qreal _rightValt);
    void ChartIncrement(qreal _leftVal, qreal _rightVal);
    void ChartScroll(qreal plotWidth);
    QChart *chart;

signals:

private:
    QString title;
    QString yAxisText;
    QLineSeries *series_left;
    QLineSeries *series_right;
    QDateTimeAxis *axis_X;
    QValueAxis    *axis_Y;

    QDateTime time;
};

#endif // UNIQUEGRAPH_H
