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

    sensor = new SensorConnector(2607, QHostAddress("192.168.1.20"), 8888, this);
    connect( sensor, &SensorConnector::sensorPackReceive, this, &GraphPlot::SensorDataUpdate );

    QDateTime all(QDateTime::currentDateTime().date(), QDateTime::currentDateTime().time().addSecs(-10));
    timePiston = timeRod = timeLeftPressure = timeLeftZadan = timeLeftZolotPosit = timeLeftShtokPosit = timeRightPressure = timeRightZadan = timeRightZolotPosit = timeRightShtokPosit= all;

    SetGraphPiston();
    SetGraphRod();
    SetGraphForce();
    SetGraphZadanLeft();
    SetGraphZolotPositionLeft();
    SetGraphShtokPositionLeft();

    SetGraphPressureRight();
    SetGraphZadanRight();
    SetGraphZolotPositionRight();
    SetGraphShtokPositionRight();


    connect( ui->tab_lef_cb_position_shtok, &QCheckBox::clicked, this, &GraphPlot::TabGraphShowingLeft );
    connect( ui->tab_lef_cb_position_zolot, &QCheckBox::clicked, this, &GraphPlot::TabGraphShowingLeft );
    connect( ui->tab_lef_cb_press_pist_rod, &QCheckBox::clicked, this, &GraphPlot::TabGraphShowingLeft );
    connect( ui->tab_lef_cb_zadan_klapan,   &QCheckBox::clicked, this, &GraphPlot::TabGraphShowingLeft );

    connect( ui->tab_right_cb_position_shtok, &QCheckBox::clicked, this, &GraphPlot::TabGraphShowingRight );
    connect( ui->tab_right_cb_position_zolot, &QCheckBox::clicked, this, &GraphPlot::TabGraphShowingRight );
    connect( ui->tab_right_cb_press_pist_rod, &QCheckBox::clicked, this, &GraphPlot::TabGraphShowingRight );
    connect( ui->tab_right_cb_zadan_klapan,   &QCheckBox::clicked, this, &GraphPlot::TabGraphShowingRight );


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
    connect( timer, &QTimer::timeout, this, &GraphPlot::handleRightTabPlot );
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
    qreal x = ui->graphView3->chart()->plotArea().width() / ax_X_LeftPressure->tickCount();
    qreal y = (ax_X_LeftPressure->max().toMSecsSinceEpoch() - ax_X_LeftPressure->min().toMSecsSinceEpoch()) / ax_X_LeftPressure->tickCount();
    timeLeftPressure = timeLeftPressure.addMSecs(y);
//    if( is_new_rod ){

//    }
    ser_left_piston_pressure->append(timeLeftPressure.toMSecsSinceEpoch(),  QRandomGenerator::global()->bounded(1,250));
    ser_left_rod_pressure->append(timeLeftPressure.toMSecsSinceEpoch(),  QRandomGenerator::global()->bounded(1,250));
    chartLeftPressure->scroll(x, 0);


    qreal x_zadan = ui->tab_left_gv_zadan_klapan->chart()->plotArea().width() / ax_X_LeftZadan->tickCount();
    qreal y_zadan = ( ax_X_LeftZadan->max().toMSecsSinceEpoch() - ax_X_LeftZadan->min().toMSecsSinceEpoch() ) / ax_X_LeftZadan->tickCount();
    timeLeftZadan = timeLeftZadan.addMSecs(y_zadan);
    ser_left_zadan->append( timeLeftZadan.toMSecsSinceEpoch(),  QRandomGenerator::global()->bounded(-100,100));
    chartLeftZadan->scroll( x_zadan, 0 );

    qreal x_zolot = ui->tab_left_gv_zolot_position->chart()->plotArea().width() / ax_X_LeftZolotPosit->tickCount();
    qreal y_zolot = ( ax_X_LeftZolotPosit->max().toMSecsSinceEpoch() - ax_X_LeftZolotPosit->min().toMSecsSinceEpoch() ) / ax_X_LeftZolotPosit->tickCount();
    timeLeftZolotPosit = timeLeftZolotPosit.addMSecs(y_zolot);
    ser_left_ZolotPosit->append(timeLeftZolotPosit.toMSecsSinceEpoch(), QRandomGenerator::global()->bounded(-100,100));
    chartLeftZolotPosit->scroll( x_zolot, 0 );

    qreal x_shtok = ui->tab_left_gv_position_shtok->chart()->plotArea().width() / ax_X_LeftShtokPosit->tickCount();
    qreal y_shtok = ( ax_X_LeftShtokPosit->max().toMSecsSinceEpoch() - ax_X_LeftShtokPosit->min().toMSecsSinceEpoch() ) / ax_X_LeftShtokPosit->tickCount();
    timeLeftShtokPosit = timeLeftShtokPosit.addMSecs(y_shtok);
    ser_left_ShtokPosit->append(timeLeftShtokPosit.toMSecsSinceEpoch(), QRandomGenerator::global()->bounded(0,25));
    chartLeftShtokPosit->scroll( x_shtok, 0 );
}

