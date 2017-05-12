#include "curvedata.h"
#include <Qlist>
#include <qmutex.h>
#include <qreadwritelock.h>

class SignalData::PrivateData
{
public:
	PrivateData():
	  boundingRect(1.0, 1.0, -2.0, -2.0) // invalid
	  {
		  
	  }

	  inline void append(const QPointF &sample)
	  {
		  values.append(sample);

		  // adjust the bounding rectangle

		  if ( boundingRect.width() < 0 || boundingRect.height() < 0 )
		  {
			  boundingRect.setRect(sample.x(), sample.y(), 0.0, 0.0);
		  }
		  else
		  {
			  boundingRect.setRight(sample.x());

			  if ( sample.y() > boundingRect.bottom() )
				  boundingRect.setBottom(sample.y());

			  if ( sample.y() < boundingRect.top() )
				  boundingRect.setTop(sample.y());
		  }
	  }

	  QReadWriteLock lock;

	  QList<QPointF> values;
	  QRectF boundingRect;

	  QMutex mutex; // protecting pendingValues
	  QList<QPointF> pendingValues;
};

SignalData::SignalData()
{
	d_data = new PrivateData();
}

SignalData::~SignalData()
{
	delete d_data;
}

int SignalData::size() const
{
	return d_data->values.size();
}   

QPointF SignalData::value(int index) const
{
	return d_data->values[index];
}   

QRectF SignalData::boundingRect() const
{
	return d_data->boundingRect;
}

void SignalData::lock()
{
	d_data->lock.lockForRead();
}

void SignalData::unlock()
{
	d_data->lock.unlock();
}

void SignalData::append(const QPointF &sample)
{
	d_data->mutex.lock();
	d_data->pendingValues += sample;

	const bool isLocked = d_data->lock.tryLockForWrite();
	if ( isLocked )
	{
		while(!d_data->pendingValues.isEmpty())
		{
			d_data->append(d_data->pendingValues.takeFirst());
		}
		d_data->lock.unlock();
	}

	d_data->mutex.unlock();
}

void SignalData::clearStaleValues( double min/*=-1*/ )
{
	d_data->lock.lockForWrite();

	d_data->boundingRect = QRectF(1.0, 1.0, -2.0, -2.0); // invalid
	if (min==-1)
	{
		d_data->values.clear();
	}
	else
	{
		while(!d_data->values.isEmpty()&&d_data->values.first().x()<min)
		{
			d_data->values.removeFirst();
		}
	}
	d_data->lock.unlock();
}

QPointF SignalData::lastValue() const
{
	d_data->lock.lockForRead();
	QPointF p;
	if (!d_data->values.isEmpty())
	{
		p=d_data->values.last();
	}
	d_data->lock.unlock();
	return p;
}

const SignalData &CurveData::values() const
{
	return signalData;
}

SignalData &CurveData::values() 
{
	return signalData;
}

QPointF CurveData::sample(size_t i) const
{
	return signalData.value(i);
}

size_t CurveData::size() const
{
	return signalData.size();
}

QRectF CurveData::boundingRect() const
{
	return signalData.boundingRect();
}
