/*************************************************************************
**************************************************************************
**************************************************************************
******
******  PHYSICS TOOLKIT: Library of utilites and Sage classes
******             which facilitate calculations with the
******             BEAMLINE class library.
******
******  File:      DriftConverter.h
******
******             Description for class DriftConverter
******             - automatically sandwiches bends between small Slots
******               for realizing simplest geometries.
******             - used after beamline is constructed using
******               drifts only.
******             - supersedes global function DriftsToSlots,
******               but with an altered algorithm.
******               - recall: DriftsToSlots replaced entire drifts
******                 with Slots.
******
******  Author:    Leo Michelotti
******             Phone: (630) 840 4956
******             Email: michelotti@fnal.gov
******
******
******  Copyright (c) 2010  Universities Research Association, Inc.
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms
******  of the License supplied with this software.
******
******  Software and documentation created under
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000.
******  The U.S. Government retains a world-wide non-exclusive,
******  royalty-free license to publish or reproduce documentation
******  and software for U.S. Government purposes. This software
******  is protected under the U.S. and Foreign Copyright Laws.
******
******  ----------------
******  REVISION HISTORY
******  ----------------
******  January, 2010          michelotti@fnal.gov
******  - original version
******    : altered algorithm starting from DriftsToSlots.cc
******    : requires further testing and improvement but
******      am uploading nonetheless.
****** 
**************************************************************************
*************************************************************************/


#ifndef DRIFTCONVERTER_H
#define DRIFTCONVERTER_H

#include <beamline/beamline.h>  // ??? Needed here for beamline::iterator ???

class DriftConverter
{
 public:
  DriftConverter();
  ~DriftConverter();

 public:
  BmlPtr convert( beamline const& );

 private:
  bool a_rb_, a_CFrb_, a_sb_, a_CFsb_;
  bool c_rb_, c_CFrb_, c_sb_, c_CFsb_;

  ElmPtr a_;
  ElmPtr c_;

  bool lookUpStream_( beamline::iterator );
  bool lookDownStream_( beamline::iterator );
  bool rbendLike_( bmlnElmnt const& );
  bool sbendLike_( bmlnElmnt const& );
  void appendSlotUpstream_(   BmlPtr&, double slotLength );
  void appendSlotDownstream_( BmlPtr&, double slotLength );
};

#endif // DRIFTCONVERTER_H