void GraphPlot::handleRightTabPlot()
{
    qreal x = ui->tab_right_gv_pressure->chart()->plotArea().width() / ax_X_RightPressure->tickCount();
    qreal y = (ax_X_RightPressure->max().toMSecsSinceEpoch() - ax_X_RightPressure->min().toMSecsSinceEpoch()) / ax_X_RightPressure->tickCount();
    timeRightPressure = timeRightPressure.addMSecs(y);
    ser_right_piston_pressure->append(timeRightPressure.toMSecsSinceEpoch(),  QRandomGenerator::global()->bounded(1,250));
    ser_right_rod_pressure->append(timeRightPressure.toMSecsSinceEpoch(),  QRandomGenerator::global()->bounded(1,250));
    chartRightPressure->scroll(x, 0);

    qreal x_zadan = ui->tab_right_gv_zadan_klapan->chart()->plotArea().width() / ax_X_RightZadan->tickCount();
    qreal y_zadan = ( ax_X_RightZadan->max().toMSecsSinceEpoch() - ax_X_RightZadan->min().toMSecsSinceEpoch() ) / ax_X_RightZadan->tickCount();
    timeRightZadan = timeRightZadan.addMSecs(y_zadan);
    ser_right_zadan->append( timeRightZadan.toMSecsSinceEpoch(),  QRandomGenerator::global()->bounded(-100,100));
    chartRightZadan->scroll( x_zadan, 0 );

    qreal x_zolot = ui->tab_right_gv_zolot_position->chart()->plotArea().width() / ax_X_RightZolotPosit->tickCount();
    qreal y_zolot = ( ax_X_RightZolotPosit->max().toMSecsSinceEpoch() - ax_X_RightZolotPosit->min().toMSecsSinceEpoch() ) / ax_X_RightZolotPosit->tickCount();
    timeRightZolotPosit = timeRightZolotPosit.addMSecs(y_zolot);
    ser_right_ZolotPosit->append(timeRightZolotPosit.toMSecsSinceEpoch(), QRandomGenerator::global()->bounded(-100,100));
    chartRightZolotPosit->scroll( x_zolot, 0 );

    qreal x_shtok = ui->tab_right_gv_position_shtok->chart()->plotArea().width() / ax_X_RightShtokPosit->tickCount();
    qreal y_shtok = ( ax_X_RightShtokPosit->max().toMSecsSinceEpoch() - ax_X_RightShtokPosit->min().toMSecsSinceEpoch() ) / ax_X_RightShtokPosit->tickCount();
    timeRightShtokPosit = timeRightShtokPosit.addMSecs(y_shtok);
    ser_right_ShtokPosit->append(timeRightShtokPosit.toMSecsSinceEpoch(), QRandomGenerator::global()->bounded(0,25));
    chartRightShtokPosit->scroll( x_shtok, 0 );
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
    ser_left_piston_pressure = new QLineSeries();
    ser_left_piston_pressure->setName("Поршень");

    ser_left_rod_pressure = new QLineSeries();
    ser_left_rod_pressure->setName("Шток");

    QDateTime temp_time = timeLeftPressure;
    ser_left_piston_pressure->append(timeLeftPressure.toMSecsSinceEpoch(), 0);
    ser_left_rod_pressure->append(timeLeftPressure.toMSecsSinceEpoch(), 0);

    for( int i = 1; i <=10; i++ ){
        timeLeftPressure = timeLeftPressure.addSecs(1);
        ser_left_piston_pressure->append(timeLeftPressure.toMSecsSinceEpoch(), 0);
        ser_left_rod_pressure->append(timeLeftPressure.toMSecsSinceEpoch(), 0);
    }

    ax_X_LeftPressure = new QDateTimeAxis;
    ax_Y_LeftPressure = new QValueAxis;
    ax_X_LeftPressure->setTitleText("Время, сек");
    ax_X_LeftPressure->setFormat("hh:mm:ss");
    ax_Y_LeftPressure->setTitleText("Давление, бар");

    chartLeftPressure = new QChart();
    chartLeftPressure->legend()->setAlignment(Qt::AlignRight);
    chartLeftPressure->legend()->hide();
    chartLeftPressure->addSeries(ser_left_piston_pressure);
    chartLeftPressure->addSeries(ser_left_rod_pressure);
    chartLeftPressure->addAxis(ax_X_LeftPressure, Qt::AlignBottom);
    chartLeftPressure->addAxis(ax_Y_LeftPressure, Qt::AlignLeft);
    chartLeftPressure->setAnimationOptions(QChart::SeriesAnimations);

    ser_left_piston_pressure->attachAxis( ax_X_LeftPressure );
    ser_left_piston_pressure->attachAxis( ax_Y_LeftPressure );
    ser_left_rod_pressure->attachAxis( ax_X_LeftPressure );
    ser_left_rod_pressure->attachAxis( ax_Y_LeftPressure );

    ax_X_LeftPressure->setRange(temp_time, timeLeftPressure.addSecs(1));
    ax_Y_LeftPressure->setRange(0, 250);
    ax_X_LeftPressure->setTickCount(10);
    ax_Y_LeftPressure->setTickCount(5);
    chartLeftPressure->setTitle("Давление в поршневой и штоковой полости");

    ui->graphView3->setChart(chartLeftPressure);
    ui->graphView3->setRenderHint(QPainter::Antialiasing);
    ui->graphView3->setRubberBand( QChartView::RectangleRubberBand );
}
void GraphPlot::SetGraphZadanLeft()
{
    ser_left_zadan = new QLineSeries();
    ser_left_zadan->setName("Задание");

    QDateTime temp_time = timeLeftZadan;
    ser_left_zadan->append(timeLeftZadan.toMSecsSinceEpoch(), 0);

    for( int i = 1; i <=10; i++ ){
        timeLeftZadan = timeLeftZadan.addSecs(1);
        ser_left_zadan->append(timeLeftZadan.toMSecsSinceEpoch(), 0);
    }

    ax_X_LeftZadan =  new QDateTimeAxis;
    ax_X_LeftZadan->setTitleText("Время, сек");
    ax_X_LeftZadan->setFormat("hh:mm:ss");
    ax_Y_LeftZadan = new QValueAxis;
    ax_Y_LeftZadan->setTitleText("Задание на клапан, %");
    chartLeftZadan = new QChart();

    chartLeftZadan->legend()->setAlignment(Qt::AlignRight);
    chartLeftZadan->legend()->hide();
    chartLeftZadan->addSeries(ser_left_zadan);
    chartLeftZadan->addAxis(ax_X_LeftZadan, Qt::AlignBottom);
    chartLeftZadan->addAxis(ax_Y_LeftZadan, Qt::AlignLeft);
    chartLeftZadan->setAnimationOptions(QChart::SeriesAnimations);

    ser_left_zadan->attachAxis(ax_X_LeftZadan);
    ser_left_zadan->attachAxis(ax_Y_LeftZadan);
    ax_X_LeftZadan->setRange(temp_time, timeLeftZadan.addSecs(1));
    ax_X_LeftZadan->setTickCount(10);
    ax_Y_LeftZadan->setRange( -100, 100 );
    ax_Y_LeftZadan->setTickCount(5);
    chartLeftZadan->setTitle("Задание на проп. клапан");
    ui->tab_left_gv_zadan_klapan->setChart(chartLeftZadan);

    ui->tab_left_gv_zadan_klapan->setRenderHint(QPainter::Antialiasing);
    ui->tab_left_gv_zadan_klapan->setRubberBand( QChartView::RectangleRubberBand );

}
void GraphPlot::SetGraphZolotPositionLeft()
{
    ser_left_ZolotPosit = new QLineSeries();
    ser_left_ZolotPosit->setName("Положение золотника");
    QDateTime temp_time = timeLeftZolotPosit;
    ser_left_ZolotPosit->append( timeLeftZolotPosit.toMSecsSinceEpoch(), 0 );
    for( int i = 1; i <=10; i++ ){
        timeLeftZolotPosit = timeLeftZolotPosit.addSecs(1);
        ser_left_ZolotPosit->append(timeLeftZolotPosit.toMSecsSinceEpoch(), 0);
    }
    ax_X_LeftZolotPosit = new QDateTimeAxis;
    ax_X_LeftZolotPosit->setTitleText("Время, сек");
    ax_X_LeftZolotPosit->setFormat("hh:mm:ss");
    ax_Y_LeftZolotPosit = new QValueAxis;
    ax_Y_LeftZolotPosit->setTitleText("Положение золотника, %");
    chartLeftZolotPosit = new QChart();

    chartLeftZolotPosit->legend()->setAlignment(Qt::AlignRight);
    chartLeftZolotPosit->legend()->hide();
    chartLeftZolotPosit->addSeries(ser_left_ZolotPosit);
    chartLeftZolotPosit->addAxis(ax_X_LeftZolotPosit, Qt::AlignBottom);
    chartLeftZolotPosit->addAxis(ax_Y_LeftZolotPosit, Qt::AlignLeft);
    chartLeftZolotPosit->setAnimationOptions(QChart::SeriesAnimations);

    ser_left_ZolotPosit->attachAxis(ax_X_LeftZolotPosit);
    ser_left_ZolotPosit->attachAxis(ax_Y_LeftZolotPosit);
    ax_X_LeftZolotPosit->setRange(temp_time, timeLeftZolotPosit.addSecs(1));
    ax_X_LeftZolotPosit->setTickCount(10);
    ax_Y_LeftZolotPosit->setRange( -100, 100 );
    ax_Y_LeftZolotPosit->setTickCount(5);
    chartLeftZolotPosit->setTitle("Обратная связь положения золотника проп. клапана");
    ui->tab_left_gv_zolot_position->setChart(chartLeftZolotPosit);
    ui->tab_left_gv_zolot_position->setRenderHint(QPainter::Antialiasing);
    ui->tab_left_gv_zolot_position->setRubberBand( QChartView::RectangleRubberBand );
}
void GraphPlot::SetGraphShtokPositionLeft()
{
    ser_left_ShtokPosit = new QLineSeries();
    ser_left_ShtokPosit->setName("Положение штока");
    QDateTime temp_time = timeLeftShtokPosit;
    ser_left_ShtokPosit->append( timeLeftShtokPosit.toMSecsSinceEpoch(), 0 );
    for( int i = 1; i <=10; i++ ){
        timeLeftShtokPosit = timeLeftShtokPosit.addSecs(1);
        ser_left_ShtokPosit->append(timeLeftShtokPosit.toMSecsSinceEpoch(), 0);
    }
    ax_X_LeftShtokPosit = new QDateTimeAxis;
    ax_X_LeftShtokPosit->setTitleText("Время, сек");
    ax_X_LeftShtokPosit->setFormat("hh:mm:ss");
    ax_Y_LeftShtokPosit = new QValueAxis;
    ax_Y_LeftShtokPosit->setTitleText("Положение штока, мм");
    chartLeftShtokPosit = new QChart();

    chartLeftShtokPosit->legend()->setAlignment(Qt::AlignRight);
    chartLeftShtokPosit->legend()->hide();
    chartLeftShtokPosit->addSeries(ser_left_ShtokPosit);
    chartLeftShtokPosit->addAxis(ax_X_LeftShtokPosit, Qt::AlignBottom);
    chartLeftShtokPosit->addAxis(ax_Y_LeftShtokPosit, Qt::AlignLeft);
    chartLeftShtokPosit->setAnimationOptions(QChart::SeriesAnimations);

    ser_left_ShtokPosit->attachAxis(ax_X_LeftShtokPosit);
    ser_left_ShtokPosit->attachAxis(ax_Y_LeftShtokPosit);
    ax_X_LeftShtokPosit->setRange(temp_time, timeLeftShtokPosit.addSecs(1));
    ax_X_LeftShtokPosit->setTickCount(10);
    ax_Y_LeftShtokPosit->setRange( 0, 25 );
    ax_Y_LeftShtokPosit->setTickCount(5);
    chartLeftShtokPosit->setTitle("Обратная связь положения штока");
    ui->tab_left_gv_position_shtok->setChart(chartLeftShtokPosit);
    ui->tab_left_gv_position_shtok->setRenderHint(QPainter::Antialiasing);
    ui->tab_left_gv_position_shtok->setRubberBand( QChartView::RectangleRubberBand );

}

