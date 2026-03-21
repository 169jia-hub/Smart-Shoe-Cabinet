#include "dht11.h"
#include "delay.h"

// ================= 内部辅助函数：改变引脚方向 =================

// 将 PB12 设置为推挽输出 (用于主机发送开始信号)
// PB12 在 CRH 的 (12-8)*4 = 16位
static void DHT11_IO_OUT(void)
{
    GPIOB->CRH &= 0xFFF0FFFF; // 清空 16~19位
    GPIOB->CRH |= 0x00030000; // 设置为 0011 (推挽输出 50MHz)
}

// 将 PB12 设置为上拉输入 (用于读取 DHT11 的响应)
static void DHT11_IO_IN(void)
{
    GPIOB->CRH &= 0xFFF0FFFF; // 清空
    GPIOB->CRH |= 0x00080000; // 设置为 1000 (上拉/下拉输入)
    GPIOB->ODR |= 1<<12;      // ODR置1，确定为上拉
}

// ================= DHT11 核心协议逻辑 =================

// 复位DHT11 (发送开始信号)
void DHT11_Rst(void)
{
    DHT11_IO_OUT();  // 切换为输出
    PBout(12) = 0;   // 拉低总线
    delay_ms(20);    // 拉低至少 18ms (手册要求)
    PBout(12) = 1;   // 拉高
    delay_us(30);    // 拉高 20~40us
}

// 等待 DHT11 的回应
// 返回1: 未检测到存在; 返回0: 检测到存在
u8 DHT11_Check(void)
{
    u8 retry = 0;
    DHT11_IO_IN();   // 切换为输入模式，准备听
    
    // 等待 DHT11 拉低引脚 (表示它响应了)
    while (PBin(12) && retry < 100) 
    {
        retry++;
        delay_us(1);
    }
    if (retry >= 100) return 1; else retry = 0;

    // 等待 DHT11 再次拉高 (响应信号结束)
    while (!PBin(12) && retry < 100)
    {
        retry++;
        delay_us(1);
    }
    if (retry >= 100) return 1;
    return 0;
}

// 从 DHT11 读取一个位 (0 或 1)
u8 DHT11_Read_Bit(void)
{
    u8 retry = 0;
    // 等待变为低电平 (上一位数据传输结束)
    while (PBin(12) && retry < 100)
    {
        retry++;
        delay_us(1);
    }
    retry = 0;
    
    // 等待变为高电平 (新的一位数据开始传输)
    // 0和1的区别在于高电平持续的时间长短
    while (!PBin(12) && retry < 100)
    {
        retry++;
        delay_us(1);
    }
    delay_us(40); // 等待 40us 后再看
    
    // 如果 40us 后还是高电平，说明是数据 1 (因为 0 的高电平只有 26us)
    if (PBin(12)) return 1;
    else return 0;
}

// 从 DHT11 读取一个字节
u8 DHT11_Read_Byte(void)
{
    u8 i, dat = 0;
    for (i = 0; i < 8; i++)
    {
        dat <<= 1;
        dat |= DHT11_Read_Bit();
    }
    return dat;
}

// ================= 对外接口函数 =================

// 读取一次温湿度数据
// temp: 温度值指针, humi: 湿度值指针
// 返回值: 0-成功, 1-失败
u8 DHT11_Read_Data(u8 *temp, u8 *humi)
{
    u8 buf[5]; // 存储5个字节的数据 (湿高, 湿低, 温高, 温低, 校验)
    u8 i;
    
    DHT11_Rst(); // 复位
    if (DHT11_Check() == 0) // 检测到响应
    {
        for (i = 0; i < 5; i++)
        {
            buf[i] = DHT11_Read_Byte();
        }
        // 校验数据 (前4个字节之和 == 第5个字节)
        if ((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4])
        {
            *humi = buf[0]; // 湿度整数部分
            *temp = buf[2]; // 温度整数部分
            return 0;
        }
    }
    return 1;
}

// 初始化函数
u8 DHT11_Init(void)
{
    RCC->APB2ENR |= 1<<3; // 开启 GPIOB 时钟
    DHT11_Rst();
    return DHT11_Check();
}