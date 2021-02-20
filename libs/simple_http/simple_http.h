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

/* Things to do in next revision
 *   - Do not use the callback functions generate the buffer
 *   - Use strdup in server and client
 *   - More malloc/free and less stack filling
 *   - Handle properly variable alloc/free
 *   - Handle partial responses
 *   - Handle chunked decodes
 *   - FIX ip connections
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SIMPLE_HTTP_SINGLE_CONN_ONLY

#define SIMPLE_HTTP_SERVER_MAX_REQUEST_PATH   100
#define SIMPLE_HTTP_SERVER_MAX_REQUEST_METHOD 8

#define SIMPLE_HTTP_CLIENT_MAX_REQUEST_PATH SIMPLE_HTTP_SERVER_MAX_REQUEST_PATH

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

#define HTTP_DEFAULT_PORT 80
#define HTTPS_DEFAULT_PORT 443

#define HTTP_PROTO_VERSION     "HTTP/1.0"
#define HTTP_MAX_RESPONSE_SIZE 4096

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
    SIMPLE_HTTP_OK,                             /** Everything ok */
    SIMPLE_HTTP_REQUEST_SENT,
    SIMPLE_HTTP_NO_CALLBACK,
    SIMPLE_HTTP_PROTOCOL_NOT_VALID,
    SIMPLE_HTTP_MALFORMED_URL,
    SIMPLE_HTTP_DNS_ARG_ERROR,
    SIMPLE_HTTP_DNS_ERROR,
    SIMPLE_HTTP_NO_CONNECTION,
    SIMPLE_HTTP_MEM_ERROR,
    SIMPLE_HTTP_CLIENT_NOT_READY,
    SIMPLE_HTTP_ERROR
} simple_http_status_t;

/**
 * \brief           Info of the incoming request
 */
typedef struct simple_http_server_request_info {
    struct espconn* p_conn;                     /** System connection object */
    char* request_method;                       /** Request type. Ex. GET, POST, etc.. */
    char* request_path;                         /** HTTP object path. Ex. /api/v1/thing */
    char* request_data;                         /** Raw data to send */
    char* raw_request;                          /** Raw request header + data */
    size_t raw_request_length;                  /** Raw request header + data length */
} simple_http_server_request_info_t;

/**
 * \brief           HTTP server callback function definition
 */
typedef char* (*simple_http_server_callback_t)(simple_http_server_request_info_t* p_data, size_t* p_data_size, uint16_t* p_response_code, http_content_type_t* p_content_type);

/**
 * \brief           HTTP request callback function definition
 */
typedef void (*simple_http_response_callback_t)(const char* const data, size_t data_len, uint8_t status, const char* const buffer);

/**
 * \brief           Info of the outgoing request
 */
typedef struct simple_http_client_request_info {
    struct espconn* p_conn;                     /** System connection object */
    char* hostname;                             /** Request destination hostname */
    char* path;                                 /** Request destination path */
    uint16_t port;                              /** Request destination port */
    uint8_t secure;                             /** Protocol to HTTP or HTTPS */
    char* data;                                 /** Request included data */
    char* headers;                              /** Request extra headers */
    char* request_method;                       /** Request extra headers length */
    simple_http_response_callback_t callback;   /** Request response user callback */
    char* response_buffer;                      /** HTTP response buffer */
    size_t response_buffer_len;                 /** HTTP response buffer length */
} simple_http_client_request_info_t;

typedef struct simple_http_server_config {
    simple_http_server_callback_t user_callback;
} simple_http_server_config_t;

/**
 * \brief           Configure and start a basic HTTP server
 * \param[in]       p_conn: Pointer to the espconn sdk struct
 * \param[in]       port: Server listen port
 * \param[in]       callback: Callback function that process the incoming request
 * \return          simple_http_status_t status code 
 */
simple_http_status_t create_basic_http_server(struct espconn* p_conn, uint16_t port, simple_http_server_callback_t callback);

#define simple_http_get(url, headers, callback) simple_http_request(url, NULL, headers, "GET", callback)
#define simple_http_post(url, data, headers, callback) simple_http_request(url, data, headers, "POST", callback)

/**
 * \brief
 */
simple_http_status_t simple_http_request(char* url, char* data, char* headers, char* request_method, simple_http_response_callback_t response_callback);

/**
 * \brief
 */
simple_http_status_t simple_http_client_ready(void);

/**
 * \brief
 */
simple_http_status_t simple_http_client_reset(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SIMPLE_HTTP_H */
