/**
 * \file simple_http_server.c
 * \author Mario Rubio (mario@mrrb.eu)
 * \brief 
 * \version 0.1
 * \date 2021-02-15
 */

#include <osapi.h>
#include <mem.h>
#include <espconn.h>

#include "simple_http/simple_http.h"


static void ICACHE_FLASH_ATTR
send_server_response(struct espconn* p_conn, char* data, size_t data_len, uint16_t response_code, http_content_type_t content_type) {
    size_t msg_size = (3 + 22 + 20 + 20 + 70 + data_len);

    char* content_type_txt;

    char* msg;
    char response_code_msg[22];

    if (response_code >= 100 && response_code < 200) {
        os_strcpy(response_code_msg, "Continue");
    } else if (response_code >= 200 && response_code < 300) {
        os_strcpy(response_code_msg, "OK");
    } else if (response_code >= 300 && response_code < 400) {
        os_strcpy(response_code_msg, "Multiple Choices");
    } else if (response_code >= 400 && response_code < 500) {
        if (response_code == 404) {
            os_strcpy(response_code_msg, "Not Found");
        } else {
            os_strcpy(response_code_msg, "Bad Request");
        }
    } else if (response_code >= 500 && response_code < 600) {
        os_strcpy(response_code_msg, "Internal Server Error");
    } else {
        return;
    }

    switch(content_type) {
        case HTTP_CONTENT_TYPE_TEXT_HTML:
            content_type_txt = (char*)os_malloc(sizeof(char) * HTTP_CONTENT_TYPE_TEXT_HTML_SIZE);
            os_strcpy(content_type_txt, HTTP_CONTENT_TYPE_TEXT_HTML_TEXT);
            break;
        case HTTP_CONTENT_TYPE_TEXT_PLAIN:
            content_type_txt = (char*)os_malloc(sizeof(char) * HTTP_CONTENT_TYPE_TEXT_PLAIN_SIZE);
            os_strcpy(content_type_txt, HTTP_CONTENT_TYPE_TEXT_PLAIN_TEXT);
            break;
        case HTTP_CONTENT_TYPE_TEXT_XML:
            content_type_txt = (char*)os_malloc(sizeof(char) * HTTP_CONTENT_TYPE_TEXT_XML_SIZE);
            os_strcpy(content_type_txt, HTTP_CONTENT_TYPE_TEXT_XML_TEXT);
            break;
        case HTTP_CONTENT_TYPE_MULTIPART_FORM_DATA:
            content_type_txt = (char*)os_malloc(sizeof(char) * HTTP_CONTENT_TYPE_MULTIPART_FORM_DATA_SIZE);
            os_strcpy(content_type_txt, HTTP_CONTENT_TYPE_MULTIPART_FORM_DATA_TEXT);
            break;
        case HTTP_CONTENT_TYPE_APPLICATION_JSON:
            content_type_txt = (char*)os_malloc(sizeof(char) * HTTP_CONTENT_TYPE_APPLICATION_JSON_SIZE);
            os_strcpy(content_type_txt, HTTP_CONTENT_TYPE_APPLICATION_JSON_TEXT);
            break;
        case HTTP_CONTENT_TYPE_APPLICATION_XML:
            content_type_txt = (char*)os_malloc(sizeof(char) * HTTP_CONTENT_TYPE_APPLICATION_XML_SIZE);
            os_strcpy(content_type_txt, HTTP_CONTENT_TYPE_APPLICATION_XML_TEXT);
            break;
        default:
            content_type_txt = (char*)os_malloc(sizeof(char) * HTTP_CONTENT_TYPE_TEXT_HTML_SIZE);
            os_strcpy(content_type_txt, HTTP_CONTENT_TYPE_TEXT_HTML_TEXT);
            break;
    }

    msg = (char*)os_zalloc(sizeof(char) * msg_size);
    if (msg == NULL) {
        return;
    }

    if (data == NULL || data_len == 0) {
        msg_size = os_sprintf(msg,
                              "HTTP/1.1 %d %s\r\nContent-Length: %d\r\nServer: lwIP/1.4.0\r\n" \
                              "Content-Type: %s\r\n",
                              response_code, response_code_msg, data_len, content_type_txt);
    } else {
        msg_size = os_sprintf(msg,
                              "HTTP/1.1 %d %s\r\nContent-Length: %d\r\nServer: lwIP/1.4.0\r\n" \
                              "Content-Type: %s\r\n\r\n%s",
                              response_code, response_code_msg, data_len, content_type_txt, data);
    }

    // os_printf("%s\n", msg);
    espconn_send(p_conn, (uint8*)msg, msg_size);

    os_free(msg);
}

