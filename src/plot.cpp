#include "include/plot.h"
#include <qwt_plot_grid.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_directpainter.h>
#include <qwt_curve_fitter.h>
#include <qwt_painter.h>
#include <qevent.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include "include/PlotCurve.h"


class Canvas: public QwtPlotCanvas
{
public:
    Canvas( QwtPlot *plot = NULL ):
        QwtPlotCanvas( plot )
    {
        // The backing store is important, when working with widget
        // overlays ( f.e rubberbands for zooming ).
        // Here we don't have them and the internal
        // backing store of QWidget is good enough.

        setPaintAttribute( QwtPlotCanvas::BackingStore, false );
        setBorderRadius( 10 );

        if ( QwtPainter::isX11GraphicsSystem() )
        {
#if QT_VERSION < 0x050000
            // Even if not liked by the Qt development, Qt::WA_PaintOutsidePaintEvent
            // works on X11. This has a nice effect on the performance.

            setAttribute( Qt::WA_PaintOutsidePaintEvent, true );
#endif

            // Disabling the backing store of Qt improves the performance
            // for the direct painter even more, but the canvas becomes
            // a native window of the window system, receiving paint events
            // for resize and expose operations. Those might be expensive
            // when there are many points and the backing store of
            // the canvas is disabled. So in this application
            // we better don't disable both backing stores.

            if ( testPaintAttribute( QwtPlotCanvas::BackingStore ) )
            {
                setAttribute( Qt::WA_PaintOnScreen, true );
                setAttribute( Qt::WA_NoSystemBackground, true );
            }
        }

        setupPalette();
    }

private:
    void setupPalette()
    {
        QPalette pal = palette();

#if QT_VERSION >= 0x040400
        QLinearGradient gradient;
        gradient.setCoordinateMode( QGradient::StretchToDeviceMode );
        gradient.setColorAt( 0.0, QColor( 0, 49, 110 ) );
        gradient.setColorAt( 1.0, QColor( 0, 87, 174 ) );

        pal.setBrush( QPalette::Window, QBrush( gradient ) );
#else
        pal.setBrush( QPalette::Window, QBrush( color ) );
#endif

        // QPalette::WindowText is used for the curve color
        pal.setColor( QPalette::WindowText, Qt::green );

        setPalette( pal );
    }
};

Plot::Plot(PlotCurveVector *plotCurveVector__, QWidget *parent):
    QwtPlot( parent ),
    plotCurveVector_(plotCurveVector__),
    d_paintedPoints( 0 ),
    d_interval( 0.0, 10.0 ),
    d_timerId( -1 )
{
    setAutoReplot( false );
    setCanvas( new Canvas() );

    plotLayout()->setAlignCanvasToScales( true );

    setAxisTitle( QwtPlot::xBottom, "Time [s]" );
    setAxisScale( QwtPlot::xBottom, d_interval.minValue(), d_interval.maxValue() );
    setAxisScale( QwtPlot::yLeft, -200.0, 200.0 );

    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->setPen( Qt::gray, 0.0, Qt::DotLine );
    grid->enableX( true );
    grid->enableXMin( true );
    grid->enableY( true );
    grid->enableYMin( false );
    grid->attach( this );

    d_origin = new QwtPlotMarker();
    d_origin->setLineStyle( QwtPlotMarker::Cross );
    d_origin->setValue( d_interval.minValue() + d_interval.width() / 2.0, 0.0 );
    d_origin->setLinePen( Qt::gray, 0.0, Qt::DashLine );
    d_origin->attach( this );

    for(auto plotCurve_ : this->plotCurveVector_->getVector())
    {
        plotCurve_.getQwtPlotCurvePtr()->attach(this);
    }


    // panning with the left mouse button
    new QwtPlotPanner( this->canvas() );

    // zoom in/out with the wheel
    new QwtPlotMagnifier( this->canvas() );
}

Plot::~Plot()
{
}

void Plot::start()
{
    d_clock.start();
    d_timerId = startTimer( 1 );
}

void Plot::replot()
{
    this->plotCurveVector_->lockForRead();
    QwtPlot::replot();
    this->d_paintedPoints = this->plotCurveVector_->getPointNum();
    this->plotCurveVector_->unlock();
}

void Plot::setIntervalLength( double interval )
{
    if ( interval > 0.0 && interval != d_interval.width() )
    {
        d_interval.setMaxValue( d_interval.minValue() + interval );
        setAxisScale( QwtPlot::xBottom,
            d_interval.minValue(), d_interval.maxValue() );

        replot();
    }
}

void Plot::updateCurve()
{
    this->plotCurveVector_->lockForRead();
    const int pointNum = this->plotCurveVector_->getPointNum();
    if ( pointNum > d_paintedPoints )
    {
        const bool doClip = !canvas()->testAttribute( Qt::WA_PaintOnScreen );
        for(auto plotCurve_ : this->plotCurveVector_->getVector())
        {
            plotCurve_.updateCurve(doClip, this, pointNum, d_paintedPoints);
        }

        d_paintedPoints = pointNum;
    }
    this->plotCurveVector_->unlock();
}

void Plot::incrementInterval()
{
    d_interval = QwtInterval( d_interval.maxValue(),
        d_interval.maxValue() + d_interval.width() );

    this->plotCurveVector_->lockForWrite();
    for(auto plotCurve_ : this->plotCurveVector_->getVector())
    {
        plotCurve_.clearStaleValues(d_interval.minValue());
    }
    this->plotCurveVector_->unlock();

    // To avoid, that the grid is jumping, we disable
    // the autocalculation of the ticks and shift them
    // manually instead.

    QwtScaleDiv scaleDiv = axisScaleDiv( QwtPlot::xBottom );
    scaleDiv.setInterval( d_interval );

    for ( int i = 0; i < QwtScaleDiv::NTickTypes; i++ )
    {
        QList<double> ticks = scaleDiv.ticks( i );
        for ( int j = 0; j < ticks.size(); j++ )
            ticks[j] += d_interval.width();
        scaleDiv.setTicks( i, ticks );
    }
    setAxisScaleDiv( QwtPlot::xBottom, scaleDiv );

    d_origin->setValue( d_interval.minValue() + d_interval.width() / 2.0, 0.0 );

    d_paintedPoints = 0;
    replot();
}

void Plot::timerEvent( QTimerEvent *event )
{
    if ( event->timerId() == d_timerId )
    {
        updateCurve();

        const double elapsed = d_clock.elapsed() / 1000.0;
        if ( elapsed > d_interval.maxValue() )
            incrementInterval();

        return;
    }

    QwtPlot::timerEvent( event );
}

void Plot::resizeEvent( QResizeEvent *event )
{
    for(auto plotCurve_ : this->plotCurveVector_->getVector())
    {
        plotCurve_.getQwtPlotDirectPainterPtr()->reset();
    }
    QwtPlot::resizeEvent( event );
}

void Plot::showEvent( QShowEvent * )
{
    replot();
}

bool Plot::eventFilter( QObject *object, QEvent *event )
{
    if ( object == canvas() && 
        event->type() == QEvent::PaletteChange )
    {
//        d_curve->setPen( canvas()->palette().color( QPalette::WindowText ) );
    }

    return QwtPlot::eventFilter( object, event );
}
