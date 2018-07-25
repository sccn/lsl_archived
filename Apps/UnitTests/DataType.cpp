#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <lsl_cpp.h>


// Confirm that cf_int8 is preserved during transmission
TEST_CASE("cf_int8", "[DataType]") {
	std::cout << "cf_int8, [DataType]" << std::endl;
	const int numBounces = 8;
	double timestamps[numBounces][2];

	lsl::stream_outlet outlet(lsl::stream_info("Sender", "Bounce", 1, lsl::IRREGULAR_RATE, lsl::cf_int8, "streamid"));
	auto found_stream_info = lsl::resolve_stream("name", "Sender");

	REQUIRE(!found_stream_info.empty());
	lsl::stream_info si = found_stream_info[0];

	lsl::stream_inlet inlet(si);

	inlet.open_stream(2);
	outlet.wait_for_consumers(2);


	char sent_data = 0x1;
	for (int32_t counter = 0; counter < numBounces; counter++) {
		char received_data;
		timestamps[counter][0] = lsl::local_clock();
		outlet.push_sample(&sent_data, timestamps[counter][0], true);

		REQUIRE(inlet.pull_sample(&received_data, 1, .5) != 0.0);

		timestamps[counter][1] = lsl::local_clock();
		REQUIRE(received_data == sent_data);
		sent_data = sent_data << 1;

	}


}


// Confirm that cf_int16 is preserved during transmission
TEST_CASE("cf_int16", "[DataType]") {
	std::cout << "cf_int16, [DataType]" << std::endl;
	const int numBounces = 16;
	double timestamps[numBounces][2];

	lsl::stream_outlet outlet(lsl::stream_info("Sender", "Bounce", 1, lsl::IRREGULAR_RATE, lsl::cf_int16, "streamid"));
	auto found_stream_info = lsl::resolve_stream("name", "Sender");

	REQUIRE(!found_stream_info.empty());
	lsl::stream_info si = found_stream_info[0];

	lsl::stream_inlet inlet(si);

	inlet.open_stream(2);
	outlet.wait_for_consumers(2);

	
	int16_t sent_data = 0x1;
	for (int32_t counter = 0; counter < numBounces; counter++) {
		int16_t received_data;
		timestamps[counter][0] = lsl::local_clock();
		outlet.push_sample(&sent_data, timestamps[counter][0], true);

		REQUIRE(inlet.pull_sample(&received_data, 1, .5) != 0.0);

		timestamps[counter][1] = lsl::local_clock();
		REQUIRE(received_data == sent_data);
		sent_data = sent_data << 1;

	}


}


//Confirm that cf_int32 is preserved during transmission
TEST_CASE("cf_int32", "[DataType]") {
	std::cout << "cf_int32, [DataType]" << std::endl;
	const int numBounces = 32;
	double timestamps[numBounces][2];

	lsl::stream_outlet outlet(lsl::stream_info("Sender", "Bounce", 1, lsl::IRREGULAR_RATE, lsl::cf_int32, "streamid"));
	auto found_stream_info = lsl::resolve_stream("name", "Sender");
	REQUIRE(!found_stream_info.empty());

	lsl::stream_info si = found_stream_info[0];

	lsl::stream_inlet inlet(si);

	inlet.open_stream(2);
	outlet.wait_for_consumers(2);

	int32_t sent_data = 0x1;
	for (int32_t counter = 0; counter < numBounces; counter++) {
		int32_t received_data;
		timestamps[counter][0] = lsl::local_clock();
		outlet.push_sample(&sent_data, timestamps[counter][0], true);

		REQUIRE(inlet.pull_sample(&received_data, 1, .5) != 0.0);

		timestamps[counter][1] = lsl::local_clock();
		REQUIRE(received_data == sent_data);
		sent_data = sent_data << 1;

	}
}


//Confirm that cf_int64 is preserved during transmission
TEST_CASE("cf_int64", "[DataTye]") {
	std::cout << "cf_int64, [DataType]" << std::endl;
	const int numBounces = 64;
	double timestamps[numBounces][2];

	lsl::stream_outlet outlet(lsl::stream_info("Sender", "Bounce", 1, lsl::IRREGULAR_RATE, lsl::cf_int64, "streamid"));
	auto found_stream_info = lsl::resolve_stream("name", "Sender");
	REQUIRE(!found_stream_info.empty());
	lsl::stream_info si = found_stream_info[0];

	lsl::stream_inlet inlet(si);

	inlet.open_stream(2);
	outlet.wait_for_consumers(2);

	int64_t sent_data = 0x1LL;
	for (int32_t counter = 0; counter < numBounces; counter++) {
		int64_t received_data;
		timestamps[counter][0] = lsl::local_clock();
		outlet.push_sample(&sent_data, timestamps[counter][0], true);

		REQUIRE(inlet.pull_sample(&received_data, 1, .5) != 0.0);

		timestamps[counter][1] = lsl::local_clock();
		REQUIRE(received_data == sent_data);
		sent_data = sent_data << 1LL;

	}
}

