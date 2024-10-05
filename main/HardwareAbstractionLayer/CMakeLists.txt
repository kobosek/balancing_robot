idf_component_register(SRCS 
                        "MPU6050.cpp"
                        "I2CBus.cpp"
                        "I2CDevice.cpp"
                        "GPIO.cpp"
                        "WiFiManager.cpp"
                       INCLUDE_DIRS 
                        "."
                       REQUIRES 
                        esp_wifi
                        esp_netif
                        nvs_flash
                        esp_timer
                        driver)

