/************************************************************************
 * This file has been generated automatically from                      *
 *                                                                      *
 * src/gui/symbology-ng/qgsdashspacedialog.h                            *
 *                                                                      *
 * Do not edit manually ! Edit header and run scripts/sipify.pl again   *
 ************************************************************************/



class QgsDashSpaceDialog: QDialog
{
%Docstring
 A dialog to enter a custom dash space pattern for lines
%End

%TypeHeaderCode
#include "qgsdashspacedialog.h"
%End
  public:
    QgsDashSpaceDialog( const QVector<qreal> &v, QWidget *parent /TransferThis/ = 0, Qt::WindowFlags f = 0 );

    QVector<qreal> dashDotVector() const;
%Docstring
 :rtype: list of qreal
%End

};

/************************************************************************
 * This file has been generated automatically from                      *
 *                                                                      *
 * src/gui/symbology-ng/qgsdashspacedialog.h                            *
 *                                                                      *
 * Do not edit manually ! Edit header and run scripts/sipify.pl again   *
 ************************************************************************/
