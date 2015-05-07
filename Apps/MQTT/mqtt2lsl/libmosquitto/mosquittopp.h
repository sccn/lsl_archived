/*
Copyright (c) 2010-2012 Roger Light <roger@atchoo.org>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. Neither the name of mosquitto nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _MOSQUITTOPP_H_
#define _MOSQUITTOPP_H_

#ifdef _WIN32
#	ifdef mosquittopp_EXPORTS
#		define mosqpp_EXPORT  __declspec(dllexport)
#	else
#		define mosqpp_EXPORT  __declspec(dllimport)
#	endif
#else
#	define mosqpp_EXPORT
#endif

#include <cstdlib>
#include <time.h>
#include <mosquitto.h>

namespace mosqpp {

mosqpp_EXPORT const char *strerror(int mosq_errno);
mosqpp_EXPORT const char *connack_string(int connack_code);
mosqpp_EXPORT int sub_topic_tokenise(const char *subtopic, char ***topics, int *count);
mosqpp_EXPORT int sub_topic_tokens_free(char ***topics, int count);
mosqpp_EXPORT int lib_version(int *major, int *minor, int *revision);
mosqpp_EXPORT int lib_init();
mosqpp_EXPORT int lib_cleanup();
mosqpp_EXPORT int topic_matches_sub(const char *sub, const char *topic, bool *result);

/*
 * Class: mosquittopp
 *
 * A mosquitto client class. This is a C++ wrapper class for the mosquitto C
 * library. Please see mosquitto.h for details of the functions.
 */
class mosqpp_EXPORT mosquittopp {
	private:
		struct mosquitto *m_mosq;
	public:
		mosquittopp(const char *id=NULL, bool clean_session=true);
		~mosquittopp();

		int reinitialise(const char *id, bool clean_session);
		int socket();
		int will_set(const char *topic, int payloadlen=0, const void *payload=NULL, int qos=0, bool retain=false);
		int will_clear();
		int username_pw_set(const char *username, const char *password=NULL);
		int connect(const char *host, int port=1883, int keepalive=60);
		int connect_async(const char *host, int port=1883, int keepalive=60);
		int reconnect();
		int disconnect();
		int publish(int *mid, const char *topic, int payloadlen=0, const void *payload=NULL, int qos=0, bool retain=false);
		int subscribe(int *mid, const char *sub, int qos=0);
		int unsubscribe(int *mid, const char *sub);
		void message_retry_set(unsigned int message_retry);
		void user_data_set(void *userdata);
		int tls_set(const char *cafile, const char *capath=NULL, const char *certfile=NULL, const char *keyfile=NULL, int (*pw_callback)(char *buf, int size, int rwflag, void *userdata)=NULL);
		int tls_opts_set(int cert_reqs, const char *tls_version=NULL, const char *ciphers=NULL);
		int tls_psk_set(const char *psk, const char *identity, const char *ciphers=NULL);

		int loop(int timeout=-1, int max_packets=1);
		int loop_misc();
		int loop_read(int max_packets=1);
		int loop_write(int max_packets=1);
		int loop_forever(int timeout=-1, int max_packets=1);
		int loop_start();
		int loop_stop(bool force=false);
		bool want_write();
		
		virtual void on_connect(int rc) {return;};
		virtual void on_disconnect(int rc) {return;};
		virtual void on_publish(int mid) {return;};
		virtual void on_message(const struct mosquitto_message *message) {return;};
		virtual void on_subscribe(int mid, int qos_count, const int *granted_qos) {return;};
		virtual void on_unsubscribe(int mid) {return;};
		virtual void on_log(int level, const char *str) {return;};
		virtual void on_error() {return;};
};

}
#endif
