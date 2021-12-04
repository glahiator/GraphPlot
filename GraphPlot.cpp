#include "GraphPlot.h"
#include "ui_GraphPlot.h"

GraphPlot::GraphPlot(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GraphPlot)
{
    ui->setupUi(this);

    timer = new QTimer();
    timer->stop();
    timer->setInterval(100);
    connect( timer, &QTimer::timeout, this, &GraphPlot::timerHandler );
    timer->start();

    series = new QLineSeries();
//    QPen green(Qt::red);
//    green.setWidth(1);
//    series->setPen(green);
    series->append(0, 6);
    series->append(2, 4);
    series->append(3, 8);
    series->append(7, 4);
    series->append(10, 5);
    *series << QPointF(11, 1) << QPointF(13, 3) << QPointF(17, 6) << QPointF(18, 3) << QPointF(20, 2);

    series1 = new QSplineSeries();
    series1->append(0, 6);
    series1->append(2, 4);
    series1->append(3, 8);
    series1->append(7, 4);
    series1->append(10, 5);
    *series1 << QPointF(11, 1) << QPointF(13, 3)
             << QPointF(17, 6) << QPointF(18, 3) << QPointF(20, 2);

    prev_x = 20;
    prev_y = 2;
    series->append(prev_x, prev_y);
    series1->append(prev_x, prev_y);

    ax_X = new QValueAxis;
    ax_Y = new QValueAxis;

    chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    chart->addSeries(series1);
    chart->addAxis(ax_X, Qt::AlignBottom);
    chart->addAxis(ax_Y, Qt::AlignLeft);
//    chart->setAnimationOptions(QChart::SeriesAnimations);


    series->attachAxis( ax_X );
    series->attachAxis( ax_Y );
    series1->attachAxis( ax_X );
    series1->attachAxis( ax_Y );

    ax_X->setRange(0, 25);
    ax_Y->setRange(-10, 10);
    ax_X->setTickCount(11);
    ax_Y->setTickCount(10);
    chart->setTitle("Simple line chart example");

    ui->graphicsView->setChart(chart);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setRubberBand( QChartView::RectangleRubberBand );

    connect( ui->btn_debug, &QPushButton::clicked, this, &GraphPlot::DebugSlot );


}

GraphPlot::~GraphPlot()
{
    delete ui;
}

void GraphPlot::DebugSlot()
{
    timer->stop();
}

void GraphPlot::timerHandler()
{
    static int counter = 0;
    counter++;

    qreal x = chart->plotArea().width() / ax_X->tickCount();
    qreal y = (ax_X->max() - ax_X->min()) / ax_X->tickCount();
    prev_x += y;
    prev_y = QRandomGenerator::global()->bounded(10) - 2.5;
    series->append(prev_x, prev_y);
    series1->append(prev_x, prev_y);
    chart->scroll(x, 0);

}