static void ICACHE_FLASH_ATTR
server_response(void *arg, char *pusrdata, unsigned short length) {
    struct espconn* p_conn = arg;

    char* first_space;
    char* second_space;
    char* data_start;

    char* request_method;
    char* request_path;

    char* data = NULL;
    size_t data_len;
    uint16_t response_code;
    http_content_type_t content_type;

    simple_http_server_config_t* p_config = (simple_http_server_config_t*)p_conn->reverse;

    simple_http_server_request_info_t request_data;

    first_space = os_strchr(pusrdata, ' ');
    if (first_space == NULL) {
        send_server_response(p_conn, NULL, 0, 500, HTTP_CONTENT_TYPE_TEXT_HTML);
        return;
    }

    second_space = os_strchr(first_space + 1, ' ');
    if (second_space == NULL) {
        send_server_response(p_conn, NULL, 0, 500, HTTP_CONTENT_TYPE_TEXT_HTML);
        return;
    }

    if (!os_strncmp(second_space + 1, "HTTP", 4) == 0) {
        send_server_response(p_conn, NULL, 0, 400, HTTP_CONTENT_TYPE_TEXT_HTML);
        return;
    }

    data_start = os_strstr(pusrdata, "\n\r\n");
    if (data_start == NULL) {
        // return;
    } else {
        data_start += 3;
    }

    request_method = (char*)os_malloc(sizeof(char) * SIMPLE_HTTP_SERVER_MAX_REQUEST_METHOD);
    request_path = (char*)os_malloc(sizeof(char) * SIMPLE_HTTP_SERVER_MAX_REQUEST_PATH);

    if (request_path == NULL || request_method == NULL) {
        send_server_response(p_conn, NULL, 0, 500, HTTP_CONTENT_TYPE_TEXT_HTML);
        return;
    }

    os_strncpy(request_method, pusrdata, first_space - pusrdata);
    os_strncpy(request_path, first_space+1, second_space - first_space - 1);

    // Call user response generator
    if (p_config->user_callback == NULL) {
        send_server_response(p_conn, NULL, 0, 404, HTTP_CONTENT_TYPE_TEXT_HTML);
    } else {
        request_data.p_conn = p_conn;
        request_data.request_method = request_method;
        request_data.request_path = request_path;
        request_data.request_data = data_start;
        request_data.raw_request = pusrdata;
        request_data.raw_request_length = length;

        data = p_config->user_callback(&request_data, &data_len, &response_code, &content_type);
        send_server_response(p_conn, data, data_len, response_code, content_type);
    }

    os_free(request_method);
    os_free(request_path);

    if (data != NULL) {
        os_free(data);
        data = NULL;
    }
}

static void ICACHE_FLASH_ATTR
server_conn_call(void *arg) {
    struct espconn* conn = arg;
    espconn_regist_recvcb(conn, server_response);
    // espconn_regist_reconcb(conn, );
}

simple_http_status_t ICACHE_FLASH_ATTR
create_basic_http_server(struct espconn* p_conn, uint16_t port, simple_http_server_callback_t callback) {
    simple_http_server_config_t* p_config = (simple_http_server_config_t*)os_zalloc(sizeof(simple_http_server_config_t));

    // if (callback == NULL) {
    //     return SIMPLE_HTTP_NO_CALLBACK;
    // }

    p_conn->type = ESPCONN_TCP;
    p_conn->state = ESPCONN_NONE;

    p_conn->proto.tcp = (esp_tcp *)os_zalloc(sizeof(esp_tcp));
    p_conn->proto.tcp->local_port = port;

    p_config->user_callback = callback;
    p_conn->reverse = p_config;

    espconn_regist_connectcb(p_conn, server_conn_call);
    espconn_accept(p_conn);

    return SIMPLE_HTTP_OK;
}
