import numarray

a1 = numarray.array([1,2,3,4,5])
print a1

a2 = a1
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

