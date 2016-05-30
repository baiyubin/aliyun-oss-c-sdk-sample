#include "aos_log.h"
#include "aos_util.h"
#include "aos_string.h"
#include "aos_status.h"
#include "oss_auth.h"
#include "oss_util.h"
#include "oss_api.h"


const char OSS_ENDPOINT[] = "<your endpoint>";
const char ACCESS_KEY_ID[] = "<your access key id>";
const char ACCESS_KEY_SECRET[] = "<your access key secret>";
const char BUCKET_NAME[] = "<your bucket name>";
const char OBJECT_NAME[] = "<your object key>";

void init_sample_request_options(oss_request_options_t *options, int is_cname);

void put_object_from_buffer();
void put_object_from_file();
void get_object_to_buffer();
void get_object_to_file();
void delete_object();

int main(int argc, char *argv[])
{
    apr_file_t *output = NULL;
    aos_pool_t *pool = NULL;
    apr_status_t ret;
    
    // initialize http io system, call it olny once
    if (aos_http_io_initialize(NULL, 0) != AOSE_OK) {
        exit(1);
    }
    
    aos_pool_create(&pool, NULL);

    // open log file
    ret = apr_file_open(&output, "oss-c-sdk-sample.log", APR_FOPEN_CREATE | APR_FOPEN_WRITE | APR_FOPEN_APPEND, APR_OS_DEFAULT, pool);
    if (APR_SUCCESS != ret) {
        exit(1);
    }
 
    // set log file
    aos_log_set_output(output);
    // set log level
    aos_log_set_level(AOS_LOG_INFO);

    aos_info_log("start");

    // run samples
    put_object_from_buffer();
    put_object_from_file();
    get_object_to_buffer();
    get_object_to_file();
    delete_object();

    // close log file
    apr_file_close(output); 

    aos_pool_destroy(pool);

    // deinitialize http io system, call it olny once
    aos_http_io_deinitialize();

    system("pause");

    return 0;
}

void init_sample_request_options(oss_request_options_t *options, int is_cname)
{
    options->config = oss_config_create(options->pool);
    aos_str_set(&options->config->endpoint, OSS_ENDPOINT);
    aos_str_set(&options->config->access_key_id, ACCESS_KEY_ID);
    aos_str_set(&options->config->access_key_secret, ACCESS_KEY_SECRET);
    options->config->is_cname = is_cname;

    options->ctl = aos_http_controller_create(options->pool, 0);
}

void put_object_from_buffer()
{
    aos_pool_t *p = NULL;
    aos_string_t bucket;
    aos_string_t object;
    int is_cname = 0;
    aos_table_t *headers = NULL;
    aos_table_t *resp_headers = NULL;
    oss_request_options_t *options = NULL;
    aos_list_t buffer;
    aos_buf_t *content = NULL;
    char *str = "test oss c sdk";
    aos_status_t *s = NULL;

    aos_pool_create(&p, NULL);
    options = oss_request_options_create(p);
    init_sample_request_options(options, is_cname);
    headers = aos_table_make(p, 1);
    apr_table_set(headers, "x-oss-meta-author", "oss");
    aos_str_set(&bucket, BUCKET_NAME);
    aos_str_set(&object, OBJECT_NAME);

    aos_list_init(&buffer);
    content = aos_buf_pack(options->pool, str, strlen(str));
    aos_list_add_tail(&content->node, &buffer);

    s = oss_put_object_from_buffer(options, &bucket, &object, 
                   &buffer, headers, &resp_headers);

    if (aos_status_is_ok(s)) {
        printf("put object from buffer succeeded\n");
    } else {
        printf("put object from buffer failed\n");      
    }    

    aos_pool_destroy(p);
}

