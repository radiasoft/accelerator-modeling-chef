#ifndef FINDCOVARIANCE_H
#define FINDCOVARIANCE_H

class WIREData : public BarnacleData {
public:
  float hwires[48];
  float vwires[48];
  float hmean;
  float hsigma;
  float vmean;
  float vsigma;
  WIREData() {}
  ~WIREData() {}
};

MatrixD FindCovariance( const beamline& , const slist& , JetProton );
void    TestCovariance( const beamline& , const slist& , JetProton ,
		        const MatrixD& );

#endif
