#include "GraphPlot.h"
#include "ui_GraphPlot.h"

GraphPlot::GraphPlot(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GraphPlot)
{
    ui->setupUi(this);
    counter = 0;
    switcher = false;

    timer = new QTimer();
    timer->stop();
    timer->setInterval(100);
    connect( timer, &QTimer::timeout, this, &GraphPlot::handlePistonPlot );
    connect( timer, &QTimer::timeout, this, &GraphPlot::handleRodPlot );
    connect( timer, &QTimer::timeout, this, &GraphPlot::handleForcePlot );
    //timer->start();

    QDateTime all(QDateTime::currentDateTime().date(), QTime(13,10,0));
    timePiston = timeRod = timeForce = all;



//    connect( ui->btn_debug, &QPushButton::clicked, this, &GraphPlot::DebugSlot );
//    connect( ui->pushButton, &QPushButton::clicked, this, &GraphPlot::StartTimer );

    ui->themeComboBox->addItem("Light", QChart::ChartThemeLight);
    ui->themeComboBox->addItem("Blue Cerulean", QChart::ChartThemeBlueCerulean);
    ui->themeComboBox->addItem("Dark", QChart::ChartThemeDark);
    ui->themeComboBox->addItem("Brown Sand", QChart::ChartThemeBrownSand);
    ui->themeComboBox->addItem("Blue NCS", QChart::ChartThemeBlueNcs);
    ui->themeComboBox->addItem("High Contrast", QChart::ChartThemeHighContrast);
    ui->themeComboBox->addItem("Blue Icy", QChart::ChartThemeBlueIcy);
    ui->themeComboBox->addItem("Qt", QChart::ChartThemeQt);

//    ui->themeComboBox->setCurrentIndex(4);
    connect( ui->themeComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &GraphPlot::updateUI );

    QPalette pal = qApp->palette();
    pal.setColor(QPalette::Window, QRgb(0xf0f0f0));
    pal.setColor(QPalette::WindowText, QRgb(0x404044));
    qApp->setPalette(pal);

    updateUI();
}

GraphPlot::~GraphPlot()
{
    delete ui;
}

void GraphPlot::updateUI()
{
    //![6]
    QChart::ChartTheme theme = static_cast<QChart::ChartTheme>(
                ui->themeComboBox->itemData(ui->themeComboBox->currentIndex()).toInt());
    //![6]

    //![7]
    ui->graphView1->chart()->setTheme(theme);
    ui->graphView2->chart()->setTheme(theme);
    ui->graphView3->chart()->setTheme(theme);
    //![7]


    // Set palette colors based on selected theme
    //![8]
    QPalette pal = window()->palette();
    if (theme == QChart::ChartThemeLight) {
        pal.setColor(QPalette::Window, QRgb(0xf0f0f0));
        pal.setColor(QPalette::WindowText, QRgb(0x404044));
        //![8]
    } else if (theme == QChart::ChartThemeDark) {
        pal.setColor(QPalette::Window, QRgb(0x121218));
        pal.setColor(QPalette::WindowText, QRgb(0xd6d6d6));
    } else if (theme == QChart::ChartThemeBlueCerulean) {
        pal.setColor(QPalette::Window, QRgb(0x40434a));
        pal.setColor(QPalette::WindowText, QRgb(0xd6d6d6));
    } else if (theme == QChart::ChartThemeBrownSand) {
        pal.setColor(QPalette::Window, QRgb(0x9e8965));
        pal.setColor(QPalette::WindowText, QRgb(0x404044));
    } else if (theme == QChart::ChartThemeBlueNcs) {
        pal.setColor(QPalette::Window, QRgb(0x018bba));
        pal.setColor(QPalette::WindowText, QRgb(0x404044));
    } else if (theme == QChart::ChartThemeHighContrast) {
        pal.setColor(QPalette::Window, QRgb(0xffab03));
        pal.setColor(QPalette::WindowText, QRgb(0x181818));
    } else if (theme == QChart::ChartThemeBlueIcy) {
        pal.setColor(QPalette::Window, QRgb(0xcee7f0));
        pal.setColor(QPalette::WindowText, QRgb(0x404044));
    } else {
        pal.setColor(QPalette::Window, QRgb(0xf0f0f0));
        pal.setColor(QPalette::WindowText, QRgb(0x404044));
    }
    window()->setPalette(pal);
}


