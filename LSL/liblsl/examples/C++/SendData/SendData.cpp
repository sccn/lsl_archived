#include "../../../include/lsl_cpp.h"
#include <iostream>
#include <time.h>
#include <stdlib.h>

using namespace std;


#include <stdio.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")
#include <string>


string get_adapter(PIP_ADAPTER_ADDRESSES aa)
{
	char buf[BUFSIZ];
	memset(buf, 0, BUFSIZ);
	WideCharToMultiByte(CP_ACP, 0, aa->FriendlyName, wcslen(aa->FriendlyName), buf, BUFSIZ, NULL, NULL);
	//printf("adapter_name:%s\n", buf);
	string adapter_name = string(buf);
	return adapter_name;
}

string get_address(PIP_ADAPTER_UNICAST_ADDRESS ua)
{
	char buf[BUFSIZ];
	memset(buf, 0, BUFSIZ);
	getnameinfo(ua->Address.lpSockaddr, ua->Address.iSockaddrLength, buf, sizeof(buf), NULL, 0,NI_NUMERICHOST);
	//printf("%s\n", buf);	
	string addr_name = string(buf);
	return addr_name;
}

string get_IP_fam(PIP_ADAPTER_UNICAST_ADDRESS ua)
{
	char buf[BUFSIZ];
	int family = ua->Address.lpSockaddr->sa_family;
	//printf("\t%s ",  family == AF_INET ? "IPv4":"IPv6");
	return family == AF_INET ? "IPv4":"IPv6";
}

std::vector<string> print_ipaddress()
{
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
	
	std::vector<string> myVector;
	for (aa = adapter_addresses; aa != NULL; aa = aa->Next) {
		string adapter = get_adapter(aa);		
		for (ua = aa->FirstUnicastAddress; ua != NULL; ua = ua->Next) {
			string address = get_address(ua);
			string family = get_IP_fam(ua);
			//std::cout << "\n\tRETURNS: " + adapter + " " + addr + " " + family<< std::endl;
			myVector.push_back(address);
		}
	}

	free(adapter_addresses);
	return myVector;
}

int main(int argc, char *argv[])
{
	WSAData d;
	if (WSAStartup(MAKEWORD(2, 2), &d) != 0) {
		return -1;
	}

	std::vector<string> myVector = print_ipaddress();
	std::cout << "myVector contains:";
	unsigned int i;
	for (i=0; i<sizeof(myVector)-1; i++)
		std::cout << "\n" + myVector.at(i)<< std::endl;			
	system("PAUSE");
	//std::cin.ignore();
	WSACleanup();
	
	return 0;
}

/**
* This example program offers an 8-channel stream, float-formatted, that resembles EEG data.
* The example demonstrates also how per-channel meta-data can be specified using the .desc() field of the stream information object.
*
* Note that the timer used in the send loop of this program is not particularly accurate.
*/


/*
int main() {
  std::cout << "main" << std::endl;	
  char buf[10000];  
  FILE *p = _popen("netstat -s -p TCP", "r");
  std::string s;
  for (size_t count; (count = fread(buf, 1, sizeof(buf), p));)
	s += string(buf, buf + count);
  std::cout<<s<<std::endl;    
  _pclose(p);  
  std::cin.ignore();
}
*/



/*
const char *channels[] = {"C3","C4","Cz","FPz","POz","CPz","O1","O2"};

void do_32(){
   //define something for Windows (32-bit)
	std::cout << "do_32" << std::endl;	
}

void do_64(){
   //define something for Windows (32-bit)
	std::cout << "do_64" << std::endl;	
}

void do_86(){
   //define something for Windows (32-bit)
	std::cout << "do_86" << std::endl;	
}

int main(int argc, char* argv[]) { 

	#ifdef _WIN32
	   //define something for Windows (32-bit and 64-bit, this part is common)
	   #ifdef _WIN64
		  //define something for Windows (64-bit only)
			do_64();
	   #elif _WIN86
			do_86();
	   #else
		  //define something for Windows (32-bit only)
			do_32();
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

	string name, type;
	if (argc != 3) {
		cout << "This opens a stream under some user-defined name and with a user-defined content type." << endl;
		cout << "Please enter the stream name and the stream type (e.g. \"BioSemi EEG\" (without the quotes)):" << endl;
		cin >> name >> type;
	} else {
		name = argv[1];
		type = argv[2];
	}

	try {

		// make a new stream_info (100 Hz)
		lsl::stream_info info(name,type,8,100,lsl::cf_float32,string(name)+=type);

		// add some description fields
		info.desc().append_child_value("manufacturer", "BioSemi");
		lsl::xml_element chns = info.desc().append_child("channels");
		for (int k=0;k<8;k++)
			chns.append_child("channel")
			.append_child_value("label",channels[k])
			.append_child_value("unit","microvolts")
			.append_child_value("type","EEG");

		// make a new outlet
		lsl::stream_outlet outlet(info);

		// send data forever
		cout << "Now sending data... " << endl;
		double starttime = ((double)clock())/CLOCKS_PER_SEC;
		for(unsigned t=0;;t++) {
			
			// wait a bit and create random data
			while (((double)clock())/CLOCKS_PER_SEC < starttime + t*0.01);
			float sample[8];
			for (int c=0;c<8;c++)
				sample[c] = (float)((rand()%1500)/500.0-1.5);

			// send the sample
			outlet.push_sample(sample);
		}

	} catch(std::exception &e) {
		cerr << "Got an exception: " << e.what() << endl;
	}
	cout << "Press any key to exit. " << endl; cin.get();
	return 0;
}
*/