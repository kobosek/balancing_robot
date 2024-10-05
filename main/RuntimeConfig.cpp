#include "include/RuntimeConfig.hpp"
#include <fstream>
#include "dirent.h" 
#include "cJSON.h"


RuntimeConfig::RuntimeConfig() {
    m_mutex = xSemaphoreCreateMutex();
}

RuntimeConfig::~RuntimeConfig() {
    if (m_mutex) {
        vSemaphoreDelete(m_mutex);
    }
}

esp_err_t RuntimeConfig::initNVS(void) { 
    ESP_LOGI(TAG, "Initializing NVS Flash");
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS partition was truncated and needs to be erased");
        ret = nvs_flash_erase();
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to erase NVS: %s", esp_err_to_name(ret));
            return ret;
        }
        ret = nvs_flash_init();
    }
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize NVS Flash: %s", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "NVS Flash initialized successfully");
    }
    return ret;
}

esp_err_t RuntimeConfig::initSPIFFS() {
    ESP_LOGI(TAG, "Initializing SPIFFS");
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = "storage",
        .max_files = 5,
        .format_if_mount_failed = true
    };
    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        return ret;
    }
    
    size_t total = 0, used = 0;
    ret = esp_spiffs_info(conf.partition_label, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "SPIFFS Partition size: total: %d, used: %d", total, used);
    }
    
    DIR* dir = opendir("/spiffs");
    if (!dir) {
        ESP_LOGE(TAG, "Failed to open directory %s", "/spiffs");
        return ESP_FAIL;
    }
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        ESP_LOGD(TAG, "Found file: %s", entry->d_name);
    }
    closedir(dir);   

    ESP_LOGI(TAG, "SPIFFS initialized successfully");
    return ESP_OK;
}

esp_err_t RuntimeConfig::init(const std::string& filename) {
    ESP_LOGI(TAG, "Initializing RuntimeConfig");

    esp_err_t ret = initNVS();
    if (ret != ESP_OK) return ret;

    ret = initSPIFFS();
    if (ret != ESP_OK) return ret;

    std::ifstream file(filename);
    if (!file.is_open()) {
        ESP_LOGE(TAG, "Failed to open config file: %s", filename.c_str());
        return ESP_FAIL;
    }
    std::string json((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    ret = fromJson(json);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "RuntimeConfig initialized successfully from %s", filename.c_str());
    } else {
        ESP_LOGE(TAG, "Failed to parse JSON from config file");
    }
    return ret;
}

esp_err_t RuntimeConfig::save(const std::string& filename) const {
    ESP_LOGI(TAG, "Saving RuntimeConfig to file: %s", filename.c_str());
    std::string json = toJson();
    std::ofstream file(filename);
    if (!file.is_open()) {
        ESP_LOGE(TAG, "Failed to open config file for writing: %s", filename.c_str());
        return ESP_FAIL;
    }

    file << json;
    file.close();

    ESP_LOGI(TAG, "RuntimeConfig saved successfully");
    return ESP_OK;
}

