#include "GraphPlot.h"
#include "ui_GraphPlot.h"

GraphPlot::GraphPlot(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GraphPlot)
{
    ui->setupUi(this);
    counter = 0;
    switcher = false;
    is_Check = false;

    is_new_piston = false;
    is_new_rod = false;
    is_new_force = false;

    adc_val_1 = 0.0;
    adc_val_2 = 0.0;
    adc_filt_val_1 = 0.0;
    adc_filt_val_2 = 0.0 ;

    sensor = new SensorConnector(2607, QHostAddress("192.168.26.20"), 8888, this);
    connect( sensor, &SensorConnector::sensorPackReceive, this, &GraphPlot::SensorDataUpdate );

    QDateTime all(QDateTime::currentDateTime().date(), QDateTime::currentDateTime().time().addSecs(-10));
    timePiston = timeRod = timeForce = all;

    SetGraphPiston();
    SetGraphRod();
    SetGraphForce();


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

    timer = new QTimer();
    timer->stop();
    timer->setInterval(1000);
    connect( timer, &QTimer::timeout, this, &GraphPlot::handlePistonPlot );
    connect( timer, &QTimer::timeout, this, &GraphPlot::handleRodPlot );
    connect( timer, &QTimer::timeout, this, &GraphPlot::handleForcePlot );
    timer->start();
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
//    pal.setColor(QPalette::Window, QRgb(0xcee7f0));
//    pal.setColor(QPalette::WindowText, QRgb(0x404044));
    window()->setPalette(pal);
}


void GraphPlot::DebugSlot()
{
//    timer->stop();
}

void GraphPlot::StartTimer()
{
    timer->start();
}


void GraphPlot::handlePistonPlot()
{
    qreal x = ui->graphView1->chart()->plotArea().width() / ax_X_Piston->tickCount();
    qreal y = (ax_X_Piston->max().toMSecsSinceEpoch() - ax_X_Piston->min().toMSecsSinceEpoch()) / ax_X_Piston->tickCount();
    timePiston = timePiston.addMSecs(y);
    if( is_new_piston ){
        seriesPistonLeft->append(timePiston.toMSecsSinceEpoch(),  adc_val_1);
//        seriesPistonRight->append(timePiston.toMSecsSinceEpoch(),  adc_val_2);
        is_new_piston = false;
    }
    chartPiston->scroll(x, 0);
}

void GraphPlot::handleRodPlot()
{
    qreal x = ui->graphView2->chart()->plotArea().width() / ax_X_Rod->tickCount();
    qreal y = (ax_X_Rod->max().toMSecsSinceEpoch() - ax_X_Rod->min().toMSecsSinceEpoch()) / ax_X_Rod->tickCount();
    timeRod = timeRod.addMSecs(y);
    if( is_new_rod ){
        seriesRodLeft->append(timeRod.toMSecsSinceEpoch(),  adc_val_2);
//        seriesRodRight->append(timeRod.toMSecsSinceEpoch(),  adc_filt_val_2);
        is_new_rod = false;
    }
    chartRod->scroll(x, 0);
}

void GraphPlot::handleForcePlot()
{
    qreal x = ui->graphView3->chart()->plotArea().width() / ax_X_Force->tickCount();
    qreal y = (ax_X_Force->max().toMSecsSinceEpoch() - ax_X_Force->min().toMSecsSinceEpoch()) / ax_X_Force->tickCount();
    timeForce = timeForce.addMSecs(y);
    if( is_new_rod ){

    }
//    seriesForceLeft->append(timeForce.toMSecsSinceEpoch(),  QRandomGenerator::global()->bounded(1,10));
//    seriesForceRight->append(timeForce.toMSecsSinceEpoch(),  QRandomGenerator::global()->bounded(1,10));
    chartForce->scroll(x, 0);
}

