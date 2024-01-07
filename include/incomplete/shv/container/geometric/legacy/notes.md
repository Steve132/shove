            struct accessor{
                uint_t& val;
                index_t i;
                constexpr operator bool() const{
                    return (val >> i) & static_cast<constexpr index_t>(0x1);
                }
                constexpr accessor& operator=(bool b){
                    const uint_t bm = static_cast<constexpr index_t>(0x1) << i;
                    if(b){
                        val |= bm;
                    }else{
                        val &= ~(bm);
                    }
                    return *this;
                }
            };
            constexpr accessor operator [] (index_t i){
                return {val,i};
            }
            uint_t val=0;