#ifndef REPORT_EXPORT_H
#define REPORT_EXPORT_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <set>
#include <string>
#include <map>
#include "chip_kpi.h"

// 从 map 中取值，不存在则返回 0
template <typename M>
static long long get0(const M& m, const std::string& k) {
    auto it = m.find(k);
    return (it == m.end()) ? 0LL : it->second;
}

// 计算 chips/prod，返回字符串
static std::string per_car(double chips, long long prod) {
    if (prod <= 0) return "";
    std::ostringstream os;
    os.setf(std::ios::fixed);
    os << std::setprecision(6) << (chips / double(prod));
    return os.str();
}

static double per_car_value(double chips, long long prod) {
    return (prod > 0) ? (chips / double(prod)) : 0.0;
}

// 计算任务达成率，chips / (prod * KPI)
static double achieve_pct(double chips, long long prod, double kpi) {
    if (prod <= 0 || kpi <= 0) return 0.0;
    return (chips / (double(prod) * kpi) * 100.0);
    // std::ostringstream os;
    // os.setf(std::ios::fixed);
    // os << std::setprecision(2) << (chips / (double(prod) * kpi));
    // return os.str();
}

// 合计
template <typename Map>
long long sumMap(const Map& m) {
    long long sum = 0;
    for (const auto& kv : m) {
        sum += kv.second;
    }
    return sum;
}

// 格式化成百分比，保留小数位
static std::string to_percent(double value, int precision = 2) {
    std::ostringstream os;
    os.setf(std::ios::fixed);
    os << std::setprecision(precision) << value << "%";
    return os.str();
}


// 单块输入参数
struct SectionInputs {
    //国产
    const std::unordered_map<std::string,long long>& production; //产量
    const std::unordered_map<std::string,long long>& chinaTotal; //国产化芯片
    const std::unordered_map<std::string,long long>& control;   //控制类芯片
    const std::unordered_map<std::string,long long>& compute;   //计算类芯片
    const std::unordered_map<std::string,long long>& analog;    //模拟类芯片
    const std::unordered_map<std::string,long long>& power;     //电源类芯片
    const std::unordered_map<std::string,long long>& drive;     //驱动类芯片
    const std::unordered_map<std::string,long long>& memory;    //存储类芯片
    const std::unordered_map<std::string,long long>& sensor;    //传感器类芯片
    const std::unordered_map<std::string,long long>& comm;      //通信类芯片
    const std::unordered_map<std::string,long long>& Power_1;     //功率类芯片

    //非国产
    const std::unordered_map<std::string,long long>& nondomesticTotal; //非国产化芯片
    const std::unordered_map<std::string,long long>& nondomesticControl;   //非国产控制类芯片
    const std::unordered_map<std::string,long long>& nondomesticCompute;   //非国产计算类芯片
    const std::unordered_map<std::string,long long>& nondomesticAnalog;    //非国产模拟类芯片
    const std::unordered_map<std::string,long long>& nondomesticPower;     //非国产电源类芯片
    const std::unordered_map<std::string,long long>& nondomesticDrive;     //非国产驱动类芯片
    const std::unordered_map<std::string,long long>& nondomesticMemory;    //非国产存储类芯片
    const std::unordered_map<std::string,long long>& nondomesticSensor;    //非国产传感器类芯片
    const std::unordered_map<std::string,long long>& nondomesticComm;      //非国产通信类芯片
    const std::unordered_map<std::string,long long>& nondomesticPower_1;     //非国产功率类芯片
    double kpi_total, kpi_control, kpi_compute, kpi_analog, kpi_memory, kpi_sensor, kpi_comm,kpi_Power_1;
    // double kpi_total, kpi_control, kpi_compute, kpi_analog, kpi_memory, kpi_sensor, kpi_comm;

    // int percar_total_nominal, percar_control_nominal, percar_compute_nominal,
    //     percar_analog_nominal, percar_power_nominal, percar_drive_nominal,
    //     percar_memory_nominal, percar_sensor_nominal, percar_comm_nominal;

    std::string title;
};


