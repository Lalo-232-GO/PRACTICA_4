#include "servo_task.hpp"
#include "app_context.hpp"
#include "mesages.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

namespace App
{
    static const char *TAG = "SERVO";

    static uint32_t angle_to_duty(const ServoTaskConfig *cfg, uint8_t angle)
    {
        uint32_t max_duty = (1UL << cfg->resolution) - 1UL;
        uint32_t pulse_us = cfg->min_us + ((uint32_t)angle * (cfg->max_us - cfg->min_us)) / 180UL;
        return (pulse_us * max_duty * cfg->freq_hz) / 1000000UL;
    }

    static void servo_write_angle(const ServoTaskConfig *cfg, uint8_t angle)
    {
        uint32_t duty = angle_to_duty(cfg, angle);
        ledc_set_duty(cfg->mode, cfg->channel, duty);
        ledc_update_duty(cfg->mode, cfg->channel);
    }

    void ServoTask::run(void *pvParameters)
    {
        auto *cfg = static_cast<ServoTaskConfig *>(pvParameters);

        ledc_timer_config_t timer_cfg = {
            .speed_mode = cfg->mode,
            .duty_resolution = cfg->resolution,
            .timer_num = cfg->timer,
            .freq_hz = cfg->freq_hz,
            .clk_cfg = LEDC_AUTO_CLK
        };
        ESP_ERROR_CHECK(ledc_timer_config(&timer_cfg));

        ledc_channel_config_t ch_cfg = {
            .gpio_num = cfg->gpio,
            .speed_mode = cfg->mode,
            .channel = cfg->channel,
            .intr_type = LEDC_INTR_DISABLE,
            .timer_sel = cfg->timer,
            .duty = 0,
            .hpoint = 0
        };
        ESP_ERROR_CHECK(ledc_channel_config(&ch_cfg));

        uint8_t current_angle = 0;
        servo_write_angle(cfg, current_angle);

        while (true)
        {
            /*implementar tarea :)*/

        }
    }
}
