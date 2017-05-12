#ifndef PLOTCURVE_H
#define PLOTCURVE_H


#include <QReadWriteLock>
#include <vector>
#include <QColor>


class QwtPlotCurve;
class CurveData;
class SignalData;
class Plot;
class QwtPlotDirectPainter;

class PlotCurve
{
    friend class MainWindow;
    friend class SamplingThread;

public:
    PlotCurve(const QColor &color_ = Qt::black);
    virtual ~PlotCurve();
    SignalData &getSignalData();
    QwtPlotCurve *getQwtPlotCurvePtr();
    QwtPlotDirectPainter *getQwtPlotDirectPainterPtr();
    void updateCurve(const bool &doClip, Plot *plot_, const int &numPoints, const int &paintedPoints);
    void clearStaleValues(double min = -1);

private:
    CurveData *curveDataPtr_;
    QwtPlotCurve *qwtPlotCurvePtr_;
    QwtPlotDirectPainter *qwtPlotDirectPainterPtr_;
};

class PlotCurveVector
{
public:
    PlotCurveVector();
    void lockForRead();
    void lockForWrite();
    void unlock();
    std::vector<PlotCurve> &getVector();
    int getPointNum() const;
private:
    QReadWriteLock lock_;
    std::vector<PlotCurve> plotCurveVector_;
};

#endif
