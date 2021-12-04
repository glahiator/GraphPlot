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
    void timerHandler();

private:
    Ui::GraphPlot *ui;
    QTimer * timer;
    QLineSeries *series;
    QSplineSeries *series1;
    QValueAxis *ax_X;
    QValueAxis *ax_Y;
    QChart *chart;
    double prev_x;
    double prev_y;


};
#endif // GRAPHPLOT_H
