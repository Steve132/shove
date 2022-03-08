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

def find_range_step(I,B,bottom,top):
	bottom_out=bottom
	top_out=top

	testmask = bottom | (top & ~B)
	a=testmask < I
	if(a):
		bottom_out=testmask			
	else:
		top_out=testmask			

	print(bottom,top,~B,testmask,uint(a,H),bottom_out,top_out)
	return bottom_out,top_out

def find_range(I):
	bottom=uint(0,H)
	top=~uint(0,H)
	print(al(["bottom","top","r","~B","test","a","bottom_out","top_out"],9))
	result=uint(0,lH)
	for rbi in range(lH):
		trbi=lH-1-rbi
		bottom,top=find_range_step(I,B[trbi],bottom,top)
	print(uint(bottom,H),uint(top,H))
	return bottom,top
	

def find_handle_range(I):
	bottom,top=find_range(I+1)
	handle=top & ~bottom
	#count trailing zeros
	print(I,uint(I,H),int(handle),handle)

def find_bottom_range(h):
	hU=uint(1 << h,H)
	bottom,top=find_range(hU)
	print(hU,h,int(bottom),bottom)

for x in range(1 << H):
	find_handle_range(x)

for x in range(H):
	find_bottom_range(x)
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
    
    return std::bit_floor(bottom); //IMPORTANT: this doesn't work.  You need to get a back.
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

"""

template<unsigned int LMASKSIZE>
struct lmask_selector;

template<> struct lmask_selector<3> { using mask_t=uint8_t; };
template<> struct lmask_selector<4> { using mask_t=uint16_t; };
template<> struct lmask_selector<5> { using mask_t=uint32_t; };
template<> struct lmask_selector<6> { using mask_t=uint64_t; };

template<unsigned int LMASKSIZE>
struct lmask_searcher
{
    using mask_t=typename lmask_selector<LMASKSIZE>::mask_t;
    std::array<mask_t,LMASKSIZE> nB;

    template<unsigned int LI>
    inline uint8_t find_handle_step(mask_t I,mask_t bottom,mask_t top,
        std::integral_constant<unsigned int, LI>)
    {
        mask_t testmask = bottom | (top & nB[LI-1]);
        bool a = testmask < I;
        if(a){
            [[likely]]
            bottom=testmask;
        }
        else{
            [[unlikely]]
            top=testmask;
        }
        return find_handle_step(I,bottom,top,std::integral_constant<unsigned int,LI-1>{});
    }

    inline uint8_t find_handle_step(mask_t I,mask_t bottom,mask_t top,std::integral_constant<unsigned int, 0>)
    {
        return std::bit_floor(bottom);
    }
    inline uint8_t find_handle_step(size_t I)
    {
        return find_handle_step(static_cast<mask_t>(I),0,~mask_t(0),std::integral_constant<unsigned int,LMASKSIZE>{});
    }
    template<unsigned int LI>
    inline uint8_t find_bottom_step(uint8_t r,mask_t bottom,mask_t top,
        std::integral_constant<unsigned int, LI>)
    {
        mask_t testmask = bottom | (top & nB[LI-1]);
        bool a = (r >> (LI-1)) & 1;
        if(a){
            [[likely]]
            bottom=testmask;
        }
        else{
            [[unlikely]]
            top=testmask;
        }
        return find_bottom_step(r,bottom,top,std::integral_constant<unsigned int,LI-1>{});
    }

    inline uint8_t find_bottom_step(uint8_t r,mask_t bottom,mask_t top,std::integral_constant<unsigned int, 0>)
    {
        return bottom;
    }
    inline uint8_t find_bottom_step(uint8_t r)
    {
        return find_bottom_step(r,0,~mask_t(0),std::integral_constant<unsigned int,LMASKSIZE>{});
    }
};

int find_lmask(size_t I,std::array<uint8_t,3> nB)
{
    lmask_searcher<3> t;
    t.nB=nB;
    return t.find_bottom_step(i);
}"""