void GraphPlot::SetGraphPiston()
{
    seriesPistonLeft = new QLineSeries();
    seriesPistonLeft->setName("Расход");

//    seriesPistonRight = new QLineSeries();
//    seriesPistonRight->setName("Правый");

    QDateTime temp_time = timePiston;
    seriesPistonLeft->append(timePiston.toMSecsSinceEpoch(), 0);
//    seriesPistonRight->append(timePiston.toMSecsSinceEpoch(), 0);

    for( int i = 1; i <= 10; i++ ){
        timePiston = timePiston.addSecs(1);
        seriesPistonLeft->append(timePiston.toMSecsSinceEpoch(), 0);
//        seriesPistonRight->append(timePiston.toMSecsSinceEpoch(), 0);
    }

    ax_X_Piston = new QDateTimeAxis;
    ax_Y_Piston = new QValueAxis;
    ax_X_Piston->setTitleText("Время, сек");
    ax_X_Piston->setFormat("hh:mm:ss");
    ax_Y_Piston->setTitleText("Расход, л/мин");

    chartPiston = new QChart();
    chartPiston->legend()->setAlignment(Qt::AlignRight);
    chartPiston->legend()->show();
    chartPiston->addSeries(seriesPistonLeft);
//    chartPiston->addSeries(seriesPistonRight);
    chartPiston->addAxis(ax_X_Piston, Qt::AlignBottom);
    chartPiston->addAxis(ax_Y_Piston, Qt::AlignLeft);
    chartPiston->setAnimationOptions(QChart::SeriesAnimations);

    seriesPistonLeft->attachAxis( ax_X_Piston );
    seriesPistonLeft->attachAxis( ax_Y_Piston );

    ax_X_Piston->setRange(temp_time, timePiston.addSecs(1));
    ax_Y_Piston->setRange(0, 100);
    ax_X_Piston->setTickCount(10);
    ax_Y_Piston->setTickCount(11);
    chartPiston->setTitle("Объемный расход из блока левого цилиндра");

    ui->graphView1->setChart(chartPiston);
    ui->graphView1->setRenderHint(QPainter::Antialiasing);
    ui->graphView1->setRubberBand( QChartView::RectangleRubberBand );
}
void GraphPlot::SetGraphRod()
{
    seriesRodLeft = new QLineSeries();
    seriesRodLeft->setName("Расход");

    QDateTime temp_time = timeRod;

    seriesRodLeft->append(timeRod.toMSecsSinceEpoch(), 0);

    for( int i = 1; i <= 10; i++ ){
        timeRod = timeRod.addSecs(1);
        seriesRodLeft->append(timeRod.toMSecsSinceEpoch(), 0);
    }

    ax_X_Rod = new QDateTimeAxis;
    ax_Y_Rod = new QValueAxis;
    ax_X_Rod->setTitleText("Время, сек");
    ax_X_Rod->setFormat("hh:mm:ss");
    ax_Y_Rod->setTitleText("Температура, град");

    chartRod = new QChart();
    chartRod->legend()->setAlignment(Qt::AlignRight);
    chartRod->legend()->show();
    chartRod->addSeries(seriesRodLeft);
    chartRod->addAxis(ax_X_Rod, Qt::AlignBottom);
    chartRod->addAxis(ax_Y_Rod, Qt::AlignLeft);
    chartRod->setAnimationOptions(QChart::SeriesAnimations);

    seriesRodLeft->attachAxis( ax_X_Rod );
    seriesRodLeft->attachAxis( ax_Y_Rod );

    ax_X_Rod->setRange(temp_time, timeRod.addSecs(1));
    ax_Y_Rod->setRange(-10, 80);
    ax_X_Rod->setTickCount(10);
    ax_Y_Rod->setTickCount(10);
    chartRod->setTitle("Температура расхода из блока левого цилиндра");

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
    seriesForceLeft->append(timeForce.toMSecsSinceEpoch(), 0);
    seriesForceRight->append(timeForce.toMSecsSinceEpoch(), 0);

    for( int i = 1; i <=10; i++ ){
        timeForce = timeForce.addSecs(1);
        seriesForceLeft->append(timeForce.toMSecsSinceEpoch(), 0);
        seriesForceRight->append(timeForce.toMSecsSinceEpoch(), 0);
    }

    ax_X_Force = new QDateTimeAxis;
    ax_Y_Force = new QValueAxis;
    ax_X_Force->setTitleText("Время, сек");
    ax_X_Force->setFormat("hh:mm:ss");
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

    ax_X_Force->setRange(temp_time, timeForce.addSecs(1));
    ax_Y_Force->setRange(0, 20);
    ax_X_Force->setTickCount(10);
    ax_Y_Force->setTickCount(5);
    chartForce->setTitle("Отношение эталонного дифф. усилия к реальному");

    ui->graphView3->setChart(chartForce);
    ui->graphView3->setRenderHint(QPainter::Antialiasing);
    ui->graphView3->setRubberBand( QChartView::RectangleRubberBand );
}

void GraphPlot::SensorDataUpdate(SensorPack pack)
{
    double rashod_val = (ui->rb_adc1_rash->isChecked()) ?  pack.adc1_data.at(ui->cb_adc1_priston->currentIndex())
                                                        :  pack.adc3_data.at(ui->cb_adc3_priston->currentIndex()) ;
    double temp_val   = (ui->rb_temper_adc_1->isChecked()) ? pack.adc1_data.at(ui->cb_adc1_temper->currentIndex())
                                                           : pack.adc3_data.at(ui->cb_adc3_temper->currentIndex());
    adc_val_1 = map( rashod_val, 0, 65536, 0, 100 );
    adc_val_2 = map( temp_val, 0, 65536, -10, 80 );
    is_new_piston = true;
    is_new_rod = true;

}

void GraphPlot::SetCheck(bool is_value)
{
    is_Check = is_value;
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
