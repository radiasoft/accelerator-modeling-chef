#!/usr/bin/env python
#=======================================================================
#
# CHEF/Beamline/Mxyzpltk Python demo
#
# J.F. Ostiguy 
# Accelerator Integration Department
# ostiguy@fnal.gov
#
#  tracking.py
#=======================================================================
#
# Of the following, only numarray needs to be imported explicitly when working
# in the CHEF embedded interpreter. Note: Importing already imported modules
# is harmless.
#
import basic_toolkit   
import mxyzptlk       
import beamline       
import physics_toolkit  
import bmlfactory      
import chefplot
####import chef
#
import numarray

#
PH_NORM_mp  = 0.938               #  [GeV]  Proton Rest Mass 
energy = 150.0                    #  Tevatron injection energy 

pr = beamline.Proton( energy )

#
# define a first order environment in 6D phase space
#

#
# read tevatron lattice file
# 

bfact    = bmlfactory.bmlfactory("Tevatron-E.lat")

#
# instantiate a beamline
#

pr = beamline.Proton(energy)

print "Reference BRho = ", pr.ReferenceBRho()

tevatron = bfact.create_beamline("normal_tevatron", pr.ReferenceBRho() )

#set initial conditions
pr.set_x(0.001)
pr.set_y (0.0)
pr.set_cdt(0.0)

#setup an array with space for nsamples turns

nsamples = 1024

x_position = numarray.zeros(nsamples, 'Float64')
turnno     = numarray.zeros(nsamples, 'Float64')

# iterate over nsamples turns

# propagate the Proton ( i.e. compute a one-turn map)

for i in xrange(nsamples):
    tevatron.propagateParticle(pr)
    x_position[i] =  pr.get_x()
    turnno[i] = i

#--------------------------------------
# plot and display the position
#---------------------------------------

# setup curves ...

x_curve  = chefplot.CHEFCurve(turnno, x_position,"Hor Position [m]")

# package data into a CHEFPlotData object

data = chefplot.CHEFPlotData()
data.addCurve(x_curve)

# pass the data to a plot widget and display the widget !

plot = chefplot.CHEFPlotMain()
plot.addData(data)
plot.resize(1000,500)
plot.show()

#--------------------------------------
# setup FFT
#---------------------------------------

import numarray.fft

xfft = numarray.fft.fft(x_position)

xfftabs = numarray.zeros(nsamples, 'Float64')

for i in xrange(nsamples):
    xfftabs[i] = xfft[i]*(xfft[i].conjugate())

fftcurve  = chefplot.CHEFCurve(turnno, xfftabs, "Amplitude ")

fftdata = chefplot.CHEFPlotData()


fftdata.addCurve(fftcurve)

fftplot = chefplot.CHEFPlotMain()
fftplot.addData(fftdata)
fftplot.resize(1000,500)
fftplot.show()









