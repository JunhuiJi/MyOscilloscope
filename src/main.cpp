#include <qapplication.h>
#include "mainwindow.h"
#include "samplingthread.h"
#include "PlotCurve.h"


int main( int argc, char **argv )
{
    QApplication app( argc, argv );
    app.setPalette( Qt::darkGray );

    PlotCurveVector plotCurveVector_;

    MainWindow window(&plotCurveVector_);
    window.resize( 800, 400 );

    SamplingThread samplingThread(&plotCurveVector_);
    samplingThread.setInterval( 0.5 );// 采样周期，单位为毫秒

    window.show();

    samplingThread.start();
    window.start();

    bool ok = app.exec();

    samplingThread.stop();
    samplingThread.wait( 1000 );

    return ok;
}
