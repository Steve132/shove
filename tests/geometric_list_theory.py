
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

#do_test()


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





""" Okay, try two.  The idea that we're going for here is that we need to exploit the fact that  
(∑ 2a_i < ∑ 2b_j ) →(2∑a_i < 2∑b_i )   Then that means that if the underlying array has each element a power of two, then you can represent it 
as just a series of non-negative exponents.

then you can convert the index to 2^{} sum form with binary, and then compare them that way in a smaller space (prove that poplog() is monotonic.

Does poplog not care about even numbers?  If so just add it.
"""

def poplog(x):
	z=bin(x)
	xlst=[(x+1) for x,v in enumerate(z[2:]) if v=='1']
	return sum(xlst)

def poplog_test():
	for x in range(256):
		for y in range(256):
			c1=x < y
			c2=poplog(x) < poplog(y)
			if(c1 != c2):
				print("Failed.")
				print(x,y)
				print(poplog(x),poplog(y))




""" Another method:
	p(a BOP b)=P(a) BOP P(b)
	~P(a)=P(~a)

	do mask based binary search on permuted items.


	unsigned int comp_and_toggle(unsigned int I,unsigned int M)
	{
		return (I < M) ? M : ~M;
	}

	result_mask=~(0);  //p is always permute up with mask to select the element, and also pre masked with occupied
					   //maybe in sorted order?

	search_mask=p(0b0000000011111111) & result_mask;
	result_mask=comp_and_toggle(I,search_mask);

	search_mask=p(0b0000111100001111) & result_mask;
	result_mask=comp_and_toggle(I,search_mask);

	search_mask=p(0b0011001100110011) & result_mask;
	result_mask=comp_and_toggle(I,search_mask);

	search_mask=p(0b0101010101010101) & result_mask;
	result_mask=comp_and_toggle(I,search_mask);

	https://programming.sirrida.de/bit_perm.html
	https://programming.sirrida.de/calcperm.php


	0b11111111 3 2 5 1 0 4 6 7

	0b00001111 3 2 5 1			0b00101110
	0b00110011 3 2     0 4      0b    1101

	It should always be possible to search for the base_offset of a particular mask by methods like the above.



"""

def perm_up(elems,selected):
	v=0
	for i,x in enumerate(elems):
		if((selected >> i) & 1):
			v|=1 << x
	return v

def cnot(I,M,MSK):
	if(I < M):
		return M
	else:
		return MSK-M


def do_test2():
	random.shuffle(h)
	eh=[(1 << i) for i in h]
	eMax=1<<max_h
	MSK=eMax-1

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
		vlist=[perm_up(h,0b00001111),perm_up(h,0b00110011),perm_up(h,0b01010101)]

		print("\nDEX: "+str((dex,abin(dex))))
		print("p(pass)"+str([abin(v) for v in vlist]))
		print("p(pass)"+str(vlist))
		print("gt_x: "+str(gtx))
		print("h: "+str(h))
		print("top: "+str(top))

		result=MSK
		v=vlist[0]


		search_msk = v & result & MSK
		result = cnot(dex,search_msk,MSK)
		print(abin(v),abin(search_msk),abin(result),abin(dex))
		print("gtx: "+str(gtx))

	#test_index(169,bottomindex,topindex)
	Nl=100
	Nu=180
	for idx in range(Nl,Nu):
		test_index(idx,bottomindex,topindex)

#do_test2()

"""
given(h (or r[h])) determine base_index
given(base_index determine h or rank)
"""


"""NOTE: if you disallow push_front entirely (a stack?) then you can guarantee that it's always
ordered and this problem is solved."""


