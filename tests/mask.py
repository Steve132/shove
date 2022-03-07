

def uint_metaclass_func(name,bases,namespace):
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
					return newfunc1(s.__class__,r,s.lM)
				return z
			namespace[nn]=rebind_z(fn,ifunc,newfunc)
	
	return type(name,bases,namespace)
		
class uint(int,metaclass=uint_metaclass_func):
	def __new__(cls, v=0,lM=64):
		M=1<<lM
		if(v < 0):
			v=M+v
		if(v >= M):
			v=v % M
		v=int.__new__(cls,v)
		v.lM=lM
		return v
	
	def __str__(self):
		return ('{0:0>'+str(H)+'b}').format(self)
	def __repr__(self):
		return ('{0:0>'+str(H)+'b}').format(self)
