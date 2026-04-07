/*
 * Copyright (c) 2025 The ZMK Contributors
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_sponge_key

#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include <zephyr/random/random.h>

#include <drivers/behavior.h>
#include <zmk/behavior.h>
#include <zmk/keys.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

static bool shift_pressed = false;

static int invoke_kp(uint32_t keycode, bool pressed, struct zmk_behavior_binding_event event) {
    struct zmk_behavior_binding binding = {
        .behavior_dev = DEVICE_DT_NAME(DT_INST(0, zmk_behavior_key_press)),
        .param1 = keycode,
    };
    if (pressed) {
        return behavior_keymap_binding_pressed(&binding, event);
    } else {
        return behavior_keymap_binding_released(&binding, event);
    }
}

static int on_sponge_key_binding_pressed(struct zmk_behavior_binding *binding,
                                         struct zmk_behavior_binding_event event) {
    uint32_t rand_val;
    sys_rand_get(&rand_val, sizeof(rand_val));
    shift_pressed = (rand_val & 1);

    if (shift_pressed) {
        invoke_kp(LSHIFT, true, event);
    }
    return invoke_kp(binding->param1, true, event);
}

static int on_sponge_key_binding_released(struct zmk_behavior_binding *binding,
                                          struct zmk_behavior_binding_event event) {
    int ret = invoke_kp(binding->param1, false, event);
    if (shift_pressed) {
        invoke_kp(LSHIFT, false, event);
        shift_pressed = false;
    }
    return ret;
}

static const struct behavior_driver_api sponge_key_driver_api = {
    .binding_pressed  = on_sponge_key_binding_pressed,
    .binding_released = on_sponge_key_binding_released,
};

static int sponge_key_init(const struct device *dev) { return 0; }

#define SPONGE_KEY_INST(n) \
    BEHAVIOR_DT_INST_DEFINE(n, sponge_key_init, NULL, NULL, NULL, POST_KERNEL, \
                            CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &sponge_key_driver_api);

DT_INST_FOREACH_STATUS_OKAY(SPONGE_KEY_INST)

#endif /* DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT) */