std::string RuntimeConfig::toJson() const {
    ESP_LOGD(TAG, "Converting RuntimeConfig to JSON");
    cJSON *root = cJSON_CreateObject();

    if (xSemaphoreTake(m_mutex, portMAX_DELAY) == pdTRUE) {
        cJSON *wifi = cJSON_CreateObject();
        cJSON_AddStringToObject(wifi, "ssid", m_wifiSSID.c_str());
        cJSON_AddStringToObject(wifi, "password", m_wifiPassword.c_str());
        cJSON_AddItemToObject(root, "wifi", wifi);

        cJSON *pid = cJSON_CreateObject();
        cJSON_AddNumberToObject(pid, "kp", m_pidConfig.kp);
        cJSON_AddNumberToObject(pid, "ki", m_pidConfig.ki);
        cJSON_AddNumberToObject(pid, "kd", m_pidConfig.kd);
        cJSON_AddNumberToObject(pid, "target_angle", m_pidConfig.targetAngle);
        cJSON_AddNumberToObject(pid, "iterm_min", m_pidConfig.itermMin);
        cJSON_AddNumberToObject(pid, "iterm_max", m_pidConfig.itermMax);
        cJSON_AddNumberToObject(pid, "output_min", m_pidConfig.outputMin);
        cJSON_AddNumberToObject(pid, "output_max", m_pidConfig.outputMax);

        cJSON_AddItemToObject(root, "pid", pid);

        cJSON *mpu6050 = cJSON_CreateObject();
        cJSON_AddNumberToObject(mpu6050, "calibration_samples", m_mpuCalibrationSamples);
        cJSON_AddItemToObject(root, "mpu6050", mpu6050);

        cJSON *main_loop = cJSON_CreateObject();
        cJSON_AddNumberToObject(main_loop, "interval_ms", m_mainLoopIntervalSamples);
        cJSON_AddItemToObject(root, "main_loop", main_loop);
        xSemaphoreGive(m_mutex);
    }

    char *json_str = cJSON_Print(root);
    std::string result(json_str);
    free(json_str);
    cJSON_Delete(root);

    return result;
}

esp_err_t RuntimeConfig::fromJson(const std::string& json) {
    ESP_LOGD(TAG, "Parsing JSON to RuntimeConfig");
    cJSON *root = cJSON_Parse(json.c_str());
    if (root == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            ESP_LOGE(TAG, "JSON Parse Error before: %s", error_ptr);
        }
        return ESP_FAIL;
    }

    if (xSemaphoreTake(m_mutex, portMAX_DELAY) == pdTRUE) {
        cJSON *item = NULL;
        cJSON *wifi = cJSON_GetObjectItem(root, "wifi");
        if (wifi) {
            if ((item = cJSON_GetObjectItem(wifi, "ssid")) && cJSON_IsString(item)) {
                m_wifiSSID = item->valuestring;
                ESP_LOGI(TAG, "Loaded WiFi SSID: %s", m_wifiSSID.c_str());
            }
            if ((item = cJSON_GetObjectItem(wifi, "password")) && cJSON_IsString(item)) {
                m_wifiPassword = item->valuestring;
                ESP_LOGI(TAG, "Loaded WiFi password (not shown for security)");
            }
        } else {
            ESP_LOGW(TAG, "WiFi configuration not found in JSON");
        }

        cJSON *pid = cJSON_GetObjectItem(root, "pid");
        if (pid) {
            if ((item = cJSON_GetObjectItem(pid, "kp")) && cJSON_IsNumber(item)) m_pidConfig.kp = item->valuedouble;
            if ((item = cJSON_GetObjectItem(pid, "ki")) && cJSON_IsNumber(item)) m_pidConfig.ki = item->valuedouble;
            if ((item = cJSON_GetObjectItem(pid, "kd")) && cJSON_IsNumber(item)) m_pidConfig.kd = item->valuedouble;
            if ((item = cJSON_GetObjectItem(pid, "target_angle")) && cJSON_IsNumber(item)) m_pidConfig.targetAngle = item->valuedouble;
            if ((item = cJSON_GetObjectItem(pid, "output_min")) && cJSON_IsNumber(item)) m_pidConfig.outputMin = item->valuedouble;
            if ((item = cJSON_GetObjectItem(pid, "output_max")) && cJSON_IsNumber(item)) m_pidConfig.outputMax = item->valuedouble;
            if ((item = cJSON_GetObjectItem(pid, "iterm_min")) && cJSON_IsNumber(item)) m_pidConfig.itermMin = item->valuedouble;
            if ((item = cJSON_GetObjectItem(pid, "iterm_max")) && cJSON_IsNumber(item)) m_pidConfig.itermMax = item->valuedouble;
            ESP_LOGI(TAG, "Loaded PID configuration");
        } else {
            ESP_LOGW(TAG, "PID configuration not found in JSON");
        }

        cJSON *mpu6050 = cJSON_GetObjectItem(root, "mpu6050");
        if (mpu6050) {
            if ((item = cJSON_GetObjectItem(mpu6050, "calibration_samples")) && cJSON_IsNumber(item)) 
                m_mpuCalibrationSamples = item->valueint;
            ESP_LOGI(TAG, "Loaded MPU6050 configuration");
        } else {
            ESP_LOGW(TAG, "MPU6050 configuration not found in JSON");
        }

        cJSON *main_loop = cJSON_GetObjectItem(root, "main_loop");
        if (main_loop) {
            if ((item = cJSON_GetObjectItem(main_loop, "interval_ms")) && cJSON_IsNumber(item)) 
                m_mainLoopIntervalSamples = item->valueint;
            ESP_LOGI(TAG, "Loaded main loop configuration");
        } else {
            ESP_LOGW(TAG, "Main loop configuration not found in JSON");
        }
        xSemaphoreGive(m_mutex);
    }

    cJSON_Delete(root);
    ESP_LOGI(TAG, "Configuration loaded from JSON successfully");
    return ESP_OK;
}

