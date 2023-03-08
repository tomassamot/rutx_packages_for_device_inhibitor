#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <string.h>
#include <signal.h>

#include <cJSON.h>
#include <tuya_error_code.h>
#include <system_interface.h>
#include <mqtt_client_interface.h>
#include <tuyalink_core.h>

#include "tuya_cacert.h"
#include "ubusfuncs.h"

static void on_connected(tuya_mqtt_context_t* context, void* user_data);
static void on_disconnect(tuya_mqtt_context_t* context, void* user_data);
static void on_messages(tuya_mqtt_context_t* context, void* user_data, const tuyalink_message_t* msg);
static int change_ip_address(char *msg);
static int check_if_is_number(char *str);
static void write_message_to_file(char msg[]);
static void handle_kill(int signum);




tuya_mqtt_context_t client_instance;
struct ubus_context *ubus_context;
int ACK=0;

int tuya_connect(char *product_id, char *device_id, char *device_secret, struct ubus_context *ctx)
{
    int ret = OPRT_OK;

    ubus_context = ctx;

    signal(SIGKILL, handle_kill);
    signal(SIGTERM, handle_kill);
    signal(SIGINT, handle_kill);

    ret = tuya_mqtt_init(&client_instance, &(const tuya_mqtt_config_t) {
        .host = "m1.tuyacn.com",
        .port = 8883,
        .cacert = tuya_cacert_pem,
        .cacert_len = sizeof(tuya_cacert_pem),
        .device_id = device_id,
        .device_secret = device_secret,
        .keepalive = 100,
        .timeout_ms = 2000,
        .on_connected = on_connected,
        .on_disconnect = on_disconnect,
        .on_messages = on_messages
    });
    if( ret != OPRT_OK ) {
        syslog(LOG_ERR, "Failed to initialize context");
        return 1;
    }
    
    ret = tuya_mqtt_connect(&client_instance);
    if( ret != OPRT_OK ){
        syslog(LOG_ERR, "Failed to connect to Tuya");
        return ret;
    }

    syslog(LOG_INFO, "Connected to Tuya cloud");
    return 0;
}

int tuya_loop(char json_msg[])
{   
    // Checking if connection to Tuya is still possible
    //
    tuyalink_subdevice_topo_get(&client_instance);
    int i = 0;
    while(ACK != 1){
        if(i == 3){
            syslog(LOG_ERR, "Acknowledgement from Tuya waiting timeout.");
            return 0;
        }
        tuya_mqtt_loop(&client_instance);
        sleep(1);
        i++;
    }

    tuyalink_thing_property_report(&client_instance, NULL, json_msg);
    
    tuya_mqtt_loop(&client_instance);

    ACK = 0;
    return 0;
}
void tuya_disconnect()
{
    tuya_mqtt_disconnect(&client_instance);
    syslog(LOG_INFO, "Disconnected from Tuya cloud");
    closelog();
    tuya_mqtt_deinit(&client_instance);
}

static void on_connected(tuya_mqtt_context_t* context, void* user_data)
{
    
}
static void on_disconnect(tuya_mqtt_context_t* context, void* user_data)
{

}
static void on_messages(tuya_mqtt_context_t* context, void* user_data, const tuyalink_message_t* msg)
{
    syslog(LOG_INFO, "Message received from Tuya cloud");
    switch(msg->type){
        case THING_TYPE_DEVICE_TOPO_GET_RSP:
            ACK = 1;
            break;
        case THING_TYPE_ACTION_EXECUTE:
            change_ip_address(msg->data_string);
            break;
    }
}
static int change_ip_address(char *json_msg)
{
    cJSON *json_obj, *json_input_params, *json_ip;
    int ret = 0;

    json_obj = cJSON_Parse(json_msg);
    json_input_params = cJSON_GetObjectItem(json_obj, "inputParams");
    json_ip = cJSON_GetObjectItem(json_input_params, "new_ip");
    char *ip = cJSON_GetStringValue(json_ip);

    char buffer[100];
    strcpy(buffer, ip);
    char *token = strtok(buffer, ".");
    int i = 0;
    while(i != 4){
        if(token == NULL || (token != NULL && check_if_is_number(token) != 0)){
            syslog(LOG_ERR, "Received incorrectly formatted IP address");
            return 1;
        }
        
        token = strtok(NULL, ".");
        i++;
    }
    
    ret = set_new_static_ip(ubus_context, ip);
    if(ret == 0){
        commit_uci_changes(ubus_context, "network");
    }
    return 0;
}
static int check_if_is_number(char *str)
{
    int num = strtol(str, NULL, 8);
    if(num == 0)
        return 1;
    return 0;
}
static void write_message_to_file(char *msg)
{
    FILE *msg_file = NULL;
    time_t rawtime = time(0);
    struct tm *timeinfo = localtime(&rawtime);
    size_t path_len = 100;
    char path[path_len];

    getcwd(path, path_len);
    strcat(path, "/received_messsages.txt");

    msg_file = fopen(path, "a");
    if(msg_file == NULL){
        syslog(LOG_WARNING, "Failed to open received_messages.txt for appending in path: %s", path);
        return;
    }

    if(msg == NULL){
        syslog(LOG_WARNING, "Message received was empty");
        fprintf(msg_file, "%s: %s\n", asctime(timeinfo), "(Empty message)");
    }
    else{
        fprintf(msg_file, "%s: %s\n", asctime(timeinfo), msg);
    }
    
    fclose(msg_file);
}
static void handle_kill(int signum)
{
    syslog(LOG_INFO, "Request to kill detected");
    // deallocation goes here

    //
    raise(SIGUSR1);
}