#include "mqtt_ibm.h"

/* FreeRTOS event group to signal when we are connected & ready to make a request */
EventGroupHandle_t wifi_event_group;

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
// const int CONNECTED_BIT = BIT0;

#define MQTT_CLIENT_THREAD_NAME         "mqtt_client_thread"
#define MQTT_CLIENT_THREAD_STACK_WORDS  4096
#define MQTT_CLIENT_THREAD_PRIO         8

static const char *TAG = "MQTT_IBM";
char MqttPubTopic[24] ={0};
char MqttSubTopic[24] ={0};
char *payload = NULL;
MQTTClient client;
int rc = 0;
uint32_t server_id,led_value;
EventGroupHandle_t led_event_group;

void mqttMessageSend(char *s)
{
    MQTTMessage message;
    message.qos = CONFIG_DEFAULT_MQTT_PUB_QOS;
    message.retained = 0;
    message.payload = s;
    message.payloadlen = strlen(s);

    if ((rc = MQTTPublish(&client, MqttPubTopic, &message)) != 0) {
        ESP_LOGE(TAG, "Return code from MQTT publish is %d", rc);
    } else {
        ESP_LOGI(TAG, "MQTT published topic %s, len:%u heap:%u", MqttPubTopic, message.payloadlen, esp_get_free_heap_size());
    }
}

static void messageArrived(MessageData *data)
{
    ESP_LOGI(TAG, "Message arrived[len:%u]: %.*s", \
           data->message->payloadlen, data->message->payloadlen, (char *)data->message->payload);

    if(0 == SwitchPacketParse((char *)data->message->payload,&server_id,&led_value))
    {
        gpio_set_level(16, led_value);
        xEventGroupSetBits(led_event_group, BIT0);
    }
}

static void mqtt_client_thread(void *pvParameters)
{

    Network network;   
    char clientID[32] = {0};

    led_event_group = xEventGroupCreate();

    ESP_LOGI(TAG, "ver:%u clientID:%s keepalive:%d username:%s passwd:%s session:%d level:%u",
             CONFIG_DEFAULT_MQTT_VERSION, CONFIG_MQTT_CLIENT_ID,
             CONFIG_MQTT_KEEP_ALIVE, CONFIG_MQTT_USERNAME, CONFIG_MQTT_PASSWORD,
             CONFIG_DEFAULT_MQTT_SESSION, CONFIG_DEFAULT_MQTT_SECURITY);

    ESP_LOGI(TAG, "broker:%s port:%u", CONFIG_MQTT_BROKER, CONFIG_MQTT_PORT);

    ESP_LOGI(TAG, "sendbuf:%u recvbuf:%u sendcycle:%u recvcycle:%u",
             CONFIG_MQTT_SEND_BUFFER, CONFIG_MQTT_RECV_BUFFER,
             CONFIG_MQTT_SEND_CYCLE, CONFIG_MQTT_RECV_CYCLE);

    MQTTPacket_connectData connectData = MQTTPacket_connectData_initializer;

    NetworkInit(&network);

    if (MQTTClientInit(&client, &network, 0, NULL, 0, NULL, 0) == false) {
        ESP_LOGE(TAG, "mqtt init err");
        vTaskDelete(NULL);
    }

    payload = malloc(CONFIG_MQTT_PAYLOAD_BUFFER);

    if (!payload) {
        ESP_LOGE(TAG, "mqtt malloc err");
    } else {
        memset(payload, 0x0, CONFIG_MQTT_PAYLOAD_BUFFER);
    }

    for (;;) {
        ESP_LOGI(TAG, "wait wifi connect...");
        xEventGroupWaitBits(wifi_event_group, ESPTOUCH_DONE_BIT, false, true, portMAX_DELAY);

        if ((rc = NetworkConnect(&network, CONFIG_MQTT_BROKER, CONFIG_MQTT_PORT)) != 0) {
            ESP_LOGE(TAG, "Return code from network connect is %d", rc);
            continue;
        }

        connectData.MQTTVersion = CONFIG_DEFAULT_MQTT_VERSION;
        sprintf(clientID, "%s_%u", CONFIG_MQTT_CLIENT_ID, esp_random());
        connectData.clientID.cstring = clientID;
        connectData.keepAliveInterval = CONFIG_MQTT_KEEP_ALIVE;

        connectData.username.cstring = CONFIG_MQTT_USERNAME;
        connectData.password.cstring = CONFIG_MQTT_PASSWORD;

        connectData.cleansession = CONFIG_DEFAULT_MQTT_SESSION;

        ESP_LOGI(TAG, "MQTT Connecting");

        if ((rc = MQTTConnect(&client, &connectData)) != 0) {
            ESP_LOGE(TAG, "Return code from MQTT connect is %d", rc);
            network.disconnect(&network);
            continue;
        }

        ESP_LOGI(TAG, "MQTT Connected");

#if defined(MQTT_TASK)

        if ((rc = MQTTStartTask(&client)) != pdPASS) {
            ESP_LOGE(TAG, "Return code from start tasks is %d", rc);
        } else {
            ESP_LOGI(TAG, "Use MQTTStartTask");
        }

#endif
        //获取mac地址（station模式）
        uint8_t mac[6];
        esp_read_mac(mac, ESP_MAC_WIFI_STA);
        sprintf(MqttPubTopic,"1/%02x%02x%02x%02x%02x%02x/set",mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        sprintf(MqttSubTopic,"1/%02x%02x%02x%02x%02x%02x/get",mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

        if ((rc = MQTTSubscribe(&client, MqttSubTopic, CONFIG_DEFAULT_MQTT_SUB_QOS, messageArrived)) != 0) {
            ESP_LOGE(TAG, "Return code from MQTT subscribe is %d", rc);
            network.disconnect(&network);
            continue;
        }

        ESP_LOGI(TAG, "MQTT subscribe to topic %s OK", MqttSubTopic);
        for (;;) { 
            xEventGroupWaitBits(led_event_group, BIT0, false, true, portMAX_DELAY);
            AckPushPacket(server_id,led_value,(char*)payload);
            mqttMessageSend((char*)payload);
            xEventGroupClearBits(led_event_group, BIT0);    
        }
        network.disconnect(&network);
    }
    ESP_LOGW(TAG, "mqtt_client_thread going to be deleted");
    vTaskDelete(NULL);
    return;
}

static void msg_update(void *pvParameters)
{
    xEventGroupWaitBits(wifi_event_group, ESPTOUCH_DONE_BIT, false, true, portMAX_DELAY);
    for (;;) {
        SensorPushPacket((char*)payload);
        mqttMessageSend((char*)payload);
        vTaskDelay(30000 / portTICK_RATE_MS);
    }
}

void mqtt_ibm_init(void)
{
    esp_err_t ret = xTaskCreate(&mqtt_client_thread,
                      MQTT_CLIENT_THREAD_NAME,
                      MQTT_CLIENT_THREAD_STACK_WORDS,
                      NULL,
                      MQTT_CLIENT_THREAD_PRIO,
                      NULL);

    if (ret != pdPASS)  {
        ESP_LOGE(TAG, "mqtt create client thread %s failed", MQTT_CLIENT_THREAD_NAME);
    }

    ret = xTaskCreate(&msg_update,
                    "msg_update_task",
                    MQTT_CLIENT_THREAD_STACK_WORDS,
                    NULL,
                    MQTT_CLIENT_THREAD_PRIO,
                    NULL);

    if (ret != pdPASS)  {
        ESP_LOGE(TAG, "msg update %s failed", MQTT_CLIENT_THREAD_NAME);
    }
}