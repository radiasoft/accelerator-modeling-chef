#!/usr/bin/env python
#=======================================================================
#
# CHEF/Beamline/Mxyzpltk Python demo
#
# J.F. Ostiguy 
# Accelerator Phyics Department
# ostiguy@fnal.gov
#
# reverse_deep_iterator.py
#
# Demo to illustrate iterator syntax.
#=======================================================================
#
import basic_toolkit   
import mxyzptlk       
import beamline       
import physics_toolkit  
import bmlfactory      

energy = 8.0 
pr     = beamline.Proton( energy )

#
# read a lattice file and instantiate a beamline
# 

bfact    = bmlfactory.bmlfactory("Tevatron-E.lat")
tevatron = bfact.create_beamline("normal_tevatron", pr.ReferenceBRho() )

# reverse_deep_iterator

#tevatron.__iter__ = tevatron.reverse_deep_iterator()

#for element in tevatron:
#	print element.Name(), element.Type()

for element in tevatron.reverse_iterator():
	print element.Name(), element.Type()



