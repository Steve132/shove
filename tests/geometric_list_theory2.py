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

def find_handle_step(I,nB,bottom,top):
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


def find_handle(I):
	bottom=uint(0,H)
	top=~uint(0,H)
	print(al(["bottom","top","I","nB","test","a","bottom_out","top_out"],9))
	result=uint(0,lH)
	for rbi in range(lH):
		bottom,top,a=find_handle_step(I,nB[lH-1-rbi],bottom,top)
		result|=a << lH-1-rbi
	print(int(I),I,int(result))



def find_bottom_step(r,nB,bottom,top):
	bottom_out=bottom
	top_out=top

	testmask = bottom | (top & nB)
	if(r):
		bottom_out=testmask			
	else:
		top_out=testmask			

	print(bottom,top,nB,testmask,uint(r,H),bottom_out,top_out)
	return bottom_out,top_out

def find_bottom(r):
	bottom=uint(0,H)
	top=~uint(0,H)
	print(al(["bottom","top","r","nB","test","a","bottom_out","top_out"],9))
	result=uint(0,lH)
	for rbi in range(lH):
		trbi=lH-1-rbi
		bottom,top=find_bottom_step((r>>trbi) & 1,nB[trbi],bottom,top)
	print(r,uint(bottom,lH),int(bottom))

#for x in range(1 << H):
#	find_handle(x)

for x in range(H):
	find_bottom(x)

#is the gfaffine method workable too?
#how do mul div rem factor into this?
# https://en.wikipedia.org/wiki/De_Bruijn_sequence  D(2,n)..  Every possible 2^n occurs exactly once as a substring.

"""
using mask_t=uint64_t;
static constexpr unsigned int LNUM_BITS=6;
static inline void find_handle_step(mask_t I,mask_t& bottom,mask_t& top,mask_t nB)
{
    mask_t testmask = bottom | (top & nB);
	if(testmask < I)
		bottom=testmask;
	else
		top=testmask;
}    
uint8_t find_handle(mask_t I,const std::array<mask_t,LNUM_BITS>& mP)
{
    std::array<mask_t,LNUM_BITS> mB=mP;
    mask_t bottom=0;
    mask_t top=~mask_t(0);
    
    find_handle_step(I,bottom,top,mB[5]);
    find_handle_step(I,bottom,top,mB[4]);
    find_handle_step(I,bottom,top,mB[3]);
    find_handle_step(I,bottom,top,mB[2]);
    find_handle_step(I,bottom,top,mB[1]);
    find_handle_step(I,bottom,top,mB[0]);
    
    return std::bit_floor(bottom);
}
static inline void find_bottom_step(bool r,mask_t& bottom,mask_t& top,mask_t nB)
{
    mask_t testmask = bottom | (top & nB);
	if(r)
		bottom=testmask;
	else
		top=testmask;
}
mask_t find_bottom(uint8_t r,const std::array<mask_t,LNUM_BITS>& mP)
{
    std::array<mask_t,LNUM_BITS> mB=mP;
    mask_t bottom=0;
    mask_t top=~mask_t(0);
    
    find_bottom_step((r >> 5) & 1,bottom,top,mB[5]);
    find_bottom_step((r >> 4) & 1,bottom,top,mB[4]);
    find_bottom_step((r >> 3) & 1,bottom,top,mB[3]);
    find_bottom_step((r >> 2) & 1,bottom,top,mB[2]);
    find_bottom_step((r >> 1) & 1,bottom,top,mB[1]);
    find_bottom_step((r >> 0) & 1,bottom,top,mB[0]);
    
    return bottom;
}




"""