void GraphPlot::DebugSlot()
{
    timer->stop();
}

void GraphPlot::StartTimer()
{
    timer->start();
}


void GraphPlot::handlePistonPlot()
{
    counter++;

    timePiston = timePiston.addSecs(1);    

    if(counter > 100 ){

        if( !switcher ){
            ui->graphView1->chart()->setTheme(QChart::ChartThemeBlueCerulean);
            switcher = true;
        }
        seriesPistonLeft->append(timePiston.toMSecsSinceEpoch(), 14);
        seriesPistonRight->append(timePiston.toMSecsSinceEpoch(),  QRandomGenerator::global()->bounded(1,10));
    }else{
        seriesPistonLeft->append(timePiston.toMSecsSinceEpoch(), QRandomGenerator::global()->bounded(1,10));
        seriesPistonRight->append(timePiston.toMSecsSinceEpoch(),  QRandomGenerator::global()->bounded(1,10));
    }

    double val = ui->graphView1->chart()->plotArea().width() / ax_X_Piston->tickCount();
//    qDebug() << counter;// ui->graphView1->chart()->plotArea().width() << ax_X_Piston->tickCount() ;
    chartPiston->scroll(131.75, 0);

}

void GraphPlot::handleRodPlot()
{
    timeRod = timeRod.addSecs(1);
    seriesRodLeft->append(timeRod.toMSecsSinceEpoch(),  QRandomGenerator::global()->bounded(1,10));
    seriesRodRight->append(timeRod.toMSecsSinceEpoch(),  QRandomGenerator::global()->bounded(1,10));
    chartRod->scroll(131.75, 0);
}

void GraphPlot::handleForcePlot()
{
    timeForce = timeForce.addSecs(1);
    seriesForceLeft->append(timeForce.toMSecsSinceEpoch(),  QRandomGenerator::global()->bounded(1,10));
    seriesForceRight->append(timeForce.toMSecsSinceEpoch(),  QRandomGenerator::global()->bounded(1,10));
    chartForce->scroll(131.75, 0);
}

void GraphPlot::SetGraphPiston()
{
    seriesPistonLeft = new QLineSeries();
//    QPen green(Qt::red);
//    green.setWidth(1);
//    series->setPen(green);
    seriesPistonLeft->setName("Левый");

    seriesPistonRight = new QLineSeries();
    seriesPistonRight->setName("Правый");;


    QDateTime temp_time = timePiston;

    seriesPistonLeft->append(timePiston.toMSecsSinceEpoch(), 2);
    seriesPistonRight->append(timePiston.toMSecsSinceEpoch(), 1);
    timePiston = timePiston.addSecs(1);
    seriesPistonLeft->append(timePiston.toMSecsSinceEpoch(), 4);
    seriesPistonRight->append(timePiston.toMSecsSinceEpoch(), 5);
    timePiston = timePiston.addSecs(1);
    seriesPistonLeft->append(timePiston.toMSecsSinceEpoch(), 8);
    seriesPistonRight->append(timePiston.toMSecsSinceEpoch(), 7);
    timePiston = timePiston.addSecs(1);
    seriesPistonLeft->append(timePiston.toMSecsSinceEpoch(), 4);
    seriesPistonRight->append(timePiston.toMSecsSinceEpoch(), 5);
    timePiston = timePiston.addSecs(1);
    seriesPistonLeft->append(timePiston.toMSecsSinceEpoch(), 5);
    seriesPistonRight->append(timePiston.toMSecsSinceEpoch(), 6);

    timePiston = timePiston.addSecs(1);
    seriesPistonLeft->append(timePiston.toMSecsSinceEpoch(), 7);
    seriesPistonRight->append(timePiston.toMSecsSinceEpoch(), 7);

    ax_X_Piston = new QDateTimeAxis;
    ax_Y_Piston = new QValueAxis;
    ax_X_Piston->setTitleText("Время, мин");
    ax_X_Piston->setFormat("hh:mm:ss.z");
    ax_Y_Piston->setTitleText("Потери, %");

    chartPiston = new QChart();
    chartPiston->legend()->setAlignment(Qt::AlignRight);
    chartPiston->legend()->show();
    chartPiston->addSeries(seriesPistonLeft);
    chartPiston->addSeries(seriesPistonRight);
    chartPiston->addAxis(ax_X_Piston, Qt::AlignBottom);
    chartPiston->addAxis(ax_Y_Piston, Qt::AlignLeft);
//    chart->setAnimationOptions(QChart::SeriesAnimations);


    seriesPistonLeft->attachAxis( ax_X_Piston );
    seriesPistonLeft->attachAxis( ax_Y_Piston );
    seriesPistonRight->attachAxis( ax_X_Piston );
    seriesPistonRight->attachAxis( ax_Y_Piston );


    QDateTime temp_time_finish = timePiston.addSecs(1);
//    temp_time_finish.setDate(timePiston.date());
//    temp_time_finish.setTime(QTime(13,10,6));

    ax_X_Piston->setRange(temp_time, temp_time_finish);
    ax_Y_Piston->setRange(0, 15);
    ax_X_Piston->setTickCount(7);
    ax_Y_Piston->setTickCount(4);
    chartPiston->setTitle("Процент потерь расхода в поршневой полости");

    ui->graphView1->setChart(chartPiston);
    ui->graphView1->setRenderHint(QPainter::Antialiasing);
    ui->graphView1->setRubberBand( QChartView::RectangleRubberBand );
}