void GraphPlot::TabGraphShowingLeft()
{
    if( ui->tab_lef_cb_position_shtok->isChecked() ){
        ui->tab_left_gv_position_shtok->show();
    }
    else{
        ui->tab_left_gv_position_shtok->hide();
    }
    if( ui->tab_lef_cb_position_zolot->isChecked() ){
        ui->tab_left_gv_zolot_position->show();
    }
    else{
        ui->tab_left_gv_zolot_position->hide();
    }
    if( ui->tab_lef_cb_press_pist_rod->isChecked() ){
        ui->graphView3->show();
    }
    else{
        ui->graphView3->hide();
    }
    if( ui->tab_lef_cb_zadan_klapan->isChecked() ){
        ui->tab_left_gv_zadan_klapan->show();
    }
    else{
        ui->tab_left_gv_zadan_klapan->hide();
    }
}

void GraphPlot::SetGraphPressureRight()
{
    ser_right_piston_pressure = new QLineSeries();
    ser_right_piston_pressure->setName("Поршень");

    ser_right_rod_pressure = new QLineSeries();
    ser_right_rod_pressure->setName("Шток");

    QDateTime temp_time = timeRightPressure;
    ser_right_piston_pressure->append(timeRightPressure.toMSecsSinceEpoch(), 0);
    ser_right_rod_pressure->append(timeRightPressure.toMSecsSinceEpoch(), 0);

    for( int i = 1; i <=10; i++ ){
        timeRightPressure = timeRightPressure.addSecs(1);
        ser_right_piston_pressure->append(timeRightPressure.toMSecsSinceEpoch(), 0);
        ser_right_rod_pressure->append(timeRightPressure.toMSecsSinceEpoch(), 0);
    }

    ax_X_RightPressure = new QDateTimeAxis;
    ax_Y_RightPressure = new QValueAxis;
    ax_X_RightPressure->setTitleText("Время, сек");
    ax_X_RightPressure->setFormat("hh:mm:ss");
    ax_Y_RightPressure->setTitleText("Давление, бар");

    chartRightPressure = new QChart();
    chartRightPressure->legend()->setAlignment(Qt::AlignRight);
    chartRightPressure->legend()->hide();
    chartRightPressure->addSeries(ser_right_piston_pressure);
    chartRightPressure->addSeries(ser_right_rod_pressure);
    chartRightPressure->addAxis(ax_X_RightPressure, Qt::AlignBottom);
    chartRightPressure->addAxis(ax_Y_RightPressure, Qt::AlignLeft);
    chartRightPressure->setAnimationOptions(QChart::SeriesAnimations);

    ser_right_piston_pressure->attachAxis( ax_X_RightPressure );
    ser_right_piston_pressure->attachAxis( ax_Y_RightPressure );
    ser_right_rod_pressure->attachAxis( ax_X_RightPressure );
    ser_right_rod_pressure->attachAxis( ax_Y_RightPressure );

    ax_X_RightPressure->setRange(temp_time, timeRightPressure.addSecs(1));
    ax_Y_RightPressure->setRange(0, 250);
    ax_X_RightPressure->setTickCount(10);
    ax_Y_RightPressure->setTickCount(5);
    chartRightPressure->setTitle("Давление в поршневой и штоковой полости");

    ui->tab_right_gv_pressure->setChart(chartRightPressure);
    ui->tab_right_gv_pressure->setRenderHint(QPainter::Antialiasing);
    ui->tab_right_gv_pressure->setRubberBand( QChartView::RectangleRubberBand );
}
void GraphPlot::SetGraphZadanRight()
{
    ser_right_zadan = new QLineSeries();
    ser_right_zadan->setName("Задание");

    QDateTime temp_time = timeRightZadan;
    ser_right_zadan->append(timeRightZadan.toMSecsSinceEpoch(), 0);

    for( int i = 1; i <=10; i++ ){
        timeRightZadan = timeRightZadan.addSecs(1);
        ser_right_zadan->append(timeRightZadan.toMSecsSinceEpoch(), 0);
    }

    ax_X_RightZadan =  new QDateTimeAxis;
    ax_X_RightZadan->setTitleText("Время, сек");
    ax_X_RightZadan->setFormat("hh:mm:ss");
    ax_Y_RightZadan = new QValueAxis;
    ax_Y_RightZadan->setTitleText("Задание на клапан, %");
    chartRightZadan = new QChart();

    chartRightZadan->legend()->setAlignment(Qt::AlignRight);
    chartRightZadan->legend()->hide();
    chartRightZadan->addSeries(ser_right_zadan);
    chartRightZadan->addAxis(ax_X_RightZadan, Qt::AlignBottom);
    chartRightZadan->addAxis(ax_Y_RightZadan, Qt::AlignLeft);
    chartRightZadan->setAnimationOptions(QChart::SeriesAnimations);

    ser_right_zadan->attachAxis(ax_X_RightZadan);
    ser_right_zadan->attachAxis(ax_Y_RightZadan);
    ax_X_RightZadan->setRange(temp_time, timeRightZadan.addSecs(1));
    ax_X_RightZadan->setTickCount(10);
    ax_Y_RightZadan->setRange( -100, 100 );
    ax_Y_RightZadan->setTickCount(5);
    chartRightZadan->setTitle("Задание на проп. клапан");
    ui->tab_right_gv_zadan_klapan->setChart(chartRightZadan);

    ui->tab_right_gv_zadan_klapan->setRenderHint(QPainter::Antialiasing);
    ui->tab_right_gv_zadan_klapan->setRubberBand( QChartView::RectangleRubberBand );
}
void GraphPlot::SetGraphZolotPositionRight()
{
    ser_right_ZolotPosit = new QLineSeries();
    ser_right_ZolotPosit->setName("Положение золотника");
    QDateTime temp_time = timeRightZolotPosit;
    ser_right_ZolotPosit->append( timeRightZolotPosit.toMSecsSinceEpoch(), 0 );
    for( int i = 1; i <=10; i++ ){
        timeRightZolotPosit = timeRightZolotPosit.addSecs(1);
        ser_right_ZolotPosit->append(timeRightZolotPosit.toMSecsSinceEpoch(), 0);
    }
    ax_X_RightZolotPosit = new QDateTimeAxis;
    ax_X_RightZolotPosit->setTitleText("Время, сек");
    ax_X_RightZolotPosit->setFormat("hh:mm:ss");
    ax_Y_RightZolotPosit = new QValueAxis;
    ax_Y_RightZolotPosit->setTitleText("Положение золотника, %");
    chartRightZolotPosit = new QChart();

    chartRightZolotPosit->legend()->setAlignment(Qt::AlignRight);
    chartRightZolotPosit->legend()->hide();
    chartRightZolotPosit->addSeries(ser_right_ZolotPosit);
    chartRightZolotPosit->addAxis(ax_X_RightZolotPosit, Qt::AlignBottom);
    chartRightZolotPosit->addAxis(ax_Y_RightZolotPosit, Qt::AlignLeft);
    chartRightZolotPosit->setAnimationOptions(QChart::SeriesAnimations);

    ser_right_ZolotPosit->attachAxis(ax_X_RightZolotPosit);
    ser_right_ZolotPosit->attachAxis(ax_Y_RightZolotPosit);
    ax_X_RightZolotPosit->setRange(temp_time, timeRightZolotPosit.addSecs(1));
    ax_X_RightZolotPosit->setTickCount(10);
    ax_Y_RightZolotPosit->setRange( -100, 100 );
    ax_Y_RightZolotPosit->setTickCount(5);
    chartRightZolotPosit->setTitle("Обратная связь положения золотника проп. клапана");
    ui->tab_right_gv_zolot_position->setChart(chartRightZolotPosit);
    ui->tab_right_gv_zolot_position->setRenderHint(QPainter::Antialiasing);
    ui->tab_right_gv_zolot_position->setRubberBand( QChartView::RectangleRubberBand );
}

