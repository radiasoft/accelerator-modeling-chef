#!/usr/bin/env python

import local_paths

from basic_toolkit import *
from mxyzptlk import *
from beamline import *
from physics_toolkit import *
from bmlfactory import *
from physics_constants import *

import math


kinetic_energy = 0.067
JetParticle.createStandardEnvironments(1)

mass = PH_NORM_mp
kinetic_energy = kinetic_energy
energy = kinetic_energy + mass
momentum = math.sqrt(energy**2 -
                                  mass**2)

factory = bmlfactory("channeln.mad")
proton = Proton(energy)
brho = proton.ReferenceBRho()
beamline = factory.create_beamline("channel",brho)
context = BeamlineContext(proton,beamline,0)
iterator = DeepBeamlineIterator(beamline)
iterator.reset()
element = iterator.next()
while element:
    print element.Type()
    element = iterator.next()
