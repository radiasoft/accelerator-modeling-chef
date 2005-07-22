#ifndef TMATRIX_H
#include "TMatrix.h"
#endif
#ifndef VECTOR_H
#include "VectorD.h"
#endif

#include <qdialog.h>

class beamline;
class BeamlineContext;
class ProtonBunch;

class QButtonGroup;
class QGridLayout;
class QLabel;
class QLineEdit;
class QRadioButton;
class QResizeEvent;
class QVBox;
class QWidget;

class DistributionWidget : public QDialog
{
  Q_OBJECT
  public:
    // Constructors
    DistributionWidget();
    DistributionWidget( ProtonBunch&, QWidget* = 0, const char* = 0, WFlags = 0 );
      // Can be used to fill the ProtonBunch.
    DistributionWidget( beamline&, ProtonBunch&, QWidget* = 0, const char* = 0, WFlags = 0 );
    DistributionWidget( BeamlineContext&, QWidget* = 0, const char* = 0, WFlags = 0 );
      // Will also propagate the bunch down a line.
      // !!! NOT WRITTEN ???
    DistributionWidget( const DistributionWidget& );
      // Produces an error: DistributionWidget should not be copied.
    ~DistributionWidget();

    void resizeEvent( QResizeEvent* );

    void setStatistics( const Vector& avg, const Matrix& cov );
    void getStatistics( Vector& avg, Matrix& cov );
    // avg: 

  private:
    beamline*        _bmlPtr;      // NOT OWNED
    BeamlineContext* _bmlConPtr;   // NOT OWNED
                                   // Takes precedence
                                   // over _bmlPtr.

    enum distType {uniform,gaussian,equilibrium} _kind;

    QVBox*        _qvbPtr;
    QVBox*        _leftStuff;
    QWidget*      _selPtr;
    QGridLayout*  _selQglPtr;
    QButtonGroup* _qbgPtr;
    QRadioButton* _aPtr;
    QRadioButton* _bPtr;
    QRadioButton* _cPtr;

    QWidget*      _unifPtr;
    QWidget*      _gaussPtr;
    QWidget*      _equilPtr;

    QLineEdit*    _populationPtr;

    QLineEdit*    _unif_xMin;
    QLineEdit*    _unif_xMax;
    QLineEdit*    _unif_yMin;
    QLineEdit*    _unif_yMax;
    QLineEdit*    _unif_cdtMin;
    QLineEdit*    _unif_cdtMax;
    QLineEdit*    _unif_pxMin;
    QLineEdit*    _unif_pxMax;
    QLineEdit*    _unif_pyMin;
    QLineEdit*    _unif_pyMax;
    QLineEdit*    _unif_dppMin;
    QLineEdit*    _unif_dppMax;

    QLineEdit*    _gauss_xAvg;
    QLineEdit*    _gauss_xSig;
    QLineEdit*    _gauss_yAvg;
    QLineEdit*    _gauss_ySig;
    QLineEdit*    _gauss_cdtAvg;
    QLineEdit*    _gauss_cdtSig;
    QLineEdit*    _gauss_pxAvg;
    QLineEdit*    _gauss_pxSig;
    QLineEdit*    _gauss_pyAvg;
    QLineEdit*    _gauss_pySig;
    QLineEdit*    _gauss_dppAvg;
    QLineEdit*    _gauss_dppSig;

    QLineEdit*    _equil_xBeta;
    QLineEdit*    _equil_yBeta;
    QLineEdit*    _equil_xAlpha;
    QLineEdit*    _equil_yAlpha;
    QLineEdit*    _equil_xEps;
    QLineEdit*    _equil_yEps;

  private:
    ProtonBunch* _theSamplePtr;
    Vector       _average;
    Matrix       _covariance;
    bool         _statsGiven;

    void         _finishConstructor();

    void         _genUniform();
    void         _genGaussian();
    void         _genEquilibrium();

  public slots:
    void changeLabels();
    void sample();
    void empty();
};

