#include <cmath>
#ifndef NATIVE_SYSTEMC
#include "stratus_hls.h"
#endif

#include "SobelFilter.h"

SobelFilter::SobelFilter(sc_module_name n) : sc_module(n) {
	SC_THREAD(do_filter);
	sensitive << i_clk.pos();
	dont_initialize();
	reset_signal_is(i_rst, false);
#ifndef NATIVE_SYSTEMC
	i_rgb.clk_rst(i_clk, i_rst);
	o_result.clk_rst(i_clk, i_rst);
#endif
}

void SobelFilter::reset()
{
#ifndef NATIVE_SYSTEMC
	HLS_DEFINE_PROTOCOL("reset 1");
	i_rgb.reset();
	o_result.reset();
#endif
	wait();
}

rgb_t SobelFilter::read() {
	rgb_t rgb{0};
	{
#ifndef NATIVE_SYSTEMC
		HLS_DEFINE_PROTOCOL("in 1");
		rgb = i_rgb.get();
#else
		rgb = i_rgb.read();
#endif
		wait();
	}
	return rgb;
}

template<typename T>
void SobelFilter::write(T result) {
#ifndef NATIVE_SYSTEMC
	HLS_DEFINE_PROTOCOL("out 1");
	o_result.put(result);
#else
	o_result.write(result);
#endif
	wait();
}

SobelFilter::~SobelFilter() {}

void SobelFilter::do_filter() {
	reset();
	std::array<uint8_t, 25> buffer{};
	HLS_FLATTEN_ARRAY(buffer);
	while (true) {
		HLS_CONSTRAIN_LATENCY(0, HLS_ACHIEVABLE, "");
		HLS_PIPELINE_LOOP(HARD_STALL, 29, "pipeline 1");
		for (uint8_t i{}; i < 25; i++) {
			buffer[i] = (i < 20) ? buffer[i + 5] : (read() + read() + read()) / 3;
		}
		write( 
				(
				 buffer[ 0] *  1 +
				 buffer[ 1] *  4 +
				 buffer[ 2] *  7 +
				 buffer[ 3] *  4 +
				 buffer[ 4] *  1 +
				 buffer[ 5] *  4 +
				 buffer[ 6] * 16 +
				 buffer[ 7] * 26 +
				 buffer[ 8] * 16 +
				 buffer[ 9] *  4 +
				 buffer[10] *  7 +
				 buffer[11] * 26 +
				 buffer[12] * 41 +
				 buffer[13] * 26 +
				 buffer[14] *  7 +
				 buffer[15] *  4 +
				 buffer[16] * 16 +
				 buffer[17] * 26 +
				 buffer[18] * 16 +
				 buffer[19] *  4 +
				 buffer[20] *  1 +
				 buffer[21] *  4 +
				 buffer[22] *  7 +
				 buffer[23] *  4 +
				 buffer[24] *  1
				 )
				 / 273
				 );
	}
}
