# Copyright 2015 The Android Open Source Project
# Copyright (C) 2015 Valve Corporation

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

#      http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

APP_ABI := armeabi-v7a arm64-v8a
APP_PLATFORM := android-27
APP_STL := gnustl_static
APP_MODULES :=  iozone
NDK_TOOLCHAIN_VERSION := clang
APP_ALLOW_MISSING_DEPS=true

local_dir :=$(call my-dir)
APP_BUILD_SCRIPT := $(local_dir)/../Android.mk
