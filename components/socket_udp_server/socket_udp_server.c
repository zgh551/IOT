#include "socket_udp_server.h"

static const char *TAG = "UDP_Server";
//"52981"
#define DEVICE_TYPE   "gh_9c3be86cddc0"
#define DEVICE_ID     "wx8d6e759ee154ad06"
#define DEFAULT_LAN_PORT    12476

#define HOST_IP_ADDR "255.255.255.255"

const airkiss_config_t udp_airkiss_config =
{
	(airkiss_memset_fn)&memset,
	(airkiss_memcpy_fn)&memcpy,
	(airkiss_memcmp_fn)&memcmp,
    (airkiss_printf_fn)&printf
};

uint8_t payload[300];
uint16_t payload_len;

airkiss_lan_ret_t packret;

int sock;
struct sockaddr_in client_addr;



static void udp_server_task(void *pvParameters)
{
    char rx_buffer[128];  
    char addr_str[128];
    int addr_family;
    int ip_protocol;

    // xEventGroupWaitBits(wifi_event_group, ESPTOUCH_DONE_BIT, true, false, portMAX_DELAY); 

    // memset(&client_addr, 0, sizeof(client_addr));
    // client_addr.sin_addr.s_addr = inet_addr(HOST_IP_ADDR);
    // client_addr.sin_family = AF_INET;
    // client_addr.sin_port = htons(DEFAULT_LAN_PORT);
    // addr_family = AF_INET;
    // ip_protocol = IPPROTO_IP;
    // inet_ntoa_r(client_addr.sin_addr, addr_str, sizeof(addr_str) - 1);

    // int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
    // if (sock < 0) {
    //     ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
    //     vTaskDelete(NULL);
    // }
    // ESP_LOGI(TAG, "Socket created");

    // int err = bind(sock, (struct sockaddr *)&client_addr, sizeof(client_addr));
    // if (err < 0) {
    //     ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
    // }
    // ESP_LOGI(TAG, "Socket binded");

    // ESP_LOGI(TAG, "airkiss version: %s",airkiss_version());

    // xTaskCreate(udp_client_time_5s_task, "udp_client", 4096, NULL, 5, NULL);
    struct sockaddr_in server_addr;
    struct sockaddr_in echo_client_addr;

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_addr.s_addr = inet_addr(HOST_IP_ADDR);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(DEFAULT_LAN_PORT);
    addr_family = AF_INET;
    ip_protocol = IPPROTO_IP;

    int sock_fd = socket(addr_family, SOCK_DGRAM, ip_protocol);
    ESP_LOGE(TAG, "the server address create value %d", sock_fd);
    if (sock_fd == -1)
    {
        printf("failed to create sock_fd!\n");
        vTaskDelete(NULL);
    }
    int err = bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (err < 0) {
        ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
    }
    ESP_LOGE(TAG, "the server address bind value %d", sock_fd);
    ESP_LOGI(TAG, "Socket binded");
    while (1) {
        vTaskDelay(1000 / portTICK_RATE_MS);
        ESP_LOGI(TAG, "Waiting for data");
        struct sockaddr_in sourceAddr;
        socklen_t socklen = sizeof(sourceAddr);
        int len = recvfrom(sock_fd, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&sourceAddr, &socklen);
        // Error occured during receiving
        if (len < 0) {
            ESP_LOGE(TAG, "recvfrom failed: errno %d", errno);
            break;
        }
        // Data received
        else {
            rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string...
            airkiss_lan_ret_t sts = airkiss_lan_recv(rx_buffer,len,&udp_airkiss_config);
            ESP_LOGI(TAG, "status of the lan recv %d", sts); 
            switch(sts)
            {
                case AIRKISS_LAN_SSDP_REQ:
                    ESP_LOGI(TAG, "enter switch case");
                    payload_len = sizeof(payload); 
                    airkiss_lan_ret_t packret = airkiss_lan_pack(AIRKISS_LAN_SSDP_RESP_CMD,DEVICE_TYPE,DEVICE_ID, 0, 0, payload, &payload_len,&udp_airkiss_config);                      
                    ESP_LOGI(TAG, "status of the lan packet %d", packret); 
                    ESP_LOGI(TAG, "length of packet %d", payload_len); 
                    if(packret != AIRKISS_LAN_PAKE_READY)
                    {
                        ESP_LOGI(TAG, "Pack lan packet error!");
                        continue;
                    }
                    inet_ntoa_r(((struct sockaddr_in *)&sourceAddr)->sin_addr.s_addr, addr_str, sizeof(addr_str) - 1);
                    ESP_LOGI(TAG, "Received %d bytes from %s:", len, addr_str);

                    ESP_LOGI(TAG, "the echo source add port %d", sourceAddr.sin_port);

                    memset(&server_addr, 0, sizeof(echo_client_addr));
                    echo_client_addr.sin_addr.s_addr = sourceAddr.sin_addr.s_addr;
                    echo_client_addr.sin_family = AF_INET;
                    echo_client_addr.sin_port = htons(DEFAULT_LAN_PORT);

                    int err_bind = bind(sock_fd, (struct sockaddr *)&echo_client_addr, sizeof(echo_client_addr));
                    if (err_bind < 0) {
                        ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
                    }
                    ESP_LOGI(TAG, "the source add bind socket value %d", sock_fd);

                    int err = sendto(sock_fd, payload, payload_len, 0, (struct sockaddr *)&sourceAddr, sizeof(sourceAddr));
                    // int err = sendto(sock, rx_buffer, len, 0, (struct sockaddr *)&sourceAddr, sizeof(sourceAddr));        
                    if (err < 0) {
                        ESP_LOGE(TAG, "Error occured during sending: errno %d", errno);
                    }

                break;

                default:
                    ESP_LOGE(TAG, "Pack is not ssdq req!");
                break;
            }
        }
    }
    if (sock != -1) {
        ESP_LOGE(TAG, "Shutting down socket and restarting...");
        shutdown(sock, 0);
        close(sock);
    }
    vTaskDelete(NULL);
}

