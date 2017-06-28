#ifndef UTIL_H
#define UTIL_H
#include <QStringList>
#include <QString>
#include <QList>
#include <complex>
#include "array3d.h"

//macro used to do printf on QString for debugging purposes
//it is safe to delete this.
#define PRINT(x) printf("%s\n", x.toStdString().c_str())

class QWidget;
class QPlainTextEdit;
class QFrame;
class CartesianGrid;
class Attribute;
class CategoryDefinition;
class VariogramModel;

/*! Display resolution classes used to select an adequate set of icons and maybe other
 *  GUI measures sensitive to display resolution. */
enum class DisplayResolution : uint {
    NORMAL_DPI = 0, /*!< For usual SVGA displays or high vertical resultion in physically large screens. */
    HIGH_DPI        /*!< For the so-called 4k displays, unless exceptionally large screens. */
};

/*! FFT computation mode for fft1d() and fft2d(). */
enum class FFTComputationMode : int {
    DIRECT = 0, /*!< Functions fft1d() and fft2d() takes an input in real space and result is in frequency space. */
    REVERSE     /*!< Functions fft1d() and fft2d() takes an input in frequency space and result is in real space. */
};

/*! Computation direction for fft1d() when taking a 3D array. */
enum class FFT1DDirection : int {
    DIR_I = 0, /*!< Computes along I (inlines). */
    DIR_J,     /*!< Computes along J (crosslines). */
    DIR_K      /*!< Computes along K (traces). */
};

/**
 * @brief The Util class organizes system-wide utilitary functions.
 */
class Util
{
public:
    Util();

    /** Returns the list of variable names available in the given
     * GSLib format data file.  GSLib files are in GEO-EAS format.
     * The list is in the same order as found in the file, so you
     * can use QStringList's index to find the variable column in
     * the file.
     */
    static QStringList getFieldNames( const QString gslib_data_file_path );

    /**
     * @brief Decomposes a parameter file template line into its tags and description.
     * Param types in parameter file templates follows the pattern "<type[specifiers]> <type2>  -description"
     * @return A pair composed by a string list with the tags found and a separate string with the description text.
     */
    static std::pair<QStringList, QString> parseTagsAndDescription( const QString gslib_param_file_template_line );

    /**
     * @brief Returns the number of leading spaces in the given template file line.
     * This value defines the scope of the special <repeat> tag.
     */
    static uint getIndentation( const QString gslib_param_file_template_line );

    /**
     * Returns the type name of the given tag.  For example: for the tag "<range [-1:L] [1:R]>" the
     * function returns "range".
     */
    static QString getNameFromTag( const QString tag );

    /**
     * Returns wether the passed tag has a plus (+) sign or not, denoting a variable
     * length multivalued parameter.
     */
    static bool hasPlusSign(const QString tag);

    /**
     * Returns the reference name of the given tag.  For example: for the tag "<string (title)>" the
     * function returns "title".  Returns an empty string if the parameter is anonymous.
     */
    static QString getRefNameFromTag( const QString tag );

    /**
     * Returns a list of pairs (option value, option description) that may be found inside a tag.
     * For example: for the tag "<range [-1:L] [1:R]>" the function returns the list of two pairs (-1, L),(1, R)
     */
    static std::vector< std::pair<QString, QString> > getTagOptions( const QString tag );

    /**
     * Returns a tag reference by name that may be found inside a tag.
     * For example: for the tag "<repeat [nvar]>" the function returns "nvar", which is a name given to
     * another tag.  Returns an empty string if there is no referece to another tag.
     */
    static QString getReferenceName( const QString tag );

    /**  Perfoms a reliable way to compare float values.
    * credit: Bruce Dawson
    * source: http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm
    */
    static bool almostEqual2sComplement(float A, float B, int maxUlps);

    /**
     * 64-bit version of almostEqual2sComplement by me.
     */
    static bool almostEqual2sComplement(double A, double B, int maxUlps);

    /**
      *  Remove all child widgets from the given widget.
      */
    static void clearChildWidgets( QWidget* widget );

    /**
     * Fills the given QPlainTextEdit with some lines of the given text file.
     */
    static void readFileSample( QPlainTextEdit* text_field, QString file_path );

    /**
     * Adds a triling hiphen to each parameter line of a .par text file.
     * Some GSLib programs fail when the .par file lacks those.
     */
    static void addTrailingHiphens(const QString par_file_path );

    /**
     * @brief Returns the first number in the given text file line.
     */
    static uint getFirstNumber( const QString line );

    /**
     * Returns the part of a GSLib parameter file line before the hiphen signaling the line comment.
     * Returns the entire line is no comment exists.
     * Example: for "10 5 2    -nx, ny, nz" returns "10 5 2"
     */
    static QString getValuesFromParFileLine( const QString line );

    /**
     *  Replaces asterisks created by a bug in varmap with a no-data value.
     */
    static void fixVarmapBug( const QString varmap_grid_file_path );

