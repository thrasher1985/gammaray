#ifndef VERTICALPROPORTIONCURVE_H
#define VERTICALPROPORTIONCURVE_H

#include "domain/datafile.h"

/** An entry in the Vertical Proportion Curve */
struct VPCEntry{
    VPCEntry( double pRelativeDepth ) : relativeDepth( pRelativeDepth ){}
    /** The relave depth varying between 0.0 (base) and 1.0 (top). */
    double relativeDepth;
    /** The proportions of facies varying between 0.0 (0%) and 1.0 (100%) and summing up 1.0. */
    std::vector<double> proportions;
};

/**
 * The VerticalProportionCurve class models a Vertical Proportion Curve, a series of values defining
 * the expected proportion of categories (e.g. lithofacies) along the vertical.  The vertical scale of
 * the curve is relative, varying from 1.0 at top and 0.0 at the base.  The absolute depth at which a
 * certain proportion occurs should be computed on the fly with respect to some reference such as top
 * and base horizons or max/min depth of point sets or segment sets.  The curves are represented by polygonal
 * lines between top and base. The curve points are values between 0.0 and 1.0 and are stored as columns of a
 * GEO-EAS file (similiarly to the data sets).  These values logically must be in ascending order from first
 * column to last column. The proportion of a given category is computed by the delta between two poly lines
 * (or between 0.0 and 1st poly line or between last poly line and 1.0) at any given relative depth.
 *
 * Although this class extends DataFile, it does so to reuse the file reading/writing infrastructure
 * as VPC's are not spatial objects like wells, drillholes, reservoirs, etc.
 */
class VerticalProportionCurve : public DataFile
{
public:
    VerticalProportionCurve( QString path, QString associatedCategoryDefinitionName );

    /** Adds a new entry with zeros for all categories for the given relative depth (0.0==base, 1.0==top). */
    void addNewEntry( double relativeDepth );

    /** Returns the current number of entries in this VPC. */
    int getEntriesCount();

    /** Sets a proportion value for a given category in one of the entries. */
    void setProportion( int entryIndex, int categoryCode, double proportion );

    /**
     * Returns the pointer to the CategoryDefinition object whose name is
     * in m_associatedCategoryDefinitionName.  Returns nullptr it the name
     * is not set or the object with the name does not exist.
     */
    CategoryDefinition* getAssociatedCategoryDefinition() const;

    // ProjectComponent interface
public:
    virtual QIcon getIcon();
    virtual QString getTypeName();
    virtual void save(QTextStream *txt_stream);

    // File interface
public:
    virtual bool canHaveMetaData();
    virtual QString getFileType();
    virtual void updateMetaDataFile();
    virtual bool isDataFile();
    virtual bool isDistribution();
    virtual void deleteFromFS();

    //DataFile interface
public:
    bool isWeight( Attribute* at );
    Attribute* getVariableOfWeight( Attribute* weight );
    virtual void deleteVariable( uint columnToDelete );
    virtual bool isRegular();
    virtual double getDataSpatialLocation( uint line, CartesianCoord whichCoord );
    virtual void getDataSpatialLocation( uint line, double& x, double& y, double& z );
    virtual bool isTridimensional();

    // ICalcPropertyCollection interface
public:
    virtual void getSpatialAndTopologicalCoordinates( int iRecord, double& x, double& y, double& z, int& i, int& j, int& k );
    virtual double getNeighborValue( int iRecord, int iVar, int dI, int dJ, int dK );

protected:
    ///--------------data read from metadata file------------
    QString m_associatedCategoryDefinitionName;

    std::vector< VPCEntry > m_entries;
};

#endif // VERTICALPROPORTIONCURVE_H