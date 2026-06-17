#include "task_manager.hpp"
#include "app_config.hpp"
#include "app_context.hpp"
#include "mesages.hpp"
#include "sensor_task.hpp"
#include "button_task.hpp"
#include "servo_task.hpp"
#include "ready_led_task.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

namespace App
{
    static const char *TAG = "MANAGER";

    static SensorTaskConfig sensor_cfg 
    { 
        /* CONFIGURACION DEL SENSOR :)*/
        .channel = LDR_ADC_CHANNEL,
        .filter_window = FILTER_WINDOW_SIZE,
        .name = "SENSOR_LDR",
        .period_ms = SENSOR_PERIOD_MS,
        .unit_id = LDR_ADC_UNIT
    };

    static ServoTaskConfig servo_cfg 
    { 
        /*CONFIGURACION DEL SERVO :)*/
        .channel = SERVO_PWM_CHANNEL,
        .freq_hz = SERVO_PWM_FREQ_HZ,
        .gpio = SERVO_GPIO,
        .max_us = SERVO_MAX_US,
        .min_us = SERVO_MIN_US,
        .mode = SERVO_PWM_MODE,
        .name = "SERVO",
        .resolution = SERVO_PWM_RES_BITS,
        .timer = SERVO_PWM_TIMER
    };

    static ButtonTaskConfig start_btn_cfg 
    { 
        /* CONFIGURACION DEL BOTON DE INICIO :)*/
        .gpio = START_BUTTON_GPIO,
        .name = "BTN_START",
        .poll_ms = BUTTON_POLL_MS,
        .send_on_change_only = true // Revisar si sí es true xd
    };

    static ButtonTaskConfig speed_btn_cfg 
    { 
        /* CONFIGURACION DEL BOTON DE VELOCIDAD :)*/
        .gpio = SPEED_BUTTON_GPIO,
        .name = "BTN_SPEED",
        .poll_ms = BUTTON_POLL_MS,
        .send_on_change_only = true // Revisar si sí es true xd
    };

    static ReadyLedTaskConfig ready_led_cfg
    {
        /* CONFIGURACION DEL LED LISTO :)*/
        .gpio = READY_LED_GPIO,
        .name = "LED_READY",
        .off_ms = READY_LED_OFF_MS,
        .on_ms = READY_LED_ON_MS
    };

    static ManagerTaskConfig manager_cfg 
    { 
        /* CONFIGURACION DEL ADMINISTRADOR DE TAREAS :)*/
        .fast_delay_ms = SERVO_DELAY_FAST_MS,
        .hold_target_ms = HOLD_TARGET_MS,
        .name = "MANAGER",
        .slow_delay_ms = SERVO_DELAY_SLOW_MS,
        .step_deg = SERVO_STEP_DEG,
        .tolerance_deg = SERVO_TOLERANCE_DEG
    };

    static const char *state_text(eTaskState st)
    {
        switch (st)
        {
            case eRunning: 
                return "RUNNING";
            case eReady: 
                return "READY";
            case eBlocked: 
                return "BLOCKED";
            case eSuspended: 
                return "SUSPENDED";
            case eDeleted: 
                return "DELETED";
            default: 
                return "UNKNOWN";
        }
    }

    static void send_servo_cmd(uint8_t target, bool fast, const ManagerTaskConfig *cfg)
    {
        ServoCmd cmd 
        { 
            target, 
            cfg->tolerance_deg, 
            fast ? cfg->fast_delay_ms : cfg->slow_delay_ms, 
            cfg->step_deg 
        };
        xQueueOverwrite(g_queues.servo_cmd, &cmd);
    }

    static void print_states()
    {
        ESP_LOGI(TAG, "STATES sensor=%s servo=%s start=%s speed=%s ready=%s",
                 state_text(eTaskGetState(g_handles.sensor)),
                 state_text(eTaskGetState(g_handles.servo)),
                 state_text(eTaskGetState(g_handles.start_button)),
                 state_text(eTaskGetState(g_handles.speed_button)),
                 state_text(eTaskGetState(g_handles.ready_led)));
    }

    void TaskManager::run(void *pvParameters)
    {
        auto *cfg = static_cast<ManagerTaskConfig *>(pvParameters);

        bool fast_mode = false;
        bool operating = false;
        bool reached = false;
        uint8_t current_target = AppConfig::SERVO_ANGLE_DARK;
        TickType_t reached_tick = 0;

        vTaskSuspend(g_handles.sensor);
        vTaskSuspend(g_handles.servo);
        vTaskSuspend(g_handles.speed_button);
        vTaskResume(g_handles.ready_led);

        while (true)
        {
                /* IMPLEMENTAR TAREA TASK MANAGER :)*/
            //
            ButtonEventType event;
            vTaskDelay(pdMS_TO_TICKS(20));
        }
    }

    void app_tasks_create()
    {
        /* VERIFIQUEN LA PRIORIDAD DE LAS TAREAS y/o AJUSTENLAS EN FUNCION DE SUS RESULTADOS*/
        g_queues.sensor = xQueueCreate(1, sizeof(SensorMsg));
        g_queues.buttons = xQueueCreate(AppConfig::BUTTON_QUEUE_LEN, sizeof(ButtonMsg));
        g_queues.servo_cmd = xQueueCreate(1, sizeof(ServoCmd));
        g_queues.servo_status = xQueueCreate(1, sizeof(ServoStatusMsg));

        xTaskCreate(SensorTask::run, sensor_cfg.name, 1024, &sensor_cfg, 30, &g_handles.sensor);
        xTaskCreate(ServoTask::run, servo_cfg.name, 1024, &servo_cfg, 30, &g_handles.servo);
        xTaskCreate(ButtonTask::run, start_btn_cfg.name, 1024, &start_btn_cfg, 40, &g_handles.start_button);
        xTaskCreate(ButtonTask::run, speed_btn_cfg.name, 1024, &speed_btn_cfg, -4, &g_handles.speed_button);
        xTaskCreate(ReadyLedTask::run, ready_led_cfg.name, 1024, &ready_led_cfg, -1, &g_handles.ready_led);
        xTaskCreate(TaskManager::run, manager_cfg.name, 1024, &manager_cfg, -5, &g_handles.manager);
    }
}