static void udp_client_time_5s_task(void *pvParameters)
{
    char rx_buffer[128];  
    char addr_str[128];
    int addr_family;
    int ip_protocol;

    xEventGroupWaitBits(wifi_event_group, ESPTOUCH_DONE_BIT, true, false, portMAX_DELAY); 

    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_addr.s_addr = inet_addr(HOST_IP_ADDR);
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(DEFAULT_LAN_PORT);
    addr_family = AF_INET;
    ip_protocol = IPPROTO_IP;
    inet_ntoa_r(client_addr.sin_addr, addr_str, sizeof(addr_str) - 1);
    ESP_LOGI(TAG, "5s the send address %s:", addr_str);
    int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
    if (sock < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        vTaskDelete(NULL);
    }
    ESP_LOGI(TAG, "Socket created");
    
    xTaskCreate(udp_server_task, "udp_server", 4096, NULL, 5, NULL);
    while (1) {
        payload_len = sizeof(payload);
        airkiss_lan_ret_t packret = airkiss_lan_pack(AIRKISS_LAN_SSDP_NOTIFY_CMD,DEVICE_TYPE,DEVICE_ID, 0, 0, payload, &payload_len,&udp_airkiss_config); 
        if(packret != AIRKISS_LAN_PAKE_READY)
        {
            ESP_LOGI(TAG, "Pack lan packet error!");
            continue;
        }
        ESP_LOGI(TAG, "5s status of the lan packet %d", packret); 
        ESP_LOGI(TAG, "5s length of packet %d", payload_len); 
        int err = sendto(sock, (char*)payload, payload_len, 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
        if (err < 0) {
            ESP_LOGE(TAG, "5ms Error occured during sending: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "5ms Message send finish");

        // ESP_LOGI(TAG, "Waiting for data");
        // struct sockaddr_in sourceAddr;
        // socklen_t socklen = sizeof(sourceAddr);
        // int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&sourceAddr, &socklen);
        // // Error occured during receiving
        // if (len < 0) {
        //     ESP_LOGE(TAG, "recvfrom failed: errno %d", errno);
        //     break;
        // }
        // // Data received
        // else {
        //     rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string...
        //     airkiss_lan_ret_t sts = airkiss_lan_recv(rx_buffer,len,&udp_airkiss_config);
        //     ESP_LOGI(TAG, "status of the lan recv %d", sts); 
        //     switch(sts)
        //     {
        //         case AIRKISS_LAN_SSDP_REQ:
        //             ESP_LOGI(TAG, "enter switch case");
        //             payload_len = sizeof(payload); 
        //             airkiss_lan_ret_t packret = airkiss_lan_pack(AIRKISS_LAN_SSDP_RESP_CMD,DEVICE_TYPE,DEVICE_ID, 0, 0, payload, &payload_len,&udp_airkiss_config);                      
        //             ESP_LOGI(TAG, "status of the lan packet %d", packret); 
        //             ESP_LOGI(TAG, "length of packet %d", payload_len); 
        //             if(packret != AIRKISS_LAN_PAKE_READY)
        //             {
        //                 ESP_LOGI(TAG, "Pack lan packet error!");
        //                 continue;
        //             }
        //             inet_ntoa_r(((struct sockaddr_in *)&sourceAddr)->sin_addr.s_addr, addr_str, sizeof(addr_str) - 1);
        //             ESP_LOGI(TAG, "Received %d bytes from %s:", len, addr_str);
        //             int err = sendto(sock, payload, payload_len, 0, (struct sockaddr *)&sourceAddr, sizeof(sourceAddr));
        //             // int err = sendto(sock, rx_buffer, len, 0, (struct sockaddr *)&sourceAddr, sizeof(sourceAddr));        
        //             if (err < 0) {
        //                 ESP_LOGE(TAG, "Error occured during sending: errno %d", errno);
        //             }
        //         break;

        //         default:
        //             ESP_LOGE(TAG, "Pack is not ssdq req!");
        //         break;
        //     }
        // }
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
    if (sock != -1) {
        ESP_LOGE(TAG, "5ms Shutting down socket and restarting...");
        shutdown(sock, 0);
        close(sock);
    }
    vTaskDelete(NULL);
}

void socket_udp_server_init(void)
{
    // xTaskCreate(udp_server_task, "udp_server", 4096, NULL, 5, NULL);
    xTaskCreate(udp_client_time_5s_task, "udp_client", 4096, NULL, 5, NULL);
}