void GraphPlot::SetGraphShtokPositionRight()
{
    ser_right_ShtokPosit = new QLineSeries();
    ser_right_ShtokPosit->setName("Положение штока");
    QDateTime temp_time = timeRightShtokPosit;
    ser_right_ShtokPosit->append( timeRightShtokPosit.toMSecsSinceEpoch(), 0 );
    for( int i = 1; i <=10; i++ ){
        timeRightShtokPosit = timeRightShtokPosit.addSecs(1);
        ser_right_ShtokPosit->append(timeRightShtokPosit.toMSecsSinceEpoch(), 0);
    }
    ax_X_RightShtokPosit = new QDateTimeAxis;
    ax_X_RightShtokPosit->setTitleText("Время, сек");
    ax_X_RightShtokPosit->setFormat("hh:mm:ss");
    ax_Y_RightShtokPosit = new QValueAxis;
    ax_Y_RightShtokPosit->setTitleText("Положение штока, мм");
    chartRightShtokPosit = new QChart();

    chartRightShtokPosit->legend()->setAlignment(Qt::AlignRight);
    chartRightShtokPosit->legend()->hide();
    chartRightShtokPosit->addSeries(ser_right_ShtokPosit);
    chartRightShtokPosit->addAxis(ax_X_RightShtokPosit, Qt::AlignBottom);
    chartRightShtokPosit->addAxis(ax_Y_RightShtokPosit, Qt::AlignLeft);
    chartRightShtokPosit->setAnimationOptions(QChart::SeriesAnimations);

    ser_right_ShtokPosit->attachAxis(ax_X_RightShtokPosit);
    ser_right_ShtokPosit->attachAxis(ax_Y_RightShtokPosit);
    ax_X_RightShtokPosit->setRange(temp_time, timeRightShtokPosit.addSecs(1));
    ax_X_RightShtokPosit->setTickCount(10);
    ax_Y_RightShtokPosit->setRange( 0, 25 );
    ax_Y_RightShtokPosit->setTickCount(5);
    chartRightShtokPosit->setTitle("Обратная связь положения штока");
    ui->tab_right_gv_position_shtok->setChart(chartRightShtokPosit);
    ui->tab_right_gv_position_shtok->setRenderHint(QPainter::Antialiasing);
    ui->tab_right_gv_position_shtok->setRubberBand( QChartView::RectangleRubberBand );
}

