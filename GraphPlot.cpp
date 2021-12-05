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
    //timer->start();


    Set1Graph();

    connect( ui->btn_debug, &QPushButton::clicked, this, &GraphPlot::DebugSlot );
    connect( ui->pushButton, &QPushButton::clicked, this, &GraphPlot::StartTimer );

    ui->themeComboBox->addItem("Light", QChart::ChartThemeLight);
    ui->themeComboBox->addItem("Blue Cerulean", QChart::ChartThemeBlueCerulean);
    ui->themeComboBox->addItem("Dark", QChart::ChartThemeDark);
    ui->themeComboBox->addItem("Brown Sand", QChart::ChartThemeBrownSand);
    ui->themeComboBox->addItem("Blue NCS", QChart::ChartThemeBlueNcs);
    ui->themeComboBox->addItem("High Contrast", QChart::ChartThemeHighContrast);
    ui->themeComboBox->addItem("Blue Icy", QChart::ChartThemeBlueIcy);
    ui->themeComboBox->addItem("Qt", QChart::ChartThemeQt);

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


void GraphPlot::timerHandler()
{
    time = time.addSecs(1);

    prev_y = QRandomGenerator::global()->bounded(1,10) ;
    seriesPistonLeft->append(time.toMSecsSinceEpoch(), prev_y);
    prev_y = QRandomGenerator::global()->bounded(1,10) ;
    seriesPistonRight->append(time.toMSecsSinceEpoch(), prev_y);
    chartPiston->scroll(131.75, 0);
}

void GraphPlot::Set1Graph()
{
    seriesPistonLeft = new QLineSeries();
//    QPen green(Qt::red);
//    green.setWidth(1);
//    series->setPen(green);
    seriesPistonLeft->setName("Левый");

    seriesPistonRight = new QLineSeries();
    seriesPistonRight->setName("Правый");;

    time.setDate(QDateTime::currentDateTime().date());
    time.setTime(QTime(13,10,0));
    QDateTime temp_time = time;

    seriesPistonLeft->append(time.toMSecsSinceEpoch(), 2);
    seriesPistonRight->append(time.toMSecsSinceEpoch(), 1);
    time.setTime(QTime(13,10,1));
    seriesPistonLeft->append(time.toMSecsSinceEpoch(), 4);
    seriesPistonRight->append(time.toMSecsSinceEpoch(), 5);
    time.setTime(QTime(13,10,2));
    seriesPistonLeft->append(time.toMSecsSinceEpoch(), 8);
    seriesPistonRight->append(time.toMSecsSinceEpoch(), 7);
    time.setTime(QTime(13,10,3));
    seriesPistonLeft->append(time.toMSecsSinceEpoch(), 4);
    seriesPistonRight->append(time.toMSecsSinceEpoch(), 5);
    time.setTime(QTime(13,10,4));
    seriesPistonLeft->append(time.toMSecsSinceEpoch(), 5);
    seriesPistonRight->append(time.toMSecsSinceEpoch(), 6);

    time.setTime(QTime(13,10,5));
    prev_x = time.toMSecsSinceEpoch();
    prev_y = 6;
    seriesPistonLeft->append(prev_x, prev_y);
    seriesPistonRight->append(prev_x, prev_y);

    ax_X = new QDateTimeAxis;
    ax_Y = new QValueAxis;
    ax_X->setTitleText("Время, мин");
    ax_X->setFormat("hh:mm:ss.zzz");
    ax_Y->setTitleText("Потери, %");

    chartPiston = new QChart();
    chartPiston->legend()->setAlignment(Qt::AlignRight);
    chartPiston->legend()->show();
    chartPiston->addSeries(seriesPistonLeft);
    chartPiston->addSeries(seriesPistonRight);
    chartPiston->addAxis(ax_X, Qt::AlignBottom);
    chartPiston->addAxis(ax_Y, Qt::AlignLeft);
//    chart->setAnimationOptions(QChart::SeriesAnimations);


    seriesPistonLeft->attachAxis( ax_X );
    seriesPistonLeft->attachAxis( ax_Y );
    seriesPistonRight->attachAxis( ax_X );
    seriesPistonRight->attachAxis( ax_Y );


    QDateTime temp_time_finish;
    temp_time_finish.setDate(QDateTime::currentDateTime().date());
    temp_time_finish.setTime(QTime(13,10,6));

    ax_X->setRange(temp_time, temp_time_finish);
    ax_Y->setRange(0, 15);
    ax_X->setTickCount(7);
    ax_Y->setTickCount(4);
    chartPiston->setTitle("Процент потерь расхода в поршневой полости");

    ui->graphView1->setChart(chartPiston);
    ui->graphView1->setRenderHint(QPainter::Antialiasing);
    ui->graphView1->setRubberBand( QChartView::RectangleRubberBand );
}

