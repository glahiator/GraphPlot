#include "GraphPlot.h"
#include "ui_GraphPlot.h"

GraphPlot::GraphPlot(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GraphPlot)
{
    ui->setupUi(this);
    sens_cylinders = SensorVals();
    counter = 0;
    switcher = false;
    is_Check = false;

    isDemo_fT_tfT = false;

    is_new_sensor = false;
    is_new_force = false;

    is_PLC_CALC_new_data = false;
    is_PLC_new_data_left = false;
    is_PLC_new_data_right = false;


    fT_graph = new UniqueGraph( "Объемный расход", this);
    tfT_graph = new UniqueGraph( "Температура расхода", this);
    fT_graph->Configure("Расход, л/мин");
    ui->view_fT->setChart(fT_graph->chart);
    ui->view_fT->setRenderHint(QPainter::Antialiasing);


    tfT_graph->Configure("Температура, град");
    ui->view_tfT->setChart( tfT_graph->chart );
    ui->view_tfT->setRenderHint(QPainter::Antialiasing);


    sensor = new Sensor(2607, QHostAddress("192.168.1.20"), 8888, this);
    connect( sensor, &Sensor::sensorPackReceive, this, &GraphPlot::SensorDataUpdate );

    plc = new PLC_Connector(2688, QHostAddress("192.168.1.31"), 8888, this);
//    connect( plc, &PLC_Connector::plcDataReceive, this, &GraphPlot::PLC_DataUpdate );

    QDateTime all(QDateTime::currentDateTime().date(), QDateTime::currentDateTime().time().addSecs(-10));
    timeLeftPressure = timeLeftZadan = timeLeftZolotPosit = timeLeftShtokPosit = timeRightPressure
            = timeRightZadan = timeRightZolotPosit = timeRightShtokPosit =
            timePistonLoss = timeStockLoss = timeDiffForce = all;

//    SetGraph_fT();
//    SetGraph_tfT();
    SetGraphForce();
    SetGraphZadanLeft();
    SetGraphZolotPositionLeft();
    SetGraphShtokPositionLeft();

    SetGraphPressureRight();
    SetGraphZadanRight();
    SetGraphZolotPositionRight();
    SetGraphShtokPositionRight();

    SetGraphPistonLoss();
    SetGraphStockLoss();
    SetGraphDiffForce();


    connect( ui->tab_lef_cb_position_shtok, &QCheckBox::clicked, this, &GraphPlot::TabGraphShowingLeft );
    connect( ui->tab_lef_cb_position_zolot, &QCheckBox::clicked, this, &GraphPlot::TabGraphShowingLeft );
    connect( ui->tab_lef_cb_press_pist_rod, &QCheckBox::clicked, this, &GraphPlot::TabGraphShowingLeft );
    connect( ui->tab_lef_cb_zadan_klapan,   &QCheckBox::clicked, this, &GraphPlot::TabGraphShowingLeft );

    connect( ui->tab_right_cb_position_shtok, &QCheckBox::clicked, this, &GraphPlot::TabGraphShowingRight );
    connect( ui->tab_right_cb_position_zolot, &QCheckBox::clicked, this, &GraphPlot::TabGraphShowingRight );
    connect( ui->tab_right_cb_press_pist_rod, &QCheckBox::clicked, this, &GraphPlot::TabGraphShowingRight );
    connect( ui->tab_right_cb_zadan_klapan,   &QCheckBox::clicked, this, &GraphPlot::TabGraphShowingRight );

    connect( ui->cb_calc_tab_diff_force,  &QCheckBox::clicked, this, &GraphPlot::TabGraphShowingCalc );
    connect( ui->cb_calc_tab_piston_loss,  &QCheckBox::clicked, this, &GraphPlot::TabGraphShowingCalc );
    connect( ui->cb_calc_tab_stock_loss,  &QCheckBox::clicked, this, &GraphPlot::TabGraphShowingCalc );

    connect( ui->act_demo, &QAction::toggled, this, &GraphPlot::SetDemo);


//    ui->themeComboBox->addItem("Light", QChart::ChartThemeLight);
//    ui->themeComboBox->addItem("Blue Cerulean", QChart::ChartThemeBlueCerulean);
//    ui->themeComboBox->addItem("Dark", QChart::ChartThemeDark);
//    ui->themeComboBox->addItem("Brown Sand", QChart::ChartThemeBrownSand);
//    ui->themeComboBox->addItem("Blue NCS", QChart::ChartThemeBlueNcs);
//    ui->themeComboBox->addItem("High Contrast", QChart::ChartThemeHighContrast);
//    ui->themeComboBox->addItem("Blue Icy", QChart::ChartThemeBlueIcy);
//    ui->themeComboBox->addItem("Qt", QChart::ChartThemeQt);

////    ui->themeComboBox->setCurrentIndex(4);
//    connect( ui->themeComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &GraphPlot::updateUI );

    QPalette pal = qApp->palette();
    pal.setColor(QPalette::Window, QRgb(0xf0f0f0));
    pal.setColor(QPalette::WindowText, QRgb(0x404044));
    qApp->setPalette(pal);

    updateUI();

    timer = new QTimer();
    timer->stop();
    timer->setInterval(1000);
//    connect( timer, &QTimer::timeout, this, &GraphPlot::handlePistonPlot );
//    connect( timer, &QTimer::timeout, this, &GraphPlot::handle_fT_tfT );
    connect( timer, &QTimer::timeout, this, &GraphPlot::handle_fT_tfT );

    connect( timer, &QTimer::timeout, this, &GraphPlot::handleForcePlot );
    connect( timer, &QTimer::timeout, this, &GraphPlot::handleRightTabPlot );
    connect( timer, &QTimer::timeout, this, &GraphPlot::handleCalcTabPlot );
    timer->start();
}