void GraphPlot::SetGraphRod()
{
    seriesRodLeft = new QLineSeries();
    //    QPen green(Qt::red);
    //    green.setWidth(1);
    //    series->setPen(green);
    seriesRodLeft->setName("Левый");

    seriesRodRight = new QSplineSeries();
    seriesRodRight->setName("Правый");;


    QDateTime temp_time = timeRod;

    seriesRodLeft->append(timeRod.toMSecsSinceEpoch(), 2);
    seriesRodRight->append(timeRod.toMSecsSinceEpoch(), 1);
    timeRod.setTime(QTime(13,10,1));
    seriesRodLeft->append(timeRod.toMSecsSinceEpoch(), 4);
    seriesRodRight->append(timeRod.toMSecsSinceEpoch(), 5);
    timeRod.setTime(QTime(13,10,2));
    seriesRodLeft->append(timeRod.toMSecsSinceEpoch(), 8);
    seriesRodRight->append(timeRod.toMSecsSinceEpoch(), 7);
    timeRod.setTime(QTime(13,10,3));
    seriesRodLeft->append(timeRod.toMSecsSinceEpoch(), 4);
    seriesRodRight->append(timeRod.toMSecsSinceEpoch(), 5);
    timeRod.setTime(QTime(13,10,4));
    seriesRodLeft->append(timeRod.toMSecsSinceEpoch(), 5);
    seriesRodRight->append(timeRod.toMSecsSinceEpoch(), 6);

    timeRod.setTime(QTime(13,10,5));
    prev_x = timeRod.toMSecsSinceEpoch();
    prev_y = 6;
    seriesRodLeft->append(prev_x, prev_y);
    seriesRodRight->append(prev_x, prev_y);

    ax_X_Rod = new QDateTimeAxis;
    ax_Y_Rod = new QValueAxis;
    ax_X_Rod->setTitleText("Время, мин");
    ax_X_Rod->setFormat("hh:mm:ss.z");
    ax_Y_Rod->setTitleText("Потери, %");

    chartRod = new QChart();
    chartRod->legend()->setAlignment(Qt::AlignRight);
    chartRod->legend()->show();
    chartRod->addSeries(seriesRodLeft);
    chartRod->addSeries(seriesRodRight);
    chartRod->addAxis(ax_X_Rod, Qt::AlignBottom);
    chartRod->addAxis(ax_Y_Rod, Qt::AlignLeft);
    //    chart->setAnimationOptions(QChart::SeriesAnimations);

    seriesRodLeft->attachAxis( ax_X_Rod );
    seriesRodLeft->attachAxis( ax_Y_Rod );
    seriesRodRight->attachAxis( ax_X_Rod );
    seriesRodRight->attachAxis( ax_Y_Rod );

    QDateTime temp_time_finish;
    temp_time_finish.setDate(QDateTime::currentDateTime().date());
    temp_time_finish.setTime(QTime(13,10,6));

    ax_X_Rod->setRange(temp_time, temp_time_finish);
    ax_Y_Rod->setRange(0, 15);
    ax_X_Rod->setTickCount(7);
    ax_Y_Rod->setTickCount(4);
    chartRod->setTitle("Процент потерь расхода в штоковой полости");

    ui->graphView2->setChart(chartRod);
    ui->graphView2->setRenderHint(QPainter::Antialiasing);
    ui->graphView2->setRubberBand( QChartView::RectangleRubberBand );
}