    /**
     * Renames one variable name in a GEO-EAS file.
     * @note This function only works on the physical file only.  Metadata kept by client code
     *       must update its internally kept information from the changed file.
     * @note This function also trims heading and trailing spaces from the variable names.
     */
    static void renameGEOEASvariable( const QString file_path,
                                      const QString old_name,
                                      const QString new_name );
    /**
     * Copies a physical file.  If there is a file in the destination path, overwrites.
     */
    static void copyFile( const QString from_path, const QString to_path );

    /**
     * Copies a physical file to the specified directory.
     * If there is a file in the destination path, overwrites.
     * @return The complete path after the copy operation.
     */
    static QString copyFileToDir( const QString from_path, const QString path_to_directory );

    /**
     * Creates a GEO-EAS regular grid file using the grid specs from the
     * passed CartesianGrid object.  The resulting file contains a single binary variable
     * that results in a checkerboard battern in the plots.
     */
    static void createGEOEAScheckerboardGrid( CartesianGrid* cg, QString path );

    /**
     * Creates a GEO-EAS regular grid file using the given grid specs and the values
     * passed in the unidimensional vector of complex values.
     * If you omit a name for a field/column, the corresponding values will not be
     * written to the file.  If you omit (empty string) both names, no file will be generated.
     * @note the array elements are expected to follow the GEO-EAS grid scan protocol for the elemental
     * three indexes (i, j and k): array[ i + j*nI + k*nJ*nI ]
     */
    static void createGEOEASGrid( const QString columnNameForRealPart,
                                  const QString columnNameForImaginaryPart,
                                  std::vector< std::complex<double> > &array,
                                  QString path );

    /**
     * Runs the GSLib program pixelplt and opens the plot dialog to view a
     * variable in a regular grid.
     * @param parent Parent QWidget for the plot dialog.
     * @param modal If true, the method returns only when the user closes the Plot Dialog.
     * @param cd If informed, the grid is renderd as a categorical variable.
     * @return True if modal == true and if the user did not cancel the Plot Dialog; false otherwise.
     */
    static bool viewGrid(Attribute* variable ,
                         QWidget *parent,
                         bool modal = false,
                         CategoryDefinition *cd = nullptr);

    /**
     * Runs the GSLib program locmap and opens the plot dialog to view a
     * variable in a point set file.
     * @param parent Parent QWidget for the plot dialog.
     * @param modal If true, the method returns only when the user closes the Plot Dialog.
     */
    static bool viewPointSet(Attribute* variable ,
                             QWidget *parent,
                             bool modal = false);

    /**
     * Runs the GSLib program scatplt and opens the plot dialog to view a
     * a crossplot between two or three variable.
     * @param parent Parent QWidget for the plot dialog.
     * @note This method assumes all variables belong to the same parent file.
     */
    static void viewXPlot( Attribute* xVariable, Attribute* yVariable, QWidget *parent, Attribute* zVariable = nullptr );

    /** Returns the total size, in bytes, of the contents of the given directory.
     *  It is a recursive function, so it may take a while for large directories.
     */
    static qint64 getDirectorySize( const QString path );

    /** Deletes all files in the given directory. */
    static void clearDirectory( const QString path );

    /** Renames the file given by path.
     *  @param newName The new file name, e.g. "foo.txt"
     *  @return The complete path to the file after renaming.
     */
    static QString renameFile( const QString path, const QString newName );

    /**
     * The value of no-data value generated by the varmap GSLib program.
     * See util.cpp for the constant definition.
     */
    static const QString VARMAP_NDV;

    /**
    *  Imports a univariate distribution file to the project as a new UnivariateDistribution object.
    *  @param at The attribute that originated the distribution.
    *  @param path_from The complete path to the file.
    *  @param dialogs_owner Widget used to anchor the modal dialogs to.
    */
    static void importUnivariateDistribution(Attribute *at, const QString path_from , QWidget *dialogs_owner);

    /**
     * Populates the passed list with QColor objects containing colors according to GSLib convention.
     * The indexes in the list + 1 are the GSLib color codes.  For instance, red is the first color,
     * list index == 0, thus the color code is 1.
     */
    static void makeGSLibColorsList( QList<QColor> &colors );

    /**
     * Creates a 16x16 pixel QIcon filled with a GSLib color given its code.
     */
    static QIcon makeGSLibColorIcon( uint color_code );

    /**
      * Returns a QColor given a GSLib color code.
      */
    static QColor getGSLibColor( uint color_code );

    /**
      * Returns the name of a GSLib color given its code.
      */
    static QString getGSLibColorName( uint color_code );

    /**
     * Imports the registry/user home seetings from a previus version of GammaRay.
     * The import happens only if there are no seetings for this version.
     * Nothing happens if there no previous settings are found.
     */
    static void importSettingsFromPreviousVersion();

    /**
     * Returns an ID to classify display resolutions.
     */
    static DisplayResolution getDisplayResolutionClass();

