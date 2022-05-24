#include "GraphPlot.h"
#include "ui_GraphPlot.h"

GraphPlot::GraphPlot(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GraphPlot)
{
    ui->setupUi(this);
    sens_data = SensorVals();

    LoadConfigure();

    counter = 0;
    switcher = false;
    is_Check = false;

    isDemo_fT_tfT = false;

    is_new_sensor = false;
    is_new_force = false;

    is_PLC_CALC_new_data = false;
    is_PLC_new_data_left = false;
    is_PLC_new_data_right = false;

    pA_graph = new UniqueGraph( "Давление в поршневой полости", this);
    pB_graph = new UniqueGraph( "Давление в штоковой полости", this);
    sY_graph = new UniqueGraph( "Задание на проп.клапан", this);
    fY_graph = new UniqueGraph( "Обратная связь положения золотника проп. клапана", this);
    fS_graph = new UniqueGraph( "Обратная связь положения штока", this);

    pA_graph->Configure("Давление, бар", QPoint(0,250), 6);
    pB_graph->Configure("Давление, бар", QPoint(0,250), 6);
    fY_graph->Configure("Положение, %", QPoint(-100,100), 6);
    sY_graph->Configure("Задание, %", QPoint(-100,100), 9);
    fS_graph->Configure("Положение, мм",QPoint(0,25), 6);
    ui->view_pA->setChart(pA_graph->chart);
    ui->view_pB->setChart(pB_graph->chart);
    ui->view_fY->setChart(fY_graph->chart);
    ui->view_sY->setChart(sY_graph->chart);
    ui->view_fS->setChart(fS_graph->chart);
    ui->view_pA->setRenderHint( QPainter::Antialiasing);
    ui->view_pB->setRenderHint( QPainter::Antialiasing);
    ui->view_fY->setRenderHint( QPainter::Antialiasing);
    ui->view_sY->setRenderHint( QPainter::Antialiasing);

    fT_graph  = new UniqueGraph( "Объемный расход", this);
    tfT_graph = new UniqueGraph( "Температура расхода", this);
    fT_graph->Configure( "Расход, л/мин",    QPoint(0,100), 11);
    tfT_graph->Configure("Температура, град",QPoint(0,100), 11);
    ui->view_fT->setChart(   fT_graph->chart);
    ui->view_tfT->setChart( tfT_graph->chart );
    ui->view_fT->setRenderHint( QPainter::Antialiasing);
    ui->view_tfT->setRenderHint(QPainter::Antialiasing);


    sensor = new Sensor(2607, QHostAddress("192.168.1.20"), 8888, this);
    connect( sensor, &Sensor::sensorPackReceive, this, &GraphPlot::SensorDataUpdate );

    plc = new PLC_Connector(2688, QHostAddress("192.168.1.31"), 8888, this);
//    connect( plc, &PLC_Connector::plcDataReceive, this, &GraphPlot::PLC_DataUpdate );

    QDateTime all(QDateTime::currentDateTime().date(), QDateTime::currentDateTime().time().addSecs(-10));
    timeRightPressure
            = timeRightZadan = timeRightZolotPosit = timeRightShtokPosit =
            timePistonLoss = timeStockLoss = timeDiffForce = all;

//    SetGraphForce();
//    SetGraphZadanLeft();
//    SetGraphZolotPositionLeft();
//    SetGraphShtokPositionLeft();

    SetGraphPressureRight();
    SetGraphZadanRight();
    SetGraphZolotPositionRight();
    SetGraphShtokPositionRight();

    SetGraphPistonLoss();
    SetGraphStockLoss();
    SetGraphDiffForce();


    connect( ui->chb_pA, &QCheckBox::clicked, this, &GraphPlot::TabGraphShowingLeft );
    connect( ui->chb_pB, &QCheckBox::clicked, this, &GraphPlot::TabGraphShowingLeft );
    connect( ui->chb_fY, &QCheckBox::clicked, this, &GraphPlot::TabGraphShowingLeft );
    connect( ui->chb_sY, &QCheckBox::clicked, this, &GraphPlot::TabGraphShowingLeft );

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
    connect( timer, &QTimer::timeout, this, &GraphPlot::handle_fT_tfT );
    connect( timer, &QTimer::timeout, this, &GraphPlot::handle_pA_pB_fY_sY );

//    connect( timer, &QTimer::timeout, this, &GraphPlot::handleForcePlot );
    connect( timer, &QTimer::timeout, this, &GraphPlot::handleRightTabPlot );
    connect( timer, &QTimer::timeout, this, &GraphPlot::handleCalcTabPlot );
    timer->start();
}

