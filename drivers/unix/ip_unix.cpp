/*************************************************************************/
/*  ip_unix.cpp                                                          */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                    http://www.godotengine.org                         */
/*************************************************************************/
/* Copyright (c) 2007-2016 Juan Linietsky, Ariel Manzur.                 */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/
#include "ip_unix.h"

#if defined(UNIX_ENABLED) || defined(WINDOWS_ENABLED)

#include <string.h>

#ifdef WINDOWS_ENABLED
  // Workaround mingw missing flags!
  #ifndef AI_ADDRCONFIG
    #define AI_ADDRCONFIG 0x00000400
  #endif
  #ifndef AI_V4MAPPED
    #define AI_V4MAPPED 0x00000800
  #endif
 #ifdef UWP_ENABLED
  #include <ws2tcpip.h>
  #include <winsock2.h>
  #include <windows.h>
  #include <stdio.h>
 #else
  #define WINVER 0x0600
  #include <ws2tcpip.h>
  #include <winsock2.h>
  #include <windows.h>
  #include <stdio.h>
  #include <iphlpapi.h>
 #endif
#else
 #include <netdb.h>
 #ifdef ANDROID_ENABLED
  #include "platform/android/ifaddrs_android.h"
 #else
  #ifdef __FreeBSD__
   #include <sys/types.h>
  #endif
  #include <ifaddrs.h>
 #endif
 #include <arpa/inet.h>
 #include <sys/socket.h>
 #ifdef __FreeBSD__
  #include <netinet/in.h>
 #endif
#endif

static IP_Address _sockaddr2ip(struct sockaddr* p_addr) {

	IP_Address ip;
	if (p_addr->sa_family == AF_INET) {
		struct sockaddr_in* addr = (struct sockaddr_in*)p_addr;
		ip.field32[0] = *((unsigned long*)&addr->sin_addr);
		ip.type = IP_Address::TYPE_IPV4;
	} else {
		struct sockaddr_in6* addr6 = (struct sockaddr_in6*)p_addr;
		for (int i=0; i<16; i++)
			ip.field8[i] = addr6->sin6_addr.s6_addr[i];
		ip.type = IP_Address::TYPE_IPV6;
	};

	return ip;
};

IP_Address IP_Unix::_resolve_hostname(const String& p_hostname, IP_Address::AddrType p_type) {

	struct addrinfo hints;
	struct addrinfo* result;

	memset(&hints, 0, sizeof(struct addrinfo));
	if (p_type == IP_Address::TYPE_IPV4) {
		hints.ai_family = AF_INET;
	} else if (p_type == IP_Address::TYPE_IPV6) {
		hints.ai_family = AF_INET6;
		hints.ai_flags = 0;
	} else {
		hints.ai_family = AF_UNSPEC;
		hints.ai_flags = (AI_V4MAPPED | AI_ADDRCONFIG);
	};

	int s = getaddrinfo(p_hostname.utf8().get_data(), NULL, &hints, &result);
	if (s != 0) {
		ERR_PRINT("getaddrinfo failed!");
		return IP_Address();
	};

	if (result == NULL || result->ai_addr == NULL) {
		ERR_PRINT("Invalid response from getaddrinfo");
		return IP_Address();
	};

	IP_Address ip = _sockaddr2ip(result->ai_addr);

	freeaddrinfo(result);

	return ip;

}

#if defined(WINDOWS_ENABLED)

#if defined(UWP_ENABLED)

void IP_Unix::get_local_addresses(List<IP_Address> *r_addresses) const {

	using namespace Windows::Networking;
	using namespace Windows::Networking::Connectivity;

	auto hostnames = NetworkInformation::GetHostNames();

	for (int i = 0; i < hostnames->Size; i++) {

		if (hostnames->GetAt(i)->Type == HostNameType::Ipv4 || hostnames->GetAt(i)->Type == HostNameType::Ipv6 && hostnames->GetAt(i)->IPInformation != nullptr) {

			r_addresses->push_back(IP_Address(String(hostnames->GetAt(i)->CanonicalName->Data())));
		}
	}

};
#else

void IP_Unix::get_local_addresses(List<IP_Address> *r_addresses) const {

	ULONG buf_size = 1024;
	IP_ADAPTER_ADDRESSES* addrs;

	while (true) {

		addrs = (IP_ADAPTER_ADDRESSES*)memalloc(buf_size);
		int err = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_SKIP_ANYCAST |
									   GAA_FLAG_SKIP_MULTICAST |
									   GAA_FLAG_SKIP_DNS_SERVER |
									   GAA_FLAG_SKIP_FRIENDLY_NAME,
									 NULL, addrs, &buf_size);
		if (err == NO_ERROR) {
			break;
		};
		memfree(addrs);
		if (err == ERROR_BUFFER_OVERFLOW) {
			continue; // will go back and alloc the right size
		};

		ERR_EXPLAIN("Call to GetAdaptersAddresses failed with error " + itos(err));
		ERR_FAIL();
		return;
	};


	IP_ADAPTER_ADDRESSES* adapter = addrs;

	while (adapter != NULL) {

		IP_ADAPTER_UNICAST_ADDRESS* address = adapter->FirstUnicastAddress;
		while (address != NULL) {

			IP_Address ip;

			if (address->Address.lpSockaddr->sa_family == AF_INET) {

				SOCKADDR_IN* ipv4 = reinterpret_cast<SOCKADDR_IN*>(address->Address.lpSockaddr);

				ip.field32[0] = *((unsigned long*)&ipv4->sin_addr);
				ip.type = IP_Address::TYPE_IPV4;
			} else { // ipv6

				SOCKADDR_IN6* ipv6 = reinterpret_cast<SOCKADDR_IN6*>(address->Address.lpSockaddr);
				for (int i=0; i<16; i++) {
					ip.field8[i] = ipv6->sin6_addr.s6_addr[i];
				};
				ip.type = IP_Address::TYPE_IPV6;
			};


			r_addresses->push_back(ip);

			address = address->Next;
		};
		adapter = adapter->Next;
	};

	memfree(addrs);
};

#endif

#else

void IP_Unix::get_local_addresses(List<IP_Address> *r_addresses) const {

	struct ifaddrs * ifAddrStruct=NULL;
	struct ifaddrs * ifa=NULL;

	getifaddrs(&ifAddrStruct);

	for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
		if (!ifa->ifa_addr)
			continue;

		IP_Address ip = _sockaddr2ip(ifa->ifa_addr);
		r_addresses->push_back(ip);
	}

	if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);

}
#endif

void IP_Unix::make_default() {

	_create=_create_unix;
}

IP* IP_Unix::_create_unix() {

	return memnew( IP_Unix );
}

IP_Unix::IP_Unix() {
}

#endif
