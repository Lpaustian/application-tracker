#pragma once

#include <QString>

// Colors used across the app.
namespace style {

constexpr const char* background = "#f3f5f4";
constexpr const char* surface = "#ffffff";
constexpr const char* sunken = "#e8ecea";
constexpr const char* text = "#15201c";
constexpr const char* muted = "#5b6a64";
constexpr const char* line = "#d4dbd8";
constexpr const char* accent = "#1d6b57";
constexpr const char* accentHover = "#237d66";
constexpr const char* accentSoft = "#dcefe8";
constexpr const char* warn = "#9a5b00";
constexpr const char* warnSoft = "#fbedd5";
constexpr const char* bad = "#b23a2e";
constexpr const char* badSoft = "#f8e0dc";
constexpr const char* info = "#2c5a9e";
constexpr const char* infoSoft = "#e0e9f7";

// The stylesheet for the whole application.
QString styleSheet();

}  // namespace style
