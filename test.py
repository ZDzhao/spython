cc = -1
a = 10
b = a + 1
c = b - 2
d = c * 2
e = c / 3

g = 3.5
x, y = 113, 224
if a*0:
    h = 10
elif b:
    h = 15
else:
    h = 20
i = 0
j = a
while j:
    j = j - 1
    i = i + 1
    pass
    if not j > 10:
        break
else:
    i = 1
    

def f1(a, b, c):
    return a + b

s= f1(**{'a':1, 'b':2, 'c':5})
class F:
    def hi(self):
        self.d = 99
        print('hi')
        a = b
        return 1023

class Foo(F):
    a = 10
    def __str__(self):
        return 'Foo instance:'
    def __len__(self):
        return 10
    def __eq__(self, v):
        print('eeeeeeeee')
        return v.c == 12
    def go(self):
        self.c = 20
        print('go')
        return self
    def say(a):
        return a
f = Foo()
f2 = Foo()
print('lenf', len(f))
f2.c = 10
if f == f2:
    print('equal2')

foo = Foo()
print(foo)
ee = foo.hi()
foo.c = 201
foo.a = 20
Fa = Foo.a
fa = foo.a
print('fa', Fa, fa)
foo.go()

foo.c = 202
fc = foo.c
print 1, 2, 3
print('fc', fc)


fhi = foo.hi()
fd  = foo.d
a, b, c = (1, 2, 3)
from mod import *
from mod import mb


import ma

try:
    ta = 109
    raise 1002
except int, i:
    teai = 2399
except Foo, f:
    teafoo = 199
except F, f:
    teaf = 299
except:
    tea = 99
else:
    tel = 89
finally:
    tef = 78

ecpp = Exception

def wf2():
    return 6633
def wrap(f):
    print('wrap....')
    return f
def wrap2(f):
    print('wrap2....')
    return wf2
@wrap2
@wrap
def wf():
    return 1122334

testD = wf()
testN = None
testtuple = (1, 2, 3, 4, 2)
testtupcount = testtuple.index(4)

testlen = len('1222')
tests1 = 'abcdefg'
tests2 = tests1.upper()
tests3 = tests2.lower()
tests3 = tests2.lower()

a1 = isinstance(1, tests3)
xx = tests3[3:100]
print(xx)
fooname = Foo.__name__

lista = [1, 2, 3, 4]
listb = [5, 6]
listc = lista + listb
listalen = len(lista)
listtuple = (1, 2, 3, 4)
for k in listtuple:
    print(k)
testlistfor = [k*2  for k in lista]

