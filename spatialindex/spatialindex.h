#ifndef SPATIALINDEX_H
#define SPATIALINDEX_H

#include <QList>
#include <vector>
#include <boost/geometry.hpp>
#include <boost/geometry/index/rtree.hpp>

class PointSet;
class CartesianGrid;
class DataCell;
class SearchStrategy;
class DataFile;
class GeoGrid;
class SegmentSet;

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;
typedef bg::model::point<double, 3, bg::cs::cartesian> Point3D;
typedef bg::model::box<Point3D> Box;
typedef std::pair<Box, size_t> BoxAndDataIndex;
typedef bgi::rtree< BoxAndDataIndex, bgi::rstar<16,5,5,32> > RStarRtree;
typedef std::pair< BoxAndDataIndex, double > BoxAndDataIndexAndDistance;

/**
 * This class exposes functionalities related to spatial indexes and queries with GammaRay objects.
 */
class SpatialIndex
{
public:
    SpatialIndex();
    virtual ~SpatialIndex();

    /** Fills the index with the PointSet points (bulk load).
     * It erases current index.
     * @param tolerance Sets the size of the bounding boxes around each point.
     */
	void fill( PointSet* ps, double tolerance );

	/** Fills the index with the CartesianGrid cells (bulk load).
     * It erases current index.
     */
	void fill( CartesianGrid* cg );

	/** Fills the index with the GeoGrid cells (bulk load).
     * It erases current index.
	 */
	void fill( GeoGrid* gg );

    /** Fills the index with the SegmentSet segments (bulk load).
     * It erases current index.
     * @param tolerance Sets the size of the bounding boxes around each segment.
     */
    void fill( SegmentSet* ss, double tolerance );

	/**
     * Returns the indexes of the n-nearest (in space) data lines to some data line given by its index.
	 * The indexes are the data record indexes (file data lines) of the DataFile used to fill
     * the index.
     */
    QList<uint> getNearest( uint index, uint n ) const;

	/**
     * Returns the indexes of the n-nearest (in space) data lines to a point in space.
	 * The indexes are the data record indexes (file data lines) of the DataFile used to fill
	 * the index.
	 */
    QList<uint> getNearest( double x, double y, double z, uint n ) const;

    /**
     * Returns the data line indexes of the n-nearest (in space) data lines within the given distance
     * to the point given by its index. The indexes are the data line indexes
     * (file data lines) of the DataFile used fill the index.  May return
     * an empty list.
     * @param distance The distance the returned points must be within.
     */
    QList<uint> getNearestWithin(uint index, uint n, double distance) const;

	/**
     * Returns the data line indexes of the n-nearest (in space) data lines within the given neighborhood
     * centered at given data cell (e.g. grid cell). The indexes are the data line indexes
     * (file data lines) of the DataFile used fill the index.  May return
     * an empty list.
	 */
    QList<uint> getNearestWithinGenericRTreeBased(const DataCell& dataCell,
                                        const SearchStrategy & searchStrategy ) const;

    /**
     * Does the same as getNearestWithinGenericRTreeBased() but is tuned for large, high-density data sets.
     * It may run slower for smaller data sets than the former, though.
     */
    QList<uint> getNearestWithinTunedForLargeDataSets(const DataCell& dataCell,
                                        const SearchStrategy & searchStrategy ) const;


    /** Clears the spatial index. */
	void clear();

	/** Returns whether the spatial index has not been built. */
    bool isEmpty() const;

private:
	void setDataFile( DataFile* df );

	/** The R* variant of the rtree
	* WARNING: incorrect R-Tree parameter may lead to crashes with element insertions
	*/
    RStarRtree m_rtree; //TODO: make these parameters variable (passed in the constructor?)

	/** The data file which is being indexed. */
	DataFile* m_dataFile;
};

#endif // SPATIALINDEX_H
