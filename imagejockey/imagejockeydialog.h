#ifndef IMAGEJOCKEYDIALOG_H
#define IMAGEJOCKEYDIALOG_H

#include <QDialog>

class IJCartesianGridSelector;
class IJVariableSelector;
class ImageJockeyGridPlot;
class IJAbstractVariable;
class IJAbstractCartesianGrid;
class QwtWheel;
class GRCompass;
class Spectrogram1DParameters;
class Spectrogram1DPlot;
class EqualizerWidget;
class SVDFactor;

namespace spectral {
    struct array;
}

namespace Ui {
class ImageJockeyDialog;
}

/**
 * @brief The Image Jockey user interface.  The Image Jockey allows one to perform filtering in frequancy domain
 * of grid data like a DJ does to enhance frequencies via an equalizer.
 */
class ImageJockeyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImageJockeyDialog( const std::vector< IJAbstractCartesianGrid* >& grids, QWidget *parent = 0);
    ~ImageJockeyDialog();

signals:
    void errorOccurred( QString message );
    void warningOccurred( QString message );
    void infoOccurred( QString message );
	void savePreviewAs( const SVDFactor* factor );

private:
    Ui::ImageJockeyDialog *ui;

    /** Selector of the grid with the data in frequency domain (Fourier image). */
    IJCartesianGridSelector* m_cgSelector;

    /** Variable with the amplitude part of the Fourier transform. */
    IJVariableSelector* m_varAmplitudeSelector;

    /** Variable with the phase part of the Fourier transform. */
    IJVariableSelector* m_varPhaseSelector;

    /** Widget that displays the grid. */
    ImageJockeyGridPlot* m_gridPlot;

    /** Controls the color scale dB max. value. */
    QwtWheel* m_wheelColorMax;

    /** Controls the color scale dB min. value. */
    QwtWheel* m_wheelColorMin;

    /** Controls the reference dB value (0dB). */
    QwtWheel* m_wheelColorDecibelReference;

    /** Controls the azimuth of the band to collect 1D spectrum data. */
    GRCompass* m_azimuthCompass;

    /** Controls the angle variation around the azimuth to collect 1D spectrum data. */
    QwtWheel* m_azimthTolControl;

    /** Controls the width of the band to collect 1D spectrum data. */
    QwtWheel* m_bandwidthControl;

    /** Controls the distance from the spectrogram center to collect 1D spectrum data. */
    QwtWheel* m_radiusControl;

    /** The set of parameters used to calculate a 1D spectrogram from a band on a 2D spectrogram. */
    Spectrogram1DParameters* m_spectrogram1Dparams;

    /** The plot widget with the 1D spectrogram taken from a band in the 2D spectrogram map. */
    Spectrogram1DPlot* m_spectrogram1Dplot;

    /** The set of sliders to attenuate or amplify frequency components. */
    EqualizerWidget* m_equalizerWidget;

    /** The number of SVD factors set by the user in the SVD curve dialog. */
    int m_numberOfSVDFactorsSetInTheDialog;

    /** The list of available Cartesian grids. */
    std::vector< IJAbstractCartesianGrid* > m_grids;

    /** Causes a replot in the 2D grid spectrogram display.
     * TODO: Think of a more elegant way to trigger a replot, since QwtPlot's replot() is not working.
    */
    void spectrogramGridReplot();

private Q_SLOTS:
    void onUpdateGridPlot( IJAbstractVariable *var );
    void resetReferenceCurve();
    /** Negative dB variation means attenuation, positive variations mean amplification. */
    void equalizerAdjusted( double centralFrequency, double delta_dB );
    void save();
    void preview();
    void restore();
    void onSVD();
    void onUserSetNumberOfSVDFactors( int number );
    void onSumOfFactorsWasComputed( spectral::array* sumOfFactors );
	void onWidgetErrorOccurred( QString message );
    void onWidgetWarningOccurred( QString message );
	void onSavePreview( const SVDFactor* factor  ); //this is evoked when the user wants to save the grid shown in the preview dialog.
};

#endif // IMAGEJOCKEYDIALOG_H
