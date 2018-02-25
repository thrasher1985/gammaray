#ifndef SVDFACTOR_H
#define SVDFACTOR_H

//#include "spectral/spectral.h"

#include <QString>
#include <QIcon>
#include "../ijabstractcartesiangrid.h"

//third-party library Eigen
namespace spectral{
   class array;
   class complex_array;
}

enum class SVDFactorPlaneOrientation : int {
	XY,
	XZ,
	YZ
};


/**
 * @brief The SVDFactor class represents one factor obtained from Singular Value Decomposition (SVD).
 */
class SVDFactor : public IJAbstractCartesianGrid
{
public:
    static double getSVDFactorTreeSplitThreshold( bool reset = false );

    /**
	 * @param factorData The array of values of the factor.
	 * @param number The factor number in the series. This number is used to make the factor name.
	 * @param weight The information contribution of the factor, normally a value between 0.0 and 1.0.
	 * @param x0 Grid origin (useful for viewers).
	 * @param y0 Grid origin (useful for viewers).
	 * @param z0 Grid origin (useful for viewers).
	 * @param dx Cell size (useful for viewers).
	 * @param dy Cell size (useful for viewers).
	 * @param dz Cell size (useful for viewers).
     * @param mergeThreshold the minimum ammount of information content per child factor, e.g. 0.1 == 10%.
     *         Low information factors are merged until reaching this value.  Setting a value less than or equal to zero
     *         causes all individual factors to become a child.
     * @param parentFactor If set, this SVD factor is a decomposition of another SVD factor.
	 */
    SVDFactor(spectral::array &&factorData,
               uint number,
               double weight,
               double x0, double y0, double z0,
               double dx, double dy, double dz,
               double mergeThreshold,
               SVDFactor* parentFactor = nullptr );

	/** Default constructor used for the root "factor" in SVDFactorTree. */
	SVDFactor();

	virtual ~SVDFactor();

	void addChildFactor( SVDFactor* child );

	bool isSelected(){ return m_selected; }
	void setSelected( bool value ){ m_selected = value; }

    spectral::array& getFactorData(){ return *m_factorData; }

	/** Returns a text showing the factor number reflecting its hierarchy, e.g. 4.2 meaning that it
	 * is the second factor of the fourth root factor.
	 */
	QString getHierarchicalNumber();

	/** Returns a value at the current X,Y position with respect to the currently selected plane.
	 * The select plane depends on m_currentPlaneOrientation and m_currentPlane settings.
	 * The passed coordinates are with respect to the plane and not world coordinates, for example,
	 * localY is actually a Z coordinate if current plane orientation is XZ.
	 */
	double valueAtCurrentPlane( double localX, double localY );

	/** Returns whether the passed value represents a non-informed datum. */
	bool isNDV( double value );

	/** Getters for the geometry of the currently selected plane. */
	//@{
	double getCurrentPlaneX0();
	double getCurrentPlaneDX();
	uint getCurrentPlaneNX();
	double getCurrentPlaneY0();
	double getCurrentPlaneDY();
	uint getCurrentPlaneNY();
	//@}

    //@{
    /** Getters for the absolute max. and min. values in the factor. */
    double getMinValue();
	double getMaxValue();
    //@}

	//@{
	/** Getters for the absolute grid geometry . */
	double getX0(){ return m_x0; }
	double getY0(){ return m_y0; }
	double getZ0(){ return m_z0; }
	double getDX(){ return m_dx; }
	double getDY(){ return m_dy; }
	double getDZ(){ return m_dz; }
    //@}

    /** Returns the number of slices.  This depends on the current plane orientation (XY, XZ, YZ). */
	uint getCurrentPlaneNumberOfSlices();

    //@{
    /** These methods control data selection in 3D volumes for a 2D slice viewer. */
    void setCurrentSlice( uint slice ){ m_currentSlice = slice; }
    void setPlaneOrientation( SVDFactorPlaneOrientation orientation );
    //@}

    /** Adds this factor's data array's values to the passed array's values.
     * The arrays involved in the sum must be compatible, similarly to addition of matrices.
     * @param ifSelected If true, only factors set as selected will be added.
     */
    void addTo( spectral::array* array , bool ifSelected );

    /** Returns whether this factor has child factors. */
    bool hasChildren(){ return !m_childFactors.empty(); }

