#ifndef GRAPHPLOT_H
#define GRAPHPLOT_H

#include <QMainWindow>
#include <QTimer>

#include <QtCharts>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

#include "SensorConnector.h"

QT_BEGIN_NAMESPACE
namespace Ui { class GraphPlot; }
QT_END_NAMESPACE

class GraphPlot : public QMainWindow
{
    Q_OBJECT

public:
    GraphPlot(QWidget *parent = nullptr);
    ~GraphPlot();

    void DebugSlot();
    void StartTimer();
    void handlePistonPlot();
    void handleRodPlot();
    void handleForcePlot();

    void SetGraphPiston();
    void SetGraphRod();
    void SetGraphForce();

    void SensorDataUpdate( SensorPack pack );
    void SetCheck( bool is_value );

private Q_SLOTS:
    void updateUI();

    void on_action_13_triggered();

    void on_action_14_triggered();


private:
    Ui::GraphPlot *ui;
    QTimer * timer;

    SensorConnector * sensor;

    int counter;
    bool switcher;
    QLineSeries *seriesPistonLeft;
    QLineSeries *seriesPistonRight;
    QDateTimeAxis *ax_X_Piston;
    QValueAxis *ax_Y_Piston;
    QChart *chartPiston; // график процента потерь расхода в поршневой полости
    QDateTime timePiston;

    QLineSeries *seriesRodLeft;
    QLineSeries *seriesRodRight;
    QDateTimeAxis *ax_X_Rod;
    QValueAxis *ax_Y_Rod;
    QChart *chartRod; // график процента потерь расхода в штоковой полости
    QDateTime timeRod;

    QLineSeries *seriesForceLeft;
    QSplineSeries *seriesForceRight;
    QDateTimeAxis *ax_X_Force;
    QValueAxis *ax_Y_Force;
    QChart *chartForce; // график процента потерь расхода в штоковой полости
    QDateTime timeForce;


    double prev_x;
    double prev_y;

    bool is_Check;

    bool is_new_piston;
    qreal adc_val_1;
    qreal adc_val_2;
    qreal adc_filt_val_1;
    qreal adc_filt_val_2;
    bool is_new_rod;
    bool is_new_force;


};
#endif // GRAPHPLOT_H
