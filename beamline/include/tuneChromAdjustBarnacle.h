#ifndef TUNE_CHROM_BARNACLE_H
#define TUNE_CHROM_BARNACLE_H

#include <Barnacle.h>
#include <beamline.h>

class tuneChromAdjustBarnacle : public BarnacleData {
public:
  tuneChromAdjustBarnacle() { h_tune = v_tune = h_chrom = v_chrom = 0; }
  int h_tune, v_tune, h_chrom, v_chrom;
};

#endif /* TUNE_CHROM_BARNACLE_H */
