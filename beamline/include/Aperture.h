#ifndef __Aperture_H
#define __Aperture_H

class Aperture {
public:
  inline Aperture( void ) {}
  inline virtual ~Aperture() {}
  virtual Aperture* Clone() = 0;
  virtual void get(void*) = 0;
  virtual void set(void*) = 0;
};

#endif // __Aperture_H
