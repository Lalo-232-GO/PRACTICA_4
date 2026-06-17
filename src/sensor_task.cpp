#include "sensor_task.hpp"
#include "app_config.hpp"
#include "app_context.hpp"
#include "mesages.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

namespace App
{
    static const char *TAG = "SENSOR";

    static uint16_t median_u16(uint16_t *v, uint8_t n)
    {

        /* AQUI SE IMPLEMENTA EL FILTRO :) */
        

        return v[n / 2];
    }

    static uint8_t target_from_ldr(uint16_t filtered)
    {
        if (filtered >= AppConfig::LDR_THRESHOLD_HIGH)
        {
            return AppConfig::SERVO_ANGLE_LIGHT;
        }
            
        if (filtered <= AppConfig::LDR_THRESHOLD_LOW) 
        {
            return AppConfig::SERVO_ANGLE_DARK;
        }
        return (filtered > ((AppConfig::LDR_THRESHOLD_LOW + AppConfig::LDR_THRESHOLD_HIGH) / 2)) ? AppConfig::SERVO_ANGLE_LIGHT : AppConfig::SERVO_ANGLE_DARK;
    }

    void SensorTask::run(void *pvParameters)
    {
        auto *cfg = static_cast<SensorTaskConfig *>(pvParameters);

        adc_oneshot_unit_handle_t adc_handle;
        adc_oneshot_unit_init_cfg_t unit_cfg = { .unit_id = cfg->unit_id };
        ESP_ERROR_CHECK(adc_oneshot_new_unit(&unit_cfg, &adc_handle));

        adc_oneshot_chan_cfg_t chan_cfg = { .atten = ADC_ATTEN_DB_12, .bitwidth = ADC_BITWIDTH_12 };
        ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, cfg->channel, &chan_cfg));

        ESP_LOGI(TAG, "%s iniciado", cfg->name);

        while (true)
        {
            /* IMPLEMENTAR TAREA :)*/

            vTaskDelay(pdMS_TO_TICKS(cfg->period_ms));
        }
    }
}
