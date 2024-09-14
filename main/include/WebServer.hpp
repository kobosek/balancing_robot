#pragma once

#include "interfaces/IWebServer.hpp"

class IComponentHandler;

class WebServer : public IWebServer {
    public:
        esp_err_t init(const IRuntimeConfig&) override;
        void update_telemetry(const TelemetryData& telemetry) override;
        bool hasConfigurationRequest() override;
        PIDConfig getConfigurationRequest() override;
        void notifyConfigurationUpdated() override;

    private:
        static constexpr const char* TAG = "WebServer";
        static constexpr int CONFIG_QUEUE_SIZE = 1;

        httpd_handle_t m_server;
        QueueHandle_t m_configRequestQueue;
        SemaphoreHandle_t m_telemetryMutex;
        TelemetryData m_lastTelemetry;
        bool m_configUpdated;

        static esp_err_t indexHandler(httpd_req_t *req);
        static esp_err_t telemetryHandler(httpd_req_t *req);
        static esp_err_t configHandler(httpd_req_t *req);

        void setupRoutes();
};