#include <chrono>
#include <algorithm>

#include "time_manager.h"

namespace time_m
{
	long long get_now()
	{
		auto a = std::chrono::system_clock::now() - std::chrono::system_clock::now();

		return std::chrono::duration_cast< std::chrono::milliseconds >( a ).count();
	}



	time_manager::time_manager( ) : start_( std::chrono::system_clock::now( ) ), end_( std::chrono::system_clock::now( ) ), log( "fps.txt" )
	{
		calc_result_ = std::chrono::duration_cast< std::chrono::milliseconds >( end_ - start_ ).count();
	}

}