    /**
     * Sets a name to be used instead of the default names like ROOT, Factor 1, Factor 2, etc.
     */
    void setCustomName( QString name ){ m_customName = name; }

	/**
	 * Sums the weight and the factor data of the other factor, which is destroyed and the pointer set to null.
	 * This and the other factor must be compatible, like matrices compatible for addition.
	 */
	void merge( SVDFactor *& other );

	double getWeight(){ return m_weight; }

    /** Returns the information content corresponding to this factor.
     * It equals the factor weight if it is a top-level factor.  For a factor that was
     * split from a parent factor, it equals this factor's weight times the parent's information content.
     */
    double getInfoContent();

private:
    SVDFactor* m_parentFactor;
    spectral::array* m_factorData;
	uint m_number;
	std::vector< SVDFactor* > m_childFactors;
	bool m_selected;
	double m_weight;
	SVDFactorPlaneOrientation m_currentPlaneOrientation;
    uint m_currentSlice;
	double m_x0, m_y0, m_z0;
	double m_dx, m_dy, m_dz;
	bool m_isMinValueDefined, m_isMaxValueDefined;
	double m_minValue, m_maxValue;
    IJAbstractVariable* m_variableProxy; //this object represents the internal data for the IJAbstractCartesianGrid interface.
    QString m_customName; //if defined, this is used as name, instead of Factor 1, Factor 2, etc.
    double m_mergeThreshold;
    uint getIndexOfChild( SVDFactor* child );
	bool isRoot();
	void setParentFactor( SVDFactor* parent );
	void setWeight( double weight ){ m_weight = weight; }
	bool isTopLevel();
	bool XYtoIJinCurrentPlane(double localX, double localY, uint& i, uint& j );
	double dataIJK( uint i, uint j, uint k);
    /**
     * Returns, via output variables (i,j and k), the IJK coordinates corresponding to a XYZ spatial coordinate.
     * Returns false if the spatial coordinate lies outside the grid.
     */
    bool XYZtoIJK( double x, double y, double z,
                   uint& i,   uint& j,   uint& k );

	// Methods to support the QAbstractItemModel interface
public:
	SVDFactor* getChildByIndex( uint index );
	SVDFactor* getParent( );
	uint getIndexInParent( );
	uint getChildCount( );
	QString getPresentationName( );
	QIcon getIcon( );

    // IJAbstractCartesianGrid interface
public:
    virtual double getRotation(){ return 0.0; }
    virtual int getNI();
    virtual int getNJ();
    virtual int getNK();
    virtual double getCellSizeI(){ return m_dx; }
    virtual double getCellSizeJ(){ return m_dy; }
    virtual double getCellSizeK(){ return m_dz; }
    virtual double getOriginX(){ return m_x0; }
    virtual double getOriginY(){ return m_y0; }
    virtual double getOriginZ(){ return m_z0; }
    virtual double getData(int variableIndex, int i, int j, int k);
    virtual bool isNoDataValue(double){ return false; }
    virtual double getDataAt(int variableIndex, double x, double y, double z);
    virtual double absMax(int variableIndex);
    virtual double absMin(int variableIndex);
    virtual void dataWillBeRequested(){} //SVDFactors are in-memory grids, there is no need to prefetch data.
    virtual QString getGridName(){ return getPresentationName(); }
    virtual QIcon getGridIcon(){ return getIcon(); }
    virtual int getVariableIndexByName(QString){ return 0; } //SVDFactors have just one variable.
    virtual IJAbstractVariable *getVariableByName(QString){ return m_variableProxy; }
    virtual void getAllVariables(std::vector<IJAbstractVariable *> &result);
    virtual IJAbstractVariable *getVariableByIndex(int){ return m_variableProxy; }
    virtual void equalizeValues(QList<QPointF> &area, double delta_dB, int variableIndex, double dB_reference, const QList<QPointF> &secondArea);
    virtual void saveData();
    virtual spectral::array *createSpectralArray(int variableIndex);
    virtual spectral::complex_array *createSpectralComplexArray(int variableIndex1, int variableIndex2);
    virtual void clearLoadedData(){} //SVDFactors are not persistible
    virtual long appendAsNewVariable(const QString variableName, const spectral::array &array);

};

#endif // SVDFACTOR_H
