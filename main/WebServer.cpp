#include "include/WebServer.hpp"

#include <string.h>
#include <sstream>
#include "cJSON.h"

WebServer::WebServer()
    : m_server(nullptr), m_configRequestQueue(nullptr), m_telemetryMutex(nullptr), m_configUpdated(false) {
    m_configRequestQueue = xQueueCreate(CONFIG_QUEUE_SIZE, sizeof(PIDConfig));
    m_telemetryMutex = xSemaphoreCreateMutex();
}

WebServer::~WebServer() {
    if (m_server) {
        httpd_stop(m_server);
    }
    if (m_configRequestQueue) {
        vQueueDelete(m_configRequestQueue);
    }
    if (m_telemetryMutex) {
        vSemaphoreDelete(m_telemetryMutex);
    }
}

esp_err_t WebServer::init(const IRuntimeConfig&) {
    ESP_LOGI(TAG, "Initializing web server");
    httpd_config_t l_config = HTTPD_DEFAULT_CONFIG();
    l_config.lru_purge_enable = true;
    l_config.stack_size = 8192;

    esp_err_t ret = httpd_start(&m_server, &l_config);
    if (ret == ESP_OK) {
        setupRoutes();
        ESP_LOGI(TAG, "Web server started successfully");
    } else {
        ESP_LOGE(TAG, "Failed to start web server");
    }
    return ret;
}

void WebServer::setupRoutes() {
    httpd_uri_t index = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = indexHandler,
        .user_ctx = this
    };
    httpd_register_uri_handler(m_server, &index);

    httpd_uri_t telemetry = {
        .uri = "/telemetry",
        .method = HTTP_GET,
        .handler = telemetryHandler,
        .user_ctx = this
    };
    httpd_register_uri_handler(m_server, &telemetry);

    httpd_uri_t config = {
        .uri = "/config",
        .method = HTTP_POST,
        .handler = configHandler,
        .user_ctx = this
    };
    httpd_register_uri_handler(m_server, &config);
    ESP_LOGI(TAG, "All URI handlers registered");
}

void WebServer::update_telemetry(const TelemetryData& telemetry) {
    if (xSemaphoreTake(m_telemetryMutex, portMAX_DELAY) == pdTRUE) {
        m_lastTelemetry = telemetry;
        xSemaphoreGive(m_telemetryMutex);
    }
}

bool WebServer::hasConfigurationRequest() {
    return uxQueueMessagesWaiting(m_configRequestQueue) > 0;
}

PIDConfig WebServer::getConfigurationRequest() {
    PIDConfig config;
    if (xQueueReceive(m_configRequestQueue, &config, 0) == pdTRUE) {
        return config;
    }
    return PIDConfig(); // Return default config if queue is empty
}

void WebServer::notifyConfigurationUpdated() {
    m_configUpdated = true;
}

esp_err_t WebServer::indexHandler(httpd_req_t *req) {
    httpd_resp_send_type(req, "text/html");
    httpd_resp_sendstr(req, "<!DOCTYPE html><html><body><h1>Balancing Robot Control</h1></body></html>");
    return ESP_OK;
}


esp_err_t WebServer::telemetryHandler(httpd_req_t *req) {
    WebServer* server = static_cast<WebServer*>(req->user_ctx);
    TelemetryData telemetry;
    
    if (xSemaphoreTake(server->m_telemetryMutex, portMAX_DELAY) == pdTRUE) {
        telemetry = server->m_lastTelemetry;
        xSemaphoreGive(server->m_telemetryMutex);
    }

    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "pitch", telemetry.sensorData.pitch);
    cJSON_AddNumberToObject(root, "roll", telemetry.sensorData.roll);
    cJSON_AddNumberToObject(root, "yaw", telemetry.sensorData.yaw);
    cJSON_AddNumberToObject(root, "pidOutput", telemetry.pidOutput.output);
    cJSON_AddNumberToObject(root, "motorSpeed", telemetry.motorSpeed);

    char *json_str = cJSON_Print(root);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, json_str);

    free(json_str);
    cJSON_Delete(root);

    return ESP_OK;
}

esp_err_t WebServer::configHandler(httpd_req_t *req) {
    WebServer* server = static_cast<WebServer*>(req->user_ctx);
    char buf[100];
    int ret = httpd_req_recv(req, buf, sizeof(buf) - 1);
    if (ret <= 0) {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }
    buf[ret] = '\0';

    cJSON *root = cJSON_Parse(buf);
    if (root == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            ESP_LOGE(TAG, "JSON Parse Error before: %s", error_ptr);
        }
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    PIDConfig config;
    cJSON *kp = cJSON_GetObjectItem(root, "kp");
    cJSON *ki = cJSON_GetObjectItem(root, "ki");
    cJSON *kd = cJSON_GetObjectItem(root, "kd");
    cJSON *targetAngle = cJSON_GetObjectItem(root, "targetAngle");

    if (cJSON_IsNumber(kp)) config.kp = kp->valuedouble;
    if (cJSON_IsNumber(ki)) config.ki = ki->valuedouble;
    if (cJSON_IsNumber(kd)) config.kd = kd->valuedouble;
    if (cJSON_IsNumber(targetAngle)) config.targetAngle = targetAngle->valuedouble;

    cJSON_Delete(root);

    if (xQueueSend(server->m_configRequestQueue, &config, 0) != pdTRUE) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    httpd_resp_sendstr(req, "Configuration updated");
    return ESP_OK;
}