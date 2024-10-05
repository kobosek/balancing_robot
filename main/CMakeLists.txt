idf_component_register(SRCS 
                         "SensorTask.cpp" 
                         "PIDTask.cpp" 
                         "MotorControlTask.cpp" 
                         "TelemetryTask.cpp" 
                         "ConfigurationTask.cpp" 
                         "StateMachine.cpp" 
                         "MPU6050Manager.cpp" 
                         "RuntimeConfig.cpp"
                         "ComponentHandler.cpp"
                         "WebServer.cpp"                      
                         "PIDController.cpp"
                         "main.cpp"
                       INCLUDE_DIRS 
                         "."
                       REQUIRES 
                         esp_wifi
                         esp_event
                         esp_netif
                         esp_http_server
                         nvs_flash
                         esp_timer
                         spiffs
                         driver
                         json
                         HardwareAbstractionLayer)

set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/components/json/cJSON)
