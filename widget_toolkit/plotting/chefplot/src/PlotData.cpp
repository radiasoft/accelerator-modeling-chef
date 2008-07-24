/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF:      A Qt-based Application 
******             Layered on top of of BEAMLINE.
******                                                                
******  File:      Plotdata.cpp
******                                                                
******  Copyright (c) Universities Research Association, Inc.   
******                All Rights Reserved                             
******                                                                
******  Author:    Jean-Francois Ostiguy
******                                                                
******             Fermilab                                           
******             Batavia, IL  60510                                
******             ostiguy@fnal.gov                         
******                                                                
******                                                                
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******* U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******* The U.S. Government retains a world-wide non-exclusive, 
******* royalty-free license to publish or reproduce documentation 
******* and software for U.S. Government purposes. This software 
******* is protected under the U.S. and Foreign Copyright Laws. 
******* URA/FNAL reserves all rights.
*******                                                                
**************************************************************************
**************************************************************************
*************************************************************************/

#include <basic_toolkit/GenericException.h>
#include <sqlite/query.hpp>
#include <PlotData.h>
#include <CurveData.h>
#include <beamline.h>
#include <string>
#include <vector>
#include <algorithm>

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

PlotData::PlotData( sqlite::connection& db)
 : curves_(), 
   label_top_("label_top"),   label_bottom_("label_bottom"),
   label_left_("label_left"), label_right_("label_right"),
   scalemag_top_(1.0),  scalemag_bottom_(1.0), 
   scalemag_left_(1.0), scalemag_right_(1.0),
   tunes_(), db_(&db), bml_()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

PlotData::PlotData()
 : curves_(), 
   label_top_("label_top"),   label_bottom_("label_bottom"),
   label_left_("label_left"), label_right_("label_right"),
   scalemag_top_(1.0),  scalemag_bottom_(1.0), 
   scalemag_left_(1.0), scalemag_right_(1.0),
   tunes_(), db_(0), bml_()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

PlotData::~PlotData()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

