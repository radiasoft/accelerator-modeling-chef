import numarray

a1 = numarray.array([1.0,2.0,3.0,4.0,5.0])
print a1

a2 = numarray.array([1.0,2.0,3.0,4.0,5.0,6.0])
print a2

c = chef.CHEFCurve(a1,a2,"numarraytest")

print c

###### c.setData(a,a)

d = chef.CHEFPlotData()
d.addCurve(c)

print d

p = chef.CHEFPlotMain()
p.addData(d)
p.show()

