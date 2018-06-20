// Most target architectures are little-endian with IEC559/IEEE754 floats
// This header provides conversion functions for all others and
// functions to write binary data to streambufs

#ifndef CONVERSIONS_H_
#define CONVERSIONS_H_

#include <cstdint>
#include <streambuf>
#include <vector>
#include <limits>

#ifdef EXOTIC_ARCH_SUPPORT
#include <boost/version.hpp>

// support for endianness and binary floating-point storage
// this import scheme is part of the portable_archive code by
// christian.pfligersdorffer@eos.info (under boost license)
#include <boost/spirit/home/support/detail/math/fpclassify.hpp>
// namespace alias fp_classify
namespace fp = boost::spirit::math;

#if BOOST_VERSION >= 105800
#include <boost/endian/conversion.hpp>
using boost::endian::native_to_little_inplace;
#else
#error "BOOST_VERSION >= 1.58 required"
#endif

// === writer functions ===
// write an integer value in little endian
// derived from portable archive code by christian.pfligersdorffer@eos.info (under boost license)
template <typename T>
typename std::enable_if<std::is_integral<T>::value>::type write_little_endian(std::streambuf* dst, T t) {
	native_to_little_inplace(t);
	dst->sputn((char*)(&t), sizeof(t));
}

// write a floating-point value in little endian
// derived from portable archive code by christian.pfligersdorffer@eos.info (under boost license)
template <typename T>
typename std::enable_if<std::is_floating_point<T>::value>::type
write_little_endian(std::streambuf* dst, T t) {
	//Get a type big enough to hold
	using traits = typename fp::detail::fp_traits<T>::type;
	static_assert(sizeof(typename traits::bits) == sizeof(T), "floating point type can't be represented accurately");

	typename traits::bits bits;
	// remap to bit representation
	switch (fp::fpclassify(t)) {
	case FP_NAN: bits = traits::exponent | traits::mantissa; break;
	case FP_INFINITE: bits = traits::exponent | (t < 0) * traits::sign; break;
	case FP_SUBNORMAL: break;
	case FP_ZERO: // note that floats can be ±0.0
	case FP_NORMAL: traits::get_bits(t, bits); break;
	default: bits = 0; break;
	}
	write_little_endian(dst, bits);
}

// store a sample's values to a stream (numeric version) */
template <class T>
inline void write_sample_values(std::streambuf* dst, const std::vector<T>& sample) {
	// [Value1] .. [ValueN] */
	for (const T s : sample) write_little_endian(dst, s);
}

#else

static_assert(std::numeric_limits<float>::is_iec559,
              "Non-IEC559/IEEE754-floats need EXOTIC_ARCH_SUPPORT defined");
static_assert(std::numeric_limits<float>::has_denorm, "denormalized floats not supported");
static_assert(sizeof(float) == 4, "Unexpected float size!");
static_assert(sizeof(double) == 8, "Unexpected double size!");

template <typename T>
typename std::enable_if<sizeof(T) == 1>::type write_little_endian(std::streambuf* dst, T t) {
	dst->sputc(t);
}

template <typename T>
typename std::enable_if<sizeof(T) >= 2>::type write_little_endian(std::streambuf* dst, T t) {
	dst->sputn(reinterpret_cast<const char*>(&t), sizeof(T));
}

template <class T>
inline void write_sample_values(std::streambuf* dst, const std::vector<T>& sample) {
	// [Value1] .. [ValueN] */
	dst->sputn(reinterpret_cast<const char*>(sample.data()), sample.size() * sizeof(T));
}
#endif

// write a variable-length integer (int8, int32, or int64)
inline void write_varlen_int(std::streambuf* dst, uint64_t val) {
	if (val < 256) {
		dst->sputc(1);
		dst->sputc(static_cast<uint8_t>(val));
	} else if (val <= 4294967295) {
		dst->sputc(4);
		write_little_endian(dst, static_cast<uint32_t>(val));
	} else {
		dst->sputc(8);
		write_little_endian(dst, static_cast<uint64_t>(val));
	}
}

// store a sample's values to a stream (string version)
template <>
inline void write_sample_values(std::streambuf* dst, const std::vector<std::string>& sample) {
	// [Value1] .. [ValueN] */
	for (const std::string& s : sample) {
		// [NumLengthBytes], [Length] (as varlen int)
		write_varlen_int(dst, s.size());
		// [StringContent] */
		dst->sputn(s.data(), s.size());
	}
}

#endif
