import numarray

a = numarray.array([1.0,2.0,3.0,4.0,5.0])
print a

c  = chefplot.CHEFCurve(a+a,a*a,"numarraytest")
c2 = chefplot.CHEFCurve(a+a,a*a*a,"numarraytest2")
c3 = chefplot.CHEFCurve(a+a,a*a*a*a,"numarraytest3")

print c

###### c.setData(a,a)

d = chefplot.CHEFPlotData()
d.addCurve(c)
d.addCurve(c2)
d.addCurve(c3)

print d

p = chefplot.CHEFPlotMain()
p.addData(d)
p.resize(1000,500)
p.show()

