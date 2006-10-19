#!/usr/bin/env python
# apron module: apron wraps around chef

import local_paths

from basic_toolkit import *
from mxyzptlk import *
from beamline import *
from physics_toolkit import *
from bmlfactory import *
from physics_constants import *

import math
import sys
import numarray
import string

accuracy_marker = marker("accuracy")
space_charge_marker = marker("space charge")
pacifier = drift("pacifier",0.0)

class Apron:
    def __init__(self, mad_file, line_name, kinetic_energy,
                 order=1,particle='proton'):
        self.mad_file = mad_file
        self.line_name = line_name
        self.order = order
        self.saved_elements = []
        createStandardEnvironments(self.order)

        self.particle = particle
        if self.particle == 'proton':
            self.mass = PH_NORM_mp
        elif self.particle == 'positron':
            self.mass = PH_NORM_me
        else:
            raise RuntimeError, 'Unknown particle %s' % self.particle
        self.kinetic_energy = kinetic_energy
        self.mass = PH_NORM_mp
        self.energy = self.kinetic_energy + self.mass
        self.momentum = math.sqrt(self.energy**2 -
                                          self.mass**2)

        self.factory = bmlfactory(mad_file)
        brho = self.get_new_particle().ReferenceBRho()
        beamline_orig = self.factory.create_beamline(line_name,brho)
        beamline_orig.flatten()
        beamline_orig.insert(pacifier)
        beamline_orig.append(pacifier)
        self.beamline = DriftsToSlots(beamline_orig)
        self.context = BeamlineContext(self.get_new_particle(),
                                       self.beamline,0)
        if not self.context.isTreatedAsRing():
            self.context.handleAsRing()
        self.iterator = DeepBeamlineIterator(self.beamline)

    def get_new_particle(self,energy=None):
        if not energy:
            energy = self.energy
        if self.particle == 'proton':
            particle = Proton(energy)
        else:
            particle = Positron(energy)
        return particle

    def get_new_jet_particle(self,energy=None):
        if not energy:
            energy = self.energy
        if self.particle == 'proton':
            jet_particle = JetProton(energy)
        else:
            jet_particle = JetPositron(energy)
        return jet_particle

    def _commission(self):
        compulsory_visitor = RefRegVisitor(self.get_new_particle())
        self.beamline.accept(compulsory_visitor)
        
    def insert_accuracy_markers(self, num_markers_per_element):
        master_insertion_point = 0.0
        insertion_list = InsertionList(self.momentum)
        self.iterator.reset()
        element = self.iterator.next()
        ile_list = []
        particle = self.get_new_particle()
        while element:
                if element.OrbitLength(particle) > 0:
                        marker_interval = element.OrbitLength(particle)/ \
                        (num_markers_per_element + 1.0)
                        insertion_point = master_insertion_point
                        for i in range(0,num_markers_per_element):
                                insertion_point += marker_interval
                                ile = InsertionListElement(insertion_point,
                                                           accuracy_marker)
                                ile_list.append(ile)
                                insertion_list.Append(ile)
                master_insertion_point += element.OrbitLength(particle)
                element.propagateParticle(particle)
                element = self.iterator.next()
        removed_elements = slist()
        s_0 = 0.0
        self.beamline.InsertElementsFromList(s_0, insertion_list,
                                             removed_elements)
        self.beamline.append(accuracy_marker)
        self._commission()
        
    def orbit_length(self):
        return self.beamline.OrbitLength(self.get_new_particle())
    
    def _insert_elements(self,elements,positions):
        if type(elements) != type([]) and type(elements) != type(()):
            elements = (elements,)
            positions = (positions,)
        ile_list = []
        insertion_list = InsertionList(self.momentum)
        index = 0
        elements_to_insert = 0
        for element in elements:
            self.saved_elements.append(element)
            position = positions[index]
            if position > 0.0:
                ile = InsertionListElement(position,element)
                ile_list.append(ile)
                insertion_list.Append(ile)
                elements_to_insert += 1
            else:
                self.beamline.insert(element)
            index += 1
        if elements_to_insert > 0:
            removed_elements = slist()
            s_0 = 0.0
            self.beamline.InsertElementsFromList(s_0, insertion_list,
                                                 removed_elements)
        self._commission()
    
    def insert_space_charge_markers(self, num_kicks):
        kick_interval = self.orbit_length()/(1.0*num_kicks)
        elements = []
        positions = []
        insertion_point = 0.0
        for i in range(0,num_kicks):
            elements.append(marker("mapping endpoint:space charge endpoint"))
            positions.append(insertion_point)
            insertion_point += kick_interval/2.0
            elements.append(marker("mapping endpoint:space charge kick"))
            positions.append(insertion_point)
            insertion_point += kick_interval/2.0
        end_marker = marker("mapping endpoint:space charge endpoint")
        self.saved_elements.append(end_marker)
        self.beamline.append(end_marker)
        self._insert_elements(elements,positions)

    def get_lattice_functions(self):
        lattice_function_array = Lattice_function_array()
        self.iterator.reset()
        i = 0
        element = self.iterator.next()
        while element:
            lattice_function = self.context.getLattFuncPtr(i)
            if lattice_function:
                lattice_function_array.append(lattice_function)
            element = self.iterator.next()
            i += 1
        return lattice_function_array

    def get_maps(self):
        self.iterator.reset()
        element = self.iterator.next()
        s = 0.0
        particle = self.get_new_particle()
        jet_particle = self.get_new_jet_particle()
        has_propagated = 0
        energy = self.energy
        maps = []
        while element:
            split_name = element.Name().split(":")
            if split_name[0] == "mapping endpoint":
                if has_propagated:
                    maps.append(jet_particle.State().toPython())
                    s = element.OrbitLength(particle)
                    element.propagateParticle(particle)
                    element.propagateJetParticle(jet_particle)
                energy = jet_particle.ReferenceEnergy()
                jet_particle = self.get_new_jet_particle(energy)
                has_propagated = 0
                s = 0.0
            else:
                if not element.Type() == "marker":
                    s += element.OrbitLength(particle)
                    element.propagateParticle(particle)
                    element.propagateJetParticle(jet_particle)
                    has_propagated = 1
            element = self.iterator.next()
        return maps

    def get_quad_strengths(self):
        self.iterator.reset()
        element = self.iterator.next()
        particle = self.get_new_particle()
        brho = particle.ReferenceBRho()
        kxs = []
        kys = []
        ss = []
        s = 0.0
        while element:
            kx = 0.0
            ky = 0.0
            strength = element.Strength()
            length = element.OrbitLength(particle)
            s += length
            if (element.Type() == "quadrupole") or \
                   (element.Type() == "thinQuad"):
                kx = strength/brho;
                ky = -kx
            elif (element.Type() == "CF_sbend") or \
                 (element.Type() == "CF_rbend"):
                ky = -element.getQuadrupole()/brho/length
                kx = -ky + strength**2/(brho**2) 
            else:
                pass
            kxs.append(kx)
            kys.append(ky)
            ss.append(s)
            element.propagateParticle(particle)
            element = self.iterator.next()
        return (numarray.array(ss),numarray.array(kxs),numarray.array(kys))
    
    def _convert_linear_maps(self, chef_linear_maps):
        linear_maps = []
        for chef_map in chef_linear_maps:
            map = numarray.zeros((6,6),'d')
            for row in range(0,6):
                for column in range(0,6):
                    map[row,column] = chef_map.get(row,column)
            linear_maps.append(map)
        return linear_maps

    def get_single_linear_map(self):
        jet_particle = self.get_new_jet_particle()
        self.beamline.propagateJetParticle(jet_particle)
        return self._convert_linear_maps([jet_particle.State().jacobian()])[0]

    def get_single_full_map(self):
        jet_particle = self.get_new_jet_particle()
        self.beamline.propagateJetParticle(jet_particle)
        mapping = jet_particle.State()
        return jet_particle.State().toPython()
    
    def propagate_reference_particle(self,print_coeffs=0):
        jet_particle = self.get_new_jet_particle()
        self.beamline.propagateJetParticle(jet_particle)
        mapping = jet_particle.State()
        if print_coeffs:
            mapping.printCoeffs()
        testpart = self.get_new_particle()
        self.beamline.propagateParticle(testpart)
        return (self.get_new_particle(),testpart)

class Lattice_function_array:
    def __init__(self):
        self.s = []
        self.beta_x = []
        self.beta_y = []
        self.alpha_x = []
        self.alpha_y = []

    def append(self, lattice_fn):
        self.s.append(lattice_fn.arcLength)
        self.beta_x.append(lattice_fn.beta.hor)
        self.beta_y.append(lattice_fn.beta.ver)
        self.alpha_x.append(lattice_fn.alpha.hor)
        self.alpha_y.append(lattice_fn.alpha.ver)
