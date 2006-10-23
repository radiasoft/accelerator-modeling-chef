#!/usr/bin/env python

import os
import unittest
import shelve
import apron
from custom_assertions import *

from local_paths import tests_dir

write_answers = 0
accuracy = 7

class Testclass_for_synergia(unittest.TestCase):
    
    def setUp(self):
        mad_file_name = os.path.join(tests_dir,self.mad_file_name)
        self.apron = apron.Apron(mad_file_name,self.line_name,
                                 self.kinetic_energy,self.order,self.particle)
        data_file_name = os.path.join(tests_dir,self.data_file_name)
        self.answers = shelve.open(data_file_name)

    def tearDown(self):
        self.answers.close()

    def test_01_get_quad_strengths(self):
        (s,kx,ky) = self.apron.get_quad_strengths()
        if write_answers:
            self.answers['s'] = s
            self.answers['kx'] = kx
            self.answers['ky'] = ky
        assertArrayAlmostEqual(self,self.answers['s'],s,accuracy)
        assertArrayAlmostEqual(self,self.answers['kx'],kx,accuracy)
        assertArrayAlmostEqual(self,self.answers['ky'],ky,accuracy)

    def test_02_closed(self):
        (initial,final) = self.apron.propagate_reference_particle()
        if write_answers:
            self.answers['cdt offset'] = final.get_cdt() - initial.get_cdt()
        assertPropagatedParticleAlmostEqual(self,initial,final,
                                            self.answers['cdt offset'],
                                            accuracy)
        
    def test_03_single_linear_map(self):
        map = self.apron.get_single_linear_map()
        if write_answers:
            self.answers['expected linear'] = map
        expected = self.answers['expected linear']
        assertLinearMapAlmostEqual(self,expected,map,accuracy)
        
    def test_04_single_full_map(self):
        map = self.apron.get_single_full_map()
        if write_answers:
            self.answers['full map'] = map
        expected = self.answers['full map']
        assertMapAlmostEqual(self,expected,map,accuracy)


    def test_05_get_lattice_functions(self):
        lattice_fns = self.apron.get_lattice_functions()
        if write_answers:
            self.answers['lattice fns'] = lattice_fns
        expected = self.answers['lattice fns']
        assertArrayAlmostEqual(self,expected.s,lattice_fns.s,accuracy)
        assertArrayAlmostEqual(self,expected.beta_x,lattice_fns.beta_x,
                               accuracy)
        assertArrayAlmostEqual(self,expected.beta_y,lattice_fns.beta_y,
                               accuracy)
        assertArrayAlmostEqual(self,expected.alpha_x,lattice_fns.alpha_x,
                               accuracy)
        assertArrayAlmostEqual(self,expected.alpha_y,lattice_fns.alpha_y,
                               accuracy)
        
    def test_06_insert_markers(self):
        self.apron.insert_accuracy_markers(self.accuracy_markers)
        self.apron.insert_space_charge_markers(self.space_charge_markers)

    def test_07_get_space_charge_maps(self):
        self.apron.insert_space_charge_markers(self.space_charge_markers)
        maps = self.apron.get_maps()
        if write_answers:
            self.answers['sc maps'] = maps
        expected = self.answers['sc maps']
        self.assertEqual(len(expected),len(maps))
        for i in range(0,len(expected)):
            assertMapAlmostEqual(self,expected[i],maps[i],accuracy)
