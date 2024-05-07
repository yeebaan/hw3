#include <cstdio>
#include <cstdlib>
using namespace std;

#include "Testbench.h"

unsigned char header[54] = {
	0x42,          // identity : B
	0x4d,          // identity : M
	0,    0, 0, 0, // file size
	0,    0,       // reserved1
	0,    0,       // reserved2
	54,   0, 0, 0, // RGB data offset
	40,   0, 0, 0, // struct BITMAPINFOHEADER size
	0,    0, 0, 0, // bmp width
	0,    0, 0, 0, // bmp height
	1,    0,       // planes
	24,   0,       // bit per pixel
	0,    0, 0, 0, // compression
	0,    0, 0, 0, // data size
	0,    0, 0, 0, // h resolution
	0,    0, 0, 0, // v resolution
	0,    0, 0, 0, // used colors
	0,    0, 0, 0  // important colors
};

Testbench::Testbench(sc_module_name n) : sc_module(n), output_rgb_raw_data_offset(54) {
	SC_THREAD(feed_rgb);
	sensitive << i_clk.pos();
	dont_initialize();
	SC_THREAD(fetch_result);
	sensitive << i_clk.pos();
	dont_initialize();
}

Testbench::~Testbench() {
	cout << "elapsed sc_time_stamp = " << total_run_time << endl;
}

int_fast8_t Testbench::read_bmp(string infile_name) {
	FILE *fp_s = NULL; // source file handler
	fp_s = fopen(infile_name.c_str(), "rb");
	if (fp_s == NULL) {
		printf("fopen %s error\n", infile_name.c_str());
		return -1;
	}
	// move offset to 10 to find rgb raw data offset
	fseek(fp_s, 10, SEEK_SET);
	fread(&input_rgb_raw_data_offset, sizeof(unsigned int), 1, fp_s);

	// move offset to 18 to get width & height;
	fseek(fp_s, 18, SEEK_SET);
	fread(&width, sizeof(unsigned int), 1, fp_s);
	fread(&height, sizeof(unsigned int), 1, fp_s);

	// get bit per pixel
	fseek(fp_s, 28, SEEK_SET);
	fread(&bits_per_pixel, sizeof(unsigned short), 1, fp_s);
	bytes_per_pixel = bits_per_pixel / 8;

	// move offset to input_rgb_raw_data_offset to get RGB raw data
	fseek(fp_s, input_rgb_raw_data_offset, SEEK_SET);

	source_bitmap =
		(unsigned char *)malloc((size_t)width * height * bytes_per_pixel);
	if (source_bitmap == NULL) {
		printf("malloc images_s error\n");
		return -1;
	}

	target_bitmap =
		(unsigned char *)malloc((size_t)width * height * bytes_per_pixel);
	if (target_bitmap == NULL) {
		printf("malloc target_bitmap error\n");
		return -1;
	}

	fread(source_bitmap, sizeof(unsigned char),
			(size_t)(long)width * height * bytes_per_pixel, fp_s);
	fclose(fp_s);
	return 0;
}

int_fast8_t Testbench::write_bmp(string outfile_name) {
	FILE *fp_t = NULL;      // target file handler
	unsigned int file_size; // file size

	fp_t = fopen(outfile_name.c_str(), "wb");
	if (fp_t == NULL) {
		printf("fopen %s error\n", outfile_name.c_str());
		return -1;
	}

	// file size
	file_size = width * height * bytes_per_pixel + output_rgb_raw_data_offset;
	header[2] = (unsigned char)(file_size & 0x000000ff);
	header[3] = (file_size >> 8) & 0x000000ff;
	header[4] = (file_size >> 16) & 0x000000ff;
	header[5] = (file_size >> 24) & 0x000000ff;

	// width
	header[18] = width & 0x000000ff;
	header[19] = (width >> 8) & 0x000000ff;
	header[20] = (width >> 16) & 0x000000ff;
	header[21] = (width >> 24) & 0x000000ff;

	// height
	header[22] = height & 0x000000ff;
	header[23] = (height >> 8) & 0x000000ff;
	header[24] = (height >> 16) & 0x000000ff;
	header[25] = (height >> 24) & 0x000000ff;

	// bit per pixel
	header[28] = bits_per_pixel;

	// write header
	fwrite(header, sizeof(unsigned char), output_rgb_raw_data_offset, fp_t);

	// write image
	fwrite(target_bitmap, sizeof(unsigned char),
			(size_t)(long)width * height * bytes_per_pixel, fp_t);

	fclose(fp_t);
	return 0;
}

template<typename T>
void Testbench::write(T rgb) {
#ifndef NATIVE_SYSTEMC
	o_rgb.put(rgb);
#else
	o_rgb.write(rgb);
#endif
}

result_t Testbench::read() {
#ifndef NATIVE_SYSTEMC
	result_t result = i_result.get();
#else
	result_t result = i_result.read();
#endif
	return result;
}

template<typename T, typename U, typename V>
rgb_t Testbench::get(T row, U col, V channel) {
	if (0 <= row && row < height && 0 <= col && col < width) {
		return source_bitmap[bytes_per_pixel * (width * row + col) + channel];
	} else {
		return 0;
	}
}			

template<typename T, typename U, typename V, typename W>
void Testbench::set(T row, U col, V channel, W result) {
	if (0 <= row && row < height && 0 <= col && col < width) {
		target_bitmap[bytes_per_pixel * (width * row + col) + channel] = result;
	} else {
	}
}			

void Testbench::feed_rgb() {
	total_start_time = sc_time_stamp();
#ifndef NATIVE_SYSTEMC
	o_rgb.reset();
#endif
	o_rst.write(false);
	wait();
	o_rst.write(true);
	for (int_fast16_t row(0); row < height; row++) {
		for (int_fast16_t col(0); col < width + 4; col++) {
			write(get(row - 2, col - 2, 0));
			write(get(row - 2, col - 2, 1));
			write(get(row - 2, col - 2, 2));
			write(get(row - 1, col - 2, 0));
			write(get(row - 1, col - 2, 1));
			write(get(row - 1, col - 2, 2));
			write(get(row    , col - 2, 0));
			write(get(row    , col - 2, 1));
			write(get(row    , col - 2, 2));
			write(get(row + 1, col - 2, 0));
			write(get(row + 1, col - 2, 1));
			write(get(row + 1, col - 2, 2));
			write(get(row + 2, col - 2, 0));
			write(get(row + 2, col - 2, 1));
			write(get(row + 2, col - 2, 2));
		}
	}
	while (true) {
		write(0);
	}
}

void Testbench::fetch_result() {
#ifndef NATIVE_SYSTEMC
	i_result.reset();
#endif
	wait();
	for (int_fast16_t row(0); row < height; row++) {
		cout << row << endl;
		for (int_fast16_t col(0); col < width + 4; col++) {
			uint_fast8_t result = read();
			set(row, col - 4, 0, result);
			set(row, col - 4, 1, result);
			set(row, col - 4, 2, result);
		}
	}
	total_run_time = sc_time_stamp() - total_start_time;
	sc_stop();
}
