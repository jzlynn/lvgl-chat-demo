#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define DISP_BUF_SIZE (128 * 1024)

int main(void)
{
    /*LittlevGL init*/
    lv_init();

    /*Linux frame buffer device init*/
    fbdev_init();

    /*A small buffer for LittlevGL to draw the screen's content*/
    static lv_color_t buf[DISP_BUF_SIZE];

    /*Initialize a descriptor for the buffer*/
    static lv_disp_draw_buf_t disp_buf;
    lv_disp_draw_buf_init(&disp_buf, buf, NULL, DISP_BUF_SIZE);

    /*Initialize and register a display driver*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf   = &disp_buf;
    disp_drv.flush_cb   = fbdev_flush;
    disp_drv.hor_res    = 240;
    disp_drv.ver_res    = 320;

    disp_drv.rotated = LV_DISP_ROT_270;
    disp_drv.sw_rotate = 1;

    lv_disp_drv_register(&disp_drv);

    lv_obj_t *scr = lv_scr_act();

    //horizontal_background_image
    lv_obj_t *img_bg = lv_img_create(scr);
    lv_img_set_src(img_bg, &hor_bg2);
    lv_obj_align(img_bg, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_size(img_bg, LV_HOR_RES, LV_VER_RES);

    // Apply opacity style
    static lv_style_t style_bg;
    lv_style_init(&style_bg);
    lv_style_set_img_opa(&style_bg, LV_OPA_90);
    lv_obj_add_style(img_bg, &style_bg, 0);

    // Create a label for the clock
    clock_label = lv_label_create(scr);
    // lv_obj_align(clock_label, LV_ALIGN_TOP_MID, 0, 20);
    lv_obj_set_align(clock_label, LV_ALIGN_CENTER);
    update_clock(clock_label);
    lv_timer_create(timer_update_cb, 1000, clock_label);

    // Create a label for displaying responses from the pipe
    response_label = lv_label_create(scr);
    lv_obj_set_width(response_label, lv_pct(80));
    lv_obj_set_height(response_label, LV_SIZE_CONTENT);
    lv_obj_set_align(response_label, LV_ALIGN_TOP_MID);
    // lv_obj_align(response_label, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_long_mode(response_label, LV_LABEL_LONG_WRAP); //LV_LABEL_LONG_SCROLL LV_LABEL_LONG_WRAP
    lv_obj_set_scroll_dir(response_label, LV_DIR_VER);
    lv_obj_set_scroll_snap_y(response_label, LV_SCROLL_SNAP_CENTER);  // 设置滚动方向：Y
    lv_label_set_text(response_label, "");

    lv_obj_set_style_text_font(response_label, &lv_font_lxgw_24, LV_STATE_DEFAULT); //lv_font_song_all_16 lv_font_song_common3k5_14,lv_font_sans_common3k5_16

    // lv_timer_create(update_label_task, 300, NULL);
    lv_timer_create(scroll_label, 10, response_label); //创建定时器，每30毫秒更新一次标签位置

    pthread_t tid;
    pthread_mutex_init(&lock, NULL);
    pthread_create(&tid, NULL, read_pipe, NULL);

    while (1) {
        lv_task_handler();
        usleep(5000);
    }

    pthread_join(tid, NULL);
    pthread_mutex_destroy(&lock);
    return 0;

/*Create a Demo*/
// #if LV_USE_DEMO_WIDGETS
//     lv_demo_widgets();
// #endif
// #if LV_USE_DEMO_BENCHMARK
//     lv_demo_benchmark();
// #endif

//     /*Handle LitlevGL tasks (tickless mode)*/
//     while(1) {
//         lv_timer_handler();
//         usleep(5000);
//     }

//     return 0;
}

/*Set in lv_conf.h as `LV_TICK_CUSTOM_SYS_TIME_EXPR`*/
uint32_t custom_tick_get(void)
{
    static uint64_t start_ms = 0;
    if(start_ms == 0) {
        struct timeval tv_start;
        gettimeofday(&tv_start, NULL);
        start_ms = (tv_start.tv_sec * 1000000 + tv_start.tv_usec) / 1000;
    }

    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    uint64_t now_ms;
    now_ms = (tv_now.tv_sec * 1000000 + tv_now.tv_usec) / 1000;

    uint32_t time_ms = now_ms - start_ms;
    return time_ms;
}
