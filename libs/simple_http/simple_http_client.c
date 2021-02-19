/**
 * \file simple_http_client.c
 * \author Mario Rubio (mario@mrrb.eu)
 * \brief Based of espressif esp8266-nonos-sample-code
 * \version 0.1
 * \date 2021-02-17
 */

#include <osapi.h>
#include <mem.h>
#include <espconn.h>
#include <stdlib.h>
#include <user_interface.h>
#include <inttypes.h>

#include "common.h"
#include "simple_http/simple_http.h"


static void ICACHE_FLASH_ATTR
request_disconnect_callback(void* arg) {
	struct espconn* p_conn = (struct espconn*)arg;
    simple_http_client_request_info_t* p_info = (simple_http_client_request_info_t*)p_conn->reverse;

    int http_status;

    char* proto_version = HTTP_PROTO_VERSION" ";
    char* response_data;
    char* chunked_response_data;

    size_t response_data_len;

    system_soft_wdt_feed();
	if(p_conn == NULL) {
		return;
	}

	if(p_info != NULL) {
		http_status = -1;

		if (p_info->response_buffer == NULL) {

		} else if (p_info->response_buffer[0] != '\0') {
			// TODO: Handle partial response using the Content-Length header.
            // TODO: Add chunked_decode

			if (os_strncmp(p_info->response_buffer, proto_version, strlen(proto_version)) != 0) {

			} else {
				http_status = atoi(p_info->response_buffer + strlen(proto_version));
				response_data = (char*)os_strstr(p_info->response_buffer, "\r\n\r\n") + 4;

				// if (os_strstr(p_info->response_buffer, "Transfer-Encoding: chunked")) {
				// 	response_data_len = p_info->response_buffer_len - (response_data - p_info->response_buffer);
				// 	chunked_response_data = (char*)os_malloc(sizeof(char) * response_data_len);

                //     if (chunked_response_data != NULL) {
                //         os_memset(chunked_response_data, 0, response_data_len);

                //         // chunked_decode(response_data, chunked_response_data);
                //         // os_memcpy(response_data, chunked_response_data, response_data_len);

                //         os_free(chunked_response_data);
                //     }
				// }
			}
		}

		if (p_info->callback != NULL) {
			p_info->callback(response_data, http_status, p_info->response_buffer);
		}

		os_free(p_info->hostname);
		os_free(p_info->path);
		os_free(p_info->data);
		os_free(p_info->headers);
		os_free(p_info->request_method);
		os_free(p_info->response_buffer);
		os_free(p_info->send_buff);
		os_free(p_info);
	}

	espconn_delete(p_conn);
	if(p_conn->proto.tcp != NULL) {
		os_free(p_conn->proto.tcp);
	}
	os_free(p_conn);
}

static void ICACHE_FLASH_ATTR
sent_callback(void* arg) {
    struct espconn* p_conn = (struct espconn*)arg;
    simple_http_client_request_info_t* p_info = (simple_http_client_request_info_t*)p_conn->reverse;

    system_soft_wdt_feed();
    if (p_info->data == NULL) {
        /* No need to send more data */
	} else {
        /* Now its time to send the request data */
		if (p_info->secure)
			espconn_secure_send(p_conn, (uint8_t*)p_info->data, strlen(p_info->data));
		else
			espconn_send(p_conn, (uint8_t*)p_info->data, strlen(p_info->data));

		// os_free(p_info->data);
		// p_info->data = NULL;
	}
}

static void ICACHE_FLASH_ATTR
receive_callback(void* arg, char* buf, unsigned short len) {
	struct espconn* p_conn = (struct espconn*)arg;
    simple_http_client_request_info_t* p_info = (simple_http_client_request_info_t*)p_conn->reverse;

    const size_t new_len = p_info->response_buffer_len + len;
    char* new_buffer;

    system_soft_wdt_feed();
	if (p_info->response_buffer == NULL) {
		return;
	}

    new_buffer = (char*)os_malloc(sizeof(char) * new_len);
	if (new_len > HTTP_MAX_RESPONSE_SIZE || new_buffer == NULL) {
		p_info->response_buffer[0] = '\0';
		if (p_info->secure)
			espconn_secure_disconnect(p_conn);
		else
			espconn_disconnect(p_conn);
		return;
	}

	os_memcpy(new_buffer, p_info->response_buffer, p_info->response_buffer_len);
	os_memcpy(new_buffer + p_info->response_buffer_len - 1, buf, len);
	new_buffer[new_len - 1] = '\0';

	os_free(p_info->response_buffer);
	p_info->response_buffer     = new_buffer;
	p_info->response_buffer_len = new_len;
}

