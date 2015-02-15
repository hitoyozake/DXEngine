
#include <chrono>
#include <Windows.h>
#include <algorithm>

namespace time_m
{
	class time_manager
	{
	public:
		time_manager();
		double get_fps() const
		{
			return ( 16.66 * 60 ) / static_cast< double >( ( std::max< long long > )( calc_result_, 1 ) );
		}

		void update()
		{
			calc_result_ = std::chrono::duration_cast< std::chrono::milliseconds >( end_ - start_ ).count();
			start_ = end_;
			end_ = std::chrono::system_clock::now();
		}

		void wait_auto()
		{
			//60fpsÇ…é©ìÆÇ≈í≤êÆÇ∑ÇÈ
			if( calc_result_ < 16 )
			{
				Sleep( 16 - calc_result_ );
			}

			update();
		}

	private:
		long long calc_result_;

		std::chrono::system_clock::time_point start_;
		std::chrono::system_clock::time_point end_;

	};

}