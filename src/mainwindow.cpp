#include "mainwindow.h"
#include "plot.h"
#include "../include/wheelbox.h"
#include <qlayout.h>
#include "PlotCurve.h"


MainWindow::MainWindow(PlotCurveVector *plotCurveVector__, QWidget *parent ):
    QWidget( parent ),
    d_plot(new Plot(plotCurveVector__, this ))
{
    const double intervalLength = 1.0; // seconds
    d_plot->setIntervalLength( intervalLength );

    d_intervalWheel = new WheelBox( "Displayed [s]", 1.0, 100.0, 1.0, this );
    d_intervalWheel->setValue( intervalLength );

    QVBoxLayout* vLayout1 = new QVBoxLayout();
    vLayout1->addWidget( d_intervalWheel );
    vLayout1->addStretch( 10 );

    QHBoxLayout *layout = new QHBoxLayout( this );
    layout->addWidget( d_plot, 10 );
    layout->addLayout( vLayout1 );

    connect( d_intervalWheel, SIGNAL( valueChanged( double ) ),
        d_plot, SLOT( setIntervalLength( double ) ) );
}

void MainWindow::start()
{
    d_plot->start();
}
