#ifndef FDPOLARITY_H
#define FDPOLARITY_H

struct FDPolarity : public BarnacleData {
  double polarity;
  FDPolarity(double x) : polarity(x) {}
  FDPolarity(const FDPolarity& x){polarity = x.polarity;}
  ~FDPolarity() {}
};

#endif //  FDPOLARITY_H
