#ifndef GRAPHPLOT_H
#define GRAPHPLOT_H

#include <QMainWindow>
#include <QTimer>

#include <QtCharts>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

#include <QFile>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>
#include <QMap>

#include "Sensor.h"
#include "PLC_Connector.h"
#include "CalculateCylinder.h"
#include "Graphs/UniqueGraph.h"

QT_BEGIN_NAMESPACE
namespace Ui { class GraphPlot; }
QT_END_NAMESPACE

class GraphPlot : public QMainWindow
{
    Q_OBJECT

public:
    GraphPlot(QWidget *parent = nullptr);
    ~GraphPlot();

    UniqueGraph * fT_graph;
    UniqueGraph * tfT_graph;
    void handle_fT_tfT();

    void handleForcePlot();
    void handleRightTabPlot();
    void handleCalcTabPlot();

    // left tab graphs
    void SetGraphForce();
    void SetGraphZadanLeft();
    void SetGraphZolotPositionLeft();
    void SetGraphShtokPositionLeft();
    void TabGraphShowingLeft();
    // right tab graphs
    void SetGraphPressureRight();
    void SetGraphZadanRight();
    void SetGraphZolotPositionRight();
    void SetGraphShtokPositionRight();
    void TabGraphShowingRight();
    void TabGraphShowingCalc();

    // calc tab
    void SetGraphDiffForce();
    void SetGraphStockLoss();
    void SetGraphPistonLoss();

    void SensorDataUpdate( SensorPack pack );
//    void PLC_DataUpdate( PLC_Data data );

    // утилиты для конфигурации
    void LoadConfigure();
    void SaveConfigure();

private Q_SLOTS:
    void updateUI();

private:
    Ui::GraphPlot *ui;
    QTimer * timer;

    Sensor * sensor;
    PLC_Connector * plc;

    CalculateCylinder calc_right;
    CalculateCylinder calc_left;

    int counter;
    bool switcher;
    bool isDemo_fT_tfT;

    void SetDemo();

    QLineSeries *ser_left_piston_pressure;
    QLineSeries *ser_left_rod_pressure;
    QDateTimeAxis *ax_X_LeftPressure;
    QValueAxis *ax_Y_LeftPressure;
    QChart *chartLeftPressure;
    QDateTime timeLeftPressure;

    QValueAxis *ax_Y_LeftZadan;
    QDateTimeAxis *ax_X_LeftZadan;
    QLineSeries *ser_left_zadan;
    QDateTime timeLeftZadan;
    QChart * chartLeftZadan;

    QValueAxis *ax_Y_LeftShtokPosit;
    QDateTimeAxis *ax_X_LeftShtokPosit;
    QLineSeries *ser_left_ShtokPosit;
    QDateTime timeLeftShtokPosit;
    QChart * chartLeftShtokPosit;

    QValueAxis *ax_Y_LeftZolotPosit;
    QDateTimeAxis *ax_X_LeftZolotPosit;
    QLineSeries *ser_left_ZolotPosit;
    QDateTime timeLeftZolotPosit;
    QChart * chartLeftZolotPosit;

    QLineSeries *ser_right_piston_pressure;
    QLineSeries *ser_right_rod_pressure;
    QDateTimeAxis *ax_X_RightPressure;
    QValueAxis *ax_Y_RightPressure;
    QChart *chartRightPressure;
    QDateTime timeRightPressure;

    QValueAxis *ax_Y_RightZadan;
    QDateTimeAxis *ax_X_RightZadan;
    QLineSeries *ser_right_zadan;
    QDateTime timeRightZadan;
    QChart * chartRightZadan;

    QValueAxis *ax_Y_RightShtokPosit;
    QDateTimeAxis *ax_X_RightShtokPosit;
    QLineSeries *ser_right_ShtokPosit;
    QDateTime timeRightShtokPosit;
    QChart * chartRightShtokPosit;

    QValueAxis *ax_Y_RightZolotPosit;
    QDateTimeAxis *ax_X_RightZolotPosit;
    QLineSeries *ser_right_ZolotPosit;
    QDateTime timeRightZolotPosit;
    QChart * chartRightZolotPosit;


    QLineSeries *ser_piston_loss_right;
    QLineSeries *ser_piston_loss_left;
    QDateTimeAxis *ax_X_PistonLoss;
    QValueAxis *ax_Y_PistonLoss;
    QChart *chartPistonLoss;
    QDateTime timePistonLoss;

    QLineSeries *ser_Stock_loss_right;
    QLineSeries *ser_Stock_loss_left;
    QDateTimeAxis *ax_X_StockLoss;
    QValueAxis *ax_Y_StockLoss;
    QChart *chartStockLoss;
    QDateTime timeStockLoss;

    QLineSeries *ser_Diff_Force_right;
    QLineSeries *ser_Diff_Force_left;
    QDateTimeAxis *ax_X_DiffForce;
    QValueAxis *ax_Y_DiffForce;
    QChart *chartDiffForce;
    QDateTime timeDiffForce;

    bool is_PLC_CALC_new_data;
//    PLC_Data plc_Data;

    bool is_PLC_new_data_left;
    bool is_PLC_new_data_right;

    double prev_x;
    double prev_y;
    bool is_Check;

    bool is_new_sensor;

    bool is_new_force;
    SensorVals sens_data;
};
#endif // GRAPHPLOT_H
