#pragma once

// 标准库（高频）
#include<iostream>
#include <vector>
#include <memory>
#include <string>
#include <map>
#include <ctime>
#include<chrono>
#include<thread>
// EasyX 图形库
#include <easyx.h>
#include <graphics.h>   // 部分版本需要

// Windows 常用（如果需要）
#include <windows.h>
// 引入必要的头文件和库
#include <wingdi.h>
#pragma comment(lib, "msimg32.lib")
// 禁用特定警告（如果需要）
#pragma warning(disable: 4244)   // 禁用 4244 警告（类型转换可能丢失数据）
#pragma warning(disable: 4305)   // 禁用 4305（截断，如 double 到 float）
#pragma warning(disable: 4267)   // 禁用 4267（从 size_t 到 int 的转换可能丢失数据）
