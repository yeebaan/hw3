#ifndef TESTBENCH_H_
#define TESTBENCH_H_
#include <string>
using namespace std;
#include <systemc>
using namespace sc_core;
#ifndef NATIVE_SYSTEMC
#include <cynw_p2p.h>
#endif
#include "filter_def.h"
class Testbench : public sc_module {
	public:
		sc_in_clk i_clk;
		sc_out < bool >  o_rst;
#ifndef NATIVE_SYSTEMC
		cynw_p2p< rgb_t >::base_out o_rgb;
		cynw_p2p< result_t >::base_in i_result;
#else
		sc_fifo_out< rgb_t > o_rgb;
		sc_fifo_in< result_t > i_result;
#endif
		SC_HAS_PROCESS(Testbench);
		Testbench(sc_module_name n);
		~Testbench();
		int_fast8_t read_bmp(string infile_name);
		int_fast8_t write_bmp(string outfile_name);
		uint_fast32_t input_rgb_raw_data_offset;
		uint_fast32_t output_rgb_raw_data_offset;
		int_fast16_t width;
		int_fast16_t height;
		uint_fast8_t bits_per_pixel;
		uint_fast8_t bytes_per_pixel;
		uint8_t *source_bitmap;
		uint8_t *target_bitmap;
		void feed_rgb();
		void fetch_result();
		sc_time total_start_time;
		sc_time total_run_time;
		template<typename T>
			void write(T rgb);
		rgb_t read();
		template<typename T, typename U, typename V>
			rgb_t get(T row, U col, V channel);
		template<typename T, typename U, typename V, typename W>
			void set(T row, U col, V channel, W rgb);
};
#endif
