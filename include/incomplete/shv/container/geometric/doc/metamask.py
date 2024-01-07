import itertools
import random
lH = 3
H = 1 << lH
random.seed(10)

h = list(range(H))

def al(l, c=5, sep=''):
	return sep.join([str(x).ljust(c) for x in l])

def find_first(l, f):
	return next((i, x) for i, x in enumerate(l) if f(i, x))

def cto(x):
	s = abin(x)
	return len(s) - len(s.rstrip('1'))

def mask_metaclass_func(name,bases,namespace):
	int_funcs="""add,sub,mul,truediv,floordiv,mod,
			divmod,pow,lshift,rshift,and,xor,or,radd,
			rsub,rmul,rtruediv,rfloordiv,rmod,
			rdivmod,rpow,rlshift,rrshift,rand,rxor,ror,
			iadd,isub,imul,itruediv,ifloordiv,
			imod,ipow,ilshift,irshift,iand,ixor,ior,
			neg,pos,abs,round,invert,trunc,floor,ceil"""
	int_funcs=[x.strip() for x in int_funcs.split(',')]
	newfunc=namespace['__new__']
	for fn in int_funcs:
		nn='__'+fn+'__'
		if(nn in int.__dict__):
			ifunc=int.__dict__[nn]
			def rebind_z(fn1,ifunc1,newfunc1):
				def z(s,*args,**kwargs):
					r=ifunc1(s,*args,**kwargs)
					return newfunc1(s.__class__,r)
				return z
			namespace[nn]=rebind_z(fn,ifunc,newfunc)
	
	return type(name,bases,namespace)
		
class mask(int,metaclass=mask_metaclass_func):
	def __new__(cls, v=0):
		if(v < 0):
			v=H+v
		if(v >= H):
			v=v % H
		v=int.__new__(cls,v)
		return v

	def __str__(self):
		return ('{0:0>'+str(H)+'b}').format(self)



def genB():
	B=[mask(0)]*lH
	for bi in range(H):
		for r in range(lH):
			B[r] |= ((bi >> r) & 1) << bi
	return B

print(genB())