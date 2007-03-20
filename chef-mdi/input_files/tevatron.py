#!/usr/bin/env python
#=======================================================================
#
# CHEF/Beamline/Mxyzpltk Python demo
#
# J.F. Ostiguy 
# Accelerator Integration Department
# ostiguy@fnal.gov
#
#
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

# Preserve the current Jet environment
storedEnv    =   mxyzptlk.Jet__environment.getLastEnv()
storedEnvC   =   mxyzptlk.JetC__environment.getLastEnv()

pr = beamline.Proton( energy )

#
# define a first order environment in 6D phase space
#

mxyzptlk.BeginEnvironment(1)
x    = mxyzptlk.coord(0.0)
y    = mxyzptlk.coord(0.0)
ct   = mxyzptlk.coord(0.0)
npx  = mxyzptlk.coord(0.0)
npy  = mxyzptlk.coord(0.0)
np   = mxyzptlk.coord(0.0)
mxyzptlk.EndEnvironment()

#
# read tevatron lattice file
# 

bfact    = bmlfactory.bmlfactory("Tevatron-E.lat")

#
# instantiate a beamline
#

pr = beamline.Proton(energy)
jp = beamline.JetProton(energy)

print "Reference BRho = ", pr.ReferenceBRho()

tevatron = bfact.create_beamline("normal_tevatron", pr.ReferenceBRho() )
lfsage = physics_toolkit.LattFuncSage(tevatron)

# propagate the JetProton ( i.e. compute a one-turn map)
tevatron.propagateJetParticle(jp)

lfsage.TuneCalc(jp, 1)
lfsage.Disp_Calc(jp)

lfsage.Slow_CS_Calc(jp)
#lfsage.Fast_CS_Calc(jp)

# Restore the Jet environment

mxyzptlk.Jet__environment.setLastEnv(storedEnv)
mxyzptlk.JetC__environment.setLastEnv(storedEnvC)

# ----------------------------
# output lattice functions 
# -----------------------------

# create a bunch of arrays to store lattice functions

nelm = tevatron.countHowManyDeeply()

arcLength_array = numarray.zeros(nelm, 'Float64')

hor_beta_array = numarray.zeros(nelm, 'Float64')
ver_beta_array = numarray.zeros(nelm, 'Float64')

hor_alpha_array = numarray.zeros(nelm, 'Float64')
ver_alpha_array = numarray.zeros(nelm, 'Float64')

hor_disp_array = numarray.zeros(nelm,  'Float64')
ver_disp_array = numarray.zeros(nelm,  'Float64')

hor_dPrime_array = numarray.zeros(nelm,  'Float64')
ver_dPrime_array = numarray.zeros(nelm,  'Float64')

# copy the lattice functions from the bealine elements to the arrays 

i=0
for  be in  tevatron.deep_iterator(): 
#	print i, be.Type()
	lf  = be.dataHook.find("Twiss",1).info().lattFunc() # this should return a LattFuncSage.lattFunc
 	print lf
#	arcLength_array[i]      = lf.arcLength
#	hor_beta_array[i]       = lf.beta.hor
#	ver_beta_array[i]       = lf.beta.ver
#	hor_alpha_array[i]      = lf.alpha.hor
#	ver_alpha_array[i]      = lf.alpha.ver
#	hor_disp_array[i]       = lf.dispersion.hor
#	ver_disp_array[i]       = lf.dispersion.ver
#	hor_dPrime_array[i]     = lf.dPrime.hor
#	ver_dPrime_array[i]     = lf.dPrime.ver
	i = i+1   


#--------------------------------------
# plot and display the lattice functions
#---------------------------------------

# setup curves ...

betax_curve  = chefplot.CurveData(arcLength_array, hor_beta_array,"Hor Beta  [m]")
betay_curve  = chefplot.CurveData(arcLength_array, ver_beta_array,"Ver Beta  [m]")

dispx_curve  = chefplot.CurveData(arcLength_array, hor_disp_array, "Hor Dispersion  [m]")
dispy_curve  = chefplot.CurveData(arcLength_array, ver_disp_array, "Ver Dispersion  [m]")

# package data into a PlotData object

data = chefplot.PlotData()

data.addCurve(betax_curve)
data.addCurve(betay_curve)
#data.addCurve(dispx_curve)
#data.addCurve(dispy_curve)

# pass the data to a plot widget and display the widget !

plot = chefplot.CHEFPlotMain()
plot.addData(data)
plot.resize(1000,500)
plot.show()

#print hor_beta_array
#print hor_disp_array
#print hor_dPrime_array










