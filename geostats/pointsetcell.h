#ifndef POINTSETCELL_H
#define POINTSETCELL_H

#include "datacell.h"
#include "domain/pointset.h"
#include <qglobal.h> //uint

class PointSet;

/** Data structure containing information of a point set data sample (point). */
class PointSetCell : public DataCell
{
public:

	/** Preferable constructor.
	 * @param pointSet The point set this cell refers to.
	 * @param dataIndex Data column index of the point set used to get values from (starts with 0) as the point sets are multivalued.
	 * @param sampleIndex Data row index of the point set.
	 */
	inline PointSetCell( PointSet* pointSet, int dataIndex, int sampleIndex )  : DataCell( dataIndex, pointSet ),
		m_pointSet(pointSet), m_sampleIndex( sampleIndex )
	{
		_center._x = pointSet->data( dataIndex, pointSet->getXindex() );
		_center._y = pointSet->data( dataIndex, pointSet->getYindex() );
		_center._z = 0.0;
		if( pointSet->is3D() )
			_center._z = pointSet->data( dataIndex, pointSet->getZindex() );
	}

	// DataCell	interface
	virtual double readValueFromDataSet() const;

	PointSet* m_pointSet;
	uint m_sampleIndex;

};

#endif // POINTSETCELL_H