GraphPlot::~GraphPlot()
{
    SaveConfigure();
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
    ui->view_pA->chart()->setTheme(theme);
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

// объединил fT и tfT в один обработчик, так как приходят одновременно с контроллера
void GraphPlot::handle_fT_tfT()
{
    if( is_new_sensor ){
        fT_graph->ChartIncrement_if( ui->gb_fT_L->isChecked(), sens_data.fT_L,
                                  ui->gb_fT_R->isChecked(), sens_data.fT_R);
        tfT_graph->ChartIncrement_if( ui->gb_tfT_L->isChecked(), sens_data.tfT_L,
                                   ui->gb_tfT_R->isChecked(), sens_data.tfT_R );

    }
    else if(isDemo_fT_tfT) {
        fT_graph->ChartIncrement_if( ui->gb_fT_L->isChecked(), QRandomGenerator::global()->bounded(0,10),
                                  ui->gb_fT_R->isChecked(), QRandomGenerator::global()->bounded(0,10));

        tfT_graph->ChartIncrement_if( ui->gb_tfT_L->isChecked(), QRandomGenerator::global()->bounded(0,5),
                                   ui->gb_tfT_R->isChecked(), QRandomGenerator::global()->bounded(0,5) );
    }

    fT_graph->ChartScroll(  ui->view_fT->chart()->plotArea().width()  );
    tfT_graph->ChartScroll( ui->view_tfT->chart()->plotArea().width() );
}
void GraphPlot::handle_pA_pB_fY_sY()
{
    pA_graph->ChartIncrement( QRandomGenerator::global()->bounded(0,40),
                              QRandomGenerator::global()->bounded(0,40));

    pB_graph->ChartIncrement( QRandomGenerator::global()->bounded(0,40),
                              QRandomGenerator::global()->bounded(0,40));

    fY_graph->ChartIncrement( QRandomGenerator::global()->bounded(-40,40),
                              QRandomGenerator::global()->bounded(-40,40));

    sY_graph->ChartIncrement( QRandomGenerator::global()->bounded(-40,40),
                              QRandomGenerator::global()->bounded(-40,40));


    fS_graph->ChartIncrement( QRandomGenerator::global()->bounded(0,15),
                              QRandomGenerator::global()->bounded(0,15));


    pA_graph->ChartScroll(  ui->view_pA->chart()->plotArea().width()  );
    pB_graph->ChartScroll(  ui->view_pB->chart()->plotArea().width()  );
    fY_graph->ChartScroll(  ui->view_fY->chart()->plotArea().width()  );
    sY_graph->ChartScroll(  ui->view_sY->chart()->plotArea().width()  );
    fS_graph->ChartScroll(  ui->view_fS->chart()->plotArea().width()  );

}

void GraphPlot::TabGraphShowingLeft()
{
    if( ui->chb_sY->isChecked() ){
        ui->view_sY->show();
    }
    else{
        ui->view_sY->hide();
    }
    if( ui->chb_fY->isChecked() ){
        ui->view_fY->show();
    }
    else{
        ui->view_fY->hide();
    }
    if( ui->chb_pA->isChecked() ){
        ui->view_pA->show();
    }
    else{
        ui->view_pA->hide();
    }
    if( ui->chb_pB->isChecked() ){
        ui->view_pB->show();
    }
    else{
        ui->view_pB->hide();
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

    sens_data.fT_R = map( _ft_R, 0, 4095, 0, 100 );
    sens_data.fT_L = map( _ft_L, 0, 4095, 0, 100 );
    sens_data.tfT_R = map( _tfT_R, 0, 4095, -10, 80 );
    sens_data.tfT_L = map( _tfT_L, 0, 4095, -10, 80 );
    is_new_sensor = true;
}


// Утилиты для сохранения конфигов
void GraphPlot::LoadConfigure()
{
    QFile file;
    file.setFileName(configFile);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text) ){
        qDebug() << "ERROR WHEN OPENING CONFIG FILE" << configFile << file.errorString();
    }
    QJsonParseError err;
    QJsonDocument src = QJsonDocument::fromJson( QString(file.readAll()).toUtf8(), &err);
    file.close();

    QJsonObject jsonParse = src.object().value( "gb_fT_L" ).toObject();
    ui->gb_fT_L->setChecked( jsonParse.value( "isChecked" ).toBool() );
    ui->rb_adc1_fT_L->setChecked( jsonParse.value( "isAdc1" ).toBool() );
    ui->rb_adc3_fT_L->setChecked( jsonParse.value( "isAdc3" ).toBool() );
    ui->cb_adc1_fT_L->setCurrentIndex( jsonParse.value( "adc1index" ).toInt() );
    ui->cb_adc3_fT_L->setCurrentIndex( jsonParse.value( "adc3index" ).toInt() );
    ui->chb_filt_fT_L->setChecked(jsonParse.value( "isFilter"  ).toBool() );

    jsonParse = src.object().value( "gb_fT_R" ).toObject();
    ui->gb_fT_R->setChecked( jsonParse.value( "isChecked" ).toBool() );
    ui->rb_adc1_fT_R->setChecked( jsonParse.value( "isAdc1" ).toBool() );
    ui->rb_adc3_fT_R->setChecked( jsonParse.value( "isAdc3" ).toBool() );
    ui->cb_adc1_fT_R->setCurrentIndex( jsonParse.value( "adc1index" ).toInt() );
    ui->cb_adc3_fT_R->setCurrentIndex( jsonParse.value( "adc3index" ).toInt() );
    ui->chb_filt_fT_R->setChecked(jsonParse.value( "isFilter"  ).toBool() );

    jsonParse = src.object().value( "gb_tfT_L" ).toObject();
    ui->gb_tfT_L->setChecked( jsonParse.value( "isChecked" ).toBool() );
    ui->rb_adc1_tfT_L->setChecked( jsonParse.value( "isAdc1" ).toBool() );
    ui->rb_adc3_tfT_L->setChecked( jsonParse.value( "isAdc3" ).toBool() );
    ui->cb_adc1_tfT_L->setCurrentIndex( jsonParse.value( "adc1index" ).toInt() );
    ui->cb_adc3_tfT_L->setCurrentIndex( jsonParse.value( "adc3index" ).toInt() );
    ui->chb_filt_tfT_L->setChecked(jsonParse.value( "isFilter"  ).toBool() );

    jsonParse = src.object().value( "gb_tfT_R" ).toObject();
    ui->gb_tfT_R->setChecked( jsonParse.value( "isChecked" ).toBool() );
    ui->rb_adc1_tfT_R->setChecked( jsonParse.value( "isAdc1" ).toBool() );
    ui->rb_adc3_tfT_R->setChecked( jsonParse.value( "isAdc3" ).toBool() );
    ui->cb_adc1_tfT_R->setCurrentIndex( jsonParse.value( "adc1index" ).toInt() );
    ui->cb_adc3_tfT_R->setCurrentIndex( jsonParse.value( "adc3index" ).toInt() );
    ui->chb_filt_tfT_R->setChecked(jsonParse.value( "isFilter"  ).toBool() );
}
void GraphPlot::SaveConfigure()
{
    QJsonObject fT_L_conf = QJsonObject();
    fT_L_conf.insert( "isAdc1" ,   ui->rb_adc1_fT_L->isChecked() ) ;
    fT_L_conf.insert( "isAdc3"  ,  ui->rb_adc3_fT_L->isChecked() );
    fT_L_conf.insert( "isFilter" , ui->chb_filt_fT_L->isChecked() );
    fT_L_conf.insert( "adc1index", ui->cb_adc1_fT_L->currentIndex() );
    fT_L_conf.insert( "adc3index", ui->cb_adc3_fT_L->currentIndex() );
    fT_L_conf.insert( "isChecked", ui->gb_fT_L->isChecked());

    QJsonObject fT_R_conf = QJsonObject();
    fT_R_conf.insert( "isAdc1" ,   ui->rb_adc1_fT_R->isChecked() ) ;
    fT_R_conf.insert( "isAdc3"  ,  ui->rb_adc3_fT_R->isChecked() );
    fT_R_conf.insert( "isFilter" , ui->chb_filt_fT_R->isChecked() );
    fT_R_conf.insert( "adc1index", ui->cb_adc1_fT_R->currentIndex() );
    fT_R_conf.insert( "adc3index", ui->cb_adc3_fT_R->currentIndex() );
    fT_R_conf.insert( "isChecked", ui->gb_fT_R->isChecked());

    QJsonObject tfT_L_conf = QJsonObject();
    tfT_L_conf.insert( "isAdc1" ,   ui->rb_adc1_tfT_L->isChecked() ) ;
    tfT_L_conf.insert( "isAdc3"  ,  ui->rb_adc3_tfT_L->isChecked() );
    tfT_L_conf.insert( "isFilter" , ui->chb_filt_tfT_L->isChecked() );
    tfT_L_conf.insert( "adc1index", ui->cb_adc1_tfT_L->currentIndex() );
    tfT_L_conf.insert( "adc3index", ui->cb_adc3_tfT_L->currentIndex() );
    tfT_L_conf.insert( "isChecked", ui->gb_tfT_L->isChecked());

    QJsonObject tfT_R_conf = QJsonObject();
    tfT_R_conf.insert( "isAdc1" ,   ui->rb_adc1_tfT_R->isChecked() ) ;
    tfT_R_conf.insert( "isAdc3"  ,  ui->rb_adc3_tfT_R->isChecked() );
    tfT_R_conf.insert( "isFilter" , ui->chb_filt_tfT_R->isChecked() );
    tfT_R_conf.insert( "adc1index", ui->cb_adc1_tfT_R->currentIndex() );
    tfT_R_conf.insert( "adc3index", ui->cb_adc3_tfT_R->currentIndex() );
    tfT_R_conf.insert( "isChecked", ui->gb_tfT_R->isChecked());

    QJsonObject mini_root = QJsonObject();
    mini_root.insert( "gb_fT_L",  fT_L_conf);
    mini_root.insert( "gb_fT_R",  fT_R_conf);
    mini_root.insert( "gb_tfT_L",  tfT_L_conf);
    mini_root.insert( "gb_tfT_R",  tfT_R_conf);


    QJsonDocument doc = QJsonDocument();
    doc.setObject(mini_root);
    QFile file;
    file.setFileName(configFile);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate) ){
        qDebug() << "ERROR WHEN OPENING CONFIG FILE" << configFile << file.errorString();
    }
    file.write(doc.toJson());
    file.close();
    qDebug() << "Save configure";
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

