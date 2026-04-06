# Copyright (c) 2025 The ZMK Contributors
# SPDX-License-Identifier: MIT

if(CONFIG_ZMK_BEHAVIOR_SPONGE)
  target_sources(app PRIVATE src/behaviors/behavior_sponge.c)
endif()

# Expose dts/behaviors/ so keymaps can #include <behaviors/sponge.dtsi>
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dts)