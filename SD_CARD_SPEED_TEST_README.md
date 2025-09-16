# SD卡测速功能说明

## 功能概述

为ESP32-S3项目添加了完整的SD卡性能测试功能，包括基础测速和高级性能分析。

## 测试功能

### 1. 基础测速测试 (`sdcard_speed_test`)

- **不同文件大小测试**: 测试1KB、10KB、100KB、1MB文件的读写性能
- **连续写入测试**: 测试连续写入多个文件的性能
- **随机访问测试**: 测试随机位置读取的性能
- **数据完整性验证**: 确保读写的数据正确性

### 2. 高级性能分析 (`sdcard_advanced_performance_test`)

- **写入延迟分析**: 统计写入操作的最小、最大、平均延迟
- **读取延迟分析**: 统计读取操作的最小、最大、平均延迟
- **IOPS测试**: 计算每秒输入输出操作数
- **小文件性能测试**: 测试大量小文件的创建性能

## 输出信息

### 基础测速输出示例
```
I (12345) example: Testing with 1024 bytes...
I (12346) example: File size: 1024 bytes
I (12347) example: Write speed: 2.45 MB/s
I (12348) example: Read speed: 3.21 MB/s
I (12349) example: Write time: 0.42 ms
I (12350) example: Read time: 0.32 ms
```

### 高级性能分析输出示例
```
I (12351) example: Write Performance Analysis:
I (12352) example:   Average time: 45.67 ms
I (12353) example:   Min time: 42.10 ms
I (12354) example:   Max time: 52.30 ms
I (12355) example:   Write IOPS: 21.90
```

## 使用方法

1. 确保SD卡已正确连接并配置
2. 在`main`函数中调用`sdcard_test()`
3. 程序会自动运行所有测试并输出结果
4. 测试完成后会自动清理临时文件

## 配置要求

- SD卡使用4位总线宽度 (`CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4`)
- 高速模式 (`CONFIG_EXAMPLE_SDMMC_SPEED_HS`)
- 正确的GPIO引脚配置

## 注意事项

- 测试会创建临时文件，测试完成后会自动清理
- 建议在SD卡有足够空间时运行测试
- 测试结果会受到SD卡质量、文件系统类型等因素影响
- 测试过程中请勿移除SD卡

## 性能指标说明

- **MB/s**: 每秒传输的兆字节数，表示传输速度
- **IOPS**: 每秒输入输出操作数，表示操作频率
- **延迟**: 单次操作所需的时间，单位毫秒
- **随机访问**: 测试非连续数据访问的性能

这些测试可以帮助您评估SD卡的实际性能，为应用优化提供参考。
