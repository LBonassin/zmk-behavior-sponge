/*
 * Copyright (c) 2025 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 *
 * Sponge Behavior - randomly capitalizes alpha keycodes to produce
 * "mOcKiNg SpOnGeBoB" text. Toggle on/off with &sponge.
 *
 * Usage in keymap:
 *   &sponge      <- toggle sponge mode on/off
 *   &sk_sponge A <- a sponge-aware key (randomly uppercase or lowercase)
 *
 * Typically you replace all alpha keys with &sk_sponge and bind &sponge
 * to a combo or dedicated key.
 */

#define DT_DRV_COMPAT zmk_behavior_sponge

#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include <zephyr/random/random.h>

#include <drivers/behavior.h>
#include <zmk/behavior.h>
#include <zmk/hid.h>
#include <zmk/keymap.h>
#include <zmk/event_manager.h>
#include <zmk/events/keycode_state_changed.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

/* ------------------------------------------------------------------ */
/*  Global sponge state                                                 */
/* ------------------------------------------------------------------ */

static bool sponge_active = false;

bool zmk_sponge_is_active(void) { return sponge_active; }

/* ------------------------------------------------------------------ */
/*  Toggle behavior  (&sponge)                                          */
/* ------------------------------------------------------------------ */

#define DT_DRV_COMPAT_TOGGLE zmk_behavior_sponge_toggle
#undef DT_DRV_COMPAT
#define DT_DRV_COMPAT zmk_behavior_sponge_toggle

static int on_sponge_toggle_binding_pressed(struct zmk_behavior_binding *binding,
                                            struct zmk_behavior_binding_event event) {
    sponge_active = !sponge_active;
    LOG_DBG("Sponge mode %s", sponge_active ? "ON" : "OFF");
    return ZMK_BEHAVIOR_OPAQUE;
}

static int on_sponge_toggle_binding_released(struct zmk_behavior_binding *binding,
                                             struct zmk_behavior_binding_event event) {
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api sponge_toggle_driver_api = {
    .binding_pressed  = on_sponge_toggle_binding_pressed,
    .binding_released = on_sponge_toggle_binding_released,
};

static int sponge_toggle_init(const struct device *dev) { return 0; }

BEHAVIOR_DT_INST_DEFINE(0, sponge_toggle_init, NULL, NULL, NULL, POST_KERNEL,
                        CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &sponge_toggle_driver_api);

/* ------------------------------------------------------------------ */
/*  Per-key sponge behavior  (&sk_sponge <keycode>)                    */
/* ------------------------------------------------------------------ */

#undef DT_DRV_COMPAT
#define DT_DRV_COMPAT zmk_behavior_sponge_key

static int on_sponge_key_binding_pressed(struct zmk_behavior_binding *binding,
                                         struct zmk_behavior_binding_event event) {
    uint32_t keycode = binding->param1;

    if (!sponge_active) {
        /* Pass through as a normal keypress */
        return raise_zmk_keycode_state_changed_from_encoded(keycode, true, event.timestamp);
    }

    /* Randomly uppercase or lowercase */
    uint32_t rand_val;
    sys_rand_get(&rand_val, sizeof(rand_val));
    bool use_shift = (rand_val & 1);

    if (use_shift) {
        /* Raise shift first, then the key */
        raise_zmk_keycode_state_changed_from_encoded(LSHIFT, true, event.timestamp);
    }

    return raise_zmk_keycode_state_changed_from_encoded(keycode, true, event.timestamp);
}

static int on_sponge_key_binding_released(struct zmk_behavior_binding *binding,
                                          struct zmk_behavior_binding_event event) {
    uint32_t keycode = binding->param1;

    if (!sponge_active) {
        return raise_zmk_keycode_state_changed_from_encoded(keycode, false, event.timestamp);
    }

    int ret = raise_zmk_keycode_state_changed_from_encoded(keycode, false, event.timestamp);
    /* Always release shift on key-up regardless of whether we pressed it,
       to avoid stuck shift if sponge was toggled mid-press */
    raise_zmk_keycode_state_changed_from_encoded(LSHIFT, false, event.timestamp);
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
