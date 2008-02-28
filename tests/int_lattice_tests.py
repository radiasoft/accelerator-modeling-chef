#!/usr/bin/env python

import sys
sys.path.append(".")

import unittest

from testclass_for_synergia import Testclass_for_synergia

class Test_channel(Testclass_for_synergia):

    def __init__(self,arg):
        self.mad_file_name = 'channel.mad'
        self.line_name = 'channel'
        self.kinetic_energy = 0.067
        self.order = 7
        self.particle = 'proton'
        self.data_file_name = 'expected_results/channel_answers'
        self.accuracy_markers = 10
        self.space_charge_markers = 20
        Testclass_for_synergia.__init__(self,arg)
        
    def test_05_get_lattice_functions(self):
        pass

class Test_booster(Testclass_for_synergia):
    def __init__(self,arg):
        self.mad_file_name = 'booster_classic.lat'
        self.line_name = 'booster'
        self.kinetic_energy = 0.4
        self.order = 2
        self.particle = 'proton'
        self.data_file_name = 'expected_results/booster_answers'
        self.accuracy_markers = 10
        self.space_charge_markers = 100
        Testclass_for_synergia.__init__(self,arg)
        
class Test_damping_ring(Testclass_for_synergia):
    def __init__(self,arg):
        self.mad_file_name = 'ocs6-7.lat'
        self.line_name = 'ring'
        self.kinetic_energy = 5.066 - 0.5*0.511e-3
        self.order = 1
        self.particle = 'positron'
        self.data_file_name = 'expected_results/damping_ring_answers'
        self.accuracy_markers = 2
        self.space_charge_markers = 200
        Testclass_for_synergia.__init__(self,arg)

if __name__ == '__main__':
    channel_suite = unittest.TestLoader().loadTestsFromTestCase(Test_channel)
    booster_suite = unittest.TestLoader().loadTestsFromTestCase(Test_booster)
    damping_ring_suite = unittest.TestLoader().loadTestsFromTestCase(Test_damping_ring)
    channel_result = unittest.TextTestRunner(verbosity=2).run(channel_suite)
    booster_result = unittest.TextTestRunner(verbosity=2).run(booster_suite)
    damping_ring_result = unittest.TextTestRunner(verbosity=2).run(damping_ring_suite)
    if channel_result.wasSuccessful() and \
       booster_result.wasSuccessful() and \
       damping_ring_result.wasSuccessful():
        sys.exit(0)
    else:
        sys.exit(1)

