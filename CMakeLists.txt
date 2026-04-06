# Copyright (c) 2025 The ZMK Contributors
# SPDX-License-Identifier: MIT

if(CONFIG_ZMK_BEHAVIOR_SPONGE)
  target_sources(app PRIVATE src/behaviors/behavior_sponge.c)
endif()
