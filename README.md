# GH3300 ESP32

GH3300 ESP32S3R8N8
HR HRV ADT  


再之前：
1.更改uarttask代码，使其可以通过uart1发送各种数据
2.修改gh3300 config文件，从evk示例代码拷贝，目前卡在打印hrv_fi数据，需要原厂的算法库支持
3.已可以与evk代码一样发出相同的灯光，进入中断

250710
1.增加ads1292r代码，从淘宝模块代码迁移，isr中断接收函数