GraphPlot::~GraphPlot()
{
    delete sensor;
    delete plc;
    delete timer;
    delete ui;
}

void GraphPlot::updateUI()
{
    //![6]
    QChart::ChartTheme theme = static_cast<QChart::ChartTheme>(0);
//                ui->themeComboBox->itemData(ui->themeComboBox->currentIndex()).toInt());
    //![6]

    //![7]
    ui->view_fT->chart()->setTheme(theme);
    ui->view_tfT->chart()->setTheme(theme);
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

void GraphPlot::SetDemo()
{
    isDemo_fT_tfT = ui->act_demo->isChecked();
}

void GraphPlot::handleForcePlot()
{
    qreal x = ui->graphView3->chart()->plotArea().width() / ax_X_LeftPressure->tickCount();
    qreal y = (ax_X_LeftPressure->max().toMSecsSinceEpoch() - ax_X_LeftPressure->min().toMSecsSinceEpoch()) / ax_X_LeftPressure->tickCount();
    timeLeftPressure = timeLeftPressure.addMSecs(y);

    qreal x_zadan = ui->tab_left_gv_zadan_klapan->chart()->plotArea().width() / ax_X_LeftZadan->tickCount();
    qreal y_zadan = ( ax_X_LeftZadan->max().toMSecsSinceEpoch() - ax_X_LeftZadan->min().toMSecsSinceEpoch() ) / ax_X_LeftZadan->tickCount();
    timeLeftZadan = timeLeftZadan.addMSecs(y_zadan);

    qreal x_zolot = ui->tab_left_gv_zolot_position->chart()->plotArea().width() / ax_X_LeftZolotPosit->tickCount();
    qreal y_zolot = ( ax_X_LeftZolotPosit->max().toMSecsSinceEpoch() - ax_X_LeftZolotPosit->min().toMSecsSinceEpoch() ) / ax_X_LeftZolotPosit->tickCount();
    timeLeftZolotPosit = timeLeftZolotPosit.addMSecs(y_zolot);

    qreal x_shtok = ui->tab_left_gv_position_shtok->chart()->plotArea().width() / ax_X_LeftShtokPosit->tickCount();
    qreal y_shtok = ( ax_X_LeftShtokPosit->max().toMSecsSinceEpoch() - ax_X_LeftShtokPosit->min().toMSecsSinceEpoch() ) / ax_X_LeftShtokPosit->tickCount();
    timeLeftShtokPosit = timeLeftShtokPosit.addMSecs(y_shtok);

    if( is_PLC_new_data_left ){
//        ser_left_piston_pressure->append(timeLeftPressure.toMSecsSinceEpoch(),  plc_Data.left.pA);
//        ser_left_rod_pressure->append(   timeLeftPressure.toMSecsSinceEpoch(), plc_Data.left.pB );
//        ser_left_zadan->append( timeLeftZadan.toMSecsSinceEpoch(),  plc_Data.left.sY);
//        ser_left_ZolotPosit->append(timeLeftZolotPosit.toMSecsSinceEpoch(), plc_Data.left.fY);
//        ser_left_ShtokPosit->append(timeLeftShtokPosit.toMSecsSinceEpoch(), plc_Data.left.fS);

        is_PLC_new_data_left = false;
    }
    else{
        ser_left_piston_pressure->append(timeLeftPressure.toMSecsSinceEpoch(),  QRandomGenerator::global()->bounded(1,250));
        ser_left_rod_pressure->append(timeLeftPressure.toMSecsSinceEpoch(),  QRandomGenerator::global()->bounded(1,250));
        ser_left_zadan->append( timeLeftZadan.toMSecsSinceEpoch(),  QRandomGenerator::global()->bounded(-100,100));
        ser_left_ZolotPosit->append(timeLeftZolotPosit.toMSecsSinceEpoch(), QRandomGenerator::global()->bounded(-100,100));
        ser_left_ShtokPosit->append(timeLeftShtokPosit.toMSecsSinceEpoch(), QRandomGenerator::global()->bounded(0,25));
    }

    chartLeftShtokPosit->scroll( x_shtok, 0 );
    chartLeftPressure->scroll(x, 0);
    chartLeftZadan->scroll( x_zadan, 0 );
    chartLeftZolotPosit->scroll( x_zolot, 0 );

}
void GraphPlot::handleRightTabPlot()
{
    qreal x = ui->tab_right_gv_pressure->chart()->plotArea().width() / ax_X_RightPressure->tickCount();
    qreal y = (ax_X_RightPressure->max().toMSecsSinceEpoch() - ax_X_RightPressure->min().toMSecsSinceEpoch()) / ax_X_RightPressure->tickCount();
    timeRightPressure = timeRightPressure.addMSecs(y);

    qreal x_zadan = ui->tab_right_gv_zadan_klapan->chart()->plotArea().width() / ax_X_RightZadan->tickCount();
    qreal y_zadan = ( ax_X_RightZadan->max().toMSecsSinceEpoch() - ax_X_RightZadan->min().toMSecsSinceEpoch() ) / ax_X_RightZadan->tickCount();
    timeRightZadan = timeRightZadan.addMSecs(y_zadan);

    qreal x_zolot = ui->tab_right_gv_zolot_position->chart()->plotArea().width() / ax_X_RightZolotPosit->tickCount();
    qreal y_zolot = ( ax_X_RightZolotPosit->max().toMSecsSinceEpoch() - ax_X_RightZolotPosit->min().toMSecsSinceEpoch() ) / ax_X_RightZolotPosit->tickCount();
    timeRightZolotPosit = timeRightZolotPosit.addMSecs(y_zolot);

    qreal x_shtok = ui->tab_right_gv_position_shtok->chart()->plotArea().width() / ax_X_RightShtokPosit->tickCount();
    qreal y_shtok = ( ax_X_RightShtokPosit->max().toMSecsSinceEpoch() - ax_X_RightShtokPosit->min().toMSecsSinceEpoch() ) / ax_X_RightShtokPosit->tickCount();
    timeRightShtokPosit = timeRightShtokPosit.addMSecs(y_shtok);

    if( is_PLC_new_data_right ){
//        ser_right_piston_pressure->append(timeRightPressure.toMSecsSinceEpoch(),  plc_Data.right.pA);
//        ser_right_rod_pressure->append(   timeRightPressure.toMSecsSinceEpoch(), plc_Data.right.pB );
//        ser_right_zadan->append( timeRightZadan.toMSecsSinceEpoch(),  plc_Data.right.sY);
//        ser_right_ZolotPosit->append(timeRightZolotPosit.toMSecsSinceEpoch(), plc_Data.right.fY);
//        ser_right_ShtokPosit->append(timeRightShtokPosit.toMSecsSinceEpoch(), plc_Data.right.fS);

        is_PLC_new_data_right = false;
    }
    else{
        ser_right_piston_pressure->append(timeRightPressure.toMSecsSinceEpoch(),  QRandomGenerator::global()->bounded(1,250));
        ser_right_rod_pressure->append(timeRightPressure.toMSecsSinceEpoch(),  QRandomGenerator::global()->bounded(1,250));
        ser_right_zadan->append( timeRightZadan.toMSecsSinceEpoch(),  QRandomGenerator::global()->bounded(-100,100));
        ser_right_ZolotPosit->append(timeRightZolotPosit.toMSecsSinceEpoch(), QRandomGenerator::global()->bounded(-100,100));
        ser_right_ShtokPosit->append(timeRightShtokPosit.toMSecsSinceEpoch(), QRandomGenerator::global()->bounded(0,25));

    }

    chartRightShtokPosit->scroll( x_shtok, 0 );
    chartRightPressure->scroll(x, 0);
    chartRightZadan->scroll( x_zadan, 0 );
    chartRightZolotPosit->scroll( x_zolot, 0 );
}
void GraphPlot::handleCalcTabPlot()
{
    qreal x_PistonLoss = ui->tab_calc_gv_piston_loss->chart()->plotArea().width() / ax_X_PistonLoss->tickCount();
    qreal y_PistonLoss = ( ax_X_PistonLoss->max().toMSecsSinceEpoch() - ax_X_PistonLoss->min().toMSecsSinceEpoch() ) / ax_X_PistonLoss->tickCount();
    timePistonLoss = timePistonLoss.addMSecs(y_PistonLoss);

    qreal x_StockLoss = ui->tab_calc_gv_stock_loss->chart()->plotArea().width() / ax_X_StockLoss->tickCount();
    qreal y_StockLoss = ( ax_X_StockLoss->max().toMSecsSinceEpoch() - ax_X_StockLoss->min().toMSecsSinceEpoch() ) / ax_X_StockLoss->tickCount();
    timeStockLoss = timeStockLoss.addMSecs(y_StockLoss);

    qreal x_DiffForce = ui->tab_calc_gv_diff_force->chart()->plotArea().width() / ax_X_DiffForce->tickCount();
    qreal y_DiffForce = ( ax_X_DiffForce->max().toMSecsSinceEpoch() - ax_X_DiffForce->min().toMSecsSinceEpoch() ) / ax_X_DiffForce->tickCount();
    timeDiffForce = timeDiffForce.addMSecs(y_DiffForce);


    if( is_PLC_CALC_new_data ){
        ser_piston_loss_left->append(timePistonLoss.toMSecsSinceEpoch(), calc_left.GetPistonLosses());
        ser_piston_loss_right->append(timePistonLoss.toMSecsSinceEpoch(), calc_right.GetPistonLosses());

        ser_Stock_loss_left->append(timeStockLoss.toMSecsSinceEpoch(), calc_left.GetStockLosses());
        ser_Stock_loss_right->append(timeStockLoss.toMSecsSinceEpoch(), calc_right.GetStockLosses());

        ser_Diff_Force_left->append(timeDiffForce.toMSecsSinceEpoch(), calc_left.GetForce());
        ser_Diff_Force_right->append(timeDiffForce.toMSecsSinceEpoch(), calc_right.GetForce());

        is_PLC_CALC_new_data = false;
    }else{

        ser_piston_loss_left->append(timePistonLoss.toMSecsSinceEpoch(), QRandomGenerator::global()->bounded(0,20));
        ser_piston_loss_right->append(timePistonLoss.toMSecsSinceEpoch(), QRandomGenerator::global()->bounded(0,20));
        ser_Stock_loss_left->append(timeStockLoss.toMSecsSinceEpoch(), QRandomGenerator::global()->bounded(0,20));
        ser_Stock_loss_right->append(timeStockLoss.toMSecsSinceEpoch(), QRandomGenerator::global()->bounded(0,20));
        ser_Diff_Force_left->append(timeDiffForce.toMSecsSinceEpoch(), QRandomGenerator::global()->bounded(0,20));
        ser_Diff_Force_right->append(timeDiffForce.toMSecsSinceEpoch(), QRandomGenerator::global()->bounded(0,20));

    }

    chartDiffForce->scroll( x_DiffForce, 0 );
    chartPistonLoss->scroll( x_PistonLoss, 0 );
    chartStockLoss->scroll( x_StockLoss, 0 );
}

void GraphPlot::handle_fT_tfT()
{
    if( is_new_sensor ){
        fT_graph->ChartIncrement( ui->gb_fT_L->isChecked(), sens_cylinders.fT_L,
                                  ui->gb_fT_R->isChecked(), sens_cylinders.fT_R);
        tfT_graph->ChartIncrement( ui->gb_tfT_L->isChecked(), sens_cylinders.tfT_L,
                                   ui->gb_tfT_R->isChecked(), sens_cylinders.tfT_R );

    } else if(isDemo_fT_tfT) {
        fT_graph->ChartIncrement( ui->gb_fT_L->isChecked(), QRandomGenerator::global()->bounded(0,10),
                                  ui->gb_fT_R->isChecked(), QRandomGenerator::global()->bounded(0,10));

        tfT_graph->ChartIncrement( ui->gb_tfT_L->isChecked(), QRandomGenerator::global()->bounded(0,5),
                                   ui->gb_tfT_R->isChecked(), QRandomGenerator::global()->bounded(0,5) );
    }

    fT_graph->ChartScroll(  ui->view_fT->chart()->plotArea().width()  );
    tfT_graph->ChartScroll( ui->view_tfT->chart()->plotArea().width() );
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
void GraphPlot::TabGraphShowingCalc()
{
    if( ui->cb_calc_tab_diff_force->isChecked() ){
        ui->tab_calc_gv_diff_force->show();
    }
    else{
        ui->tab_calc_gv_diff_force->hide();
    }
    if( ui->cb_calc_tab_piston_loss->isChecked() ){
        ui->tab_calc_gv_piston_loss->show();
    }
    else{
        ui->tab_calc_gv_piston_loss->hide();
    }
    if( ui->cb_calc_tab_stock_loss->isChecked() ){
        ui->tab_calc_gv_stock_loss->show();
    }
    else{
        ui->tab_calc_gv_stock_loss->hide();
    }
}

void GraphPlot::SetGraphDiffForce()
{
    ser_Diff_Force_left = new QLineSeries();
    ser_Diff_Force_left->setName("Левый");

    ser_Diff_Force_right = new QLineSeries();
    ser_Diff_Force_right->setName("Правый");

    QDateTime temp_time = timeDiffForce;
    ser_Diff_Force_left->append(timeDiffForce.toMSecsSinceEpoch(), 0);
    ser_Diff_Force_right->append(timeDiffForce.toMSecsSinceEpoch(), 0);

    for( int i = 1; i <= 10; i++ ){
        timeDiffForce = timeDiffForce.addSecs(1);
        ser_Diff_Force_left->append(timeDiffForce.toMSecsSinceEpoch(), 0);
        ser_Diff_Force_right->append(timeDiffForce.toMSecsSinceEpoch(), 0);
    }

    ax_X_DiffForce = new QDateTimeAxis;
    ax_Y_DiffForce = new QValueAxis;
    ax_X_DiffForce->setTitleText("Время, сек");
    ax_X_DiffForce->setFormat("hh:mm:ss");
    ax_Y_DiffForce->setTitleText("Отношение");

    chartDiffForce = new QChart();

    chartDiffForce = new QChart();
    chartDiffForce->legend()->setAlignment(Qt::AlignRight);
    chartDiffForce->legend()->show();
    chartDiffForce->addSeries(ser_Diff_Force_left);
    chartDiffForce->addSeries(ser_Diff_Force_right);
    chartDiffForce->addAxis(ax_X_DiffForce, Qt::AlignBottom);
    chartDiffForce->addAxis(ax_Y_DiffForce, Qt::AlignLeft);
    chartDiffForce->setAnimationOptions(QChart::SeriesAnimations);

    ser_Diff_Force_left->attachAxis( ax_X_DiffForce );
    ser_Diff_Force_left->attachAxis( ax_Y_DiffForce );

    ser_Diff_Force_right->attachAxis( ax_X_DiffForce );
    ser_Diff_Force_right->attachAxis( ax_Y_DiffForce );

    ax_X_DiffForce->setRange(temp_time, timeDiffForce.addSecs(1));
    ax_Y_DiffForce->setRange(0, 20);
    ax_X_DiffForce->setTickCount(10);
    ax_Y_DiffForce->setTickCount(5);
    chartDiffForce->setTitle("Отношение эталонного дифф. усилия к реальному");

    ui->tab_calc_gv_diff_force->setChart(chartDiffForce);
    ui->tab_calc_gv_diff_force->setRenderHint(QPainter::Antialiasing);
}
void GraphPlot::SetGraphStockLoss()
{
    ser_Stock_loss_left = new QLineSeries();
    ser_Stock_loss_left->setName("Левый");

    ser_Stock_loss_right = new QLineSeries();
    ser_Stock_loss_right->setName("Правый");

    QDateTime temp_time = timeStockLoss;
    ser_Stock_loss_left->append(timeStockLoss.toMSecsSinceEpoch(), 0);
    ser_Stock_loss_right->append(timeStockLoss.toMSecsSinceEpoch(), 0);

    for( int i = 1; i <= 10; i++ ){
        timeStockLoss = timeStockLoss.addSecs(1);
        ser_Stock_loss_left->append(timeStockLoss.toMSecsSinceEpoch(), 0);
        ser_Stock_loss_right->append(timeStockLoss.toMSecsSinceEpoch(), 0);
    }

    ax_X_StockLoss = new QDateTimeAxis;
    ax_Y_StockLoss = new QValueAxis;
    ax_X_StockLoss->setTitleText("Время, сек");
    ax_X_StockLoss->setFormat("hh:mm:ss");
    ax_Y_StockLoss->setTitleText("Потери, %");

    chartStockLoss = new QChart();

    chartStockLoss = new QChart();
    chartStockLoss->legend()->setAlignment(Qt::AlignRight);
    chartStockLoss->legend()->show();
    chartStockLoss->addSeries(ser_Stock_loss_left);
    chartStockLoss->addSeries(ser_Stock_loss_right);
    chartStockLoss->addAxis(ax_X_StockLoss, Qt::AlignBottom);
    chartStockLoss->addAxis(ax_Y_StockLoss, Qt::AlignLeft);
    chartStockLoss->setAnimationOptions(QChart::SeriesAnimations);

    ser_Stock_loss_left->attachAxis( ax_X_StockLoss );
    ser_Stock_loss_left->attachAxis( ax_Y_StockLoss );

    ser_Stock_loss_right->attachAxis( ax_X_StockLoss );
    ser_Stock_loss_right->attachAxis( ax_Y_StockLoss );

    ax_X_StockLoss->setRange(temp_time, timeStockLoss.addSecs(1));
    ax_Y_StockLoss->setRange(0, 20);
    ax_X_StockLoss->setTickCount(10);
    ax_Y_StockLoss->setTickCount(5);
    chartStockLoss->setTitle("Процент потерь расхода в штоковой полости");

    ui->tab_calc_gv_stock_loss->setChart(chartStockLoss);
    ui->tab_calc_gv_stock_loss->setRenderHint(QPainter::Antialiasing);
}
void GraphPlot::SetGraphPistonLoss()
{
    ser_piston_loss_left = new QLineSeries();
    ser_piston_loss_left->setName("Левый");

    ser_piston_loss_right = new QLineSeries();
    ser_piston_loss_right->setName("Правый");

     QDateTime temp_time = timePistonLoss;
     ser_piston_loss_left->append(timePistonLoss.toMSecsSinceEpoch(), 0);
     ser_piston_loss_right->append(timePistonLoss.toMSecsSinceEpoch(), 0);

     for( int i = 1; i <= 10; i++ ){
         timePistonLoss = timePistonLoss.addSecs(1);
         ser_piston_loss_left->append(timePistonLoss.toMSecsSinceEpoch(), 0);
         ser_piston_loss_right->append(timePistonLoss.toMSecsSinceEpoch(), 0);
     }

     ax_X_PistonLoss = new QDateTimeAxis;
     ax_Y_PistonLoss = new QValueAxis;
     ax_X_PistonLoss->setTitleText("Время, сек");
     ax_X_PistonLoss->setFormat("hh:mm:ss");
     ax_Y_PistonLoss->setTitleText("Потери, %");

     chartPistonLoss = new QChart();

     chartPistonLoss = new QChart();
     chartPistonLoss->legend()->setAlignment(Qt::AlignRight);
     chartPistonLoss->legend()->show();
     chartPistonLoss->addSeries(ser_piston_loss_left);
     chartPistonLoss->addSeries(ser_piston_loss_right);
     chartPistonLoss->addAxis(ax_X_PistonLoss, Qt::AlignBottom);
     chartPistonLoss->addAxis(ax_Y_PistonLoss, Qt::AlignLeft);
     chartPistonLoss->setAnimationOptions(QChart::SeriesAnimations);

     ser_piston_loss_left->attachAxis( ax_X_PistonLoss );
     ser_piston_loss_left->attachAxis( ax_Y_PistonLoss );

     ser_piston_loss_right->attachAxis( ax_X_PistonLoss );
     ser_piston_loss_right->attachAxis( ax_Y_PistonLoss );

     ax_X_PistonLoss->setRange(temp_time, timePistonLoss.addSecs(1));
     ax_Y_PistonLoss->setRange(0, 20);
     ax_X_PistonLoss->setTickCount(10);
     ax_Y_PistonLoss->setTickCount(5);
     chartPistonLoss->setTitle("Процент потерь расхода в поршневой полости");

     ui->tab_calc_gv_piston_loss->setChart(chartPistonLoss);
     ui->tab_calc_gv_piston_loss->setRenderHint(QPainter::Antialiasing);

}

void GraphPlot::SensorDataUpdate(SensorPack pack)
{
    double _ft_R = 0.0;
    if( ui->chb_filt_fT_R->isChecked() ){
        _ft_R = (ui->rb_adc1_fT_R->isChecked()) ?  pack.adc1_filtered_data.at(ui->cb_adc1_fT_R->currentIndex())
                                                :  pack.adc3_filtered_data.at(ui->cb_adc3_fT_R->currentIndex()) ;
    } else {
        _ft_R = (ui->rb_adc1_fT_R->isChecked()) ?  pack.adc1_data.at(ui->cb_adc1_fT_R->currentIndex())
                                                :  pack.adc3_data.at(ui->cb_adc3_fT_R->currentIndex()) ;
    }    

    double _ft_L = 0.0;
    if( ui->chb_filt_fT_L->isChecked() ){
        _ft_L = (ui->rb_adc1_fT_L->isChecked()) ?  pack.adc1_filtered_data.at(ui->cb_adc1_fT_L->currentIndex())
                                                :  pack.adc3_filtered_data.at(ui->cb_adc3_fT_L->currentIndex()) ;
    } else {
        _ft_L = (ui->rb_adc1_fT_L->isChecked()) ?  pack.adc1_data.at(ui->cb_adc1_fT_L->currentIndex())
                                                :  pack.adc3_data.at(ui->cb_adc3_fT_L->currentIndex()) ;
    }

    double _tfT_R = 0.0;
    if( ui->chb_filt_tfT_R->isChecked() ){
        _tfT_R   = (ui->rb_adc1_tfT_R->isChecked()) ? pack.adc1_filtered_data.at(ui->cb_adc1_tfT_R->currentIndex())
                                                    : pack.adc3_filtered_data.at(ui->cb_adc1_tfT_R->currentIndex());
    } else {
        _tfT_R   = (ui->rb_adc1_tfT_R->isChecked()) ? pack.adc1_data.at(ui->cb_adc1_tfT_R->currentIndex())
                                                    : pack.adc3_data.at(ui->cb_adc1_tfT_R->currentIndex());
    }

    double _tfT_L = 0.0;
    if( ui->chb_filt_tfT_L->isChecked() ){
        _tfT_L   = (ui->rb_adc1_tfT_L->isChecked()) ? pack.adc1_filtered_data.at(ui->cb_adc1_tfT_L->currentIndex())
                                                    : pack.adc3_filtered_data.at(ui->cb_adc1_tfT_L->currentIndex());
    } else {
        _tfT_L   = (ui->rb_adc1_tfT_L->isChecked()) ? pack.adc1_data.at(ui->cb_adc1_tfT_L->currentIndex())
                                                    : pack.adc3_data.at(ui->cb_adc1_tfT_L->currentIndex());
    }

    sens_cylinders.fT_R = map( _ft_R, 0, 4095, 0, 100 );
    sens_cylinders.fT_L = map( _ft_L, 0, 4095, 0, 100 );
    sens_cylinders.tfT_R = map( _tfT_R, 0, 4095, -10, 80 );
    sens_cylinders.tfT_L = map( _tfT_L, 0, 4095, -10, 80 );
    is_new_sensor = true;
}

//void GraphPlot::PLC_DataUpdate(PLC_Data data)
//{

//    calc_left.InitValues(data.left);
//    calc_right.InitValues(data.right);
//    is_PLC_CALC_new_data = true;

//    plc_Data = data;
//    is_PLC_new_data_left = true  ;
//    is_PLC_new_data_right = true ;
//}

