#!/usr/bin/env python
#=======================================================================
#
# CHEF/Beamline/Mxyzpltk Python demo
#
# J.F. Ostiguy 
# Accelerator Phyics Department
# ostiguy@fnal.gov
#
# iterators.py
#
# A demonstration of beamline iterators 
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

# pre_order iterator

iter = tevatron.iterator()
while True:
	try: element = iter.next()
	except StopIteration: break
	print element.Name(), element.Type()








