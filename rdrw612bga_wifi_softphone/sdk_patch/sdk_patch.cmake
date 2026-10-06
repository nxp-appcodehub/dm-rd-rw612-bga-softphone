# Copyright 2026 NXP
#
# SPDX-License-Identifier: BSD-3-Clause

mcux_project_remove_source(
    BASE_PATH ${SdkRootDirPath}
    SOURCES examples/_boards/rdrw612bga/display_support.c
)


mcux_add_source(
    SOURCES source/display_support.c
)

