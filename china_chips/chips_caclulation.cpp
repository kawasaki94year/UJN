#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "chip_kpi.h"
#include <map>
#include <cctype>
#include <unordered_map>
#include <set>
#include "report_export.h"

// --- 去首尾空白 ---
static inline std::string trim(const std::string& s) {
    size_t i = 0, j = s.size();
    while (i < j && std::isspace(static_cast<unsigned char>(s[i]))) ++i;
    while (j > i && std::isspace(static_cast<unsigned char>(s[j-1]))) --j;
    return s.substr(i, j - i);
}
// --- 清洗数字：去引号/逗号/空格/百分号 ---
static long long to_ll_safely(std::string s) {
    s = trim(s);
    // 去掉包裹引号
    if (!s.empty() && s.front() == '"' && s.back() == '"' && s.size() >= 2) {
        s = s.substr(1, s.size()-2);
    }
    // 去掉千分位逗号和空格、百分号
    std::string t;
    for (char c : s) {
        if (c == ',' || c == ' ' || c == '%') continue;
        t.push_back(c);
    }
    if (t.empty()) return 0;
    try { return std::stoll(t); } catch (...) { return 0; }
}

// --- 支持引号与转义的 CSV 行解析 ---
static std::vector<std::string> parse_csv_line(const std::string& line) {
    std::vector<std::string> cells;
    std::string cur;
    bool in_quotes = false;
    for (size_t i = 0; i < line.size(); ++i) {
        char ch = line[i];
        if (ch == '"') {
            if (in_quotes && i + 1 < line.size() && line[i+1] == '"') {
                cur.push_back('"'); // 处理 "" -> "
                ++i;
            } else {
                in_quotes = !in_quotes;
            }
        } else if (ch == ',' && !in_quotes) {
            cells.emplace_back(std::move(cur));
            cur.clear();
        } else {
            cur.push_back(ch);
        }
    }
    cells.emplace_back(std::move(cur));
    return cells;
}

void printResult(const std::string& title, long long valN, long long valM) {
    std::cout << "2025年8月 " << title << " 商用车 = " << valN
              << "，乘用车 = " << valM << "\n";
}

            ///输出内容
auto printMap = [](const std::string& title, const auto& mp) {
    std::cout << "-------------------------------------------------------------------\n";
    std::cout << title << "\n";
    for (const auto& kv : mp) {
        std::cout << kv.first << " = " << kv.second << "\n";
    }
};

void calcCompletion(const std::unordered_map<std::string, long long> &vehicleProduction,
                    const std::unordered_map<std::string, long long> &chipMap,
                    const std::string &chipType,
                    const std::string &vehicleType,
                    double kpiE)   // 对应的KPI
{
    for (const auto &kv : chipMap) {
        const std::string &vehicle = kv.first;  // 车型
        long long chips = kv.second;            // 芯片数量

        auto it = vehicleProduction.find(vehicle);
        if (it != vehicleProduction.end() && it->second > 0) {
            long long prod = it->second;

            // 单车芯片应用量
            double perCar = static_cast<double>(chips) / prod;

            // 单车完成比例（考虑KPI）
            double completion = (kpiE > 0) ? 
                (static_cast<double>(chips) / (prod * kpiE)) * 100.0 : 0.0;

            // std::cout << vehicleType << " " << vehicle
            //           << " " << chipType
            //           << " 单车应用量 = " << std::fixed << std::setprecision(4) << perCar
            //           << " , 完成度 = " << std::fixed << std::setprecision(2) << completion << "%"
            //           << " (chips=" << chips 
            //           << ", prod=" << prod 
            //           << ", KPI=" << kpiE << ")"
            //           << std::endl;
        } else {
            std::cout << vehicleType << " " << vehicle
                      << " 没有产量数据" << std::endl;
        }
    }
}

// 安全转换 month -> int
static int safe_stoi(const std::string& s) {
    try {
        return std::stoi(trim(s));
    } catch (const std::exception& e) {
        std::cerr << "[警告] stoi 失败: '" << s
                  << "' (" << e.what() << ")\n";
        return -1; // 返回 -1 表示无效
    }
}

static bool is_number(const std::string& s) {
    return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}


