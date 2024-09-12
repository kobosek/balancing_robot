#include "include/RuntimeConfig.hpp"
#include <fstream>
#include "nvs_flash.h"
#include "esp_spiffs.h"
#include "dirent.h" 
#include "cJSON.h"

esp_err_t RuntimeConfig::init_nvs_Flash(void) { 
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

esp_err_t RuntimeConfig::init_spiffs(void) {
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
    ESP_LOGI(TAG, "Initializing RuntimeConfig from file: %s", filename.c_str());
    
    esp_err_t ret = init_spiffs();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize SPIFFS");
        return ret;
    }

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

    cJSON *wifi = cJSON_CreateObject();
    cJSON_AddStringToObject(wifi, "ssid", wifi_ssid.c_str());
    cJSON_AddStringToObject(wifi, "password", wifi_password.c_str());
    cJSON_AddItemToObject(root, "wifi", wifi);

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

    cJSON *mpu6050 = cJSON_CreateObject();
    cJSON_AddNumberToObject(mpu6050, "calibration_samples", mpu6050_calibration_samples);
    cJSON_AddItemToObject(root, "mpu6050", mpu6050);

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
    ESP_LOGD(TAG, "Parsing JSON to RuntimeConfig");
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
        ESP_LOGW(TAG, "WiFi configuration not found in JSON");
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
        ESP_LOGI(TAG, "Loaded PID configuration");
    } else {
        ESP_LOGW(TAG, "PID configuration not found in JSON");
    }

    cJSON *mpu6050 = cJSON_GetObjectItem(root, "mpu6050");
    if (mpu6050) {
        if ((item = cJSON_GetObjectItem(mpu6050, "calibration_samples")) && cJSON_IsNumber(item)) 
            mpu6050_calibration_samples = item->valueint;
        ESP_LOGI(TAG, "Loaded MPU6050 configuration");
    } else {
        ESP_LOGW(TAG, "MPU6050 configuration not found in JSON");
    }

    cJSON *main_loop = cJSON_GetObjectItem(root, "main_loop");
    if (main_loop) {
        if ((item = cJSON_GetObjectItem(main_loop, "interval_ms")) && cJSON_IsNumber(item)) 
            main_loop_interval_ms = item->valueint;
        ESP_LOGI(TAG, "Loaded main loop configuration");
    } else {
        ESP_LOGW(TAG, "Main loop configuration not found in JSON");
    }

    cJSON_Delete(root);
    ESP_LOGI(TAG, "Configuration loaded from JSON successfully");
    return ESP_OK;
}