    /**
     * Return the last browsed directory in a file dialog.
     */
    static QString getLastBrowsedDirectory();

    /**
     * Saves the last browsed directory in a file dialog to registry/home settings.
     * The parameter must be a directory path.
     */
    static void saveLastBrowsedDirectory(QString dir_path);

    /**
     * Saves the last browsed directory in a file dialog to registry/home settings.
     * The parameter must be a file path.
     */
    static void saveLastBrowsedDirectoryOfFile(QString file_path);

    /**
     * Returns the PROGRAMFILES environmental variable in Windows or /usr otherwise.
     */
    static QString getProgramInstallDir();

    /**
     * Returns the number of file lines that make up the header of the given GEO-EAS file.
     */
    static uint getHeaderLineCount( QString file_path );

    /**
      * Returns the first line of a GEO-EAS file (the file description).
      */
    static QString getGEOEAScomment( QString file_path );

    /**
     * Creates a widget with the appearance of a horizontal line, normally used as a separator.
     */
    static QFrame* createHorizontalLine();

    /**
     * Creates a widget with the appearance of a vartical line, normally used as a separator.
     */
    static QFrame* createVerticalLine();

    /** Returns whether the given set of variograms form a Linear Model of Coregionalization.
     *  Any problems encountered are reported to the main window's message panel as error messages
     *  so the user can take corrective measures.
     *  @param vmVar1 Autovariogram of 1st variable.
     *  @param vmVar2 Autovariogram of 2nd variable.
     *  @param crossVariogram Cross variogram between the variables (no lag effect assumed).
     */
    static bool isLMC( VariogramModel *vmVar1, VariogramModel *vmVar2, VariogramModel* crossVariogram );

    /** Saves the given list of strings as lines in the given text file. */
    static void saveText( const QString filePath, const QStringList lines);

    /** Computes FFT (forward or reverse) for a vector of values.  The result will be stored in the input array.
     *  This is a C++ port from the original Fortran implementation by Jon Claerbout (1985).
     *  @note The array elements are OVERWRITTEN during computation.
     *  @note This is a WIP.  Not currently working accurately, probably by something misinterpreted from the
     *        Fortran code.  Help with this will be much appreciated.  Original Fortran code is in the .cpp.
     *        function body as a series of comments.
     *  @param lx Number of elements in values array.
     *  @param cx Input/output vector of values (complex numbers).
     *  @param startingElement Position in cx considered as 1st element (pass zero if the array is unidimensional).
     *  @param isig 0 or 1 to transform or back-transform respectively.
     */
    static void fft1D(int lx, std::vector<std::complex<double> > &cx, int startingElement, FFTComputationMode isig );

    /**
     *  Slight modification to use the more portable STL's complex type and be called from an FFT 2D routine
     *  from the modification of Paul Bourkes FFT code by Peter Cusack
     *  to utilise the Microsoft complex type.
     *
     *  This computes an in-place complex-to-complex FFT
     *  dir =  1 gives forward transform
     *  dir = -1 gives reverse transform
     *
     *  @param m log2(number of cells). Number of cells should be 4, 16, 64, etc...
     *  @note WIP: This function is not compatible with fft2D().
     */
    static void fft1DPPP(int dir, long m, std::vector< std::complex <double> > &x, long startingElement);

    /** Computes 2D FFT (forward or reverse) for an array of values.  The result will be stored in the input array.
     *  This is a C++ port from the original Fortran implementation by M.Pirttijärvi (2003).
     *  @note The array elements are OVERWRITTEN during computation.
     *  @note The array should be created by making a[nI*nJ*nK] and not a[nI][nJ][nK] to preserve memory locality (maximize cache hits)
     *  @param n1 Number of elements in X/I direction.
     *  @param n2 Number of elements in Y/J direction.
     *  @param cx Input/output array of values (complex numbers).
     *  @param isig 0 or 1 to transform or back-transform respectively.
     */
    static void fft2D(int n1, int n2, std::vector<std::complex<double> > &cp, FFTComputationMode isig );
    
    /** Split function specialized to tokenize data lines of GEO-EAS files.
     *  @note This is not a generic tokenizer, so do not use for other applications.
     */
    static QStringList fastSplit( const QString lineGEOEAS );

    /** Computes 3D FFT (forward or reverse) for an array of values.  The result will be stored in the input array.
     *  @note The array elements are OVERWRITTEN during computation.
     *  @note The array should be created by making a[nI*nJ*nK] and not a[nI][nJ][nK] to preserve memory locality (maximize cache hits)
     *  @param nI Number of elements in X/I direction.
     *  @param nJ Number of elements in Y/J direction.
     *  @param nJ Number of elements in Y/J direction.
     *  @param values Input/output array of values (complex numbers).
     *  @param isig 0 or 1 to transform or back-transform respectively.
     */
    static void fft3D(int nI, int nJ, int nK, std::vector<std::complex<double> > &values, FFTComputationMode isig );
};

#endif // UTIL_H
