#include <limits>
#include <cstdint>

// Endianness and float format test case
// compilation fails if floats are weird,
// returns endianness at runtime

static_assert(std::numeric_limits<float>::is_iec559,
              "Non-IEC559/IEEE754-floats need EXOTIC_ARCH_SUPPORT defined");
static_assert(std::numeric_limits<float>::has_denorm, "denormalized floats not supported");
static_assert(sizeof(float) == 4, "Unexpected float size!");
static_assert(sizeof(double) == 8, "Unexpected double size!");

int main(){
	const uint32_t test_val = 0x01020304;
	// return 0 if big endian, 1 if little endian
	return *reinterpret_cast<const char*>(&test_val) == 0x04;
}