void GraphPlot::SetGraphForce()
{

    seriesForceLeft = new QLineSeries();
    //    QPen green(Qt::red);
    //    green.setWidth(1);
    //    series->setPen(green);
    seriesForceLeft->setName("Левый");

    seriesForceRight = new QSplineSeries();
    seriesForceRight->setName("Правый");;


    QDateTime temp_time = timeForce;

    seriesForceLeft->append(timeForce.toMSecsSinceEpoch(), 2);
    seriesForceRight->append(timeForce.toMSecsSinceEpoch(), 1);
    timeForce.setTime(QTime(13,10,1));
    seriesForceLeft->append(timeForce.toMSecsSinceEpoch(), 4);
    seriesForceRight->append(timeForce.toMSecsSinceEpoch(), 5);
    timeForce.setTime(QTime(13,10,2));
    seriesForceLeft->append(timeForce.toMSecsSinceEpoch(), 8);
    seriesForceRight->append(timeForce.toMSecsSinceEpoch(), 7);
    timeForce.setTime(QTime(13,10,3));
    seriesForceLeft->append(timeForce.toMSecsSinceEpoch(), 4);
    seriesForceRight->append(timeForce.toMSecsSinceEpoch(), 5);
    timeForce.setTime(QTime(13,10,4));
    seriesForceLeft->append(timeForce.toMSecsSinceEpoch(), 5);
    seriesForceRight->append(timeForce.toMSecsSinceEpoch(), 6);

    timeForce.setTime(QTime(13,10,5));
    prev_x = timeForce.toMSecsSinceEpoch();
    prev_y = 6;
    seriesForceLeft->append(prev_x, prev_y);
    seriesForceRight->append(prev_x, prev_y);

    ax_X_Force = new QDateTimeAxis;
    ax_Y_Force = new QValueAxis;
    ax_X_Force->setTitleText("Время, мин");
    ax_X_Force->setFormat("hh:mm:ss.z");
    ax_Y_Force->setTitleText("Отношение");

    chartForce = new QChart();
    chartForce->legend()->setAlignment(Qt::AlignRight);
    chartForce->legend()->show();
    chartForce->addSeries(seriesForceLeft);
    chartForce->addSeries(seriesForceRight);
    chartForce->addAxis(ax_X_Force, Qt::AlignBottom);
    chartForce->addAxis(ax_Y_Force, Qt::AlignLeft);
    //    chart->setAnimationOptions(QChart::SeriesAnimations);


    seriesForceLeft->attachAxis( ax_X_Force );
    seriesForceLeft->attachAxis( ax_Y_Force );
    seriesForceRight->attachAxis( ax_X_Force );
    seriesForceRight->attachAxis( ax_Y_Force );


    QDateTime temp_time_finish;
    temp_time_finish.setDate(QDateTime::currentDateTime().date());
    temp_time_finish.setTime(QTime(13,10,6));

    ax_X_Force->setRange(temp_time, temp_time_finish);
    ax_Y_Force->setRange(0, 20);
    ax_X_Force->setTickCount(7);
    ax_Y_Force->setTickCount(4);
    chartForce->setTitle("Отношение эталонного дифф. усилия к реальному");

    ui->graphView3->setChart(chartForce);
    ui->graphView3->setRenderHint(QPainter::Antialiasing);
    ui->graphView3->setRubberBand( QChartView::RectangleRubberBand );
}


void GraphPlot::on_action_13_triggered()
{
    static bool sw = false;
    if( !sw ){
        SetGraphPiston();
        SetGraphRod();
        SetGraphForce();
        sw = true;
    }

    timer->start();
}


void GraphPlot::on_action_14_triggered()
{
    timer->stop();
    counter = 0;
    switcher = false;
}

