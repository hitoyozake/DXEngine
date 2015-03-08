
#include <chrono>
#include <Windows.h>
#include <algorithm>
#include <fstream>
#include <boost/chrono.hpp>
#include <boost/chrono/time_point.hpp>
#include <boost/timer.hpp>
#include <cassert>

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
		}

		void wait_auto()
		{
			calc_result_ = static_cast< long >( ( std::chrono::high_resolution_clock::now() - st_ ).count() / 10000.0  );

			//assert( calc_result_ >= 0 );

			auto const fps_calc = 16 - calc_result_;

			if( fps_calc > 0 )
			{
				Sleep( fps_calc );
				log << fps_calc << std::endl;

			}
			//Sleep( 16 );
			st_ = std::chrono::high_resolution_clock::now( );


		}

	private:
		long calc_result_;
		std::chrono::system_clock::time_point st_;
		boost::timer timer_;
		std::chrono::system_clock::time_point start_;
		std::chrono::system_clock::time_point end_;
		double s;
		std::ofstream log;

	};

}