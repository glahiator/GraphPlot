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
    void timerHandler();

    void Set1Graph();

private:
    Ui::GraphPlot *ui;
    QTimer * timer;
    QLineSeries *seriesPistonLeft;
    QLineSeries *seriesPistonRight;
    QDateTimeAxis *ax_X;
    QValueAxis *ax_Y;
    QChart *chartPiston; // график процента потерь расхода в поршневой полости
    QChart *chartRod; // график процента потерь расхода в штоковой полости

    double prev_x;
    double prev_y;
    QDateTime time;


};
#endif // GRAPHPLOT_H
