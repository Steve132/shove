#pragma once


#include<chrono>

namespace shv
{
    template<class TrivialClock=std::chrono::steady_clock>
	struct expiration_timer
	{
        public:
            using clock = TrivialClock;
            using duration = typename clock::duration;
            using time_point = typename clock::time_point;
        
		protected:
			clock::time_point end_time;
			clock::duration period;
		public:


			constexpr auto expires_at() const noexcept
			{
				return end_time;
			}

			auto remaining_time() const noexcept
			{
				return end_time - clock::now();
			}

			constexpr auto started_at() const
			{
				//paused means started at in the future.
				return end_time - period;
			}

			constexpr bool is_expired(const time_point& current_time=clock::now()) const
			{
				return end_time <= current_time;
			}

            constexpr operator bool() const
			{
				return !is_expired();
			}

			constexpr void reset(const time_point& current_time, duration dur)
			{
				period = dur;
				end_time = current_time + period;
			}

			void reset(const duration& dur)
			{
				reset(clock::now(), dur);
			}

			void reset(const time_point& current_time = clock::now())
			{
				reset(current_time, period);
			}

			void expire(const time_point& current_time = clock::now())
			{
				end_time = current_time;
			}

			/*constexpr void pause()
			{
                if(!is_expired()) //if it's running, it cannot expire.
                {
                    end_time=clock::now();
                }
			}*/


			constexpr expiration_timer(bool running=false):
				end_time(running ? clock::time_point::max() : clock::now()), period{}
			{}

			constexpr expiration_timer(const time_point& current_time, const duration& dur,bool running=false):
				end_time(running ? current_time+dur : current_time), period{dur}
			{}
			
			constexpr expiration_timer(const duration& dur,bool running=false):
				expiration_timer(clock::now(),dur,running)
			{}

			constexpr auto operator <=>(const expiration_timer& other) const
			{
				return end_time <=> other.end_time;
			}

			/*
			struct paused: protected ExpirationTimer{
				constexpr paused(const tp25::clock::duration& dur=tp25::clock::duration::max()):
					start_time()
				{
					
				}
			};*/
	};
}
