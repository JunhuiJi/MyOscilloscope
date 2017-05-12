#ifndef CURVEDATA_H
#define CURVEDATA_H

#include <qwt_series_data.h>
#include <qpointer.h>
#include <qrect.h>

class SignalData
{
public:
	SignalData();
	virtual ~SignalData();
	void append(const QPointF &pos);
	void clearStaleValues(double min=-1);

	int size() const;
	QPointF value(int index) const;
	QPointF lastValue() const;

	QRectF boundingRect() const;

	void lock();
	void unlock();

private:
	SignalData(const SignalData &);
	SignalData &operator=( const SignalData & );

	

	class PrivateData;
	PrivateData *d_data;
};

class CurveData: public QwtSeriesData<QPointF>
{
public:
	const SignalData &values() const;
	SignalData &values();

	virtual QPointF sample(size_t i) const;
	virtual size_t size() const;

	virtual QRectF boundingRect() const;

private:
	SignalData signalData;
};


#endif // CURVEDATA_H
