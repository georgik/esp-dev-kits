/**
 * @file ui_main.c
 * @brief UI load and call stack related fuctions.
 * @version 0.1
 * @date 2021-01-10
 * 
 * @copyright Copyright 2021 Espressif Systems (Shanghai) Co. Ltd.
 *
 *      Licensed under the Apache License, Version 2.0 (the "License");
 *      you may not use this file except in compliance with the License.
 *      You may obtain a copy of the License at
 *
 *               http://www.apache.org/licenses/LICENSE-2.0
 *
 *      Unless required by applicable law or agreed to in writing, software
 *      distributed under the License is distributed on an "AS IS" BASIS,
 *      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *      See the License for the specific language governing permissions and
 *      limitations under the License.
 */

#include "ui_main.h"
#include "audio_hal.h"
#include "sys_check.h"

static const char *TAG = "ui_main";

#define LOG_TRACE(...)  ESP_LOGD(TAG, ##__VA_ARGS__)

void *data_icon_weather = NULL;
void *data_icon_humidity = NULL;
void *data_icon_uv = NULL;
void *data_icon_pm2p5 = NULL;
void *data_icon_pm2p5_press = NULL;

void *data_icon_provision = NULL;
void *data_icon_setting = NULL;
void *data_icon_setting_btn = NULL;
void *data_icon_app = NULL;
void *data_icon_location = NULL;
void *data_icon_about = NULL;

void *data_icon_qr = NULL;

void *bedroom = NULL;
void *living_room = NULL;
void *kitchen = NULL;
void *dining_room = NULL;
void *bathroom = NULL;

void *data_ico_auto = NULL;
void *data_ico_cool = NULL;
void *data_ico_heat = NULL;
void *data_ico_dry = NULL;
void *data_ico_fan = NULL;
void *data_ico_auto_down = NULL;
void *data_ico_cool_down = NULL;
void *data_ico_heat_down = NULL;
void *data_ico_dry_down = NULL;
void *data_ico_fan_down = NULL;

void *light_control = NULL;
void *air_conditioner = NULL;
void *smart_plug = NULL;
void *curtain = NULL;
void *fan = NULL;
void *video_entertainment = NULL;
void *smart_security = NULL;
void *other_device = NULL;

void *data_music_album[1];

void *data_ico_file_doc = NULL;
void *data_ico_file_image = NULL;
void *data_ico_file_music = NULL;

void *weather_icon_data[5] = { [0 ... 4] = NULL };

/* Resources loading list */
static ui_data_fetch_t img_fetch_list[] = {
    /* Main Clock Page */
    { "S:/Icon/101.bin", &data_icon_weather },
    { "S:/UI/humid.bin", &data_icon_humidity },
    { "S:/UI/uv.bin", &data_icon_uv },
    { "S:/UI/pm2p5.bin", &data_icon_pm2p5 },
    { "S:/UI/pm2p5_press.bin", &data_icon_pm2p5_press },

    /* Setting Page */
    { "S:/UI/provision.bin", &data_icon_provision },
    { "S:/UI/setting.bin", &data_icon_setting },
    { "S:/UI/setting_pressed.bin", &data_icon_setting_btn },
    { "S:/UI/app.bin", &data_icon_app },
    { "S:/UI/location.bin", &data_icon_location },
    { "S:/UI/about.bin", &data_icon_about },

    /* Provision page */
    { "S:/Icon/qr_esp_prov.bin", &data_icon_qr },
    
    /* Room Selection Page */
    { "S:/UI/bedroom.bin", &bedroom },
    { "S:/UI/living_room.bin", &living_room },
    { "S:/UI/kitchen.bin", &kitchen },
    { "S:/UI/dining_room.bin", &dining_room },
    { "S:/UI/bathroom.bin", &bathroom },

    /* Air Conditioner Control Page */
    { "S:/UI/icon_auto.bin", &data_ico_auto },
    { "S:/UI/icon_cool.bin", &data_ico_cool },
    { "S:/UI/icon_heat.bin", &data_ico_heat },
    { "S:/UI/icon_dry.bin", &data_ico_dry },
    { "S:/UI/icon_fan.bin", &data_ico_fan },
    { "S:/UI/icon_auto_down.bin", &data_ico_auto_down },
    { "S:/UI/icon_cool_down.bin", &data_ico_cool_down },
    { "S:/UI/icon_heat_down.bin", &data_ico_heat_down },
    { "S:/UI/icon_dry_down.bin", &data_ico_dry_down },
    { "S:/UI/icon_fan_down.bin", &data_ico_fan_down },

    /* Smart Switch Page */
    { "S:/UI/light_control.bin", &light_control },
    { "S:/UI/air_conditioner.bin", &air_conditioner },
    { "S:/UI/smart_plug.bin", &smart_plug },
    { "S:/UI/curtain.bin", &curtain },
    { "S:/UI/fan.bin", &fan },
    { "S:/UI/video_entertainment.bin", &video_entertainment },
    { "S:/UI/smart_security.bin", &smart_security },
    { "S:/UI/other_device.bin", &other_device },

    /* Music Player Album */
    { "S:/Pic/Album_0.bin", &(data_music_album[0]) },

    /* File Manager Page */
    { "S:/Icon/ico_file_doc.bin", &data_ico_file_doc },
    { "S:/Icon/ico_file_image.bin", &data_ico_file_image },
    { "S:/Icon/ico_file_music.bin", &data_ico_file_music },

    /* Weather Pager */
    { "S:/Icon/999.bin", &weather_icon_data[0] },
    { "S:/Icon/999.bin", &weather_icon_data[1] },
    { "S:/Icon/999.bin", &weather_icon_data[2] },
    { "S:/Icon/999.bin", &weather_icon_data[3] },
    { "S:/Icon/999.bin", &weather_icon_data[4] },
};

