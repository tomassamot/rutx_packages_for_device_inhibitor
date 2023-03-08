#ifndef TUYAFUNCS_H
#define TUYAFUNCS_H


int tuya_connect(char *product_id, char *device_id, char *device_secret, struct ubus_context *ctx);
int tuya_loop(char json_msg[]);
void tuya_disconnect();

#endif