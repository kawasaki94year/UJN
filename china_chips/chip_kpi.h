#ifndef CHIP_KPI_H
#define CHIP_KPI_H

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>


// ===================== 常量定义 =====================

// 工信部 单类指标
constexpr double MIIT_SINGLE_2022 = 0.025;
constexpr double MIIT_SINGLE_2023 = 0.050;
constexpr double MIIT_SINGLE_2024 = 0.075;
constexpr double MIIT_SINGLE_2025 = 0.100;

// 工信部 总量指标
constexpr double MIIT_TOTAL_2022  = 0.050;
constexpr double MIIT_TOTAL_2023  = 0.100;
constexpr double MIIT_TOTAL_2024  = 0.150;
constexpr double MIIT_TOTAL_2025  = 0.200;

// 国资委 单类指标
constexpr double SASAC_SINGLE_2023 = 0.050;
constexpr double SASAC_SINGLE_2024 = 0.150;
constexpr double SASAC_SINGLE_2025 = 0.200;

// 国资委 总量指标
constexpr double SASAC_TOTAL_2023  = 0.100;
constexpr double SASAC_TOTAL_2024  = 0.350;
constexpr double SASAC_TOTAL_2025  = 0.400;

// 单车可应用芯片数量（乘用车 vs 商用车）
constexpr int TOTAL_PASSENGER        = 205;
constexpr int TOTAL_COMMERCIAL       = 94;

constexpr int CONTROL_PASSENGER      = 30;
constexpr int CONTROL_COMMERCIAL     = 20;

constexpr int COMPUTE_PASSENGER      = 4;
constexpr int COMPUTE_COMMERCIAL     = 0;

constexpr int ANALOG_POWER_PASSENGER = 60;
constexpr int ANALOG_POWER_COMMERCIAL= 30;

constexpr int MEMORY_PASSENGER       = 12;
constexpr int MEMORY_COMMERCIAL      = 4;

constexpr int SENSOR_PASSENGER       = 25;
constexpr int SENSOR_COMMERCIAL      = 10;

constexpr int POWER_PASSENGER        = 0;
constexpr int POWER_COMMERCIAL       = 0;

constexpr int COMM_PASSENGER         = 74;
constexpr int COMM_COMMERCIAL        = 30;

// ===================== 计算逻辑 =====================

namespace chipkpi {

struct Row {
    std::string name;
    int commercial;
};

struct SasacKpi {
    double single;
    double total;
};

// 计算结果
struct ResultRow {
    std::string name;  // 类别名
    int commercial;    // D列：商用车数量
    double e_value;    // E列：计算结果（商用车 × KPI）
};

// 根据年份获取国资委指标
inline SasacKpi get_sasac_kpi(int year) {
    switch (year) {
        case 2023: return {SASAC_SINGLE_2023, SASAC_TOTAL_2023};
        case 2024: return {SASAC_SINGLE_2024, SASAC_TOTAL_2024};
        case 2025: return {SASAC_SINGLE_2025, SASAC_TOTAL_2025};
        default:
            std::cerr << "警告：不支持的年份 " << year << "，默认使用 2025。\n";
            return {SASAC_SINGLE_2025, SASAC_TOTAL_2025};
    }
}

// 根据类别名获取对应的 e_value，未找到则返回 0.0
inline double getValueByName(const std::vector<ResultRow>& results,
                             const std::string& name) {
    auto it = std::find_if(results.begin(), results.end(),
                           [&](const ResultRow& r){ return r.name == name; });
    return (it != results.end()) ? it->e_value : 0.0;
}

// 返回计算结果
inline std::vector<ResultRow> compute_sasac(int year) {
    auto kpi = get_sasac_kpi(year);

    std::vector<Row> base = {
        {"合计",                 TOTAL_COMMERCIAL},
        {"控制类芯片",           CONTROL_COMMERCIAL},
        {"计算类芯片",           COMPUTE_COMMERCIAL},
        {"模拟/电源/驱动类芯片", ANALOG_POWER_COMMERCIAL},
        {"存储类芯片",           MEMORY_COMMERCIAL},
        {"传感器类芯片",         SENSOR_COMMERCIAL},
        {"功率类芯片",           POWER_COMMERCIAL},
        {"通信类芯片",           COMM_COMMERCIAL},
    };

    std::vector<ResultRow> results;
    results.reserve(base.size());

    for (auto &r : base) {
        double e = (r.name == "合计") ? r.commercial * kpi.total
                                      : r.commercial * kpi.single;
        results.push_back({r.name, r.commercial, e});
    }
    return results;
}

// ② 打印结果
inline void calc_sasac(int year) {
    auto results = compute_sasac(year);
    auto kpi = get_sasac_kpi(year);

    std::cout << "年份: " << year
              << " （国资委 单类: " << kpi.single*100
              << "%, 总量: " << kpi.total*100 << "%）\n";
    std::cout << "----------------------------------------------\n";
    std::cout << std::left << std::setw(18) << "类别"
              << std::right << std::setw(10) << "D列(商用)"
              << std::right << std::setw(12) << "E列(结果)\n";
    std::cout << "----------------------------------------------\n";

    double sum = 0.0;
    for (auto &r : results) {
        sum += r.e_value;
        std::cout << std::left  << std::setw(18) << r.name
                  << std::right << std::setw(10) << r.commercial
                  << std::right << std::setw(12) << std::fixed << std::setprecision(2) << r.e_value
                  << '\n';
    }
    std::cout << "----------------------------------------------\n";
    std::cout << std::left << std::setw(18) << "E列合计(分类)"
              << std::right << std::setw(22) << std::fixed << std::setprecision(2) << sum
              << "\n\n";
}

} // namespace chipkpi

#endif // CHIP_KPI_H
