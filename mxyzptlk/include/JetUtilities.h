#ifndef JETUTIL_H
#define JETUTIL_H

extern void normalForm( const Mapping&,      /* Input:   map                    */
                        int,                 /* Input:   order                  */
                        MatrixC*,            /* Output:  matrix of eigenvectors */
                                             /*          of Jacobian of map     */
                        CLieOperator*,       /* Output:  normal form            */
                        CLieOperator* );     /* Output:  transformation         */


extern int ConvertNtoTunes( MappingC&,       /* Output: tune map                 */
                /* const */ CLieOperator& ); /* Input:  normal form vector field */


#endif  // JETUTIL_H
