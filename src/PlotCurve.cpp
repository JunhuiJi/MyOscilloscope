#include "PlotCurve.h"
#include <qwt_plot_curve.h>
#include "curvedata.h"
#include "plot.h"
#include <qwt_scale_map.h>
#include <qwt_plot_directpainter.h>
#include <qwt_series_data.h>
#include <vector>
//#include <typeinfo>
//#include <algorithm>
//#include <QDebug>


PlotCurve::PlotCurve(const QColor &color_):
    curveDataPtr_(new CurveData()),
    qwtPlotCurvePtr_(new QwtPlotCurve()),
    qwtPlotDirectPainterPtr_(new QwtPlotDirectPainter())
{
    this->qwtPlotCurvePtr_->setStyle( QwtPlotCurve::Lines );
    this->qwtPlotCurvePtr_->setPen( color_ );
    this->qwtPlotCurvePtr_->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    this->qwtPlotCurvePtr_->setPaintAttribute( QwtPlotCurve::ClipPolygons, false );
    this->qwtPlotCurvePtr_->setData( this->curveDataPtr_ );
}

PlotCurve::~PlotCurve()
{
//    delete this->qwtPlotCurve_ptr_;
//    delete this->curveData_ptr_;
//    delete this->qwtPlotDirectPainterPtr_;
}

SignalData &PlotCurve::getSignalData()
{
    return this->curveDataPtr_->values();
}

QwtPlotCurve *PlotCurve::getQwtPlotCurvePtr()
{
    return this->qwtPlotCurvePtr_;
}

QwtPlotDirectPainter *PlotCurve::getQwtPlotDirectPainterPtr()
{
    return this->qwtPlotDirectPainterPtr_;
}

void PlotCurve::updateCurve(const bool &doClip, Plot *plot_, const int &numPoints, const int &paintedPoints)
{
    if ( doClip )
    {
        /*
            Depending on the platform setting a clip might be an important
            performance issue. F.e. for Qt Embedded this reduces the
            part of the backing store that has to be copied out - maybe
            to an unaccelerated frame buffer device.
        */
        const QwtScaleMap xMap = plot_->canvasMap( this->qwtPlotCurvePtr_->xAxis() );
        const QwtScaleMap yMap = plot_->canvasMap( this->qwtPlotCurvePtr_->yAxis() );

        QRectF br = qwtBoundingRect( *this->curveDataPtr_,
                                     paintedPoints - 1, numPoints - 1 );

        const QRect clipRect = QwtScaleMap::transform( xMap, yMap, br ).toRect();
        this->qwtPlotDirectPainterPtr_->setClipRegion( clipRect );
    }

    this->qwtPlotDirectPainterPtr_->drawSeries( this->qwtPlotCurvePtr_,
                                                paintedPoints - 1, numPoints - 1 );
}

void PlotCurve::clearStaleValues(double min)
{
    this->getSignalData().clearStaleValues(min);
}

// JJH:曲线数目与颜色
PlotCurveVector::PlotCurveVector():
    plotCurveVector_({
                     PlotCurve(Qt::green),
                     PlotCurve(Qt::red),
                     PlotCurve(Qt::cyan)
                     })
{
}

void PlotCurveVector::lockForRead()
{
    this->lock_.lockForRead();
}

void PlotCurveVector::lockForWrite()
{
    this->lock_.lockForWrite();
}

void PlotCurveVector::unlock()
{
    this->lock_.unlock();
}

std::vector<PlotCurve> &PlotCurveVector::getVector()
{
    return this->plotCurveVector_;
}

int PlotCurveVector::getPointNum() const
{
//    // JJH
//    std::vector<int> pointNumVector;


    int number = 0;
    for(auto plotCurve_ : this->plotCurveVector_)
    {
        if(number < plotCurve_.getSignalData().size())
            number = plotCurve_.getSignalData().size();


//        // JJH
//        pointNumVector.push_back(plotCurve_.getSignalData().size());
    }

//    // JJH
//    auto result_max = std::max_element(pointNumVector.begin(), pointNumVector.end());
//    auto result_min = std::min_element(pointNumVector.begin(), pointNumVector.end());
//    if(*result_max != *result_min)
//        qDebug() << "point number not equal!";


    return number;
}
