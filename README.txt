1.迁移汇顶底层驱动，根据porting guide，实现startshanmpling HR
2.最主要的问题是spi的cs引脚不能使用乐鑫的驱动，需要配置成普通gpio给
汇顶的库去驱动
TO DO：
1.看是否可以连接汇顶上位机调试
2.还需要测试ADT\HRV等测试功能，提供采样率
3.传输的adc数据和algo数据存放位置和读取，打log