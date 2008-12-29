#ifndef _h_MLBPMs
#define _h_MLBPMs 1

#include <beamline/beamline.h>
#include <basic_toolkit/Distribution.h>
#include <vector>
#include <iostream>
#include <MLBPM.h>
#include <basic_toolkit/Distribution.h>
// A container class for all my BPM;s and some utilities, excluding 
// Root plotting. 
// Will dump the data to files to be studied with R 

using std::vector;

typedef boost::shared_ptr<MLBPM> MLBPMPtr;

//! \brief Beam Position Monitor System
/*!
   A container class for the BPM system used in Steering. 
   The Std::vector contain Shared Boost Pointers, of type MLBPM
   
  Paul Lebrun, 2006-2007, finalized Feb 2008. 
*/
class MLBPMs : public  vector<MLBPMPtr> {

public :
    /*! Empty Constructor. 
    */
    explicit MLBPMs();
    ~MLBPMs();
    /*! Return the number of defined BPMs 
    */
    inline size_t GetNumBPMS() {return  size();}
    /*! Add a BPM to the system. 
    */
    inline void addOne(MLBPMPtr bpm) {push_back(bpm);}
    /*! Reset all BPMs.  
    */
    inline void reset() { for (size_t k=0; k != size(); k++) (*this)[k]->Reset();}
    /*! Set to integrate.  See definiton in the BPM class.   
    */
    inline void setToIntegrate () 
        { for (size_t k=0; k != size(); k++) (*this)[k]->IntegrateOn(true);}
    /*! Revert back to pulse by pulse reading.   
    */
    inline void setToOverWrite () 
        { for (size_t k=0; k != size(); k++) (*this)[k]->IntegrateOn(false);}
    /*! Ready to get all average
      \param n The number of bunches that were send through.     
    */
    inline void computeAverages (int n) 
        { for (size_t k=0; k != size(); k++) (*this)[k]->Average(n);}
	
    /*! Set the offset.  Note: often, these are implemented as a physical 
        change of reference, i..e, via the CHEF alignment data structure.     
    */
    void setGaussianOffsets(double sigma, double sigmaCut=999., 
                            int nBegin = 3); // First 3 BPM perfectly aligned
    /*! Define scale factors, Gaussian model.
       \param sigma  the r.m.s. of the Gaussian used to generate randomly these offsets. 
       \param sigmaCut Cutting the tail: if an offset falls above or below this parameter, 
                 we throw again. 
       \param nBegin  Don't set offset for the first nBegin BPMS	       
    */
    void setGaussianScales(double sigma,double sigmaCut=999., 
                            int nBegin = 3 );
    /*! Set the offset for a psecific BPM
      \param index the sequential index into the BPM array. 
      \param value its offset value.      
    */
    void setAnOffset(int index, double value);
    double getOffset(int index);
    /*! Set the resolution for all BPM. 
       \param value the resolution for all BPM ( all identical)  
    */
    void setResolution(double value);
    inline void setGaussianDist(Gaussian* ag) {theGaussDist = ag;}
    inline Gaussian* getGaussianDist() {return theGaussDist;}
    /*! Provide an outpute streamer...  
       \param out the std::streamer ( file, or std::cout,.. ) 
       \param sMax  No info past this longitudinal coordinate.   
    */
    void DumpIt(std::ostream& out, double sMax = 1.0e10);
    private:
      Gaussian* theGaussDist; // Owned by SteerUtil.  
};

#endif
