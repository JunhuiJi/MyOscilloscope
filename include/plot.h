#ifndef PLOT_H
#define PLOT_H


#include <qwt_plot.h>
#include <qwt_interval.h>
#include <qwt_system_clock.h>

class QwtPlotMarker;
class PlotCurveVector;


class Plot: public QwtPlot
{
    Q_OBJECT

public:
    Plot(PlotCurveVector *plotCurveVector__ = nullptr, QWidget * = NULL);
    virtual ~Plot();

    void start();
    virtual void replot();

    virtual bool eventFilter( QObject *, QEvent * );

public Q_SLOTS:
    void setIntervalLength( double );

protected:
    virtual void showEvent( QShowEvent * );
    virtual void resizeEvent( QResizeEvent * );
    virtual void timerEvent( QTimerEvent * );

private:
    void updateCurve();
    void incrementInterval();

    PlotCurveVector *plotCurveVector_;

    QwtPlotMarker *d_origin;
    int d_paintedPoints;


    QwtInterval d_interval;
    int d_timerId;

    QwtSystemClock d_clock;
};

#endif