PlotData::Tunes const& PlotData::getTunes() const
{
  return  tunes_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void PlotData::setTunes(double const& hor, double const& ver)
{
  tunes_ = Tunes(hor,ver);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

sqlite::connection& PlotData::getDb() const
{
  return *db_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ConstBmlPtr PlotData::getBeamline() const
{
  return bml_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void PlotData::setBeamline( BmlPtr bml)
{
  bml_ = bml;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void PlotData::addCurve(CurveData& cdata)
{
   curves_.push_back(cdata);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


CurveData const& PlotData::operator[](int i) const 
{
  return curves_[i];
}

    
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void PlotData::setTitle( std::string  const& label) 
{
  label_top_ = label; 
}
 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::string const&  PlotData::getTitle() const
{
  return label_top_;
}
 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void PlotData::setXLabel( std::string const& label) 
{
  label_bottom_ = label; 
}
 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void PlotData::setYLabel( CurveData::Axis id,  std::string const& label) 
{

  switch (id) {

    case  CurveData::yLeft:      label_left_   = label;  
                                 break;

    case  CurveData::yRight:     label_right_  = label;  
                                 break;

    default:
                                 break;
  }

}
 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


std::string const& PlotData::getLabel( CurveData::Axis id) const 
{

  switch (id) {

    case CurveData::xBottom :    return label_bottom_;  
                                 break;

    case  CurveData::yLeft  :    return label_left_;  
                                 break;

    case  CurveData::yRight :    return label_right_;  
                                 break;

    default:                     
      throw GenericException( __FILE__, __LINE__, "PlotData::getLabel( CurveData::Axis id) const",
                                                   "Error: Invalid Axis id " );
                                 break;
  }

}
 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


double PlotData::xMin( CurveData::Axis id ) const 
{

  std::vector<iterator> id_curves; 

  for (std::vector<CurveData>::const_iterator it  = curves_.begin();  
                                              it != curves_.end(); ++it ) { 
    if ( it->getXAxis() == id ) 
      id_curves.push_back(it);
  }
 
  if ( id_curves.empty() ) return 0.0;
 
  double xmin =  curves_.front().xMin();

  for (std::vector<iterator>::const_iterator it  = id_curves.begin();  
                                             it != id_curves.end(); ++it ) { 
    xmin = std::min( xmin, (*it)->xMin() ); 
  }

  return xmin;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


double PlotData::xMax( CurveData::Axis id ) const 
{

  std::vector<iterator> id_curves; 

  for (std::vector<CurveData>::const_iterator it  = curves_.begin();  
                                              it != curves_.end(); ++it ) { 
    if ( it->getXAxis() == id  ) 
      id_curves.push_back(it);
  }
 
  if ( id_curves.empty() ) return 0.0;
 
  double xmax =  curves_.front().xMax();

  for (std::vector<iterator>::const_iterator it  = id_curves.begin();  
                                             it != id_curves.end(); ++it ) { 
    xmax = std::max( xmax, (*it)->xMax() ); 
  }

    return xmax;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


double PlotData::yMin( CurveData::Axis id ) const 
{

  std::vector<iterator> id_curves; 

  for (std::vector<CurveData>::const_iterator it  = curves_.begin();  
                                              it != curves_.end(); ++it ) { 
    if ( it->getYAxis() == id ) 
      id_curves.push_back(it);
  }
 
  if ( id_curves.empty() ) return 0.0;
 
  double ymin =  curves_.front().yMin();

  for (std::vector<iterator>::const_iterator it  = id_curves.begin();  
                                             it != id_curves.end(); ++it ) { 
    ymin = std::min( ymin, (*it)->yMin() ); 
  }



  return ymin; 

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


double PlotData::yMax(CurveData::Axis id) const 
{

  std::vector<iterator> id_curves; 

  for (std::vector<CurveData>::const_iterator it  = curves_.begin();  
                                              it != curves_.end(); ++it ) { 
    if ( it->getYAxis() == id ) 
      id_curves.push_back(it);
  }
 
  if ( id_curves.empty() ) return 0.0;
 
  double ymax =  curves_.front().yMax();

  for (std::vector<iterator>::const_iterator it  = id_curves.begin();  
                                             it != id_curves.end(); ++it ) { 
    ymax = std::max( ymax, (*it)->yMax()); 
  }

  return ymax;
}



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int PlotData::nCurves() const  
{
  return curves_.size();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void PlotData::setScaleMag( CurveData::Axis id, double value) 
{
 
  switch ( id ) {

    case CurveData::yLeft  :  scalemag_left_   = value;
                            break;
    case CurveData::yRight :  scalemag_right_  = value;
                            break;
    case CurveData::xBottom:  scalemag_bottom_ = value;
                            break;
    case CurveData::xTop   :  scalemag_top_    = value;
                            break;
    default:                break;
  }    

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


double PlotData::getScaleMag( CurveData::Axis id) const  
{
 
  switch ( id ) {

    case CurveData::yLeft  :  return scalemag_left_;
                            break;
    case CurveData::yRight :  return scalemag_right_;
                            break;
    case CurveData::xBottom:  return scalemag_bottom_;
                            break;
    case CurveData::xTop   :  return scalemag_top_;
                            break;
    default:                return 1.0; 
                            break; 
  }    

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void PlotData::init( sqlite::connection& db, int npltattribs, char const* plot_attributes[],  
                                             int ncrvs, CurveData::curve_attribute curve_attributes[],  char const* sql[] )
{   

  sqlite::query q1(db, sql[0]);

  sqlite::result_type res1 = q1.emit_result();
  int nelms =  res1 ? res1->get_int(0) : 0;


  std::vector< boost::shared_ptr<CurveData> > curves( ncrvs );

  for (int i=0; i<ncrvs; ++i ) { 
      curves[i] = boost::shared_ptr<CurveData>(new CurveData( nelms, curve_attributes[i].label )); 
      curves[i]->setAxes(  CurveData::xBottom, curve_attributes[i].yaxis );
      curves[i]->setColor( curve_attributes[i].rgb  );
  };

  sqlite::query q2(db, sql[1] );
  sqlite::result_type res2 = q2.emit_result(); 

  std::vector<CurveData::iterator> itc(ncrvs);  

  for ( int i=0; i<ncrvs; ++i ) {
    itc[i] = curves[i]->begin();
  };

  if ( !res2 ) return;   // ERROR !!
  
  do { 
      
      double abscissa =  res2->get_double(0);  

      for ( int i=0; i<ncrvs; ++i ) {
         *(itc[i]++) = CurveData::Point( abscissa, res2->get_double(1+i));          
      }

  } while ( res2->next_row() );
  

  for (int i=0; i<ncrvs; ++i ) { 
     addCurve( *curves[i] ); 
  }

  setXLabel( plot_attributes[1] );
  setYLabel( CurveData::yLeft, plot_attributes[2]  );
  setYLabel( CurveData::yRight, plot_attributes[3] );

}




