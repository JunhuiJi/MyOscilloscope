#ifndef SAMPLINGTHREAD_H
#define SAMPLINGTHREAD_H


#include <qwt_sampling_thread.h>


class PlotCurveVector;

class SamplingThread: public QwtSamplingThread
{
    Q_OBJECT

public:
    SamplingThread(PlotCurveVector *plotCurveVector__, QObject *parent = NULL );

protected:
    virtual void sample( double elapsed );

private:
    virtual double value( double timeStamp, int index ) const;
    PlotCurveVector *plotCurveVector_;
};

#endif