void put_object_from_file()
{
    aos_pool_t *p = NULL;
    aos_string_t bucket;
    aos_string_t object;
    int is_cname = 0;
    aos_table_t *headers = NULL;
    aos_table_t *resp_headers = NULL;
    oss_request_options_t *options = NULL;
    char *filename = "SouthIsland_NewZealand.jpg";
    aos_status_t *s = NULL;
    aos_string_t file;

    aos_pool_create(&p, NULL);
    options = oss_request_options_create(p);
    init_sample_request_options(options, is_cname);
    headers = aos_table_make(options->pool, 1);
    apr_table_set(headers, OSS_CONTENT_TYPE, "image/jpeg");
    aos_str_set(&bucket, BUCKET_NAME);
    aos_str_set(&object, OBJECT_NAME);
    aos_str_set(&file, filename);

    s = oss_put_object_from_file(options, &bucket, &object, &file, 
                                 headers, &resp_headers);

    if (aos_status_is_ok(s)) {
        printf("put object from file succeeded\n");
    } else {
        printf("put object from file failed, code:%d, error_code:%s, error_msg:%s, request_id:%s\n",
            s->code, s->error_code, s->error_msg, s->req_id);
    }

    aos_pool_destroy(p);
}

void get_object_to_buffer()
{
    aos_pool_t *p = NULL;
    aos_string_t bucket;
    aos_string_t object;
    int is_cname = 0;
    oss_request_options_t *options = NULL;
    aos_table_t *headers = NULL;
    aos_table_t *params = NULL;
    aos_table_t *resp_headers = NULL;
    aos_status_t *s = NULL;
    aos_list_t buffer;
    aos_buf_t *content = NULL;
    char *buf = NULL;
    int64_t len = 0;
    int64_t size = 0;
    int64_t pos = 0;

    aos_pool_create(&p, NULL);
    options = oss_request_options_create(p);
    init_sample_request_options(options, is_cname);
    aos_str_set(&bucket, BUCKET_NAME);
    aos_str_set(&object, OBJECT_NAME);
    aos_list_init(&buffer);

    s = oss_get_object_to_buffer(options, &bucket, &object, 
                                 headers, params, &buffer, &resp_headers);

    if (aos_status_is_ok(s)) {
        printf("get object to buffer succeeded\n");
    }
    else {
        printf("get object to buffer failed\n");  
    }

    //get buffer len
    aos_list_for_each_entry(aos_buf_t, content, &buffer, node) {
        len += aos_buf_size(content);
    }

    buf = aos_pcalloc(p, (apr_size_t)(len + 1));
    buf[len] = '\0';

    //copy buffer content to memory
    aos_list_for_each_entry(aos_buf_t, content, &buffer, node) {
        size = aos_buf_size(content);
        memcpy(buf + pos, content->pos, (size_t)size);
        pos += size;
    }

    aos_pool_destroy(p);
}

void get_object_to_file()
{
    aos_pool_t *p = NULL;
    aos_string_t bucket;
    char *download_filename = "SouthIsland_NewZealand_New.jpg";
    aos_string_t object;
    int is_cname = 0;
    oss_request_options_t *options = NULL;
    aos_table_t *headers = NULL;
    aos_table_t *params = NULL;
    aos_table_t *resp_headers = NULL;
    aos_status_t *s = NULL;
    aos_string_t file;

    aos_pool_create(&p, NULL);
    options = oss_request_options_create(p);
    init_sample_request_options(options, is_cname);
    aos_str_set(&bucket, BUCKET_NAME);
    aos_str_set(&object, OBJECT_NAME);
    headers = aos_table_make(p, 0);
    aos_str_set(&file, download_filename);

    s = oss_get_object_to_file(options, &bucket, &object, headers, 
                               params, &file, &resp_headers);

    if (aos_status_is_ok(s)) {
        printf("get object to local file succeeded\n");
    } else {
        printf("get object to local file failed\n");
    }

    aos_pool_destroy(p);
}

void delete_object()
{
    aos_pool_t *p = NULL;
    aos_string_t bucket;
    aos_string_t object;
    int is_cname = 0;
    oss_request_options_t *options = NULL;
    aos_table_t *resp_headers = NULL;
    aos_status_t *s = NULL;

    aos_pool_create(&p, NULL);
    options = oss_request_options_create(p);
    init_sample_request_options(options, is_cname);
    aos_str_set(&bucket, BUCKET_NAME);
    aos_str_set(&object, OBJECT_NAME);

    s = oss_delete_object(options, &bucket, &object, &resp_headers);

    if (aos_status_is_ok(s)) {
        printf("delete object succeed\n");
    } else {
        printf("delete object failed\n");
    }

    aos_pool_destroy(p);
}
