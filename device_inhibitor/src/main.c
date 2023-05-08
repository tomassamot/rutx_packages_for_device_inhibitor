#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <argp.h>

#include "argpfuncs.h"
#include "tuyafuncs.h"
#include "becomedaemon.h"
#include "ubusfuncs.h"

static void board_cb(struct ubus_request *req, int type, struct blob_attr *msg);
static void myesp_devices_cb(struct ubus_request *req, int type, struct blob_attr *msg);
static void handle_kill(int signum);


struct arguments arguments = { {[0 ... 29] = '\0'}, {[0 ... 29] = '\0'}, {[0 ... 29] = '\0'}, {[0 ... 1023] = '\0'}, 0 };
int program_is_killed = 0;

int rc = 0;

int main(int argc, char** argv)
{
    int ret;
    struct ubus_context *context;
    uint32_t system_id, myesp_id;

    start_parser(argc, argv, &arguments);
    printf("Starting...\n");

    openlog("device_inhibitor", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL0);
    
    if(arguments.daemon != 0){
        ret = become_daemon();
        if(ret){
            syslog(LOG_ERR, "Error starting daemon");
            closelog();
            return EXIT_FAILURE;
        }
        syslog(LOG_INFO, "Daemon started");
    }
    else{
        syslog(LOG_INFO, "Program started");
    }

    signal(SIGKILL, handle_kill);
    signal(SIGTERM, handle_kill);
    signal(SIGINT, handle_kill);
    signal(SIGUSR1, handle_kill);

    while(program_is_killed == 0){
        connect_to_ubus(&context);
        if(context == NULL){
            syslog(LOG_ERR, "Failed to connect to ubus\n");
            return 1;
        }

        ret = tuya_connect(arguments.product_id, arguments.device_id, arguments.device_secret, context);
        if(ret != 0){
            return ret;
        }
        
        ubus_lookup_id(context, "system", &system_id);
        ubus_lookup_id(context, "myesp", &myesp_id);
        syslog(LOG_INFO, "Beginning to send information to cloud");
        rc = 0;
        while(rc == 0 && program_is_killed == 0){
            sleep(5);
            syslog(LOG_INFO, "Sending router RAM information...");
            //ubus_invoke(context, system_id, "info", NULL, board_cb, NULL, 3000);
            ubus_invoke(context, myesp_id, "devices", NULL, myesp_devices_cb, NULL, 3000);
        }

        disconnect_from_ubus(context);
        tuya_disconnect();
    }
    return(0);
}
static void board_cb(struct ubus_request *req, int type, struct blob_attr *msg)
{
    char message[200];
	struct blob_buf *buf = (struct blob_buf *)req->priv;
	struct blob_attr *tb[__INFO_MAX];
	struct blob_attr *memory[__MEMORY_MAX];

	blobmsg_parse(info_policy, __INFO_MAX, tb, blob_data(msg), blob_len(msg));

	if (!tb[MEMORY_DATA]) {
		syslog(LOG_WARNING, "No memory data received");
        rc--;
		return;
	}

	blobmsg_parse(memory_policy, __MEMORY_MAX, memory, blobmsg_data(tb[MEMORY_DATA]), blobmsg_data_len(tb[MEMORY_DATA]));

    sprintf(message, "{\"free_ram\":%lld,\"total_ram\":%lld}", blobmsg_get_u64(memory[FREE_MEMORY]), blobmsg_get_u64(memory[TOTAL_MEMORY]));
    int ret = tuya_loop(message);
    if(ret != 0){
        rc+=1;
    }
}
static void myesp_devices_cb(struct ubus_request *req, int type, struct blob_attr *msg)
{
	struct blob_attr *tb[_ALL_DEVICES_MAX];
    int j = 0;

	blobmsg_parse(myesp_all_devices_policy, _ALL_DEVICES_MAX, tb, blob_data(msg), blob_len(msg));

	if (!tb[ALL_DEVICES]) {
		syslog(LOG_WARNING, "No memory data received");
        rc++;
		return;
	}
    
    struct blob_attr *curr;
    size_t rem = 0;


    char *all_devices = (char *) malloc(2*512*sizeof(char));
    strcpy(all_devices, "");

    blobmsg_for_each_attr(curr, tb[0], rem){
        struct blob_attr *device[_DEVICE_MAX];
        char device_info[512];

        if(j % 2 == 0 && j != 0)
            all_devices = (char *) realloc(all_devices, 2*j*512*sizeof(char));

        blobmsg_parse(myesp_device_policy, 2, device, blobmsg_data(curr), blobmsg_data_len(curr));

    char *port = blobmsg_get_string(device[PORT_DEVICE]);
    int vid = blobmsg_get_u32(device[VENDOR_ID]);
    int pid = blobmsg_get_u32(device[PRODUCT_ID]);
        if(j == 0)
            sprintf(device_info, "'{\"port\": \"%s\",\"vendor_id\": %d,\"product_id\": %d}'", blobmsg_get_string(device[PORT_DEVICE]), blobmsg_get_u32(device[VENDOR_ID]), blobmsg_get_u32(device[PRODUCT_ID]));
        else
            sprintf(device_info, ",'{\"port\": \"%s\",\"vendor_id\": %d,\"product_id\": %d}'", blobmsg_get_string(device[PORT_DEVICE]), blobmsg_get_u32(device[VENDOR_ID]), blobmsg_get_u32(device[PRODUCT_ID]));

        strcat(all_devices, device_info);

        j++;
    }

    if(j == 0)
        j = 1;
    char *message = (char *)malloc(512*sizeof(char)+2*j*512*sizeof(char));
    sprintf(message, "{\"esp_controllers\": [%s]}", all_devices);

    int ret = tuya_loop(message);
    // int ret = tuya_loop("{\"free_ram\":113160192,\"total_ram\":255922176}");
    if(ret != 0){
        rc+=1;
    }
}
static void handle_kill(int signum)
{
    // deallocation goes here

    //
    program_is_killed = 1;
}
