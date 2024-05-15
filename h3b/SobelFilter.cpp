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

SobelFilter::~SobelFilter() {}

void SobelFilter::reset() {
#ifndef NATIVE_SYSTEMC
  HLS_DEFINE_PROTOCOL("reset 1");
  i_rgb.reset();
  o_result.reset();
#endif
  wait();
}

rgb_t SobelFilter::read() {
  rgb_t input{0};
  {
#ifndef NATIVE_SYSTEMC
    HLS_DEFINE_PROTOCOL("in 2");
    input = i_rgb.get();
#else
    input = i_rgb.read();
#endif
    wait();
  }
  return input;
}

template <typename T> void SobelFilter::write(T result) {
#ifndef NATIVE_SYSTEMC
  HLS_DEFINE_PROTOCOL("out 2");
  o_result.put(result);
#else
  o_result.write(result);
#endif
  wait();
}

void SobelFilter::do_filter() {
  reset();
  std::array<sc_uint<8>, 25> buffer{};
#define FILTER_FLATTEN_AND_PIPELINE 0
#ifndef NATIVE_SYSTEMC
#if FILTER_FLATTEN_AND_PIPELINE
  HLS_FLATTEN_ARRAY(buffer);
#endif
#endif
  while (true) {
#ifndef NATIVE_SYSTEMC
#if FILTER_FLATTEN_AND_PIPELINE
    HLS_CONSTRAIN_LATENCY(0, HLS_ACHIEVABLE, "latency 1");
    HLS_PIPELINE_LOOP(HARD_STALL, 1, "pipeline 1");
#endif
#endif
    rgb_t input{};
    {
#ifndef NATIVE_SYSTEMC
      HLS_DEFINE_PROTOCOL("in 1");
      input = i_rgb.get();
#else
      input = i_rgb.read();
#endif
      wait();
    }
    buffer[0] = buffer[5];
    buffer[1] = buffer[6];
    buffer[2] = buffer[7];
    buffer[3] = buffer[8];
    buffer[4] = buffer[9];
    buffer[5] = buffer[10];
    buffer[6] = buffer[11];
    buffer[7] = buffer[12];
    buffer[8] = buffer[13];
    buffer[9] = buffer[14];
    buffer[10] = buffer[15];
    buffer[11] = buffer[16];
    buffer[12] = buffer[17];
    buffer[13] = buffer[18];
    buffer[14] = buffer[19];
    buffer[15] = buffer[20];
    buffer[16] = buffer[21];
    buffer[17] = buffer[22];
    buffer[18] = buffer[23];
    buffer[19] = buffer[24];
    buffer[20] = (sc_uint<8>(sc_bv<8>(input.range(24 * 0 + 23, 24 * 0 + 16))) +
                  sc_uint<8>(sc_bv<8>(input.range(24 * 0 + 15, 24 * 0 + 8))) +
                  sc_uint<8>(sc_bv<8>(input.range(24 * 0 + 7, 24 * 0)))) /
                 3;
    buffer[21] = (sc_uint<8>(sc_bv<8>(input.range(24 * 1 + 23, 24 * 1 + 16))) +
                  sc_uint<8>(sc_bv<8>(input.range(24 * 1 + 15, 24 * 1 + 8))) +
                  sc_uint<8>(sc_bv<8>(input.range(24 * 1 + 7, 24 * 1)))) /
                 3;
    buffer[22] = (sc_uint<8>(sc_bv<8>(input.range(24 * 2 + 23, 24 * 2 + 16))) +
                  sc_uint<8>(sc_bv<8>(input.range(24 * 2 + 15, 24 * 2 + 8))) +
                  sc_uint<8>(sc_bv<8>(input.range(24 * 2 + 7, 24 * 2)))) /
                 3;
    buffer[23] = (sc_uint<8>(sc_bv<8>(input.range(24 * 3 + 23, 24 * 3 + 16))) +
                  sc_uint<8>(sc_bv<8>(input.range(24 * 3 + 15, 24 * 3 + 8))) +
                  sc_uint<8>(sc_bv<8>(input.range(24 * 3 + 7, 24 * 3)))) /
                 3;
    buffer[24] = (sc_uint<8>(sc_bv<8>(input.range(24 * 4 + 23, 24 * 4 + 16))) +
                  sc_uint<8>(sc_bv<8>(input.range(24 * 4 + 15, 24 * 4 + 8))) +
                  sc_uint<8>(sc_bv<8>(input.range(24 * 4 + 7, 24 * 4)))) /
                 3;
#define OPTIMAL_WIDTH 1
#if OPTIMAL_WIDTH
    const result_t result {
#else
    const auto result {
#endif
      ((buffer[0] * 1 + buffer[1] * 4 + buffer[2] * 7 + buffer[3] * 4 +
        buffer[4] * 1 + buffer[5] * 4 + buffer[6] * 16 + buffer[7] * 26 +
        buffer[8] * 16 + buffer[9] * 4 + buffer[10] * 7 + buffer[11] * 26 +
        buffer[12] * 41 + buffer[13] * 26 + buffer[14] * 7 + buffer[15] * 4 +
        buffer[16] * 16 + buffer[17] * 26 + buffer[18] * 16 + buffer[19] * 4 +
        buffer[20] * 1 + buffer[21] * 4 + buffer[22] * 7 + buffer[23] * 4 +
        buffer[24] * 1)
#define REPLACE_DIV_WITH_MUL 1
#if REPLACE_DIV_WITH_MUL
       * 15) >>
          12
#else
       / 273)
#endif
    };
    {
#ifndef NATIVE_SYSTEMC
      HLS_DEFINE_PROTOCOL("out 1");
      o_result.put(result);
#else
      o_result.write(result);
#endif
      wait();
    }
  }
}
