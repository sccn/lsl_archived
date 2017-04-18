#include "socket_utils.h"
#include "endian/conversion.hpp"
#include "common.h"


// === Implementation of the socket utils ===

using namespace lsl;

/// Measure the endian conversion performance of this machine.
double lsl::measure_endian_performance() {
	const double measure_duration = 0.01;
	double data=12335.5, t_end=lsl_clock() + measure_duration;
	double k;
	for (k=0; ((int)k & 0xFF) != 0 || lsl_clock()<t_end; k++)
		lslboost::endian::reverse_value(data);
	return k;
}

/**
/ Enumerate network adapters 
*/
std::string get_adapter(PIP_ADAPTER_ADDRESSES aa)
{
	char buf[BUFSIZ];
	memset(buf, 0, BUFSIZ);
	WideCharToMultiByte(CP_ACP, 0, aa->FriendlyName, wcslen(aa->FriendlyName), buf, BUFSIZ, NULL, NULL);
	//printf("adapter_name:%s\n", buf);
	std::string adapter_name = std::string(buf);
	return adapter_name;
}

std::string get_address(PIP_ADAPTER_UNICAST_ADDRESS ua)
{
	char buf[BUFSIZ];
	memset(buf, 0, BUFSIZ);
	getnameinfo(ua->Address.lpSockaddr, ua->Address.iSockaddrLength, buf, sizeof(buf), NULL, 0,NI_NUMERICHOST);
	//printf("%s\n", buf);	
	std::string addr_name = std::string(buf);
	return addr_name;
}

std::string get_IP_fam(PIP_ADAPTER_UNICAST_ADDRESS ua)
{
	char buf[BUFSIZ];
	int family = ua->Address.lpSockaddr->sa_family;
	//printf("\t%s\n ",  family == AF_INET ? "IPv4":"IPv6");
	return family == AF_INET ? "IPv4":"IPv6";
}
  

std::vector<std::string> net_adatpers_win32() {
	//define something for Windows (32-bit only)
	std::vector<std::string> myVector;
	WSAData d;
	if (WSAStartup(MAKEWORD(2, 2), &d) != 0) {
		return myVector;
	}
	DWORD rv, size;
	PIP_ADAPTER_ADDRESSES adapter_addresses, aa;
	PIP_ADAPTER_UNICAST_ADDRESS ua;

	rv = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, NULL, &size);
	if (rv != ERROR_BUFFER_OVERFLOW) {
		fprintf(stderr, "GetAdaptersAddresses() failed...");
		//return false;
	}
	adapter_addresses = (PIP_ADAPTER_ADDRESSES)malloc(size);

	rv = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, adapter_addresses, &size);
	if (rv != ERROR_SUCCESS) {
		fprintf(stderr, "GetAdaptersAddresses() failed...");
		free(adapter_addresses);
		//return false;
	}	
	
	for (aa = adapter_addresses; aa != NULL; aa = aa->Next) {
		std::string adapter = get_adapter(aa);		
		for (ua = aa->FirstUnicastAddress; ua != NULL; ua = ua->Next) {
			std::string address = get_address(ua);
			std::string family = get_IP_fam(ua);
			std::cout <<family + "\t" + adapter + "\n" + address + "\n"<< std::endl;
			myVector.push_back(address);
		}
	}
	free(adapter_addresses);
	WSACleanup();
	return myVector;
}

std::vector<std::string> enum_adapters() {
	std::vector<std::string> myVector;
	#ifdef _WIN32
	   //define something for Windows (32-bit and 64-bit, this part is common)
	   #ifdef _WIN64
		  //define something for Windows (64-bit only)
	   #else
		  //define something for Windows (32-bit only)
			return net_adatpers_win32();
	   #endif
	#elif __APPLE__
		#include "TargetConditionals.h"
		#if TARGET_IPHONE_SIMULATOR
			 // iOS Simulator
		#elif TARGET_OS_IPHONE
			// iOS device
		#elif TARGET_OS_MAC
			// Other kinds of Mac OS
		#else
		#   error "Unknown Apple platform"
		#endif
	#elif __linux__
		// linux
	#elif __unix__ // all unices not caught above
		// Unix
	#elif defined(_POSIX_VERSION)
		// POSIX
	#else
		void do_that();	
	#endif

}
