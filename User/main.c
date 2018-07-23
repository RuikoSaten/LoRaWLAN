/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   �����жϽ��ղ���
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:���� F103-ָ���� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
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
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{	
  /*��ʼ��USART ����ģʽΪ 115200 8-N-1���жϽ���*/
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