void GraphPlot::TabGraphShowingRight()
{
    if( ui->tab_right_cb_position_shtok->isChecked() ){
        ui->tab_right_gv_position_shtok->show();
    }
    else{
        ui->tab_right_gv_position_shtok->hide();
    }
    if( ui->tab_right_cb_position_zolot->isChecked() ){
        ui->tab_right_gv_zolot_position->show();
    }
    else{
        ui->tab_right_gv_zolot_position->hide();
    }
    if( ui->tab_right_cb_press_pist_rod->isChecked() ){
        ui->tab_right_gv_pressure->show();
    }
    else{
        ui->tab_right_gv_pressure->hide();
    }
    if( ui->tab_right_cb_zadan_klapan->isChecked() ){
        ui->tab_right_gv_zadan_klapan->show();
    }
    else{
        ui->tab_right_gv_zadan_klapan->hide();
    }
}

void GraphPlot::SensorDataUpdate(SensorPack pack)
{
    double rashod_val = (ui->rb_adc1_rash->isChecked()) ?  pack.adc1_data.at(ui->cb_adc1_priston->currentIndex())
                                                        :  pack.adc3_data.at(ui->cb_adc3_priston->currentIndex()) ;
    double temp_val   = (ui->rb_temper_adc_1->isChecked()) ? pack.adc1_data.at(ui->cb_adc1_temper->currentIndex())
                                                           : pack.adc3_data.at(ui->cb_adc3_temper->currentIndex());
    adc_val_1 = map( rashod_val, 0, 4095, 0, 100 );
    adc_val_2 = map( temp_val, 0, 4095, -10, 80 );
    is_new_piston = true;
    is_new_rod = true;

}

void GraphPlot::on_action_13_triggered()
{

}


void GraphPlot::on_action_14_triggered()
{

}
