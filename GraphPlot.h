#ifndef GRAPHPLOT_H
#define GRAPHPLOT_H

#include <QMainWindow>
#include <QTimer>

#include <QtCharts>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

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

private Q_SLOTS:
    void updateUI();

private:
    Ui::GraphPlot *ui;
    QTimer * timer;

    QLineSeries *seriesPistonLeft;
    QSplineSeries *seriesPistonRight;
    QDateTimeAxis *ax_X_Piston;
    QValueAxis *ax_Y_Piston;
    QChart *chartPiston; // график процента потерь расхода в поршневой полости
    QDateTime timePiston;

    QLineSeries *seriesRodLeft;
    QSplineSeries *seriesRodRight;
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
    double prev_y;;


};
#endif // GRAPHPLOT_H
