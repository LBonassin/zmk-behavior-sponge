/*
 * Copyright (c) 2025 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_sponge_key

#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include <zephyr/random/random.h>

#include <drivers/behavior.h>
#include <zmk/behavior.h>
#include <zmk/event_manager.h>
#include <zmk/events/keycode_state_changed.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

static bool shift_pressed = false;

static int on_sponge_key_binding_pressed(struct zmk_behavior_binding *binding,
                                         struct zmk_behavior_binding_event event) {
    uint32_t keycode = binding->param1;

    uint32_t rand_val;
    sys_rand_get(&rand_val, sizeof(rand_val));
    shift_pressed = (rand_val & 1);

    if (shift_pressed) {
        raise_zmk_keycode_state_changed_from_encoded(LSHIFT, true, event.timestamp);
    }

    return raise_zmk_keycode_state_changed_from_encoded(keycode, true, event.timestamp);
}

static int on_sponge_key_binding_released(struct zmk_behavior_binding *binding,
                                          struct zmk_behavior_binding_event event) {
    uint32_t keycode = binding->param1;

    int ret = raise_zmk_keycode_state_changed_from_encoded(keycode, false, event.timestamp);

    if (shift_pressed) {
        raise_zmk_keycode_state_changed_from_encoded(LSHIFT, false, event.timestamp);
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