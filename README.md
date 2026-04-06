# zmk-behavior-sponge

A ZMK module that adds **sponge (mocking) mode** to your keyboard firmware.

When active, every alpha keypress is randomly uppercased or lowercased,
producing output like:

```
tHiS iS SpOnGe MoDe
```

Toggle it on/off at runtime with a dedicated key or combo.

---

## Installation

### 1. Add the module to your `config/west.yml`

```yaml
manifest:
  remotes:
    - name: zmkfirmware
      url-base: https://github.com/zmkfirmware
    - name: your-github               # add this
      url-base: https://github.com/YOUR_GITHUB_USERNAME
  projects:
    - name: zmk
      remote: zmkfirmware
      revision: v0.3
      import: app/west.yml
    - name: zmk-behavior-sponge      # add this
      remote: your-github
      revision: main
  self:
    path: config
```

> Replace `YOUR_GITHUB_USERNAME` with your GitHub username after you fork/push this repo.

---

### 2. Include the behaviors in your keymap

At the top of your `.keymap` file, add:

```c
#include <behaviors/sponge.dtsi>
```

---

### 3. Add `&sponge` to your keymap

Bind `&sponge` to any key or combo to toggle sponge mode on/off:

```c
// Example: Left Ctrl + Left Shift + F12 combo (mirrors the original QMK shortcut)
combos {
    compatible = "zmk,combos";
    combo_sponge {
        timeout-ms = <50>;
        key-positions = <LEFT_CTRL_POS LEFT_SHIFT_POS F12_POS>;
        bindings = <&sponge>;
    };
};
```

Or just drop it directly on a layer key:

```c
&sponge   // wherever you want on your keymap grid
```

---

### 4. Replace alpha `&kp` bindings with `&sk_sponge`

For every alpha key you want affected by sponge mode, replace `&kp X` with `&sk_sponge X`:

```c
// Before
&kp A  &kp B  &kp C ...

// After
&sk_sponge A  &sk_sponge B  &sk_sponge C ...
```

When sponge mode is **off**, `&sk_sponge X` behaves identically to `&kp X`.
When sponge mode is **on**, each keypress is randomly uppercased or lowercased.

---

## Full Example Keymap Snippet

```c
#include <behaviors.dtsi>
#include <dt-bindings/zmk/keys.h>
#include <behaviors/sponge.dtsi>

/ {
    combos {
        compatible = "zmk,combos";
        combo_sponge {
            timeout-ms = <50>;
            key-positions = <36 37 38>; // adjust to your layout
            bindings = <&sponge>;
        };
    };

    keymap {
        compatible = "zmk,keymap";
        default_layer {
            bindings = <
                &sk_sponge Q  &sk_sponge W  &sk_sponge E  &sk_sponge R  &sk_sponge T
                &sk_sponge A  &sk_sponge S  &sk_sponge D  &sk_sponge F  &sk_sponge G
                &sk_sponge Z  &sk_sponge X  &sk_sponge C  &sk_sponge V  &sk_sponge B
                // ... right hand, thumb cluster, etc.
            >;
        };
    };
};
```

---

## How It Works

- `&sponge` is a zero-parameter toggle behavior. Each press flips `sponge_active`.
- `&sk_sponge <KEY>` is a one-parameter behavior. On press:
  - If sponge is **off**: raises a normal keycode event (identical to `&kp`).
  - If sponge is **on**: calls `sys_rand_get()` from Zephyr's random API, and either
    raises LSHIFT before the keycode (uppercase) or not (lowercase).
- On release, LSHIFT is always released to prevent stuck modifiers if sponge
  mode is toggled mid-press.

---

## Compatibility

- ZMK `v0.3` and later
- nice!nano v2, other nRF52840-based boards (hardware RNG available)
- Zephyr's `sys_rand_get()` uses the hardware RNG on nRF chips — genuinely random,
  not a PRNG, so no seeding required

---

## License

MIT
