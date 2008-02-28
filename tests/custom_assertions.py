#!/usr/bin/env python

import sys

def assertLinearMapAlmostEqual(testcase,expected,found,accuracy):
    for i in range(0,6):
        for j in range(0,6):
            testcase.assertAlmostEqual(
                expected[i,j],found[i,j],accuracy,
                'map element %d,%d: expected %g, found %g' % \
                (i,j,expected[i,j],found[i,j]))

def assertMapAlmostEqual(testcase,expected,found,accuracy):
# We do not assume that the terms stay in the same order.
    for i in range(0,6):
        coeffs_exp = {}
        coeffs_found = {}
        for term in expected[i]:
            coeffs_exp[term[1]] = term[0]
        for term in found[i]:
            coeffs_found[term[1]] = term[0]
        for term in found[i]:
            if coeffs_exp.has_key(term[1]):
                coeff_exp = coeffs_exp[term[1]]
            else:
                coeff_exp = 0.0
            coeff_found = term[0]
            testcase.assertAlmostEqual(
                coeff_exp,coeff_found,accuracy,
                'map(%d) coefficient of %s: expected %g, found %g' % \
                (i,str(term[1]),coeff_exp,coeff_found))
        for term in expected[i]:
            coeff_exp = coeffs_exp[term[1]]
            if coeffs_found.has_key(term[1]):
                coeff_found = coeffs_found[term[1]]
            else:
                coeff_found = 0.0
            testcase.assertAlmostEqual(
                coeff_exp,coeff_found,accuracy,
                'map(%d) coefficient of %s: expected %g, found %g' % \
                (i,str(term[1]),coeff_exp,coeff_found))

def assertPropagatedParticleAlmostEqual(testcase,initial,final,cdt_offset,
                                        accuracy):
        testcase.assertAlmostEqual(initial.get_x(),final.get_x(),accuracy,
                               "different initial (%g) and final (%g) x" %\
                               (initial.get_x(),final.get_x()))
        testcase.assertAlmostEqual(initial.get_npx(),final.get_npx(),accuracy,
                               "different initial (%g) and final (%g) npx" %\
                                (initial.get_npx(),final.get_npx()))
        testcase.assertAlmostEqual(initial.get_y(),final.get_y(),accuracy,
                               "different initial (%g) and final (%g) y" %\
                               (initial.get_y(),final.get_y()))
        testcase.assertAlmostEqual(initial.get_npy(),final.get_npy(),accuracy,
                               "different initial (%g) and final (%g) npy" %\
                                (initial.get_npy(),final.get_npy()))
        testcase.assertAlmostEqual(initial.get_cdt() + cdt_offset,
                               final.get_cdt(),accuracy,
                               "different expected (%g) and final (%g) cdt" %\
                               (initial.get_cdt() + cdt_offset, final.get_cdt()))
        testcase.assertAlmostEqual(initial.get_ndp(),final.get_ndp(),accuracy,
                               "different initial (%g) and final (%g) ndp" %\
                               (initial.get_ndp(),final.get_ndp()))

verbose_array = 1
def assertArrayAlmostEqual(testcase,expected,found,accuracy):
    testcase.assertEqual(len(expected),len(found))
    for i in range(0,len(expected)):
        try:
            testcase.assertAlmostEqual(expected[i],found[i],accuracy,
                                       'element %d: expected %g, found %g' %\
                                       (i,expected[i],found[i]))
        except Exception, e:
            if verbose_array:
                sys.stderr.write("assertAlmostEqual failure:\n")
                sys.stderr.write("%6s%12s%12s\n" %
                                 ("index","expected","found"))
                for j in range(0,len(expected)):
                    sys.stderr.write("%6d%12g%12g\n" %
                                     (j,expected[j],found[j]))
            raise e
        
