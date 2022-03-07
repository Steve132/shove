import itertools
import random
from uint import uint

lH = 3
H = 1 << lH

h = list(range(H))

def al(l, c=5, sep=''):
	return sep.join([str(x).ljust(c) for x in l])

def genB():
	B=[uint(0,H)]*lH
	for bi in range(H):
		for r in range(lH):
			B[r] |= ((bi >> r) & 1) << bi
	return B

B=genB()
nB=[~x for x in B]

def updater(I,nB,bottom,top):

	bottom_out=bottom
	top_out=top

	testmask = bottom | (top & nB)
	a=I >= testmask
	if(a):
		bottom_out=testmask
	else:
		top_out=testmask

	print(bottom,top,I,nB,testmask,uint(a,H),bottom_out,top_out)
	return bottom_out,top_out,a


def find_a(I):
	zeros=~uint(0,H)
	ones=uint(0,H)
	print(al(["m1","m2","I","nB","test","a","m1_out","m2_out"],9))
	ones,zeros,a2=updater(I,nB[2],ones,zeros)
	ones,zeros,a1=updater(I,nB[1],ones,zeros)
	ones,zeros,a0=updater(I,nB[0],ones,zeros)
	result=(uint(a2,3) << 2 | uint(a1,3) << 1 | uint(a0,3) << 0 )
	print(int(I),I,int(result))



#find_a(uint(62,8))

for x in range(256):
	find_a(uint(x,8))

#is the gfaffine method workable too?
#how do mul div rem factor into this?
# https://en.wikipedia.org/wiki/De_Bruijn_sequence  D(2,n)..  Every possible 2^n occurs exactly once as a substring.

"""
using mask_t=uint32_t;
static inline void compute(mask_t I,mask_t& bottom,mask_t& top,mask_t nB)
{
    mask_t testmask = bottom | (top & nB);
	bool a=I >= testmask;
	if(a)
    {
		bottom=testmask;
    }
	else
    {
		top=testmask;
    }
}
static constexpr unsigned int LNUM_BITS=3;    
static inline uint8_t rsearch(mask_t I,const std::array<mask_t,LNUM_BITS>& mP)
{
    mask_t bottom=0;
    mask_t top=~mask_t(0);
    
    compute(I,bottom,top,mP[4]);
    compute(I,bottom,top,mP[3]);
    compute(I,bottom,top,mP[2]);
    compute(I,bottom,top,mP[1]);
    compute(I,bottom,top,mP[0]);
    
    return std::bit_floor(bottom);
}



uint exponent_search(uint8_t R,size_t I,const std::array<mask_t,LNUM_BITS>& mP)
{
   return rsearch(I,mP);
}



"""