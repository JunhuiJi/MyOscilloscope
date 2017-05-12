#include "samplingthread.h"
#include <math.h>
#include <QPointF>
#include "curvedata.h"
#include "PlotCurve.h"


SamplingThread::SamplingThread(PlotCurveVector *plotCurveVector__, QObject *parent ):
    QwtSamplingThread( parent ),
    plotCurveVector_(plotCurveVector__)
{
}

void SamplingThread::sample( double elapsed )
{

    this->plotCurveVector_->lockForWrite();
    int i = 0;
    for(auto plotCurve_ : this->plotCurveVector_->getVector())
    {
        const QPointF s( elapsed, value( elapsed, i ) );
        plotCurve_.getSignalData().append(s);
        ++i;
    }
    this->plotCurveVector_->unlock();
}

// JJH：曲线数据源的y值获得函数；形参中index代表曲线的序号（从0开始编号）
double SamplingThread::value(double timeStamp , int index) const
{
    const double period = 1.0 / 5;

    const double x = ::fmod( timeStamp, period );
    double v = 0;
    switch(index)
    {
    case(0):
        v = 100 * std::sin( x / period * 2 * M_PI );
        break;
    case(1):
        v = 100 * std::cos( x / period * 2 * M_PI );
        break;
    default:
        v = 100 * std::tan( x / period * 2 * M_PI );
        break;
    }

    return v;
}