"""THE ACTUAL OPERATION IS A CASCADING COMPARATOR:
	THE ACTUAL OPERATION IS A CASCADING COMPARATOR:

	RULES: because permutation, you can't use any operations which depend
	on neighbors.  This is like simd.


	handle->rank

	(or handle->index?)

	rank->index
	index->rank

	  in     out[lane]     |
	rank | 7 6 5 4 3 2 1 0 |
	---------------------------
	 000 | 0 0 0 0 0 0 0 0 |
	 001 | 0 0 0 0 0 0 0 1 |
	 010 | 0 0 0 0 0 0 1 1 |
	 011 | 0 0 0 0 0 1 1 1 |
	 100 | 0 0 0 0 1 1 1 1 |
	 101 | 0 0 0 1 1 1 1 1 |
	 110 | 0 0 1 1 1 1 1 1 |
	 111 | 0 1 1 1 1 1 1 1 |


	This is SIMD.     Specific computation is

	out[i]=i > r

	So we are actually just doing the computation using the exec mask (lol).
	Specifically, we're actually implementing a "Cascading Comparator" which can be
	done in arbitrary chunk configurations for simd.  the permutation is applied to the lanes first.

	Lets say the biggest one is 16 bits then we actually need a 4 bit comparator.
	https://www.electronicshub.org/digital-comparator-and-magnitude-comparator/

	This is a cascading comparator operating in simd over the rank.
	Also we're lucky because the lanes are constants.

	i > r
"""

lR=3
R=1<<lR

def NOT(x):
	return (((1<<R)-1)-x)

def gen_nB():
	nB=[]
	for bi in range(lR):
		b=0
		for r in range(R):
			if((r >> bi) & 0x1):
				b|=1 << r
		nB.append(NOT(b))
	return nB



def updater(a,cur,nB):
	#x=nB if not(a) else NOT(nB)
	#out = x & cur  | (nB if a else 0)
	#truth table:

	#a   :11110000
	#cur :11001100
	#nB: :10101010
	#out: 11101000

	#y = BC + AC + AB
	#y = (A + B) (A + C) (B + C)
	#!y = !A.!B + !A.!C + !B.!C
	#with A,B=prev,C=nB,

	#BC + A(C + B)
	out = (cur & nB) | ((nB | cur) if a else 0)

	#y = (B + A)C + AB
	#y = AC + (A + C)B


	return out

def updater2(a1,a2,cur,nB,nB2):
	cur=updater(a1,cur,nB)
	cur=updater(a2,cur,nB2)

	#a1   11111111111111110000000000000000
	#a2   11111111000000001111111100000000
	#cur: 11110000111100001111000011110000
	#b1:  11001100110011001100110011001100
	#b2:  10101010101010101010101010101010

	#out: 11111110101010001110101010000000
	#http://www.32x8.com/sop5_____A-B-C-D-E_____m_7-9-11-13-14-15-19-21-23-25-26-27-28-29-30-31___________option-8_____989781966378827597705
	#http://www.32x8.com/pos5_____A-B-C-D-E_____m_7-9-11-13-14-15-19-21-23-25-26-27-28-29-30-31___________option-a_____989781966378827597705
	#y = BE + CDE + BCD + ADE + ACE + ABD + ABC

	#y = BE + ADE + ABD + CDE + BCD + ACE + ABC
	#y  = (BE + ADE + ABD) + C(DE + BD + AE + AB)
	#y  = (BE + ADE + ABD) + C(D(E + B) + A(E + B))
	#y  = (BE + ADE + ABD) + C(D+A)(E+B)
	#y = (B + E) (A + B + C) (A + B + D) (B + C + D) (A + C + E) (A + D + E) (C + D + E)
	
	return cur

def compute_par_rank(r,nB):
	result=0
	a=[(((r >> i) & 1)==1) for i in range(len(nB))]

	result=updater(a[0],result,nB[0])
	result=updater(a[1],result,nB[1])
	result=updater(a[2],result,nB[2])

	return result

def make_ttable():
	print("ttable")
	print(abin(NOT(nB[0])))
	print(abin(NOT(nB[1])))
	print(abin(updater(False,NOT(nB[1]),NOT(nB[0]))))

def ranktest():
	nB=gen_nB()
	for b in nB:
		print(abin(b))
	for x in range(1 << len(nB)):
		print(abin(x),abin(compute_par_rank(x,nB)))

	print(abin(NOT(nB[2])))
	print(abin(NOT(nB[1])))
	print(abin(NOT(nB[0])))
	print(abin(updater2(True,True,NOT(nB[2]),NOT(nB[1]),NOT(nB[0]))))