// 写一块数据
static void write_section_csv(std::ofstream& ofs, const SectionInputs& s) {
    ofs << s.title << ",总体,,,,,,,控制类,,,,,计算类,,,,,模拟/电源/驱动,,,,,"
        << "存储类,,,,,传感器类,,,,,通信类,,,,,功率类\n";
        // << "存储类,,,,,传感器类,,,,,通信类\n";

    ofs << "车型,车型产量,国产非国产芯片总量,"
        << "2025单车指标(总体), 达成情况（总体）, 单车应用比例（总体）, 国产芯片数量（总体）,非国产芯片数量（总体）,任务达成率（总体）,"
        << "2025单车指标(控制类), 类别达成情况（控制类） , 类别国产芯片数量（控制类） ,类别非国产芯片数量（控制类）, 任务达成率（控制类） ,"
        << "2025单车指标(计算类), 类别达成情况（计算类） , 类别国产芯片数量（计算类） ,类别非国产芯片数量（计算类）, 任务达成率（计算类） ,"
        << "2025单车指标(模电驱类), 类别达成情况（模电驱类） , 类别国产芯片数量（模电驱类） , 类别非国产芯片数量（模电驱类）,任务达成率（模电驱类） ,"
        << "2025单车指标(存储类), 类别达成情况（存储类） , 类别国产芯片数量（存储类） , 类别非国产芯片数量（存储类）,任务达成率（存储类） ,"
        << "2025单车指标(传感器类), 类别达成情况（传感器类） , 类别国产芯片数量（传感器类） ,类别非国产芯片数量（传感器类）, 任务达成率（传感器类） ,"
        << "2025单车指标(通信类), 类别达成情况（通信类） , 类别国产芯片数量（通信类） , 类别非国产芯片数量（通信类）,任务达成率（通信类） ,"
        << "2025单车指标(功率类), 类别达成情况（功率类） , 类别国产芯片数量（功率类） ,类别非国产芯片数量（功率类）,任务达成率（功率类） ,\n";
        

    // 收集车型
    std::set<std::string> vehicles;
    auto collect = [&](const auto& mp){ for (auto& kv : mp) vehicles.insert(kv.first); };
    collect(s.production); collect(s.chinaTotal); 
    // collect(s.control);
    // collect(s.compute); collect(s.analog); collect(s.power);
    // collect(s.drive); collect(s.memory); collect(s.sensor); collect(s.comm);

    //商用车合计
    long long sumProduction = sumMap(s.production); //总产量
    //国产
    long long sumChips = sumMap(s.chinaTotal); //国产芯片总量
    long long sumControlChips = sumMap(s.control);//控制类芯片总量
    long long sumComputeChips = sumMap(s.compute); //计算类芯片总量
    long long sumAnalogPowerDriveChips = sumMap(s.analog) +sumMap(s.power)+sumMap(s.drive); //模拟+电源+驱动类芯片总量
    long long sumMemoryChips = sumMap(s.memory); //存储类芯片总量
    long long sumSensorChips = sumMap(s.sensor); //传感器类芯片总量
    long long sumCommChips = sumMap(s.comm); //通信类芯片总量
    long long sumPower_1Chips = sumMap(s.Power_1); //功率类芯片总量
    //非国产
    long long sumnondomesticChips = sumMap(s.nondomesticTotal); //非国产芯片总量
    long long sumnondomesticControlChips = sumMap(s.nondomesticControl);//非国产控制类芯片总量
    long long sumnondomesticComputeChips = sumMap(s.nondomesticCompute); //非国产计算类芯片总量
    long long sumnondomesticAnalogPowerDriveChips = sumMap(s.nondomesticAnalog) +sumMap(s.nondomesticPower)+sumMap(s.nondomesticDrive); //非国产模拟+电源+驱动类芯片总量
    long long sumnondomesticMemoryChips = sumMap(s.nondomesticMemory); //非国产存储类芯片总量
    long long sumnondomesticSensorChips = sumMap(s.nondomesticSensor); //非国产传感器类芯片总量
    long long sumnondomesticCommChips = sumMap(s.nondomesticComm); //非国产通信类芯片总量
    long long sumnondomesticPower_1Chips = sumMap(s.nondomesticPower_1); //非国产功率类芯片总量

    double avgChipsPerCar = static_cast<double>(sumChips) / static_cast<double>(sumProduction); //单车芯片应用量-总体
    double avgControlChipsPerCar = static_cast<double>(sumControlChips) / static_cast<double>(sumProduction); //单车芯片应用量-控制类
    double avgComputeChipsPerCar = static_cast<double>(sumComputeChips) / static_cast<double>(sumProduction); //单车芯片应用量-计算类
    double avgAnalogPowerDriveChipsPerCar = static_cast<double>(sumAnalogPowerDriveChips) / static_cast<double>(sumProduction); //单车芯片应用量-模拟+电源+驱动
    double avgMemoryChipsPerCar = static_cast<double>(sumMemoryChips) / static_cast<double>(sumProduction); //单车芯片应用量-存储类
    double avgSensorChipsPerCar = static_cast<double>(sumSensorChips) / static_cast<double>(sumProduction); //单车芯片应用量-传感器类
    double avgCommChipsPerCar = static_cast<double>(sumCommChips) / static_cast<double>(sumProduction); //单车芯片应用量-通信类
    double avgPower_1ChipsPerCar = static_cast<double>(sumPower_1Chips) / static_cast<double>(sumProduction); //单车芯片应用量-功率类

    double Bicycleapplicationratio = avgChipsPerCar / TOTAL_COMMERCIAL * 100.0; //单车应用比例-总体
    double Taskcompletionrate = achieve_pct(sumChips, sumProduction, s.kpi_total); //任务达成率-总体
    double Taskcompletionratecontrol = achieve_pct(sumControlChips, sumProduction, s.kpi_control); //任务达成率-控制类
    double Taskcompletionratecompute = achieve_pct(sumComputeChips, sumProduction, s.kpi_compute); //任务达成率-计算类
    double Taskcompletionrateanalog = achieve_pct(sumAnalogPowerDriveChips, sumProduction, s.kpi_analog); //任务达成率-模拟+电源+驱动
    double Taskcompletionratememory = achieve_pct(sumMemoryChips, sumProduction, s.kpi_memory); //任务达成率-存储类
    double Taskcompletionratesensor = achieve_pct(sumSensorChips, sumProduction, s.kpi_sensor); //任务达成率-传感器类
    double Taskcompletionratecomm = achieve_pct(sumCommChips, sumProduction, s.kpi_comm); //任务达成率-通信类
    double Taskcompletionratepower_1 = achieve_pct(sumPower_1Chips, sumProduction, s.kpi_Power_1); //任务达成率-功率类

    double Totaldomesticandnondomestic = sumMap(s.nondomesticTotal) + sumMap(s.chinaTotal); //国产+非国产芯片总量

    
    ofs << s.title << "合计" << "," << sumProduction << "," 
            //总体
        << Totaldomesticandnondomestic << ","
        << s.kpi_total << "," 
        << avgChipsPerCar << ","
        << to_percent(Bicycleapplicationratio, 6) << ","
        << sumChips << ","
        << sumnondomesticChips << ","
        << to_percent(Taskcompletionrate, 6) << "," 
        //控制类
        << s.kpi_control << ","
        << avgControlChipsPerCar << ","
        << sumControlChips << "," 
        << sumnondomesticControlChips << ","
        << to_percent(Taskcompletionratecontrol, 6) << "," 
        //计算类
        << s.kpi_compute << ","
        << avgComputeChipsPerCar << ","
        << sumComputeChips << ","
        << sumnondomesticComputeChips << ","
        << to_percent(Taskcompletionratecompute, 6) << "," 
        //模拟电源驱动类
        << s.kpi_analog << ","
        << avgAnalogPowerDriveChipsPerCar << ","
        << sumAnalogPowerDriveChips << ","
        << sumnondomesticAnalogPowerDriveChips << ","
        << to_percent(Taskcompletionrateanalog, 6) << "," 
        //存储类
        << s.kpi_memory << ","
        << avgMemoryChipsPerCar << ","
        << sumMemoryChips << ","
        << sumnondomesticMemoryChips << ","
        << to_percent(Taskcompletionratememory, 6) << "," 
        //传感器类
        << s.kpi_sensor << ","
        << avgSensorChipsPerCar << ","
        << sumSensorChips << ","
        << sumnondomesticSensorChips << ","
        << to_percent(Taskcompletionratesensor, 6) << "," 
        //通信类
        << s.kpi_comm << ","
        << avgCommChipsPerCar << ","
        << sumCommChips << ","
        << sumnondomesticCommChips << ","
        << to_percent(Taskcompletionratecomm, 6) << ","
        // << to_percent(Taskcompletionratecomm, 6) << "," <<"\n";
        //功率类
        << s.kpi_Power_1 << ","
        << avgPower_1ChipsPerCar << ","
        << sumPower_1Chips << ","
        << sumnondomesticPower_1Chips << ","
        << to_percent(Taskcompletionratepower_1, 6) << "," <<"\n";
    std::cout << s.title << " 总产量: " << to_percent(Taskcompletionrate, 6)<< "\n";

    // 车型对应数据
    for (const auto& v : vehicles) {
        //国产
        const long long prod = get0(s.production, v); //产量
        const long long totalChips   = get0(s.chinaTotal, v); //芯片总量国产
        const long long controlChips = get0(s.control, v);
        const long long computeChips = get0(s.compute, v);
        const long long analogChips  = get0(s.analog, v) + get0(s.power, v) + get0(s.drive, v);
        const long long memoryChips  = get0(s.memory, v);
        const long long sensorChips  = get0(s.sensor, v);
        const long long powerChips   = get0(s.power, v);
        const long long commChips    = get0(s.comm, v);
        const long long Power_1Chips    = get0(s.Power_1, v);

        //非国产
        const long long totalnondomesticChips   = get0(s.nondomesticTotal, v); //芯片总量非国产
        const long long controlnondomesticChips = get0(s.nondomesticControl, v);
        const long long computenondomesticChips = get0(s.nondomesticCompute, v);
        const long long analognondomesticChips  = get0(s.nondomesticAnalog, v) + get0(s.nondomesticPower, v) + get0(s.nondomesticDrive, v);
        const long long memorynondomesticChips  = get0(s.nondomesticMemory, v);
        const long long sensornondomesticChips  = get0(s.nondomesticSensor, v);
        const long long powernondomesticChips   = get0(s.nondomesticPower, v);
        const long long commnondomesticChips    = get0(s.nondomesticComm, v);
        const long long Power_1nondomesticChips    = get0(s.nondomesticPower_1, v);

        // auto single_target = [&](int nominal, double kpi){
        //     if (nominal<=0 || kpi<=0) return std::string("");
        //     std::ostringstream os; os.setf(std::ios::fixed);
        //     os<<std::setprecision(2)<<(double(nominal)*kpi);
        //     return os.str();
        // };

        double Bicycleratio = (per_car_value(totalChips, prod) / TOTAL_COMMERCIAL) * 100.0; //单车应用比例-总体
        double Totaldomesticandnondomesticall = totalChips + totalnondomesticChips; //国产+非国产芯片总量

        ofs << v << "," << prod << ","; //车型,产量

        // 总体
        ofs << Totaldomesticandnondomesticall << "," 
            << s.kpi_total << "," //指标
            << per_car(totalChips, prod) << ","
            << to_percent(Bicycleratio, 6)  << ","
            << totalChips << ","
            << totalnondomesticChips << ","
            << to_percent(achieve_pct(totalChips, prod, s.kpi_total)) << ",";

        // 控制类
        ofs << s.kpi_control << ","
            << per_car(controlChips, prod) << ","
            << controlChips << ","
            << controlnondomesticChips << ","
            << to_percent(achieve_pct(controlChips, prod, s.kpi_control)) << ",";

        // // 计算类
        ofs << s.kpi_compute << ","
            << per_car(computeChips, prod) << ","
            << computeChips << ","
            << computenondomesticChips << ","
            << to_percent(achieve_pct(computeChips, prod, s.kpi_compute)) << ",";

        // // 模拟/电源/驱动
        ofs << s.kpi_analog << ","
            << per_car(analogChips, prod) << ","
            << analogChips << ","
            << analognondomesticChips << ","
            << to_percent(achieve_pct(analogChips, prod, s.kpi_analog)) << ",";

        // // 存储
        ofs << s.kpi_memory << ","
            << per_car(memoryChips, prod) << ","
            << memoryChips << ","
            << memorynondomesticChips << ","
            << to_percent(achieve_pct(memoryChips, prod, s.kpi_memory)) << ",";

        // // 传感器
        ofs << s.kpi_sensor << ","
            << per_car(sensorChips, prod) << ","
            << sensorChips << ","
            << sensornondomesticChips << ","
            << to_percent(achieve_pct(sensorChips, prod, s.kpi_sensor)) << ",";

        // // 通信
        ofs << s.kpi_comm << ","
            << per_car(commChips, prod) << ","
            << commChips << ","
            << commnondomesticChips << ","
            << to_percent(achieve_pct(commChips, prod, s.kpi_comm)) << ",";

        // // 功率
        ofs << s.kpi_Power_1 << ","
            << per_car(Power_1Chips, prod) << ","
            << Power_1Chips << ","
            << Power_1nondomesticChips << ","
            << to_percent(achieve_pct(Power_1Chips, prod, s.kpi_Power_1)) <<",";
        ofs << "\n";
    }
}

// 封装好的导出函数
inline void write_full_report_csv(
    const std::string& filename,
    const SectionInputs& commercial, // 商用
    const SectionInputs& passenger   // 乘用
) {
    std::ofstream ofs(filename, std::ios::trunc);
    if (!ofs) {
        std::cerr << "无法创建 CSV 文件: " << filename << "\n";
        return;
    }
    write_section_csv(ofs, commercial);
    ofs << "\n";

    write_section_csv(ofs, passenger);
    ofs.close();
    std::cout << "CSV 已生成: " << filename << "\n";

    // 商用车部分
    // std::cout << commercial.title 
    //           << " 2025单车指标(总体) = "
    //           << commercial.kpi_total << std::endl;

    // // 乘用车部分
    // std::cout << passenger.title 
    //           << " 2025单车指标(总体) = "
    //           << passenger.kpi_total << std::endl;
}

#endif // REPORT_EXPORT_H
