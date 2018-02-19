#ifndef ICALCPROPERTYCOLLECTION_H
#define ICALCPROPERTYCOLLECTION_H

#include <QString>

class ICalcProperty;

/**
 * The ICalcPropertyCollection interface should be implemented by classes to be usable in the Calculator.
 * ICalcPropertyCollection represents tabular data, that is, properties as columns with equal number of values each.
 */
class ICalcPropertyCollection
{
public:
    ICalcPropertyCollection();
    virtual ~ICalcPropertyCollection(){}

	/** Returns the name of the collection. */
	virtual QString getCalcPropertyCollectionName() = 0;

	/** Returns the number of properties in this collection. */
	virtual int getCalcPropertyCount() = 0;

	/** Returns one property given its index. */
	virtual ICalcProperty* getCalcProperty( int index ) = 0;

	/** Returns the number of data values in the properties (assumes all
	 * properties have the same number of values). */
	virtual int getCalcRecordCount() = 0;

	/** Returns the value of the given variable (table column) in the given record (table line). */
	virtual double getCalcValue( int iVar, int iRecord ) = 0;

	/** Sets the value of the given variable (table column) in the given record (table line). */
	virtual double setCalcValue( int iVar, int iRecord, double value ) = 0;

	/** Called when a computation will commence.  This might prompt implementations to fetch
	 *  data from file, network, etc.. */
	virtual void computationWillStart() = 0;

	/** Called when a computation has completed.  This might prompt implementations to save
	 *  changes to file, for example. */
	virtual void computationCompleted() = 0;
};

#endif // ICALCPROPERTYCOLLECTION_H
