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

const QString configFile = "../sensor.json";


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

    UniqueGraph * pA_graph;
    UniqueGraph * pB_graph;
    UniqueGraph * sY_graph;
    UniqueGraph * fY_graph;
    UniqueGraph * fS_graph;

    UniqueGraph * pP_graph;
    UniqueGraph * fD_graph;
    UniqueGraph * tfD_graph;
    UniqueGraph * tfdS_graph;
    UniqueGraph * CAN_graph;

    void handle_fT_tfT();
    void handle_pA_pB_fY_sY();
    void handle_pP_fD_tfD_tfdS();

    // graphs setters
    void SetGraphsShow();
    void SetGraphDiap();
    void SetGraphStickCount();

    // timer starters
    void StartCylinderGraphs();
    void StopCylinderGraphs();

    void StartPumpGraphs();
    void StopPumpGraphs();

    // утилиты для конфигурации
    void LoadConfigure();
    void SaveConfigure();

    // receive slots
    void SensorDataUpdate( SensorPack pack );
    void update_cylinder(plc_cylinder data );






    void handleCalcTabPlot();
    void TabGraphShowingCalc();
    // calc tab
    void SetGraphDiffForce();
    void SetGraphStockLoss();
    void SetGraphPistonLoss();
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

    void SetDemo();   

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
    bool is_PLC_new_data_left;
    bool is_PLC_new_data_right;
    double prev_x;
    double prev_y;
    bool is_Check;
    bool is_new_force;

    bool is_new_sensor;
    bool is_new_cylinder;
    bool is_new_pump;

    bool isDemo_fT_tfT;
    bool isDemo_pA_pB_fY_sY;
    bool isDemo_pP_fD_tfD_tfdS;

    SensorVals sens_data;
    plc_cylinder cylinders;
    plc_pump pumps;
};
#endif // GRAPHPLOT_H