int main(){
    //指标计算
    auto results = chipkpi::compute_sasac(2025);
    double totalE   = chipkpi::getValueByName(results, "合计");
    double controlE = chipkpi::getValueByName(results, "控制类芯片");
    double computeE = chipkpi::getValueByName(results, "计算类芯片");
    double analogE  = chipkpi::getValueByName(results, "模拟/电源/驱动类芯片");
    double memoryE  = chipkpi::getValueByName(results, "存储类芯片");
    double sensorE  = chipkpi::getValueByName(results, "传感器类芯片");
    double power_1E   = chipkpi::getValueByName(results, "功率类芯片");
    double commE    = chipkpi::getValueByName(results, "通信类芯片");
    // std::cout << "功率类芯片指标 E列 ＝ " << power_1 << std::endl;

    int col_year = -1, col_month = -1, col_vehicle = -1;
    int col_first_level = -1, col_driving_style = -1;
    int col_production = -1, col_china_chips = -1, col_non_domestic = -1;

    //国产
    std::unordered_map<std::string, long long> vehicleChinaChipsSum_N; // 商用 N1/QT 
    std::unordered_map<std::string, long long> vehicleChinaChipsSum_M; // 乘用 M1   
    std::unordered_map<std::string, long long> vehicleControlSum_N,  vehicleControlSum_M;  // 控制类
    std::unordered_map<std::string, long long> vehicleComputeSum_N,  vehicleComputeSum_M;  // 计算类
    std::unordered_map<std::string, long long> vehicleAnalogSum_N,   vehicleAnalogSum_M;   // 模拟类
    std::unordered_map<std::string, long long> vehiclePowerSum_N,    vehiclePowerSum_M;    // 电源类
    std::unordered_map<std::string, long long> vehicleDriveSum_N,    vehicleDriveSum_M;    //驱动类
    std::unordered_map<std::string, long long> vehicleAnalogPowerDrive_N, vehicleAnalogPowerDrive_M; //模拟+电源类+驱动类
    std::unordered_map<std::string, long long> vehicleStroSum_N,     vehicleStroSum_M;     // 存储类
    std::unordered_map<std::string, long long> vehicleSensorSum_N,   vehicleSensorSum_M;   // 传感器类
    std::unordered_map<std::string, long long> vehicleCommunicationSum_N,   vehicleCommunicationSum_M; // 通信类
    std::unordered_map<std::string, long long> vehiclePower_1Sum_N, vehiclePower_1Sum_M; // 功率类

    //非国产
    std::unordered_map<std::string, long long> vehiclenondomesticChipsSum_N; // 商用 N1/QT 非国产
    std::unordered_map<std::string, long long> vehiclenondomesticChipsSum_M; // 乘用 M1 非国产
    std::unordered_map<std::string, long long> vehiclenondomeControlSum_N,  vehiclenondomeControlSum_M;  // 控制类
    std::unordered_map<std::string, long long> vehiclenondomeComputeSum_N,  vehiclenondomeComputeSum_M;  // 计算类
    std::unordered_map<std::string, long long> vehiclenondomeAnalogSum_N,   vehiclenondomeAnalogSum_M;   // 模拟类
    std::unordered_map<std::string, long long> vehiclenondomePowerSum_N,    vehiclenondomePowerSum_M;    // 电源类
    std::unordered_map<std::string, long long> vehiclenondomeDriveSum_N,    vehiclenondomeDriveSum_M;    //驱动类
    std::unordered_map<std::string, long long> vehiclenondomeAnalogPowerDrive_N, vehiclenondomeAnalogPowerDrive_M; //模拟+电源类+驱动类
    std::unordered_map<std::string, long long> vehiclenondomeStroSum_N,     vehiclenondomeStroSum_M;     // 存储类
    std::unordered_map<std::string, long long> vehiclenondomeSensorSum_N,   vehiclenondomeSensorSum_M;   // 传感器类
    std::unordered_map<std::string, long long> vehiclenondomeCommunicationSum_N,   vehiclenondomeCommunicationSum_M; // 通信类
    std::unordered_map<std::string, long long> vehiclenondomePower_1Sum_N, vehiclenondomePower_1Sum_M; // 功率类


    std::set<std::pair<std::string, long long>> printedN;
    std::set<std::pair<std::string, long long>> printedM;

    std::unordered_map<std::string, long long> vehicleProduction_N; // 商用产量
    std::unordered_map<std::string, long long> vehicleProduction_M; // 乘用产量
    
    //读取CSV文件内容
    std::ifstream file("/home/XuWanLun/scripforchips/BOM芯片使用汇总.csv");
    std::string line;
    bool first = true;

    while (std::getline(file, line))
    {
        if (line.empty()) continue;
        auto cells = parse_csv_line(line);
        if (cells.empty()) continue;

        if(first){
            // 解析表头，定位列号
            for (int i = 0; i < (int)cells.size(); ++i) {
                std::string h = trim(cells[i]);
                if (h.find("年度") != std::string::npos) col_year = i;
                else if (h.find("月份") != std::string::npos) col_month = i;
                else if (h.find("车型") != std::string::npos) col_vehicle = i;
                else if (h.find("一级分类") != std::string::npos) col_first_level = i;
                else if (h.find("驾驶室") != std::string::npos) col_driving_style = i;
               //find国产芯片
                if (h.find("产量") != std::string::npos &&
                    h.find("加权") == std::string::npos &&
                    h.find("汇总") == std::string::npos) {
                        col_production = i;
                }

                if (h.find("国产_使用数量") != std::string::npos &&
                    h.find("占比") == std::string::npos &&
                    h.find("非国产") == std::string::npos) {
                        col_china_chips = i;
                }
                //find非国产芯片
                if (h.find("非国产_使用数量") != std::string::npos &&
                    h.find("占比") == std::string::npos) {
                        col_non_domestic = i;
                        // std::cout << "找到列: 非国产_使用数量, 索引 = " << i 
                        //         << ", 表头内容 = [" << h << "]" << std::endl;
                }

            }
        }

        auto get = [&](int idx)->std::string {
            return (idx >= 0 && idx < (int)cells.size()) ? trim(cells[idx]) : std::string();
        };

        std::string year   = get(col_year);
        std::string month  = get(col_month);
        std::string vehicle= get(col_vehicle);
        std::string dstyle = get(col_driving_style);
        std::string china  = get(col_china_chips); //国产芯片
        std::string nondomestic  = get(col_non_domestic); //非国产芯片
        std::string flevel  = get(col_first_level);
        std::string prodStr  = get(col_production); //产量
        long long prodVal   = to_ll_safely(prodStr);
        

        // 统计 2025年08月；驾驶室形式为 N1 或 QT以及 M1
        auto norm = [](std::string s){
            s = trim(s);
            for (auto &ch : s) ch = std::toupper(static_cast<unsigned char>(ch));
            return s;
        };

        // int m = safe_stoi(month);
        int m = -1;
        if (is_number(month)) {
            m = safe_stoi(month);
        }
        // if (year == "2025" &&  (month == "11")) { //2025年11月
        if (year == "2025" && m >= 1 && m <= 12) { // 2025年1-12月

            // std::string ds = norm(dstyle);
            // std::string flevelNorm= norm(flevel);

            const std::string ds = norm(dstyle);   // N1/QT/M1...
            const std::string flevelNorm = norm(flevel);   // 控制类/计算类/模拟类/电源类...

            if (flevelNorm.find("芯片总数") != std::string::npos) {
                // std::cerr << "[SKIP] flevel=" << flevel << "\n";
                continue;
            }

            const long long val = to_ll_safely(china); //国产芯片数量
            const long long valNon = to_ll_safely(nondomestic); //非国产芯片数量

            const bool isCommercial = (ds == "N1" || ds == "QT");
            const bool isPassenger  = (ds == "M1");

            if (!isCommercial && !isPassenger) {
                continue;
            }

            if (ds == "N1" || ds == "QT") {
                auto key = std::make_pair(vehicle, prodVal);
                if (printedN.find(key) == printedN.end()) {
                    vehicleProduction_N[vehicle] += prodVal;
                    printedN.insert(key);
                }
            } else if (ds == "M1") {
                auto key = std::make_pair(vehicle, prodVal);
                if (printedM.find(key) == printedM.end()) {
                    vehicleProduction_M[vehicle] += prodVal;
                    printedM.insert(key);
                }     
            } 
      
            //国产
            auto& totalMap   = isCommercial ? vehicleChinaChipsSum_N : vehicleChinaChipsSum_M; //国产芯片
            auto& controlMap = isCommercial ? vehicleControlSum_N    : vehicleControlSum_M;
            auto& computeMap = isCommercial ? vehicleComputeSum_N    : vehicleComputeSum_M;
            auto& analogMap  = isCommercial ? vehicleAnalogSum_N     : vehicleAnalogSum_M;
            auto& powerMap   = isCommercial ? vehiclePowerSum_N      : vehiclePowerSum_M;
            auto& DriveMap   = isCommercial ? vehicleDriveSum_N      : vehicleDriveSum_M;
            auto& StroMap   = isCommercial ? vehicleStroSum_N      : vehicleStroSum_M;
            auto& SensorMap   = isCommercial ? vehicleSensorSum_N      : vehicleSensorSum_M;
            auto& CommunicationMap   = isCommercial ? vehicleCommunicationSum_N      : vehicleCommunicationSum_M;
            auto& power_1Map   = isCommercial ? vehiclePower_1Sum_N      : vehiclePower_1Sum_M;

            //非国产
            auto& totalnondomesticMap   = isCommercial ? vehiclenondomesticChipsSum_N : vehiclenondomesticChipsSum_M; //非国产芯片
            auto& controlnondomesticMap = isCommercial ? vehiclenondomeControlSum_N    : vehiclenondomeControlSum_M;
            auto& computenondomesticMap = isCommercial ? vehiclenondomeComputeSum_N    : vehiclenondomeComputeSum_M;
            auto& analognondomesticMap  = isCommercial ? vehiclenondomeAnalogSum_N     : vehiclenondomeAnalogSum_M;
            auto& powernondomesticMap   = isCommercial ? vehiclenondomePowerSum_N      : vehiclenondomePowerSum_M;
            auto& DrivenondomesticMap   = isCommercial ? vehiclenondomeDriveSum_N      : vehiclenondomeDriveSum_M;
            auto& StronondomesticMap   = isCommercial ? vehiclenondomeStroSum_N      : vehiclenondomeStroSum_M;
            auto& SensornondomesticMap   = isCommercial ? vehiclenondomeSensorSum_N      : vehiclenondomeSensorSum_M;
            auto& CommunicationnondomesticMap   = isCommercial ? vehiclenondomeCommunicationSum_N      : vehiclenondomeCommunicationSum_M;
            auto& Power_1nondomesticMap   = isCommercial ? vehiclenondomePower_1Sum_N      : vehiclenondomePower_1Sum_M;

            totalMap[vehicle] += val; //国产
            totalnondomesticMap[vehicle] += valNon; //非国产

            if (flevelNorm == "控制类") {
                controlMap[vehicle] += val;
                controlnondomesticMap[vehicle] += valNon;
            } else if (flevelNorm == "计算类") {
                        computeMap[vehicle] += val;
                        computenondomesticMap[vehicle] += valNon;
                    } else if (flevelNorm == "模拟类") {
                        analogMap[vehicle] += val;
                        analognondomesticMap[vehicle] += valNon;
                    } else if (flevelNorm == "电源类") {
                        powerMap[vehicle] += val;
                        powernondomesticMap[vehicle] += valNon;
                    } else if (flevelNorm == "驱动类"){
                        DriveMap[vehicle] += val;
                        DrivenondomesticMap[vehicle] += valNon;
                    } else if (flevelNorm == "存储类"){
                        StroMap[vehicle] += val;
                        StronondomesticMap[vehicle] += valNon; 
                    } else if (flevelNorm == "传感器类"){
                        SensorMap[vehicle] += val;
                        SensornondomesticMap[vehicle] += valNon; 
                    } else if (flevelNorm == "通信类"){
                        CommunicationMap[vehicle] += val;
                        CommunicationnondomesticMap[vehicle] += valNon; 
                    }else if (flevelNorm == "功率类"){
                        power_1Map[vehicle] += val;
                        Power_1nondomesticMap[vehicle] += valNon; 
                    }
                }
    }

    SectionInputs commercial{
        //国产
        vehicleProduction_N, vehicleChinaChipsSum_N,
        vehicleControlSum_N, vehicleComputeSum_N, 
        vehicleAnalogSum_N,vehiclePowerSum_N,vehicleDriveSum_N,
        vehicleStroSum_N, vehicleSensorSum_N, vehicleCommunicationSum_N,vehiclePower_1Sum_N,
         //非国产
        vehiclenondomesticChipsSum_N, 
        vehiclenondomeControlSum_N, vehiclenondomeComputeSum_N,
        vehiclenondomeAnalogSum_N, vehiclenondomePowerSum_N, vehiclenondomeDriveSum_N,
        vehiclenondomeStroSum_N, vehiclenondomeSensorSum_N, vehiclenondomeCommunicationSum_N,
        vehiclenondomePower_1Sum_N,
        totalE, controlE, computeE, analogE, memoryE, sensorE, commE, 
        power_1E,
        "商用车型"
    };

    SectionInputs passenger{
        //国产
        vehicleProduction_M, vehicleChinaChipsSum_M,
        vehicleControlSum_M, vehicleComputeSum_M,
        vehicleAnalogSum_M,vehiclePowerSum_M,vehicleDriveSum_M,
        vehicleStroSum_M, vehicleSensorSum_M, vehicleCommunicationSum_M,vehiclePower_1Sum_M,
        //非国产
        vehiclenondomesticChipsSum_M, 
        vehiclenondomeControlSum_M, vehiclenondomeComputeSum_M,
        vehiclenondomeAnalogSum_M, vehiclenondomePowerSum_M, vehiclenondomeDriveSum_M,
        vehiclenondomeStroSum_M, vehiclenondomeSensorSum_M, vehiclenondomeCommunicationSum_M,
        vehiclenondomePower_1Sum_M,
        totalE, controlE, computeE, analogE, memoryE, sensorE, commE, 
        power_1E,
        "乘用车型"
    };
    


    // write_full_report_csv("2025-11芯片国产化.csv", commercial, passenger);
    write_full_report_csv("/home/XuWanLun/scripforchips/2025-1-12芯片国产化.csv", commercial, passenger);
    return 0; 
}