static void ICACHE_FLASH_ATTR
request_connect_callback(void* arg) {
    struct espconn* p_conn = (struct espconn*)arg;
    simple_http_client_request_info_t* p_info = (simple_http_client_request_info_t*)p_conn->reverse;

	char* send_buff;
    size_t buff_len, total_len;

    system_soft_wdt_feed();
	espconn_regist_sentcb(p_conn, sent_callback);
    espconn_regist_recvcb(p_conn, receive_callback);

	char data_headers[28] = "";

	if (p_info->data != NULL) {
		os_sprintf(data_headers, "Content-Length: %d\r\n", strlen(p_info->data));
	}

    buff_len  = 0;
    buff_len += os_strlen(p_info->request_method) + os_strlen(p_info->path) + os_strlen(HTTP_PROTO_VERSION) + 4;
    buff_len += os_strlen(p_info->hostname) + 5 + 9;
    buff_len += 40;
    buff_len += os_strlen(p_info->headers) + os_strlen(data_headers) + 2;

    send_buff = (char*)os_malloc(sizeof(char) * buff_len);
    
	total_len = os_sprintf(send_buff,
                           "%s %s %s\r\n"
                           "Host: %s:%d\r\n"
                           "Connection: close\r\n"
                           "User-Agent: ESP8266\r\n"
                           "%s"
                           "%s"
                           "\r\n",
                           p_info->request_method, p_info->path, HTTP_PROTO_VERSION,
                           p_info->hostname, p_info->port,
                           p_info->headers, data_headers);

    p_info->send_buff = send_buff;


    if (p_info->secure) {
		espconn_secure_send(p_conn, (uint8_t*)send_buff, total_len);
    } else {
		espconn_send(p_conn, (uint8_t*)send_buff, total_len);
	}
}

static void ICACHE_FLASH_ATTR
request_error_callback(void* arg, sint8 errType) {
    system_soft_wdt_feed();
	request_disconnect_callback(arg);
}

static void ICACHE_FLASH_ATTR
request_dns_callback(const char* name, ip_addr_t* p_ip, void* arg) {
    struct espconn* p_conn = (struct espconn*)arg;
    simple_http_client_request_info_t* p_info = (simple_http_client_request_info_t*)p_conn->reverse;

    system_soft_wdt_feed();
    p_conn->type  = ESPCONN_TCP;
    p_conn->state = ESPCONN_NONE;

    p_conn->proto.tcp = (esp_tcp*)os_malloc(sizeof(esp_tcp));
    p_conn->proto.tcp->local_port  = espconn_port();
    p_conn->proto.tcp->remote_port = p_info->port;

    os_memcpy(p_conn->proto.tcp->remote_ip, p_ip, 4);
    os_free(p_ip);

    espconn_regist_connectcb(p_conn, request_connect_callback);
    espconn_regist_disconcb(p_conn, request_disconnect_callback);
    espconn_regist_reconcb(p_conn, request_error_callback);

    if (p_info->secure) {
        espconn_secure_set_size(ESPCONN_CLIENT, 5120);
        espconn_secure_connect(p_conn);
    } else {
        espconn_connect(p_conn);
    }
}

