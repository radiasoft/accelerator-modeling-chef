#
mxyzptlk.BeginEnvironment(12)
x =  mxyzptlk.coord(1.0)
y =  mxyzptlk.coord(1.0)
z =  mxyzptlk.coord(1.0)
px = mxyzptlk.coord(0.0)
py = mxyzptlk.coord(0.0)
pz = mxyzptlk.coord(0.0)
mxyzptlk.EndEnvironment()
#

print mxyzptlk.sqrt(x)
print mxyzptlk.sqrt(x)*mxyzptlk.sqrt(x)

env = mxyzptlk.makeJetEnvironment(4,6,6)

print env

envc = mxyzptlk.makeJetCEnvironment(4,6,6)

print envc

converted = mxyzptlk.toCmplxEnvironment(env)

print converted

mapping = mxyzptlk.Mapping( (x, y, z, px, py, pz) );

print mapping

v1 = basic_toolkit.Vector( ( 1,2,3) );
v2 = basic_toolkit.Vector( ( 3,4,5) );
v2 = basic_toolkit.Vector( ( 5,6,7) );
v3 = v1+v2

print v3;

x1 = x*x +y*y + z*z
print x1

x2 = mxyzptlk.sin(x1)

print x2
