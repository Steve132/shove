
"""

[first second third fourth fifth sixth]



2^10 2^1 2^4 2^2 2^16


10 1 4 2 16

2^10 2^1 2^4 2^2 2^16




2^1 2^2 2^3 2^4 2^5 2^6 2^7 2^8






134


0b10000110

3        2        0        4        7        5        1        6
8		 12       13       29       141      173	  175



"""

"""
[4, 2, 32, 128,  16,  64,   8,   1]


k2=bitceil(index)
k1=bitceil(size-index)
its guaranteed to be to the right of the location of k1 and the left of the location of k2.
169  (to the right and left using this search method...holy fuck.)
87
"""


import random 
import itertools

max_h=8
random.seed(10)
h=list(range(max_h))

def al(l,c=5,sep=''):
	return sep.join([str(x).ljust(c) for x in l])

def abin(x):
	return ('{0:0>'+str(max_h)+'b}').format(x)

def find_first(l,f):
	return next((i,x) for i,x in enumerate(l) if f(i,x))

def cto(x):
	s=abin(x)
	return len(s) - len(s.rstrip('1'))

def msb(x):
	return len(bin(x))-3

def downpermute_bits(d,h):
	y=0
	for i,x in enumerate(h):
		bv=(d >> x) & 1
		y=y | (bv << i)
	return y


def do_test():
	random.shuffle(h)
	eh=[(1 << i) for i in h]
	eMax=1<<max_h

	topindex=list(itertools.accumulate(eh))
	bottomindex=[0]+topindex[:-1]
	loc=list(range(max_h))
	print("loc: "+al(loc))
	print("h  : "+al(h))
	print("sz : "+al(eh))
	print("btm: "+al(bottomindex))
	print("top: "+al(topindex))
	
	tsize=10
	print(al(["index","bin_index","loc","exponent"],tsize))
	def test_index(dex,btm,top):
		off,gtx=find_first(h,lambda i,x: dex >= btm[i] and dex < top[i])

		cur=0
		t=4

		next=(1 << h[t])
		if(dex < next):
			t-=2
		else:
			t+=2
			cur+=next
		print(t,next,cur)

		next=(1 << h[t])
		if(dex < next):
			t-=1
		else:
			t+=1
			cur+=next
		print(t,next,cur)
		#guess=cto(px)
		print(al([dex,abin(dex),off,gtx],tsize))
	
	test_index(169,bottomindex,topindex)	
	#for idx in range(topindex[-1]):
	#	test_index(idx,bottomindex,topindex)

do_test()


"""

Essentially, the logic is basically, that you scan from left to right, and as you do, you subtract 2^h_i from the index. (how many left to skip) 
Then you record which bucket you run out on.
(this is like an accumulate almost or a prefix sum basically but the advantage is that you don't have to load everything from cache and the prefix scan can be in avx).

The other algorithm is you scan through the whole thing


sum d[i] 2^i-2^h[0] 
sum d[i] 2^i-2^h[0]-2^h[1]
sum d[i] 2^i-2^h[0]-2^h[1]-2^h[2] ....


so you can actually do this as a bit permutation.

the other algorithm is you do the thing where you lookup the rank of the highest order bit of the index, then use that to subtract to get a new index, then 
you repeat.   Or you do a b trie.  Or whatever.


bit permute the bits of d[i] then do count leading ones

sum d[i] 2^0-2^0 
sum d[i] 2^i-2^h[0]-2^h[1]
sum d[i] 2^i-2^h[0]-2^h[1]-2^h[2] ....
sum d[i] 2^i-2^h[0]-2^h[1]-2^h[2] ....






"""

