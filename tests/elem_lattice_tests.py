#!/usr/bin/env python

import sys
import os
sys.path.append(".")

import unittest

import local_paths

from basic_toolkit import *
from mxyzptlk import *
from beamline import *
from physics_toolkit import *
from bmlfactory import *
from physics_constants import *

import math

import shelve
from custom_assertions import *

def print_names(beamline,particle):
    s = 0
    for elem in beamline:
        delta_s = elem.OrbitLength(particle)
        elem.propagateParticle(particle)
        print s,elem.Name(),delta_s
        s += delta_s
    print s
    
def get_maps(beamline,energy,order):
    if new_chef:
        JetParticle.createStandardEnvironments(order)
    else:
        createStandardEnvironments(order)
    jet_particle = JetProton(energy)
    retval = []
    for elem in beamline:
        elem.propagateJetParticle(jet_particle)
        #~ print jet_particle.State().toPython()
        retval.append(jet_particle.State().toPython())
        jet_particle = JetProton(energy)
    return retval

def get_single_map(beamline,energy,order):
    if new_chef:
        JetParticle.createStandardEnvironments(order)
    else:
        createStandardEnvironments(order)
    jet_particle = JetProton(energy)
    for elem in beamline:
        elem.propagateJetParticle(jet_particle)
    return jet_particle.State().toPython()

def insert_elements(base_beamline,particle,momentum,elements,positions):
    if type(elements) != type([]) and type(elements) != type(()):
        elements = (elements,)
        positions = (positions,)
    ile_list = []
    if not new_chef:
        insertion_list = InsertionList(momentum)
    index = 0
    elements_to_insert = 0
    saved_elements = []
    for element in elements:
        saved_elements.append(element)
        position = positions[index]
        if position > 0.0:
            if new_chef:
                ile = element,position
            else:
                ile = InsertionListElement(position,element)
            ile_list.append(ile)
            if not new_chef:
                insertion_list.Append(ile)
            elements_to_insert += 1
        else:
            base_beamline.insert(element)
        index += 1
    if elements_to_insert > 0:
        s_0 = 0.0
        if new_chef:
            base_beamline.InsertElementsFromList(particle, s_0, ile_list)
        else:
            removed_elements = slist()
            base_beamline.InsertElementsFromList(s_0, insertion_list,removed_elements)
    return saved_elements
    #~ self._commission()

class Element_test(unittest.TestCase):
    def setUp(self):
        answers_file_name = os.path.splitext(self.mad_file_name)[0] + \
            '_answers.dat'
        self.answers = shelve.open(answers_file_name)
        self.energy = math.sqrt(self.momentum**2 + PH_NORM_mp**2)
        self.particle = Proton(self.energy)
        if new_chef:
            factory = MAD8Factory(self.mad_file_name)
        else:
            factory = bmlfactory(self.mad_file_name)
        brho = self.particle.ReferenceBRho()
        self.beamline_orig = factory.create_beamline(self.line_name,brho)
        print "setup complete"
        
    def tearDown(self):
        self.answers.close()
        
    def test_01_single_map(self):
        single_map = get_single_map(self.beamline_orig,self.energy,self.order)
        if write_answers:
            self.answers['single_map'] = single_map
        expected = self.answers['single_map']
        assertMapAlmostEqual(self,expected,single_map,accuracy)
        
    def test_02_element_maps(self):
        maps = get_maps(self.beamline_orig,self.energy,self.order)
        if write_answers:
            self.answers['maps'] = maps
        expected = self.answers['maps']
        self.assertEqual(len(expected),len(maps))
        accuracy = 10
        for i in range(0,len(expected)):
            assertMapAlmostEqual(self,expected[i],maps[i],accuracy)
    
    def test_03_print(self):
        print_names(self.beamline_orig,self.particle)
        
    def test_04_insert(self):
        markers = [marker('mark')]
        positions = [4.2]
        insert_elements(self.beamline_orig,self.particle,self.momentum,markers,positions)
        print_names(self.beamline_orig,self.particle)

    def test_05_split_map(self):
        markers = [marker('mark')]
        positions = [4.2]
        insert_elements(self.beamline_orig,self.particle,self.momentum,markers,positions)
        single_map = get_single_map(self.beamline_orig,self.energy,self.order)
        expected = self.answers['single_map']
        assertMapAlmostEqual(self,expected,single_map,accuracy)

    def test_06_split_maps(self):
        markers = [marker('mark')]
        positions = [4.2]
        insert_elements(self.beamline_orig,self.particle,self.momentum,markers,positions)
        maps = get_maps(self.beamline_orig,self.energy,self.order)
        if write_answers:
            self.answers['split maps'] = maps
        expected = self.answers['split maps']
        self.assertEqual(len(expected),len(maps))
        accuracy = 10
        for i in range(0,len(expected)):
            assertMapAlmostEqual(self,expected[i],maps[i],accuracy)

class Fodo_test(Element_test):
    def __init__(self,arg):
        self.momentum = 100
        self.mad_file_name = 'fodo.lat'
        self.line_name = 'fodo'
        self.order = 7
        Element_test.__init__(self,arg)
    
write_answers = False
accuracy = 10
new_chef = True
try:
    # Something that does not work in old chef
    JetParticle.createStandardEnvironments(1)
except:
    new_chef = False

if __name__ == '__main__':
    if (len(sys.argv) > 1) and sys.argv[1] == "write":
        write_answers = True
    fodo_suite = unittest.TestLoader().loadTestsFromTestCase(Fodo_test)
    fodo_result = unittest.TextTestRunner(verbosity=2).run(fodo_suite)
    print "fodo_result.wasSuccessful() =",fodo_result.wasSuccessful()
    if fodo_result.wasSuccessful():
        sys.exit(0)
    else:
        sys.exit(1)

    
