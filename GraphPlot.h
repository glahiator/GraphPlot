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
#include "PLC.h"
#include "CalculateCylinder.h"
#include "Graphs/UniqueGraph.h"

const QString configFile = QString("../sensor.json");


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
    UniqueGraph * fiR_graph;
    UniqueGraph * fiP_graph;
    UniqueGraph * nN_graph;

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
    void handle_fiR_fiP_nN();
    void handle_pA_pB_fY_sY_fS();
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

    void StartSensorGraphs();
    void StopSensorGraphs();

    void StartCalcGraphs();
    void StopCalcGraphs();

    // утилиты для конфигурации
    void LoadConfigure();
    void SaveConfigure();

    // receive slots
    void update_sensor( sensor_pack pack );
    void update_plc( plc_pack data );

private Q_SLOTS:
    void updateUI();

private:
    Ui::GraphPlot *ui;
    QTimer * timer;

    Sensor * sensor;
    PLC * siemens;

    CalculateCylinder calc_right;
    CalculateCylinder calc_left;

    bool is_new_sensor;
    bool is_new_cylinder;
    bool is_new_pump;

    bool isDemo_fT_tfT;
    bool isDemo_fiR_fiP_nN;
    bool isDemo_pA_pB_fY_sY;
    bool isDemo_pP_fD_tfD_tfdS;

    sensor_vals sens_data;
    cyldr_vals cylinders;
    pump_vals pumps;
};
#endif // GRAPHPLOT_H
