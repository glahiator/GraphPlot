#include "GraphPlot.h"
#include "ui_GraphPlot.h"

GraphPlot::GraphPlot(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GraphPlot)
{
    ui->setupUi(this);
    sens_data = SensorVals();
    cylinders = plc_cylinder();
    pumps = plc_pump();

    LoadConfigure();
    ui->btn_cylinder_stop->setEnabled(false);
    ui->btn_pump_stop->setEnabled(false);

    counter = 0;
    switcher = false;
    is_Check = false;

    isDemo_fT_tfT = false;
    isDemo_pA_pB_fY_sY = false;
    isDemo_pP_fD_tfD_tfdS = false;

    is_new_cylinder = false;
    is_new_pump = false;
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

    connect( ui->btn_cylinder_start, &QPushButton::clicked, this, &GraphPlot::StartCylinderGraphs );
    connect( ui->btn_cylinder_stop,  &QPushButton::clicked, this, &GraphPlot::StopCylinderGraphs );
    connect( ui->chb_demo_cylinders, &QCheckBox::clicked, this, [=]{
        isDemo_pA_pB_fY_sY = ui->chb_demo_cylinders->isChecked();
    });

    pP_graph   = new UniqueGraph( "Давление в напорной линии", this);
    fD_graph   = new UniqueGraph( "Объемный расход в дренаже", this);
    tfD_graph  = new UniqueGraph( "Температура расхода дренажа", this);
    tfdS_graph = new UniqueGraph( "Положение шайбы", this);
    CAN_graph  = new UniqueGraph( "Уровень загрязнения рабочей жидкости", this);
    connect( ui->btn_pump_start, &QPushButton::clicked, this, &GraphPlot::StartPumpGraphs );
    connect( ui->btn_pump_stop,  &QPushButton::clicked, this, &GraphPlot::StopPumpGraphs );
    connect( ui->chb_demo_pump, &QCheckBox::clicked, this, [=]{
        isDemo_pP_fD_tfD_tfdS = ui->chb_demo_pump->isChecked();
    });

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
    connect( plc, &PLC_Connector::ready_cylinder, this, &GraphPlot::update_cylinder );

    QDateTime all(QDateTime::currentDateTime().date(), QDateTime::currentDateTime().time().addSecs(-10));

    timePistonLoss = timeStockLoss = timeDiffForce = all;


    SetGraphPistonLoss();
    SetGraphStockLoss();
    SetGraphDiffForce();

    connect( ui->sb_pA_diap_max, QOverload<int>::of(&QSpinBox::valueChanged), this, &GraphPlot::SetGraphDiap );
    connect( ui->sb_pA_diap_min, QOverload<int>::of(&QSpinBox::valueChanged), this, &GraphPlot::SetGraphDiap );
    connect( ui->sb_pB_diap_max, QOverload<int>::of(&QSpinBox::valueChanged), this, &GraphPlot::SetGraphDiap );
    connect( ui->sb_pB_diap_min, QOverload<int>::of(&QSpinBox::valueChanged), this, &GraphPlot::SetGraphDiap );
    connect( ui->sb_sY_diap_max, QOverload<int>::of(&QSpinBox::valueChanged), this, &GraphPlot::SetGraphDiap );
    connect( ui->sb_sY_diap_min, QOverload<int>::of(&QSpinBox::valueChanged), this, &GraphPlot::SetGraphDiap );
    connect( ui->sb_fY_diap_max, QOverload<int>::of(&QSpinBox::valueChanged), this, &GraphPlot::SetGraphDiap );
    connect( ui->sb_fY_diap_min, QOverload<int>::of(&QSpinBox::valueChanged), this, &GraphPlot::SetGraphDiap );
    connect( ui->sb_fS_diap_max, QOverload<int>::of(&QSpinBox::valueChanged), this, &GraphPlot::SetGraphDiap );
    connect( ui->sb_fS_diap_min, QOverload<int>::of(&QSpinBox::valueChanged), this, &GraphPlot::SetGraphDiap );

    connect( ui->sb_pP_diap_max,   QOverload<int>::of(&QSpinBox::valueChanged), this, &GraphPlot::SetGraphDiap );
    connect( ui->sb_pP_diap_min,   QOverload<int>::of(&QSpinBox::valueChanged), this, &GraphPlot::SetGraphDiap );
    connect( ui->sb_fD_diap_max,   QOverload<int>::of(&QSpinBox::valueChanged), this, &GraphPlot::SetGraphDiap );
    connect( ui->sb_fD_diap_min,   QOverload<int>::of(&QSpinBox::valueChanged), this, &GraphPlot::SetGraphDiap );
    connect( ui->sb_tfD_diap_max,  QOverload<int>::of(&QSpinBox::valueChanged), this, &GraphPlot::SetGraphDiap );
    connect( ui->sb_tfD_diap_min,  QOverload<int>::of(&QSpinBox::valueChanged), this, &GraphPlot::SetGraphDiap );
    connect( ui->sb_tfdS_diap_max, QOverload<int>::of(&QSpinBox::valueChanged), this, &GraphPlot::SetGraphDiap );
    connect( ui->sb_tfdS_diap_min, QOverload<int>::of(&QSpinBox::valueChanged), this, &GraphPlot::SetGraphDiap );
    connect( ui->sb_CAN_diap_max,  QOverload<int>::of(&QSpinBox::valueChanged), this, &GraphPlot::SetGraphDiap );
    connect( ui->sb_CAN_diap_min,  QOverload<int>::of(&QSpinBox::valueChanged), this, &GraphPlot::SetGraphDiap );

    connect( ui->sb_pA_discret,   QOverload<int>::of(&QSpinBox::valueChanged), this, &GraphPlot::SetGraphStickCount );
    connect( ui->sb_pB_discret,   QOverload<int>::of(&QSpinBox::valueChanged), this, &GraphPlot::SetGraphStickCount );
    connect( ui->sb_sY_discret,  QOverload<int>::of(&QSpinBox::valueChanged), this, &GraphPlot::SetGraphStickCount );
    connect( ui->sb_fY_discret, QOverload<int>::of(&QSpinBox::valueChanged), this, &GraphPlot::SetGraphStickCount );
    connect( ui->sb_fS_discret,  QOverload<int>::of(&QSpinBox::valueChanged), this, &GraphPlot::SetGraphStickCount );

    connect( ui->sb_pP_discret,   QOverload<int>::of(&QSpinBox::valueChanged), this, &GraphPlot::SetGraphStickCount );
    connect( ui->sb_fD_discret,   QOverload<int>::of(&QSpinBox::valueChanged), this, &GraphPlot::SetGraphStickCount );
    connect( ui->sb_tfD_discret,  QOverload<int>::of(&QSpinBox::valueChanged), this, &GraphPlot::SetGraphStickCount );
    connect( ui->sb_tfdS_discret, QOverload<int>::of(&QSpinBox::valueChanged), this, &GraphPlot::SetGraphStickCount );
    connect( ui->sb_CAN_discret,  QOverload<int>::of(&QSpinBox::valueChanged), this, &GraphPlot::SetGraphStickCount );


    connect( ui->chb_pA, &QCheckBox::clicked, this, &GraphPlot::SetGraphsShow );
    connect( ui->chb_pB, &QCheckBox::clicked, this, &GraphPlot::SetGraphsShow );
    connect( ui->chb_fY, &QCheckBox::clicked, this, &GraphPlot::SetGraphsShow );
    connect( ui->chb_sY, &QCheckBox::clicked, this, &GraphPlot::SetGraphsShow );
    connect( ui->chb_fS, &QCheckBox::clicked, this, &GraphPlot::SetGraphsShow );

    connect( ui->chb_pP,   &QCheckBox::clicked, this, &GraphPlot::SetGraphsShow );
    connect( ui->chb_fD,   &QCheckBox::clicked, this, &GraphPlot::SetGraphsShow );
    connect( ui->chb_tfD,  &QCheckBox::clicked, this, &GraphPlot::SetGraphsShow );
    connect( ui->chb_tfdS, &QCheckBox::clicked, this, &GraphPlot::SetGraphsShow );
    connect( ui->chb_CAN,  &QCheckBox::clicked, this, &GraphPlot::SetGraphsShow );


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


//    connect( timer, &QTimer::timeout, this, &GraphPlot::handleForcePlot );
//    connect( timer, &QTimer::timeout, this, &GraphPlot::handleRightTabPlot );
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

    jsonParse = src.object().value( "pA_graph" ).toObject();
   ui->chb_pA->setChecked(       jsonParse.value( "isChecked" ).toBool() );
    ui->sb_pA_diap_min->setValue( jsonParse.value( "y_diap_min" ).toInt() );
    ui->sb_pA_diap_max->setValue( jsonParse.value( "y_diap_max" ).toInt() );
    ui->sb_pA_discret->setValue(  jsonParse.value( "y_tick" ).toInt() );

    jsonParse = src.object().value( "pB_graph" ).toObject();
   ui->chb_pB->setChecked(       jsonParse.value( "isChecked" ).toBool() );
    ui->sb_pB_diap_min->setValue( jsonParse.value( "y_diap_min" ).toInt() );
    ui->sb_pB_diap_max->setValue( jsonParse.value( "y_diap_max" ).toInt() );
    ui->sb_pB_discret->setValue(  jsonParse.value( "y_tick" ).toInt() );

    jsonParse = src.object().value( "sY_graph" ).toObject();
   ui->chb_sY->setChecked(       jsonParse.value( "isChecked" ).toBool() );
    ui->sb_sY_diap_min->setValue( jsonParse.value( "y_diap_min" ).toInt() );
    ui->sb_sY_diap_max->setValue( jsonParse.value( "y_diap_max" ).toInt() );
    ui->sb_sY_discret->setValue(  jsonParse.value( "y_tick" ).toInt() );

    jsonParse = src.object().value( "fS_graph" ).toObject();
   ui->chb_fS->setChecked(       jsonParse.value( "isChecked" ).toBool() );
    ui->sb_fS_diap_min->setValue( jsonParse.value( "y_diap_min" ).toInt() );
    ui->sb_fS_diap_max->setValue( jsonParse.value( "y_diap_max" ).toInt() );
    ui->sb_fS_discret->setValue(  jsonParse.value( "y_tick" ).toInt() );

    jsonParse = src.object().value( "fY_graph" ).toObject();
   ui->chb_fY->setChecked(       jsonParse.value( "isChecked" ).toBool() );
    ui->sb_fY_diap_min->setValue( jsonParse.value( "y_diap_min" ).toInt() );
    ui->sb_fY_diap_max->setValue( jsonParse.value( "y_diap_max" ).toInt() );
    ui->sb_fY_discret->setValue(  jsonParse.value( "y_tick" ).toInt() );

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

    QJsonObject pA_graph_conf = QJsonObject();
    pA_graph_conf.insert( "isChecked",  ui->chb_pA->isChecked() );
    pA_graph_conf.insert( "y_diap_min", ui->sb_pA_diap_min->value() );
    pA_graph_conf.insert( "y_diap_max", ui->sb_pA_diap_max->value()  );
    pA_graph_conf.insert( "y_tick",     ui->sb_pA_discret->value()  );

    QJsonObject pB_graph_conf = QJsonObject();
    pB_graph_conf.insert( "isChecked", ui->chb_pB->isChecked() );
    pB_graph_conf.insert( "y_diap_min", ui->sb_pB_diap_min->value() );
    pB_graph_conf.insert( "y_diap_max", ui->sb_pB_diap_max->value()  );
    pB_graph_conf.insert( "y_tick",     ui->sb_pB_discret->value()  );

    QJsonObject sY_graph_conf = QJsonObject();
    sY_graph_conf.insert( "isChecked", ui->chb_sY->isChecked() );
    sY_graph_conf.insert( "y_diap_min", ui->sb_sY_diap_min->value() );
    sY_graph_conf.insert( "y_diap_max", ui->sb_sY_diap_max->value()  );
    sY_graph_conf.insert( "y_tick",     ui->sb_sY_discret->value()  );

    QJsonObject fY_graph_conf = QJsonObject();
    fY_graph_conf.insert( "isChecked", ui->chb_fY->isChecked() );
    fY_graph_conf.insert( "y_diap_min", ui->sb_fY_diap_min->value() );
    fY_graph_conf.insert( "y_diap_max", ui->sb_fY_diap_max->value()  );
    fY_graph_conf.insert( "y_tick",     ui->sb_fY_discret->value()  );

    QJsonObject fS_graph_conf = QJsonObject();
    fS_graph_conf.insert( "isChecked", ui->chb_fS->isChecked() );
    fS_graph_conf.insert( "y_diap_min", ui->sb_fS_diap_min->value() );
    fS_graph_conf.insert( "y_diap_max", ui->sb_fS_diap_max->value()  );
    fS_graph_conf.insert( "y_tick",     ui->sb_fS_discret->value()  );


    QJsonObject mini_root = QJsonObject();
    mini_root.insert( "gb_fT_L",   fT_L_conf);
    mini_root.insert( "gb_fT_R",   fT_R_conf);
    mini_root.insert( "gb_tfT_L",  tfT_L_conf);
    mini_root.insert( "gb_tfT_R",  tfT_R_conf);
    mini_root.insert( "pA_graph",  pA_graph_conf);
    mini_root.insert( "pB_graph",  pB_graph_conf);
    mini_root.insert( "sY_graph",  sY_graph_conf);
    mini_root.insert( "fY_graph",  fY_graph_conf);
    mini_root.insert( "fS_graph",  fS_graph_conf);


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

// объединил fT и tfT в один обработчик, так как приходят одновременно с контроллера
void GraphPlot::handle_fT_tfT()
{    
    if( isDemo_fT_tfT && !is_new_sensor ) {
        sens_data.fT_L  = QRandomGenerator::global()->bounded(0,10);
        sens_data.fT_R  = QRandomGenerator::global()->bounded(0,10);
        sens_data.tfT_L = QRandomGenerator::global()->bounded(0,5);
        sens_data.tfT_R = QRandomGenerator::global()->bounded(0,5);
        is_new_sensor = true;
    }
    if( is_new_sensor ){
        fT_graph->ChartIncrement_if( ui->gb_fT_L->isChecked(), sens_data.fT_L,
                                     ui->gb_fT_R->isChecked(), sens_data.fT_R);
        tfT_graph->ChartIncrement_if( ui->gb_tfT_L->isChecked(), sens_data.tfT_L,
                                      ui->gb_tfT_R->isChecked(), sens_data.tfT_R );
        is_new_sensor = false;
    }

    fT_graph->ChartScroll(  ui->view_fT->chart()->plotArea().width()  );
    tfT_graph->ChartScroll( ui->view_tfT->chart()->plotArea().width() );
}
void GraphPlot::handle_pA_pB_fY_sY()
{
    if( isDemo_pA_pB_fY_sY && !is_new_cylinder ) {
        cylinders.left.pA = QRandomGenerator::global()->bounded(40) - 0.45;
       cylinders.right.pA = QRandomGenerator::global()->bounded(40) + 0.45;
        cylinders.left.pB = QRandomGenerator::global()->bounded(40) - 0.25;
       cylinders.right.pB = QRandomGenerator::global()->bounded(40) + 0.25;
        cylinders.left.fY = QRandomGenerator::global()->bounded(-40,40);
       cylinders.right.fY = QRandomGenerator::global()->bounded(-40,40);
        cylinders.left.sY = QRandomGenerator::global()->bounded(-40,40);
       cylinders.right.sY = QRandomGenerator::global()->bounded(-40,40);
        cylinders.left.fS = QRandomGenerator::global()->bounded(15);
       cylinders.right.fS = QRandomGenerator::global()->bounded(15);
       is_new_cylinder = true;
    }
    if( is_new_cylinder ) {
        if( !ui->btn_pA_lc->isChecked() ) ui->btn_pA_lc->setText(QString::number( cylinders.left.pA ,'f', 2 ) );
        if( !ui->btn_pA_rc->isChecked() ) ui->btn_pA_rc->setText(QString::number( cylinders.right.pA,'f', 2 ) );

        if( !ui->btn_pB_lc->isChecked() ) ui->btn_pB_lc->setText(QString::number( cylinders.left.pB ,'f', 2 ) );
        if( !ui->btn_pB_rc->isChecked() ) ui->btn_pB_rc->setText(QString::number( cylinders.right.pB,'f', 2 ) );

        if( !ui->btn_fY_lc->isChecked() ) ui->btn_fY_lc->setText(QString::number( cylinders.left.fY ,'f', 2 ) );
        if( !ui->btn_fY_rc->isChecked() ) ui->btn_fY_rc->setText(QString::number( cylinders.right.fY,'f', 2 ) );

        if( !ui->btn_sY_lc->isChecked() ) ui->btn_sY_lc->setText(QString::number( cylinders.left.sY ,'f', 2 ) );
        if( !ui->btn_sY_rc->isChecked() ) ui->btn_sY_rc->setText(QString::number( cylinders.right.sY,'f', 2 ) );

        if( !ui->btn_fS_lc->isChecked() ) ui->btn_fS_lc->setText(QString::number( cylinders.left.fS ,'f', 2 ) );
        if( !ui->btn_fS_rc->isChecked() ) ui->btn_fS_rc->setText(QString::number( cylinders.right.fS,'f', 2 ) );

        pA_graph->ChartIncrement_if( !ui->btn_pA_lc->isChecked(),  cylinders.left.pA,
                                     !ui->btn_pA_rc->isChecked(), cylinders.right.pA);

        pB_graph->ChartIncrement_if( !ui->btn_pB_lc->isChecked(),  cylinders.left.pB,
                                     !ui->btn_pB_rc->isChecked(), cylinders.right.pB);

        fY_graph->ChartIncrement_if( !ui->btn_fY_lc->isChecked(),  cylinders.left.fY,
                                     !ui->btn_fY_rc->isChecked(), cylinders.right.fY);

        sY_graph->ChartIncrement_if( !ui->btn_sY_lc->isChecked(),  cylinders.left.sY,
                                     !ui->btn_sY_rc->isChecked(), cylinders.right.sY);

        fS_graph->ChartIncrement_if( !ui->btn_fS_lc->isChecked(),  cylinders.left.fS,
                                     !ui->btn_fS_rc->isChecked(), cylinders.right.fS);
        is_new_cylinder = false;
    }

    pA_graph->ChartScroll(  ui->view_pA->chart()->plotArea().width()  );
    pB_graph->ChartScroll(  ui->view_pB->chart()->plotArea().width()  );
    fY_graph->ChartScroll(  ui->view_fY->chart()->plotArea().width()  );
    sY_graph->ChartScroll(  ui->view_sY->chart()->plotArea().width()  );
    fS_graph->ChartScroll(  ui->view_fS->chart()->plotArea().width()  );
}
void GraphPlot::handle_pP_fD_tfD_tfdS()
{
    if( isDemo_pP_fD_tfD_tfdS && !is_new_pump ) {
        pumps.left.pP = QRandomGenerator::global()->bounded(40) - 0.45;
       pumps.right.pP = QRandomGenerator::global()->bounded(40) + 0.45;
        pumps.left.fD = QRandomGenerator::global()->bounded(40) - 0.25;
       pumps.right.fD = QRandomGenerator::global()->bounded(40) + 0.25;
        pumps.left.tfD = QRandomGenerator::global()->bounded(-5,40);
       pumps.right.tfD = QRandomGenerator::global()->bounded(-5,40);
        pumps.left.tfdS = QRandomGenerator::global()->bounded(0,4);
       pumps.right.tfdS = QRandomGenerator::global()->bounded(0,4);
        pumps.left.CAN = QRandomGenerator::global()->bounded(15);
       pumps.right.CAN = QRandomGenerator::global()->bounded(15);
       is_new_pump = true;
    }
    if( is_new_pump ) {
        if( !ui->btn_pP_lc->isChecked() ) ui->btn_pP_lc->setText(QString::number( pumps.left.pP ,'f', 2 ) );
        if( !ui->btn_pP_rc->isChecked() ) ui->btn_pP_rc->setText(QString::number( pumps.right.pP,'f', 2 ) );

        if( !ui->btn_fD_lc->isChecked() ) ui->btn_fD_lc->setText(QString::number( pumps.left.fD ,'f', 2 ) );
        if( !ui->btn_fD_rc->isChecked() ) ui->btn_fD_rc->setText(QString::number( pumps.right.fD,'f', 2 ) );

        if( !ui->btn_tfD_lc->isChecked() ) ui->btn_tfD_lc->setText(QString::number( pumps.left.tfD ,'f', 2 ) );
        if( !ui->btn_tfD_rc->isChecked() ) ui->btn_tfD_rc->setText(QString::number( pumps.right.tfD,'f', 2 ) );

        if( !ui->btn_tfdS_lc->isChecked() ) ui->btn_tfdS_lc->setText(QString::number(  pumps.left.tfdS ,'f', 2 ) );
        if( !ui->btn_tfdS_rc->isChecked() ) ui->btn_tfdS_rc->setText(QString::number( pumps.right.tfdS,'f', 2 ) );

        if( !ui->btn_CAN_lc->isChecked() ) ui->btn_CAN_lc->setText(QString::number(  pumps.left.CAN ,'f', 2 ) );
        if( !ui->btn_CAN_rc->isChecked() ) ui->btn_CAN_rc->setText(QString::number( pumps.right.CAN,'f', 2 ) );

        pP_graph->ChartIncrement_if( !ui->btn_pP_lc->isChecked(),  pumps.left.pP,
                                     !ui->btn_pP_rc->isChecked(), pumps.right.pP);

        fD_graph->ChartIncrement_if( !ui->btn_fD_lc->isChecked(),  pumps.left.fD,
                                     !ui->btn_fD_rc->isChecked(), pumps.right.fD);

        tfD_graph->ChartIncrement_if( !ui->btn_tfD_lc->isChecked(),  pumps.left.tfD,
                                      !ui->btn_tfD_rc->isChecked(), pumps.right.tfD);

        tfdS_graph->ChartIncrement_if( !ui->btn_tfdS_lc->isChecked(),  pumps.left.tfdS,
                                       !ui->btn_tfdS_rc->isChecked(), pumps.right.tfdS);

        CAN_graph->ChartIncrement_if( !ui->btn_CAN_lc->isChecked(),  pumps.left.CAN,
                                      !ui->btn_CAN_rc->isChecked(), pumps.right.CAN);
        is_new_pump = false;
    }

    pP_graph->ChartScroll(   ui->view_pP->chart()->plotArea().width()  );
    fD_graph->ChartScroll(   ui->view_fD->chart()->plotArea().width()  );
    tfD_graph->ChartScroll(  ui->view_tfD->chart()->plotArea().width()  );
    tfdS_graph->ChartScroll( ui->view_tfdS->chart()->plotArea().width()  );
    CAN_graph->ChartScroll(  ui->view_CAN->chart()->plotArea().width()  );
}

void GraphPlot::SetGraphsShow()
{
    if( ui->chb_sY->isChecked() )  ui->view_sY->show();
    else                           ui->view_sY->hide();
    if( ui->chb_fY->isChecked() )  ui->view_fY->show();
    else                           ui->view_fY->hide();
    if( ui->chb_pA->isChecked() )  ui->view_pA->show();
    else                           ui->view_pA->hide();
    if( ui->chb_pB->isChecked() )  ui->view_pB->show();
    else                           ui->view_pB->hide();
    if( ui->chb_fS->isChecked() )  ui->view_fS->show();
    else                           ui->view_fS->hide();
    if( ui->chb_pP->isChecked() )   ui->view_pP->show();
    else                            ui->view_pP->hide();
    if( ui->chb_fD->isChecked() )   ui->view_fD->show();
    else                            ui->view_fD->hide();
    if( ui->chb_tfD->isChecked() )  ui->view_tfD->show();
    else                            ui->view_tfD->hide();
    if( ui->chb_tfdS->isChecked())  ui->view_tfdS->show();
    else                            ui->view_tfdS->hide();
    if( ui->chb_CAN->isChecked() )  ui->view_CAN->show();
    else                            ui->view_CAN->hide();

}
void GraphPlot::SetGraphDiap()
{
    QSpinBox* sb = qobject_cast<QSpinBox*>(sender());
    if( nullptr == sb ) {
        return;
    }

    QString name = sb->objectName();
    if( name.contains("_pA_") ) {
        pA_graph->SetRange( QPoint(ui->sb_pA_diap_min->value(), ui->sb_pA_diap_max->value() ));
    }
    else if( name.contains("_pB_") ) {
        pB_graph->SetRange( QPoint(ui->sb_pB_diap_min->value(), ui->sb_pB_diap_max->value() ));
    }
    else if( name.contains("_sY_") ) {
        sY_graph->SetRange( QPoint(ui->sb_sY_diap_min->value(), ui->sb_sY_diap_max->value() ));
    }
    else if( name.contains("_fY_") ) {
        fY_graph->SetRange( QPoint(ui->sb_fY_diap_min->value(), ui->sb_fY_diap_max->value() ));
    }
    else if( name.contains("_fS_") ) {
        fS_graph->SetRange( QPoint(ui->sb_fS_diap_min->value(), ui->sb_fS_diap_max->value() ));
    }
    else if( name.contains("_pP_") ) {
        pP_graph->SetRange( QPoint(ui->sb_pP_diap_min->value(), ui->sb_pP_diap_max->value() ));
    }
    else if( name.contains("_fD_") ) {
        fD_graph->SetRange( QPoint(ui->sb_fD_diap_min->value(), ui->sb_fD_diap_max->value() ));
    }
    else if( name.contains("_tfD_") ) {
        tfD_graph->SetRange( QPoint(ui->sb_tfD_diap_min->value(), ui->sb_tfD_diap_max->value() ));
    }
    else if( name.contains("_tfdS_") ) {
        tfdS_graph->SetRange( QPoint(ui->sb_tfdS_diap_min->value(), ui->sb_tfdS_diap_max->value() ));
    }
    else if( name.contains("_CAN_") ) {
        CAN_graph->SetRange( QPoint(ui->sb_CAN_diap_min->value(), ui->sb_CAN_diap_max->value() ));
    }
}
void GraphPlot::SetGraphStickCount()
{
    QSpinBox* sb = qobject_cast<QSpinBox*>(sender());
    if( nullptr == sb ) {
        return;
    }

    QString name = sb->objectName();
    if( name.contains("_pA_") ) {
        pA_graph->SetTickCount( sb->value() );
    }
    else if( name.contains("_pB_") ) {
        pB_graph->SetTickCount( sb->value() );
    }
    else if( name.contains("_sY_") ) {
        sY_graph->SetTickCount( sb->value() );
    }
    else if( name.contains("_fY_") ) {
        fY_graph->SetTickCount( sb->value() );
    }
    else if( name.contains("_fS_") ) {
        fS_graph->SetTickCount( sb->value() );
    }
    else if( name.contains("_pP_") ) {
        pP_graph->SetTickCount( sb->value() );
    }
    else if( name.contains("_fD_") ) {
        fD_graph->SetTickCount( sb->value() );
    }
    else if( name.contains("_tfD_") ) {
        tfD_graph->SetTickCount( sb->value() );
    }
    else if( name.contains("_tfdS_") ) {
        tfdS_graph->SetTickCount( sb->value() );
    }
    else if( name.contains("_CAN_") ) {
        CAN_graph->SetTickCount( sb->value() );
    }
}

void GraphPlot::StartCylinderGraphs()
{
    pA_graph->Configure("Давление, бар", QPoint( ui->sb_pA_diap_min->value(),ui->sb_pA_diap_max->value()),    ui->sb_pA_discret->value());
    pB_graph->Configure("Давление, бар", QPoint( ui->sb_pB_diap_min->value(),ui->sb_pB_diap_max->value()),    ui->sb_pB_discret->value());
    fY_graph->Configure("Положение, %",  QPoint( ui->sb_fY_diap_min->value(),ui->sb_fY_diap_max->value()),    ui->sb_fY_discret->value());
    sY_graph->Configure("Задание, %",    QPoint( ui->sb_sY_diap_min->value(),ui->sb_sY_diap_max->value()),    ui->sb_sY_discret->value());
    fS_graph->Configure("Положение, мм", QPoint( ui->sb_fS_diap_min->value(),ui->sb_fS_diap_max->value()),    ui->sb_fS_discret->value());
    ui->view_pA->setChart(pA_graph->chart);
    ui->view_pB->setChart(pB_graph->chart);
    ui->view_fY->setChart(fY_graph->chart);
    ui->view_sY->setChart(sY_graph->chart);
    ui->view_fS->setChart(fS_graph->chart);
    ui->view_pA->setRenderHint( QPainter::Antialiasing);
    ui->view_pB->setRenderHint( QPainter::Antialiasing);
    ui->view_fY->setRenderHint( QPainter::Antialiasing);
    ui->view_fS->setRenderHint( QPainter::Antialiasing);
    ui->view_sY->setRenderHint( QPainter::Antialiasing);

    connect( timer, &QTimer::timeout, this, &GraphPlot::handle_pA_pB_fY_sY );
    ui->btn_cylinder_start->setEnabled(false);
    ui->btn_cylinder_stop->setEnabled(true);
}
void GraphPlot::StopCylinderGraphs()
{
    disconnect( timer, &QTimer::timeout, this, &GraphPlot::handle_pA_pB_fY_sY );
    ui->btn_cylinder_start->setEnabled(true);
    ui->btn_cylinder_stop->setEnabled(false);
}

void GraphPlot::StartPumpGraphs()
{
    pP_graph   ->Configure("Давление, бар", QPoint( ui->sb_pP_diap_min->value(),  ui->sb_pP_diap_max->value()),   ui->sb_pP_discret->value());
    fD_graph   ->Configure("Расход, л/мин.",QPoint( ui->sb_fD_diap_min->value(),  ui->sb_fD_diap_max->value()),   ui->sb_fD_discret->value());
    tfD_graph  ->Configure("Температура,°", QPoint( ui->sb_tfD_diap_min->value(), ui->sb_tfD_diap_max->value()),  ui->sb_tfD_discret->value());
    tfdS_graph ->Configure("Положение, °",  QPoint( ui->sb_tfdS_diap_min->value(),ui->sb_tfdS_diap_max->value()), ui->sb_tfdS_discret->value());
    CAN_graph  ->Configure("Уровень, мм",   QPoint( ui->sb_CAN_diap_min->value(), ui->sb_CAN_diap_max->value()),  ui->sb_CAN_discret->value());
    ui->view_pP->  setChart(pP_graph->chart);
    ui->view_fD->  setChart(fD_graph->chart);
    ui->view_tfD-> setChart(tfD_graph->chart);
    ui->view_tfdS->setChart(tfdS_graph->chart);
    ui->view_CAN-> setChart(CAN_graph->chart);
    ui->view_pP->  setRenderHint( QPainter::Antialiasing);
    ui->view_fD->  setRenderHint( QPainter::Antialiasing);
    ui->view_tfD-> setRenderHint( QPainter::Antialiasing);
    ui->view_tfdS->setRenderHint( QPainter::Antialiasing);
    ui->view_CAN-> setRenderHint( QPainter::Antialiasing);

    connect( timer, &QTimer::timeout, this, &GraphPlot::handle_pP_fD_tfD_tfdS );
    ui->btn_pump_start->setEnabled(false);
    ui->btn_pump_stop->setEnabled(true);
}
void GraphPlot::StopPumpGraphs()
{
    disconnect( timer, &QTimer::timeout, this, &GraphPlot::handle_pP_fD_tfD_tfdS );
    ui->btn_pump_start->setEnabled(true);
    ui->btn_pump_stop->setEnabled(false);
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


void GraphPlot::update_cylinder(plc_cylinder data)
{
    cylinders = data;
    is_new_cylinder = true;
}

