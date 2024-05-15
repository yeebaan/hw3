#ifndef SOBEL_FILTER_H_
#define SOBEL_FILTER_H_
#include <systemc>
using namespace sc_core;
#ifndef NATIVE_SYSTEMC
#include <cynw_p2p.h>
#endif
#include "filter_def.h"
class SobelFilter: public sc_module
{
	public:
		sc_in_clk i_clk;
		sc_in < bool >  i_rst;
#ifndef NATIVE_SYSTEMC
		cynw_p2p< rgb_t >::in i_rgb;
		cynw_p2p< result_t >::out o_result;
#else
		sc_fifo_in< rgb_t > i_rgb;
		sc_fifo_out< result_t > o_result;
#endif
		SC_HAS_PROCESS( SobelFilter );
		SobelFilter( sc_module_name n );
		~SobelFilter();
		void do_filter();
		void reset();
		rgb_t read();
		template<typename T>
			void write(T result);
};
#endif
