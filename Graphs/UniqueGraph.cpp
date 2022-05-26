#include "UniqueGraph.h"

UniqueGraph::UniqueGraph(QString _title, QObject *parent) : QObject(parent)
{

    title = _title;
}

void UniqueGraph::Configure(QString _yAxisText , QPoint _yRange, int _yTickCount)
{
    QDateTime all(QDateTime::currentDateTime().date(), QDateTime::currentDateTime().time().addSecs(-10));
    time =  all;
    yAxisText = _yAxisText;
    series_left = new QLineSeries();
    series_left->setName("Левый");

    series_right = new QLineSeries();
    series_right->setName("Правый");

    QDateTime temp_time = time;
    series_left->append(time.toMSecsSinceEpoch(), 0);
    series_right->append(time.toMSecsSinceEpoch(), 0);

    for( int i = 1; i <= 10; i++ ){
        time = time.addSecs(1);
        series_left->append(time.toMSecsSinceEpoch(), 0);
        series_right->append(time.toMSecsSinceEpoch(), 0);
    }

    axis_X = new QDateTimeAxis;
    axis_Y = new QValueAxis;
    axis_X->setTitleText("Время, сек");
    axis_X->setFormat("hh:mm:ss");
    axis_Y->setTitleText(yAxisText);

    chart = new QChart();
//    chart_fT->legend()->setAlignment(Qt::AlignRight);
    chart->legend()->hide();
    chart->addSeries(series_left);
    chart->addSeries(series_right);
    chart->addAxis(axis_X, Qt::AlignBottom);
    chart->addAxis(axis_Y, Qt::AlignLeft);
    chart->setAnimationOptions(QChart::SeriesAnimations);

    series_left->attachAxis( axis_X );
    series_left->attachAxis( axis_Y );

    series_right->attachAxis( axis_X );
    series_right->attachAxis( axis_Y );

    axis_X->setRange(temp_time, time.addSecs(1));
//    axis_Y->setRange(0, 100);
//    axis_Y->setRange(_yRange.x(), _yRange.y());
    SetRange( _yRange );

    axis_X->setTickCount(10);
//    axis_Y->setTickCount(11);
//    axis_Y->setTickCount(_yTickCount);
    SetTickCount(_yTickCount);
    chart->setTitle(title);
}

void UniqueGraph::SetRange(QPoint _yRange)
{
    axis_Y->setRange(_yRange.x(), _yRange.y());
}

void UniqueGraph::SetTickCount(int _yTick)
{
    axis_Y->setTickCount(_yTick);
}

void UniqueGraph::ChartIncrement_if(bool _isLeft, qreal _leftVal, bool _isRight, qreal _rightVal )
{
    if( _isLeft )  series_left->append(  time.toMSecsSinceEpoch(),  _leftVal);
    if( _isRight ) series_right->append( time.toMSecsSinceEpoch(),  _rightVal);
}

void UniqueGraph::ChartIncrement(qreal _leftVal, qreal _rightVal)
{
    series_left->append(  time.toMSecsSinceEpoch(),  _leftVal);
    series_right->append( time.toMSecsSinceEpoch(),  _rightVal);
}

void UniqueGraph::ChartScroll(qreal plotWidth)
{
    qreal x_sens_fT = plotWidth / axis_X->tickCount();
    qreal y_sens_fT = (axis_X->max().toMSecsSinceEpoch() - axis_X->min().toMSecsSinceEpoch()) / axis_X->tickCount();
    time = time.addMSecs(y_sens_fT);
    chart->scroll(x_sens_fT, 0);
}
