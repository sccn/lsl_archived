#include "xdfwriter.h"

int main(int argc, char** argv) {
	XDFWriter w("test.xdf");
	const uint32_t sid = 0x02C0FFEE;
	const std::string footer(
	    "<?xml version=\"1.0\"?>"
	    "<info>"
	    "<first_timestamp>5.1</first_timestamp>"
	    "<last_timestamp>5.9</last_timestamp>"
	    "<sample_count>9</sample_count>"
	    "<clock_offsets>"
	    "<offset><time>50979.7660030605</time><value>-3.436503902776167e-06</value></offset>"
	    "</clock_offsets></info>");
	w.write_stream_header(0, "<?xml version=\"1.0\"?>"
	                         "<info>"
	                         "<name>SendDataC</name>"
	                         "<type>EEG</type>"
	                         "<channel_count>3</channel_count>"
	                         "<nominal_srate>10</nominal_srate>"
	                         "<channel_format>int16</channel_format>"
	                         "<created_at>50942.723319709003</created_at>"
	                         "</info>");
	w.write_stream_header(sid, "<?xml version=\"1.0\"?>"
	                           "<info>"
	                           "<name>SendDataString</name>"
	                           "<type>StringMarker</type>"
	                           "<channel_count>1</channel_count>"
	                           "<nominal_srate>10</nominal_srate>"
	                           "<channel_format>string</channel_format>"
	                           "<created_at>50942.723319709003</created_at>"
	                           "</info>");
	w.write_boundary_chunk();

	// write a single int16_t sample
	w.write_data_chunk(0, {5.1}, std::vector<int16_t>{0xC0, 0xFF, 0xEE}, 3);

	// write a single std::string sample with a length > 127
	w.write_data_chunk(sid, {5.1}, std::vector<std::string>{footer}, 1);

	// write multiple samples
	std::vector<double> ts{5.2, 0, 0, 5.5};
	std::vector<int16_t> data{12, 22, 32, 13, 23, 33, 14, 24, 34, 15, 25, 35};
	std::vector<std::string> data_str{"Hello", "World", "from", "LSL"};
	w.write_data_chunk(0, ts, data, 3);
	w.write_data_chunk(sid, ts, data_str, 1);

	// write data from nested vectors
	ts = std::vector<double>{5.6,0,0,0};
	std::vector<std::vector<int16_t>> data2{{12, 22, 32}, {13, 23, 33}, {14, 24, 34}, {15, 25, 35}};
	std::vector<std::vector<std::string>> data2_str{{"Hello"}, {"World"}, {"from"}, {"LSL"}};
	w.write_data_chunk_nested(0, ts, data2);
	w.write_data_chunk_nested(sid, ts, data2_str);

	w.write_boundary_chunk();
	w.write_stream_offset(0, 6, -.1);
	w.write_stream_offset(sid, 5, -.2);

	w.write_stream_footer(0, footer);
	w.write_stream_footer(sid, footer);
}