PIDConfig RuntimeConfig::getPidConfig() const  { 
    if (xSemaphoreTake(m_mutex, portMAX_DELAY) == pdTRUE) {
        PIDConfig config = m_pidConfig;
        xSemaphoreGive(m_mutex);
        return config;
    }
    return PIDConfig(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f); 
}
void RuntimeConfig::setPidConfig(PIDConfig config) { 
    if (xSemaphoreTake(m_mutex, portMAX_DELAY) == pdTRUE) {
        m_pidConfig = config;
        xSemaphoreGive(m_mutex);
    }
};

int RuntimeConfig::getMpu6050CalibrationSamples() const { 
    if (xSemaphoreTake(m_mutex, portMAX_DELAY) == pdTRUE) {
        int value = m_mpuCalibrationSamples;
        xSemaphoreGive(m_mutex);
        return value;
    }
    return 0;
}
void RuntimeConfig::setMpu6050CalibrationSamples(int value) { 
    if (xSemaphoreTake(m_mutex, portMAX_DELAY) == pdTRUE) {
        m_mpuCalibrationSamples = value; 
        xSemaphoreGive(m_mutex);
    }
}

int RuntimeConfig::getMainLoopIntervalMs() const { 
    if (xSemaphoreTake(m_mutex, portMAX_DELAY) == pdTRUE) {
        int value = m_mainLoopIntervalSamples;
        xSemaphoreGive(m_mutex);
        return value;
    }
    return 0;
}
void RuntimeConfig::setMainLoopIntervalMs(int value) { 
    if (xSemaphoreTake(m_mutex, portMAX_DELAY) == pdTRUE) {
        m_mainLoopIntervalSamples = value; 
        xSemaphoreGive(m_mutex);
    }
}

std::string RuntimeConfig::getWifiSsid() const { 
    if (xSemaphoreTake(m_mutex, portMAX_DELAY) == pdTRUE) {
        std::string value = m_wifiSSID;
        xSemaphoreGive(m_mutex);
        return value;
    }
    return std::string();
}
std::string RuntimeConfig::getWifiPassword() const { 
    if (xSemaphoreTake(m_mutex, portMAX_DELAY) == pdTRUE) {
        std::string value = m_wifiPassword;
        xSemaphoreGive(m_mutex);
        return value;
    }
    return std::string();
}

void RuntimeConfig::setWifiSsid(const std::string& value) { 
    if (xSemaphoreTake(m_mutex, portMAX_DELAY) == pdTRUE) {
        m_wifiSSID = value; 
        xSemaphoreGive(m_mutex);
    }
}

void RuntimeConfig::setWifiPassword(const std::string& value) { 
    if (xSemaphoreTake(m_mutex, portMAX_DELAY) == pdTRUE) {
        m_wifiPassword = value; 
        xSemaphoreGive(m_mutex);
    }
}

