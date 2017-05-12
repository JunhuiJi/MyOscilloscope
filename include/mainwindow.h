#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <qwidget.h>

class Plot;
class WheelBox;
class PlotCurveVector;


class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(PlotCurveVector *plotCurveVector__, QWidget * = NULL );

    void start();

private:
    WheelBox *d_intervalWheel;
    Plot *d_plot;
};

#endif
