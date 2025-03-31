#-----------------------------------------------------------------------------------#
#----------------By XuWanLun 2025.03.31---------------------------------------------#
#-----------------------------------------------------------------------------------#
import pandas as pd

file_path = "/home/xwl/china_chips/BOM芯片使用汇总.xlsx"

# 读取 Excel 文件
df = pd.read_excel(file_path)

# 保存原始数据为 txt（tab 分隔）
df.to_csv("output.txt", index=False, sep="\t", encoding="utf-8")

#filtered_df = df[(df["年度"] == 2025.0) & (df["月份"] == 2.0) & (df["车型"] == "D3")]
# filtered_df = df[(df["年度"] == 2025.0) & (df["月份"].isin([1.0, 2.0]) & (df["车型"] == "V7") & (df["驾驶室形式"] == "N1"))]
# filtered_df.to_csv("filtered_output.txt", index=False, sep="\t", encoding="utf-8")

# subset_comunication = filtered_df[(filtered_df["一级分类"] == "通信类")]
# total_value_comunication = subset_comunication["国产_使用数量"].sum()
# print(f"通信类:{total_value_comunication}")

# subset_control = filtered_df[(filtered_df["一级分类"] == "控制类")]
# total_value_control = subset_control["国产_使用数量"].sum()
# print(f"控制类:{total_value_control}")

# subset_caculation = filtered_df[(filtered_df["一级分类"] == "计算类")]
# total_value_caculation = subset_caculation["国产_使用数量"].sum()
# print(f"计算类:{total_value_caculation}")

# subset_drivers = filtered_df[(filtered_df["一级分类"] == "驱动类")]
# total_value_drivers = subset_drivers["国产_使用数量"].sum()
# print(f"驱动类:{total_value_drivers}")

# subset_storage = filtered_df[(filtered_df["一级分类"] == "存储类")]
# total_value_storage = subset_storage["国产_使用数量"].sum()
# print(f"存储类:{total_value_storage}")

# subset_sensor = filtered_df[(filtered_df["一级分类"] == "传感器类")]
# total_value_sensor = subset_sensor["国产_使用数量"].sum()
# print(f"传感器类:{total_value_sensor}")

# subset_total = filtered_df[(filtered_df["一级分类"] == "V7_芯片总数")]
# total_value_total = subset_total["国产_使用数量"].sum()
# print(f"V7_芯片总数:{total_value_total}")

# # grouped = filtered_df.groupby(["车型", "一级分类"])["国产_使用数量"].sum().reset_index()
# # print(grouped)

# print(f"筛选后共有 {len(filtered_df)} 行数据")



year = 2025.0
# months = [1.0,2.0]
months = [2.0]
cabin_types = ["QT","N1","M1"]
categories = [
    "通信类",
    "控制类",
    "计算类",
    "驱动类",
    "存储类",
    "传感器类",
    "电源类"
]

car_models = df["车型"].unique()
print(f"车型：{car_models}")

with open("summary_output.txt", "w", encoding="utf-8") as summary_file:
    summary_file.write(f"车型：{car_models}\n")
    summary_file.write("=" * 40 + "\n")

    for car in car_models:
        filtered_df = df[
            (df["年度"] == year) &
            (df["月份"].isin(months)) &
            (df["车型"] == car) &
            (df["驾驶室形式"].isin(cabin_types))
        ]
        output_file = f"filtered_output_{car}.txt"
        filtered_df.to_csv(output_file, index=False, sep="\t", encoding="utf-8")

        summary_file.write(f"车型: {car}\n")
        summary_file.write(f"筛选后共有 {len(filtered_df)} 行数据\n")
        summary_file.write(f"筛选数据已保存到 {output_file}\n")

        print(f"车型：{car}")
        print(f"筛选后共有 {len(filtered_df)} 行数据，数据已保存至 {output_file}")

        df_nt = filtered_df[filtered_df["驾驶室形式"].isin(["N1", "QT"])]
        df_m1 = filtered_df[filtered_df["驾驶室形式"] == "M1"]

        for category in categories:
            subset_nt = df_nt[df_nt["一级分类"] == category]
            total_value_nt = subset_nt["国产_使用数量"].sum()
            print(f"{category} (N1 & QT): {total_value_nt}")
            
            subset_m1 = df_m1[df_m1["一级分类"] == category]
            total_value_m1 = subset_m1["国产_使用数量"].sum()
            print(f"{category} (M1): {total_value_m1}")

            summary_file.write(f"{category} (N1 & QT): {total_value_nt}\n")
            summary_file.write(f"{category} (M1): {total_value_m1}\n")

        combined_subset_nt = df_nt[df_nt["一级分类"].isin(["电源类", "驱动类"])]
        combined_total_nt = combined_subset_nt["国产_使用数量"].sum()
        print(f"电源类+驱动类 (N1 & QT): {combined_total_nt}")
        summary_file.write(f"电源类+驱动类 (N1 & QT): {combined_total_nt}\n")

        combined_subset_m1 = df_m1[df_m1["一级分类"].isin(["电源类", "驱动类"])]
        combined_total_m1 = combined_subset_m1["国产_使用数量"].sum()
        print(f"电源类+驱动类 (M1): {combined_total_m1}")
        summary_file.write(f"电源类+驱动类 (M1): {combined_total_m1}\n")

        chip_total_category = f"{car}_芯片总数"

        subset_total_nt = df_nt[df_nt["一级分类"] == chip_total_category]
        total_value_total_nt = subset_total_nt["国产_使用数量"].sum()
        print(f"{chip_total_category} (N1 & QT): {total_value_total_nt}")
        summary_file.write(f"{chip_total_category} (N1 & QT): {total_value_total_nt}\n")
        
        subset_total_m1 = df_m1[df_m1["一级分类"] == chip_total_category]
        total_value_total_m1 = subset_total_m1["国产_使用数量"].sum()
        print(f"{chip_total_category} (M1): {total_value_total_m1}")
        summary_file.write(f"{chip_total_category} (M1): {total_value_total_m1}\n")
        
        print("-" * 40)
        summary_file.write("-" * 40 + "\n")
