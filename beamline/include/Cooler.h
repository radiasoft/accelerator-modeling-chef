Cooler c;
c.Add( new CoolerData( xxx, xxx, xxx, xxx ));

CoolerData* p;

p = c(10);

t = p->time;
u = p->momentum;

==================================================================

#ifndef COOLER_H
#define COOLER_H

struct CoolerData
{
  double time;
  double momentum;
  double x;
  double px;

  CoolerData( const double&, const double&, const double&, const double& );
  ~CoolerData(){}
};


struct CoolerMemory
{
  CoolerDataArray myData;

  CoolerMemory();
  ~CoolerMemory();

  CoolerData* operator() ( const int& );
};



class Pickup : public bmlnElmnt
{
  Cooler* myCooler;  
};

class Kicker : public bmlnElmnt
{
  Cooler* myCooler;
};

class Cooler {
 private:
  Pickup* pkup;
  Kicker* kckr;

  // Memory arrays
  CoolerMemory info[5];

 public:
  Cooler( Pickup*, Kicker* );
  ~Cooler();

  void Add( CoolerData* );
  CoolerData* operator()( const int& );
};

inline void Cooler::Add( CoolerData* p )
{
  info[0].myData.Add( p );
}

inline CoolerData* CoolerMemory::operator() ( const int& )
{
  return myData(i);
}


#endif
