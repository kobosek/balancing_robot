#include "include/RuntimeConfig.hpp"
#include "esp_log.h"
#include <fstream>
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_spiffs.h"
#include "dirent.h" 
#include "cJSON.h"

static const char* TAG = "RuntimeConfig";

esp_err_t RuntimeConfig::init_nvs_Flash(void) { 
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    return ret;
}

esp_err_t RuntimeConfig::init_spiffs(void)
{
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
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }
    
    // Add this line to list SPIFFS contents after initialization
    DIR* dir = opendir("/spiffs");
    if (!dir) {
        ESP_LOGE(TAG, "Failed to open directory %s", "/spiffs");
        return ret;
    }
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        ESP_LOGI(TAG, "Found file: %s", entry->d_name);
    }

    closedir(dir);   
    return ESP_OK;
}

esp_err_t RuntimeConfig::init(const std::string& p_filename) {
    
    init_spiffs();
    std::ifstream file(p_filename);
    if (!file.is_open()) {
        ESP_LOGE(TAG, "Failed to open config file: %s", p_filename.c_str());
        return ESP_FAIL;
    }

    std::string json((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    esp_err_t result = fromJson(json);
    if (result == ESP_OK) {
        ESP_LOGI(TAG, "RuntimeConfig initialized successfully from %s", p_filename.c_str());
    }
    return result;
}

esp_err_t RuntimeConfig::save(const std::string& filename) const {
    std::string json = toJson();
    std::ofstream file(filename);
    if (!file.is_open()) {
        ESP_LOGE(TAG, "Failed to open config file for writing: %s", filename.c_str());
        return ESP_FAIL;
    }

    file << json;
    file.close();

    return ESP_OK;
}

std::string RuntimeConfig::toJson() const {
    cJSON *root = cJSON_CreateObject();

    // WiFi configuration
    cJSON *wifi = cJSON_CreateObject();
    cJSON_AddStringToObject(wifi, "ssid", wifi_ssid.c_str());
    cJSON_AddStringToObject(wifi, "password", wifi_password.c_str());
    cJSON_AddItemToObject(root, "wifi", wifi);

    // PID configuration
    cJSON *pid = cJSON_CreateObject();
    cJSON_AddNumberToObject(pid, "kp", pid_kp);
    cJSON_AddNumberToObject(pid, "ki", pid_ki);
    cJSON_AddNumberToObject(pid, "kd", pid_kd);
    cJSON_AddNumberToObject(pid, "target_angle", pid_target_angle);
    cJSON_AddNumberToObject(pid, "output_min", pid_output_min);
    cJSON_AddNumberToObject(pid, "output_max", pid_output_max);
    cJSON_AddNumberToObject(pid, "iterm_min", pid_iterm_min);
    cJSON_AddNumberToObject(pid, "iterm_max", pid_iterm_max);
    cJSON_AddItemToObject(root, "pid", pid);

    // MPU6050 configuration
    cJSON *mpu6050 = cJSON_CreateObject();
    cJSON_AddNumberToObject(mpu6050, "calibration_samples", mpu6050_calibration_samples);
    cJSON_AddItemToObject(root, "mpu6050", mpu6050);

    // Main loop configuration
    cJSON *main_loop = cJSON_CreateObject();
    cJSON_AddNumberToObject(main_loop, "interval_ms", main_loop_interval_ms);
    cJSON_AddItemToObject(root, "main_loop", main_loop);

    char *json_str = cJSON_Print(root);
    std::string result(json_str);
    free(json_str);
    cJSON_Delete(root);

    return result;
}

esp_err_t RuntimeConfig::fromJson(const std::string& json) {
    cJSON *root = cJSON_Parse(json.c_str());
    if (root == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            ESP_LOGE(TAG, "JSON Parse Error before: %s", error_ptr);
        }
        return ESP_FAIL;
    }

    cJSON *item = NULL;
    cJSON *wifi = cJSON_GetObjectItem(root, "wifi");
    if (wifi) {
        if ((item = cJSON_GetObjectItem(wifi, "ssid")) && cJSON_IsString(item)) {
            wifi_ssid = item->valuestring;
            ESP_LOGI(TAG, "Loaded WiFi SSID: %s", wifi_ssid.c_str());
        }
        if ((item = cJSON_GetObjectItem(wifi, "password")) && cJSON_IsString(item)) {
            wifi_password = item->valuestring;
            ESP_LOGI(TAG, "Loaded WiFi password (not shown for security)");
        }
    } else {
        ESP_LOGE(TAG, "WiFi configuration not found in JSON");
    }

    cJSON *pid = cJSON_GetObjectItem(root, "pid");
    if (pid) {
        if ((item = cJSON_GetObjectItem(pid, "kp")) && cJSON_IsNumber(item)) pid_kp = item->valuedouble;
        if ((item = cJSON_GetObjectItem(pid, "ki")) && cJSON_IsNumber(item)) pid_ki = item->valuedouble;
        if ((item = cJSON_GetObjectItem(pid, "kd")) && cJSON_IsNumber(item)) pid_kd = item->valuedouble;
        if ((item = cJSON_GetObjectItem(pid, "target_angle")) && cJSON_IsNumber(item)) pid_target_angle = item->valuedouble;
        if ((item = cJSON_GetObjectItem(pid, "output_min")) && cJSON_IsNumber(item)) pid_output_min = item->valuedouble;
        if ((item = cJSON_GetObjectItem(pid, "output_max")) && cJSON_IsNumber(item)) pid_output_max = item->valuedouble;
        if ((item = cJSON_GetObjectItem(pid, "iterm_min")) && cJSON_IsNumber(item)) pid_iterm_min = item->valuedouble;
        if ((item = cJSON_GetObjectItem(pid, "iterm_max")) && cJSON_IsNumber(item)) pid_iterm_max = item->valuedouble;
    } else {
        ESP_LOGE(TAG, "PID configuration not found in JSON");
    }

    cJSON *mpu6050 = cJSON_GetObjectItem(root, "mpu6050");
    if (mpu6050) {
        if ((item = cJSON_GetObjectItem(mpu6050, "calibration_samples")) && cJSON_IsNumber(item)) 
            mpu6050_calibration_samples = item->valueint;
    } else {
        ESP_LOGE(TAG, "MPU6050 configuration not found in JSON");
    }

    cJSON *main_loop = cJSON_GetObjectItem(root, "main_loop");
    if (main_loop) {
        if ((item = cJSON_GetObjectItem(main_loop, "interval_ms")) && cJSON_IsNumber(item)) 
            main_loop_interval_ms = item->valueint;
    } else {
        ESP_LOGE(TAG, "Main loop configuration not found in JSON");
    }

    cJSON_Delete(root);
    ESP_LOGI(TAG, "Configuration loaded from JSON");
    return ESP_OK;
}