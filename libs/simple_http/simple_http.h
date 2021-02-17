/**
 * \file simple_http.h
 * \author Mario Rubio (mario@mrrb.eu)
 * \brief 
 * \version 0.1
 * \date 2021-02-15
 */

#ifndef SIMPLE_HTTP_H
#define SIMPLE_HTTP_H

#include <c_types.h>
#include <espconn.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SIMPLE_HTTP_SERVER_MAX_REQUEST_PATH 100
#define SIMPLE_HTTP_SERVER_MAX_REQUEST_TYPE 8

#define HTTP_CONTENT_TYPE_TEXT_HTML_TEXT           "text/html"
#define HTTP_CONTENT_TYPE_TEXT_PLAIN_TEXT          "text/plain"
#define HTTP_CONTENT_TYPE_TEXT_XML_TEXT            "text/xml"
#define HTTP_CONTENT_TYPE_MULTIPART_FORM_DATA_TEXT "multipart/form-data"
#define HTTP_CONTENT_TYPE_APPLICATION_JSON_TEXT    "application/json"
#define HTTP_CONTENT_TYPE_APPLICATION_XML_TEXT     "application/xml"

#define HTTP_CONTENT_TYPE_TEXT_HTML_SIZE           10
#define HTTP_CONTENT_TYPE_TEXT_PLAIN_SIZE          11
#define HTTP_CONTENT_TYPE_TEXT_XML_SIZE            9
#define HTTP_CONTENT_TYPE_MULTIPART_FORM_DATA_SIZE 20
#define HTTP_CONTENT_TYPE_APPLICATION_JSON_SIZE    17
#define HTTP_CONTENT_TYPE_APPLICATION_XML_SIZE     16


typedef struct espconn espconn_t;

typedef enum http_content_type {
    HTTP_CONTENT_TYPE_TEXT_HTML,
    HTTP_CONTENT_TYPE_TEXT_PLAIN,
    HTTP_CONTENT_TYPE_TEXT_XML,
    HTTP_CONTENT_TYPE_MULTIPART_FORM_DATA,
    HTTP_CONTENT_TYPE_APPLICATION_JSON,
    HTTP_CONTENT_TYPE_APPLICATION_XML
} http_content_type_t;

typedef enum simple_http_status {
    SIMPLE_HTTP_OK,
    SIMPLE_HTTP_NO_CALLBACK
} simple_http_status_t;

typedef struct simple_http_request_info {
    struct espconn* p_conn;
    char* request_type;
    char* request_path;
    char* request_data;
} simple_http_request_info_t;

typedef char* (*simple_web_server_callback_t)(simple_http_request_info_t* p_data, size_t* p_data_size, uint16_t* p_response_code, http_content_type_t* p_content_type);

typedef struct simple_http_server_config {
    simple_web_server_callback_t user_callback;
} simple_http_server_config_t;


simple_http_status_t create_web_server(struct espconn* p_conn, uint16_t port, simple_web_server_callback_t callback);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SIMPLE_HTTP_H */