static void ui_init_internal(void);

static esp_err_t ui_call_stack_push(ui_func_desc_t *func);
static esp_err_t ui_call_stack_pop(ui_func_desc_t *func);
static esp_err_t ui_call_stack_peek(ui_func_desc_t *func);
static esp_err_t ui_call_stack_clear(void);

void ui_main(void)
{
    /* Init main screen */
    lv_port_sem_take();
    lv_obj_set_style_local_bg_color(lv_scr_act(), LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, COLOR_THEME);

    /* Show logo */
    LV_IMG_DECLARE(esp_logo)
    lv_obj_t *img = lv_img_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_bg_color(img, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_img_set_src(img, &esp_logo);
    lv_obj_align(img, NULL, LV_ALIGN_CENTER, 0, -50);
    lv_port_sem_give();

    if (ESP_OK != sys_check()) {
        while (1) {
            /* Stop at here if system check not pass */
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }

    lv_port_sem_take();
    /* Create a bar to update loading progress */
    lv_obj_t *bar = lv_bar_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_bg_color(bar, LV_BAR_PART_BG, LV_STATE_DEFAULT, COLOR_BG);
    lv_obj_set_style_local_border_color(bar, LV_BAR_PART_BG, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_border_color(bar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_bg_color(bar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, COLOR_DEEP);
    lv_obj_set_style_local_border_width(bar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, 2);
    lv_obj_set_size(bar, 400, 12);
    lv_obj_align(bar, img, LV_ALIGN_OUT_BOTTOM_MID, 0, 50);
    lv_bar_set_range(bar, 0, sizeof(img_fetch_list) / sizeof(ui_data_fetch_t));
    lv_bar_set_value(bar, 1, LV_ANIM_ON);

    lv_obj_t *label_loading_hint = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text_static(label_loading_hint, " ");
    lv_obj_set_style_local_text_font(label_loading_hint, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &font_en_24);
    lv_obj_set_style_local_text_color(label_loading_hint, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_align(label_loading_hint, bar, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    lv_port_sem_give();

    /* Load resource from SD crad to PSARM */
    TickType_t tick = xTaskGetTickCount();
    for (size_t i = 0; i < sizeof(img_fetch_list) / sizeof(ui_data_fetch_t); i++) {
        lv_port_sem_take();
        lv_bar_set_value(bar, i + 1, LV_ANIM_ON);
        lv_label_set_text_fmt(label_loading_hint, "Loading \"%s\"", img_fetch_list[i].path);
        lv_obj_align(label_loading_hint, bar, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
        lv_port_sem_give();

        if (ESP_OK != img_file_check(img_fetch_list[i].path)) {
            /* If any of file was missing, block the task */
            while (1) {
                vTaskDelay(1000);
            }
        }

        ui_laod_resource(img_fetch_list[i].path, img_fetch_list[i].data);

        /* Yeah, it's only because that makes ui more flexible */
        vTaskDelayUntil(&tick, pdMS_TO_TICKS(50));
    }

    vTaskDelay(pdMS_TO_TICKS(500));

    /* Remove image and bar, reset background color */
    lv_port_sem_take();
    lv_obj_del(img);
    lv_obj_del(bar);
    lv_obj_del(label_loading_hint);
    lv_obj_set_style_local_bg_color(lv_scr_act(), LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, COLOR_BG);
    lv_port_sem_give();

    /* Init call queue and pre-init widgets */
    ui_init_internal();

    /* Entering main UI */
    lv_port_sem_take();
    ui_status_bar_init();
    ui_clock_show(NULL);
    ui_call_stack_push(&ui_clock_func);
    lv_port_sem_give();
}

void ui_laod_resource(const char *path, void **dst)
{
    void *_data = NULL;
    lv_fs_file_t file_img;
    size_t file_size = 0;
    size_t real_size = 0;

    /* Try to open file */
    if (LV_FS_RES_OK != lv_fs_open(&file_img, path, LV_FS_MODE_RD)) {
        ESP_LOGE(TAG, "Can't open file : %s", path);
        return;
    }

    /* Get size of file */
    lv_fs_size(&file_img, &file_size);
    file_size -= 1;
    _data = heap_caps_malloc(file_size + 8, MALLOC_CAP_SPIRAM);
    lv_fs_read(&file_img, _data + 8, file_size, &real_size);
    lv_fs_close(&file_img);

    /* Change lv_img_dsc_t's data pointer */
    memmove(_data, _data + 8, 4);

    /* Set image information */
    *((uint32_t *)(_data + 4)) = file_size - 4;
    *((uint32_t *)(_data + 8)) = (uint32_t)(_data + 12);
    *dst = _data;
}

/* ******************************** CALL STACK ******************************** */
/**
 * @brief About call stack:
 *   - As for pages without back:
 *      ui_show(&ui_func_desc, UI_SHOW_OVERRIDE);
 *   - As for page with call queue:
 *      ui_show(&ui_func_desc, UI_SHOW_PEDDING);
 *   - As for back button:
 *      ui_show(&ui_func_desc, UI_SHOW_BACKPORT);
 */

static const int call_stack_size = 8;
typedef ui_func_desc_t call_stack_type_t;
static call_stack_type_t *call_stack = NULL;
static volatile size_t call_stack_index = 0;

/**
 * @brief An initialize task before UI start. Basicly create a call stack.
 * 
 */
static void ui_init_internal(void)
{
    assert(call_stack_size);
    call_stack = heap_caps_malloc(call_stack_size * sizeof(ui_func_desc_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
}

/**
 * @brief Push operator for call stack.
 * 
 * @param func pointer to `ui_func_desc_t`.
 * @return esp_err_t Push result.
 */
static esp_err_t ui_call_stack_push(ui_func_desc_t *func)
{
    if (call_stack_index <= call_stack_size - 1) {
        memcpy(&call_stack[call_stack_index], func, sizeof(call_stack_type_t));
        call_stack_index++;
        LOG_TRACE("Send : %s", func->name);
    } else {
        ESP_LOGE(TAG, "Call stack full");
        return ESP_ERR_NO_MEM;
    }

    return ESP_OK;
}

/**
 * @brief Pop operator for call stack.
 * 
 * @param func pointer to `ui_func_desc_t`.
 * @return esp_err_t Pop result.
 */
static esp_err_t ui_call_stack_pop(ui_func_desc_t *func)
{
    if (0 != call_stack_index) {
        call_stack_index--;
        memcpy(func, &call_stack[call_stack_index], sizeof(call_stack_type_t));
        LOG_TRACE("Recieve : %s", func->name);
    } else {
        ESP_LOGE(TAG, "Call queue empty");
        return ESP_FAIL;
    }
    
    return ESP_OK;
}

/**
 * @brief Peek top element from call stack. Keep data in call stack.
 * 
 * @param func pointer to `ui_func_desc_t`.
 * @return esp_err_t Peek result.
 */
static esp_err_t ui_call_stack_peek(ui_func_desc_t *func)
{
    if (0 != call_stack_index) {
        memcpy(func, &call_stack[call_stack_index - 1], sizeof(call_stack_type_t));
        LOG_TRACE("Peek : %s", func->name);
    } else {
        ESP_LOGE(TAG, "Call queue empty");
        return ESP_FAIL;
    }

    return ESP_OK;
}

/**
 * @brief Clear all data in call stack.
 * 
 * @return esp_err_t Always return `ESP_OK`.
 */
static esp_err_t ui_call_stack_clear(void)
{
    ui_func_desc_t func;

    while (call_stack_index) {
        call_stack_index--;
        memcpy(&func, &call_stack[call_stack_index], sizeof(call_stack_type_t));
        LOG_TRACE("Clear : %s", func.name);
    }

    call_stack_index = 0;

    return ESP_OK;
}

void ui_show(ui_func_desc_t *ui, ui_show_mode_t mode)
{
    static ui_func_desc_t ui_now;

    switch (mode) {
    case UI_SHOW_OVERRIDE:
        ui_call_stack_pop(&ui_now);
        ui_now.hide(NULL);
        ui->show(NULL);
        ui_call_stack_clear();
        ui_call_stack_push(ui);
        break;
    case UI_SHOW_PEDDING:
        ui_call_stack_peek(&ui_now);
        ui_now.hide(NULL);
        ui->show(NULL);
        ui_call_stack_push(ui);
        break;
    case UI_SHOW_BACKPORT:
        ui_call_stack_pop(&ui_now);
        ui_now.hide(NULL);
        ui_call_stack_peek(&ui_now);
        ui_now.show(NULL);
        break;
    }
}