ranktest()

#PHMINPOSUW is another option (minimum scan)
#https://github.com/HJLebbink/asm-dude/wiki/VP2INTERSECTD_VP2INTERSECTQ
#https://github.com/HJLebbink/asm-dude/wiki/VPERMI2W_D_Q_PS_PD
#permute? extract? lots of options. Hm.
#https://github.com/HJLebbink/asm-dude/wiki/VPTERNLOGD_VPTERNLOGQ
#^this thing is just cool as hell for this purpose tbh.
#https://github.com/HJLebbink/asm-dude/wiki/VTESTPD_VTESTPS
#pdep
#galois fields
#de brujin sequence

"""
#include<cstdint>
#include<cstdlib>
#include<array>


using mask_t=size_t;


using mask_t=size_t;
template<uint8_t A,uint8_t bi>
static constexpr inline bool mbittest(){
	if constexpr(((A >> bi) & 0x1) == 1) return true;
	else return false;
}

template<uint8_t A,uint8_t bi>
static constexpr inline mask_t mbiteq(mask_t M) {
	if constexpr(mbittest<A,bi>()) return ~M;
	else return M;
}

template<uint8_t LN,uint8_t R,uint8_t Li>
struct bottom_index_unroll{
	static constexpr mask_t compute(const std::array<mask_t,LN>& nB) {
		mask_t prev=bottom_index_unroll<LN,R,Li-1>::compute(nB);
		mask_t rhs=prev & mbiteq<R,Li>(nB[Li]);
		if constexpr(mbittest<R,Li>())
		{
			return rhs | nB[Li];
		}
		return rhs;
	};
};
template<uint8_t LN,uint8_t R>
struct bottom_index_unroll<LN,R,0>{
	static constexpr mask_t compute(const std::array<mask_t,LN>& nB) {
		if constexpr(mbittest<R,0>())
		{
			return nB[0];
		}
		return 0;
	};
};


template<uint8_t R,uint8_t LN>
static inline mask_t bottom_index(const std::array<mask_t,LN>& nB) //nB[b] >> i is the b_th bit of ~B in lane i
{
	return bottom_index_unroll<LN,R,LN-1>::compute(nB);
}
template<uint8_t LN>
mask_t bottom_index_dyn(uint8_t R,const std::array<mask_t,LN>& nB) //nB[b] >> i is the b_th bit of ~B in lane i
{
	switch(R & ((1 << LN)-1))
	{
		case 0:
		default: return 0; //return size technically for default.  But whatever.
		case 1: return bottom_index<1,LN>(nB);
		case 2: return bottom_index<2,LN>(nB);
		case 3: return bottom_index<3,LN>(nB);
		case 4: return bottom_index<4,LN>(nB);
		case 5: return bottom_index<5,LN>(nB);
		case 6: return bottom_index<6,LN>(nB);
		case 7: return bottom_index<7,LN>(nB);
		case 8: return bottom_index<8,LN>(nB);
		case 9: return bottom_index<9,LN>(nB);
		case 10: return bottom_index<10,LN>(nB);
		case 11: return bottom_index<11,LN>(nB);
		case 12: return bottom_index<12,LN>(nB);
		case 13: return bottom_index<13,LN>(nB);
		case 14: return bottom_index<14,LN>(nB);
		case 15: return bottom_index<15,LN>(nB);
		case 16: return bottom_index<16,LN>(nB);
		case 17: return bottom_index<17,LN>(nB);
		case 18: return bottom_index<18,LN>(nB);
		case 19: return bottom_index<19,LN>(nB);
		case 20: return bottom_index<20,LN>(nB);
		case 21: return bottom_index<21,LN>(nB);
		case 22: return bottom_index<22,LN>(nB);
		case 23: return bottom_index<23,LN>(nB);
		case 24: return bottom_index<24,LN>(nB);
		case 25: return bottom_index<25,LN>(nB);
		case 26: return bottom_index<26,LN>(nB);
		case 27: return bottom_index<27,LN>(nB);
		case 28: return bottom_index<28,LN>(nB);
		case 29: return bottom_index<29,LN>(nB);
		case 30: return bottom_index<30,LN>(nB);
		case 31: return bottom_index<31,LN>(nB);
		case 32: return bottom_index<32,LN>(nB);
		case 33: return bottom_index<33,LN>(nB);
		case 34: return bottom_index<34,LN>(nB);
		case 35: return bottom_index<35,LN>(nB);
		case 36: return bottom_index<36,LN>(nB);
		case 37: return bottom_index<37,LN>(nB);
		case 38: return bottom_index<38,LN>(nB);
		case 39: return bottom_index<39,LN>(nB);
		case 40: return bottom_index<40,LN>(nB);
		case 41: return bottom_index<41,LN>(nB);
		case 42: return bottom_index<42,LN>(nB);
		case 43: return bottom_index<43,LN>(nB);
		case 44: return bottom_index<44,LN>(nB);
		case 45: return bottom_index<45,LN>(nB);
		case 46: return bottom_index<46,LN>(nB);
		case 47: return bottom_index<47,LN>(nB);
		case 48: return bottom_index<48,LN>(nB);
		case 49: return bottom_index<49,LN>(nB);
		case 50: return bottom_index<50,LN>(nB);
		case 51: return bottom_index<51,LN>(nB);
		case 52: return bottom_index<52,LN>(nB);
		case 53: return bottom_index<53,LN>(nB);
		case 54: return bottom_index<54,LN>(nB);
		case 55: return bottom_index<55,LN>(nB);
		case 56: return bottom_index<56,LN>(nB);
		case 57: return bottom_index<57,LN>(nB);
		case 58: return bottom_index<58,LN>(nB);
		case 59: return bottom_index<59,LN>(nB);
		case 60: return bottom_index<60,LN>(nB);
		case 61: return bottom_index<61,LN>(nB);
		case 62: return bottom_index<62,LN>(nB);
		case 63: return bottom_index<63,LN>(nB);
	};
}
static constexpr unsigned int LNUM_BITS=6;
uint exponent_search(uint8_t R,size_t I,const std::array<mask_t,LNUM_BITS>& mP)
{
   return bottom_index_dyn<LNUM_BITS>(R,mP);
}

#include<cstdint>
#include<cstdlib>
#include<array>


using mask_t=size_t;


using mask_t=size_t;
static constexpr inline bool mbittest(uint8_t A,uint8_t bi){
	if (((A >> bi) & 0x1) == 1) return true;
	else return false;
}

static constexpr inline mask_t mbitmul(uint8_t A,uint8_t bi,mask_t M) {
	return mbittest(A,bi) ? M : 0;
}

template<uint8_t LN,int Li>
struct bottom_index_unroll{
	static constexpr mask_t compute(uint8_t R,const std::array<mask_t,LN>& nB) {
		mask_t cur=bottom_index_unroll<LN,Li-1>::compute(R,nB);
		mask_t nBt=nB[Li];

		//BC + AC + AB
		return (cur & nBt) | mbitmul(R,Li,cur) | mbitmul(R,Li,nBt);
		//BC + A(C+B)
		return (cur & nBt) | mbitmul(R,Li,(nBt | cur));
	};
};
template<uint8_t LN>
struct bottom_index_unroll<LN,-1>{
	static constexpr mask_t compute(uint8_t R,const std::array<mask_t,LN>& nB) {
		return 0;
	};
};


template<uint8_t LN>
static inline mask_t bottom_index(uint8_t R,const std::array<mask_t,LN>& nB) //nB[b] >> i is the b_th bit of ~B in lane i
{
	return bottom_index_unroll<LN,LN-1>::compute(R,nB);
}

static constexpr unsigned int LNUM_BITS=6;
uint exponent_search(uint8_t R,size_t I,const std::array<mask_t,LNUM_BITS>& mP)
{
   return bottom_index<LNUM_BITS>(R,mP);
}
"""
