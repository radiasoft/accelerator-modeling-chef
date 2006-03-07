/**********************************************************************/
/*                                                                    */
/* File:           DataAlignWidget.h                                  */
/*                                                                    */ 
/* Authors:        Leo Michelotti                                     */
/*                 michelotti@fnal.gov                                */     
/*                                                                    */ 
/* Creation Date:  August 25, 2005                                    */
/* Current rev:    August 25, 2005                                    */
/*                                                                    */ 
/* Copyright:      (c) URA/Fermilab                                   */
/*                                                                    */
/**********************************************************************/

#include <qdialog.h>

class beamline;
class BeamlineContext;

class QVBox;

class DataAlignWidget : public QDialog
{
  Q_OBJECT
  public:
    // Constructors
    DataAlignWidget();
    DataAlignWidget( beamline&, QWidget* = 0, const char* = 0, WFlags = 0 );
    DataAlignWidget( BeamlineContext&, QWidget* = 0, const char* = 0, WFlags = 0 );
    DataAlignWidget( const DataAlignWidget& );
      // Produces an error: DataAlignWidget should not be copied.
    ~DataAlignWidget();

    void resizeEvent( QResizeEvent* );

  private:
    beamline*        _bmlPtr;      // NOT OWNED
    BeamlineContext* _bmlConPtr;   // NOT OWNED
                                   // Takes precedence
                                   // over _bmlPtr.
    void             _finishConstructor();

    QVBox* _qvbPtr;

  public slots:
    void cancel();
    void apply();
};