//Confirm that cf_int8 multichannel is preserved during transmission
TEST_CASE("cf_int8 multichannel", "[DataTye]") {
	std::cout << "cf_int8 multichannel, [DataType]" << std::endl;
	const int numChannels = 8;

	lsl::stream_outlet outlet(lsl::stream_info("Sender", "Bounce", numChannels, lsl::IRREGULAR_RATE, lsl::cf_int8, "streamid"));
	auto found_stream_info = lsl::resolve_stream("name", "Sender");
	REQUIRE(!found_stream_info.empty());
	lsl::stream_info si = found_stream_info[0];

	lsl::stream_inlet inlet(si);

	inlet.open_stream(2);
	outlet.wait_for_consumers(2);

	std::vector<char> sent_data(numChannels);
	int8_t data = 0x1LL;
	for (int32_t counter = 0; counter < numChannels; counter++) {
		sent_data[counter] = data;
		data = data << 1LL;
	}

	std::vector<char> received_data(numChannels);

	outlet.push_sample(sent_data, lsl::lsl_local_clock(), true);

	REQUIRE(inlet.pull_sample(received_data, 0.5) != 0.0);

	REQUIRE(received_data == sent_data);

}


//Confirm that cf_int16 multichannel is preserved during transmission
TEST_CASE("cf_int16 multichannel", "[DataTye]") {
	std::cout << "cf_int16 multichannel, [DataType]" << std::endl;
	const int numChannels = 16;

	lsl::stream_outlet outlet(lsl::stream_info("Sender", "Bounce", numChannels, lsl::IRREGULAR_RATE, lsl::cf_int16, "streamid"));
	auto found_stream_info = lsl::resolve_stream("name", "Sender");
	REQUIRE(!found_stream_info.empty());
	lsl::stream_info si = found_stream_info[0];

	lsl::stream_inlet inlet(si);

	inlet.open_stream(2);
	outlet.wait_for_consumers(2);

	std::vector<int16_t> sent_data(numChannels);
	int16_t data = 0x1LL;
	for (int32_t counter = 0; counter < numChannels; counter++) {
		sent_data[counter] = data;
		data = data << 1LL;
	}

	std::vector<int16_t> received_data(numChannels);

	outlet.push_sample(sent_data, lsl::lsl_local_clock(), true);

	REQUIRE(inlet.pull_sample(received_data, 0.5) != 0.0);

	REQUIRE(received_data == sent_data);

}


//Confirm that cf_int32 multichannel is preserved during transmission
TEST_CASE("cf_int32 multichannel", "[DataTye]") {
	std::cout << "cf_int32 multichannel, [DataType]" << std::endl;
	const int numChannels = 32;

	lsl::stream_outlet outlet(lsl::stream_info("Sender", "Bounce", numChannels, lsl::IRREGULAR_RATE, lsl::cf_int32, "streamid"));
	auto found_stream_info = lsl::resolve_stream("name", "Sender");
	REQUIRE(!found_stream_info.empty());
	lsl::stream_info si = found_stream_info[0];

	lsl::stream_inlet inlet(si);

	inlet.open_stream(2);
	outlet.wait_for_consumers(2);

	std::vector<int32_t> sent_data(numChannels);
	int32_t data = 0x1LL;
	for (int32_t counter = 0; counter < numChannels; counter++) {
		sent_data[counter] = data;
		data = data << 1LL;
	}

	std::vector<int32_t> received_data(numChannels);

	outlet.push_sample(sent_data, lsl::lsl_local_clock(), true);

	REQUIRE(inlet.pull_sample(received_data, 0.5) != 0.0);

	REQUIRE(received_data == sent_data);

}



//Confirm that cf_int64 multichannel is preserved during transmission
TEST_CASE("cf_int64 multichannel", "[DataTye]") {
	std::cout << "cf_int64 multichannel, [DataType]" << std::endl;
	const int numChannels = 64;
	
	lsl::stream_outlet outlet(lsl::stream_info("Sender", "Bounce", numChannels, lsl::IRREGULAR_RATE, lsl::cf_int64, "streamid"));
	auto found_stream_info = lsl::resolve_stream("name", "Sender");
	REQUIRE(!found_stream_info.empty());
	lsl::stream_info si = found_stream_info[0];

	lsl::stream_inlet inlet(si);

	inlet.open_stream(2);
	outlet.wait_for_consumers(2);

	std::vector<int64_t> sent_data(numChannels);
	int64_t data = 0x1LL;
	for (int32_t counter = 0; counter < numChannels; counter++) {
		sent_data[counter] = data;
		data = data << 1LL;
	}
	
	std::vector<int64_t> received_data(numChannels);

	outlet.push_sample(sent_data, lsl::lsl_local_clock(), true);

	REQUIRE(inlet.pull_sample(received_data, 0.5) != 0.0);

	REQUIRE(received_data == sent_data);

}