static simple_http_status_t ICACHE_FLASH_ATTR
make_http_request(struct espconn* p_conn) {
    err_t error;
    struct ip_info ipconfig;
    ip_addr_t* p_ip;

    simple_http_client_request_info_t* p_info = (simple_http_client_request_info_t*)p_conn->reverse;

    wifi_get_ip_info(STATION_IF, &ipconfig);
    system_soft_wdt_feed();

    /* Check valid connection */
    if (wifi_station_get_connect_status() != STATION_GOT_IP || ipconfig.ip.addr == 0) {
        return SIMPLE_HTTP_NO_CONNECTION;
    }

    p_ip = (ip_addr_t*)os_malloc(sizeof(ip_addr_t));

    espconn_secure_ca_disable(0x01);
    error = espconn_gethostbyname(p_conn, p_info->hostname, p_ip, request_dns_callback);

    if (error == ESPCONN_INPROGRESS) {
		return SIMPLE_HTTP_REQUEST_SENT;
	} else if (error == ESPCONN_OK) {
		/* Already in the local names table (or hostname was an IP address), execute the callback directly */
		request_dns_callback(p_info->hostname, p_ip, p_conn);
		return SIMPLE_HTTP_REQUEST_SENT;
	} else if (error == ESPCONN_ARG) {
		return SIMPLE_HTTP_DNS_ARG_ERROR;
    }

    return SIMPLE_HTTP_DNS_ERROR;
}

simple_http_status_t ICACHE_FLASH_ATTR
simple_http_request(char* url, char* data, char* headers, char* request_method, simple_http_response_callback_t response_callback) {
    char default_path[] = "/";
    char* port_colon_position;
    char* path_start_position;

    struct espconn* p_conn;
    simple_http_client_request_info_t* p_request_info;

    system_soft_wdt_feed();
    p_conn = (struct espconn*)os_malloc(sizeof(struct espconn));
    if (p_conn == NULL) {
        return SIMPLE_HTTP_MEM_ERROR;
    }

    p_request_info = (simple_http_client_request_info_t*)os_malloc(sizeof(simple_http_client_request_info_t));
    if (p_request_info == NULL) {
        return SIMPLE_HTTP_MEM_ERROR;
    }

    p_conn->reverse = p_request_info;

    p_request_info->data     = common_strdup(data);
    p_request_info->callback = response_callback;

    if (headers == NULL) {
        p_request_info->headers = common_strdup("");
    } else {
        p_request_info->headers = common_strdup(headers);
    }

    if (request_method == NULL) {
        p_request_info->request_method = common_strdup("GET");
    } else {
        p_request_info->request_method = common_strdup(request_method);
    }

    p_request_info->response_buffer     = common_strdup("\0");
    p_request_info->response_buffer_len = os_strlen(p_request_info->response_buffer);


    if (os_strncmp(url, "http://", 7) == 0) {
        p_request_info->port = HTTP_DEFAULT_PORT;
        p_request_info->secure = 0;
        url += 7;
    } else if (os_strncmp(url, "https://", 8) == 0) {
        p_request_info->port = HTTPS_DEFAULT_PORT;
        p_request_info->secure = 1;
        url += 8;
    } else {
        /* Not valid */
        return SIMPLE_HTTP_PROTOCOL_NOT_VALID;
    }

    port_colon_position = os_strchr(url, ':');
    path_start_position = os_strchr(url, '/');

    if (port_colon_position != NULL) {
        /* Change port */
        p_request_info->port = atoi(port_colon_position + 1);

        if (p_request_info->port == 0) {
            return SIMPLE_HTTP_MALFORMED_URL;
        }
    }

    if (port_colon_position == NULL && path_start_position == NULL) {
        p_request_info->hostname = common_strdup(url);
    } else if (port_colon_position == NULL && path_start_position != NULL) {
        p_request_info->hostname = (char*)os_malloc(sizeof(char) * (path_start_position - url + 1));
        os_strncpy(p_request_info->hostname, url, path_start_position - url);
        p_request_info->hostname[path_start_position - url] = '\0';
    } else if (port_colon_position != NULL) {
        p_request_info->hostname = (char*)os_malloc(sizeof(char) * (port_colon_position - url + 1));
        os_strncpy(p_request_info->hostname, url, port_colon_position - url);
        p_request_info->hostname[port_colon_position - url] = '\0';
    } else {
        return SIMPLE_HTTP_MALFORMED_URL;
    }

    if (path_start_position == NULL) {
        p_request_info->path = common_strdup(default_path);
    } else {
        p_request_info->path = common_strdup(path_start_position);
    }

    return make_http_request(p_conn);
}
