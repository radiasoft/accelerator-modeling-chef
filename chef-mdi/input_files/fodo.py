#!/usr/bin/env python
#=======================================================================
#
# Beamline/Mxyzpltk Python demo
#
# J.F. Ostiguy 
# Accelerator Integration Department
# ostiguy@fnal.gov
#
#=======================================================================
from beamline         import *
from basic_toolkit    import *
from physics_toolkit  import *
from mxyzptlk         import *
from bmlfactory       import *
#
PH_NORM_mp  = 0.938               #  [GeV]  Proton Rest Mass 
f           =  7.0                #  [m]    focal length of thin quads
l           = 10.0                #  [m]    length of drift
E           = 100.0 - PH_NORM_mp  #  [GeV]  Proton kinetic energy (arbitrary)
#
energy = E + PH_NORM_mp
#
print 'f = ', f
print 'l = ', l
print 'E = ', E
print 'energy = ', energy
pr = Proton( energy )
#
#create an empty beamline
#
bl = beamline()
#
#
#create a few elements and append them to the beamline
#
#
O = drift("O", l/2.0)
F = thinQuad("F",   pr.ReferenceBRho() / f ) 
D = thinQuad("D", - pr.ReferenceBRho() / f )
#
print 'pr.ReferenceBRho() / f = ', pr.ReferenceBRho() / f

for i in range(5):
	bl.append(O)	
	bl.append(F)
	bl.append(O)
	bl.append(O)
	bl.append(D)
	bl.append(O)
#
bl.setEnergy(energy)  # useful for streaming
#
# define second order environment in 6D phase space
# define first  order environment in 6D phase space
#
Jet.BeginEnvironment(1)
x   = coord(0.0)
x   = coord(0.0)
x   = coord(0.0)
npx = coord(0.0)
npy = coord(0.0)
np  = coord(0.0)
Jet.EndEnvironment()
#
# instantiate a JetProton
jp = JetProton(energy)
#
# Compute lattice functions !
#
print 'Computing lattice functions ...'
lfsage = LattFuncSage(bl)
lfsage.TuneCalc(jp, 1)
lfsage.Disp_Calc(jp)
#


#i=0
#bit = DeepBeamlineIterator(tevatron)
#be = bit.reset()
#be = bit.next()
#while   be:
#	print i, be.Type()
#	lf  = be.dataHook.find("Twiss",1) # this should return a LattFuncSage.lattFunc
#	print lf
#	be   = bit.next()
#	i = i+1   
