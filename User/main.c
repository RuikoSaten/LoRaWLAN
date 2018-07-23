/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   串口中断接收测试
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火 F103-指南者 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
 
 
#include "stm32f10x.h"
#include "bsp_usart.h"
#include "LoRa.h"
#include "Sender.h"
#include "ECC.h"
#include "Receiver.h"
#include "LinkedList.h"
#include "bsp_SysTick.h"
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{	
  /*初始化USART 配置模式为 115200 8-N-1，中断接收*/
    USART_Config();
	LoRaInit();
    ECCInit();
    SenderInit();
    ReceiverInit();
    SysTick_Init();
    DataPacket packet;
    uint8_t* pdata = malloc(sizeof(uint8_t)*16);
    for(int i = 0;i < 16;i++){
        *(pdata+i) = i;
    }
    packet.dataBytes.data = pdata;
    packet.dataBytes.length = 16;
    packet.destination.Address_H = 0x05;
    packet.destination.Address_L = 0x05;
    packet.destination.Channel = 0x05;
    packet.count = 0x10;
    Sender->send(&packet);
    
    while(1)
	{	
		_send();
        _receive();

	}	
}
/*********************************************END OF FILE**********************/
