
#include "flash_if.h"

/**
  * @brief  ����flash
  * @param  None
  * @retval None
  */
void FLASH_If_Init(void)
{ 
  FLASH_Unlock(); 
}

/**
  * @brief  ����ָ������
  * @param  StartSector:��������ʼ����
  * @retval 0: �����ɹ�;1:����ʧ��        
  */
int8_t FLASH_If_Erase(uint32_t StartSector)
{
  uint32_t FlashAddress;
  
  FlashAddress = StartSector;

  //�豸��ѹ��Χ2.7V-3.6V,�ֲ��� 
 
  if (FlashAddress <= (uint32_t) USER_FLASH_LAST_PAGE_ADDRESS)
  {
//	  FLASH_EraseSector(FLASH_Sector_0, VoltageRange_3); // 128 Kbyte
//	  FLASH_EraseSector(FLASH_Sector_1, VoltageRange_3); // 128 Kbyte
//	  FLASH_EraseSector(FLASH_Sector_2, VoltageRange_3); // 128 Kbyte
//	  FLASH_EraseSector(FLASH_Sector_3, VoltageRange_3); // 128 Kbyte
//	  FLASH_EraseSector(FLASH_Sector_4, VoltageRange_3); // 128 Kbyte
//	  FLASH_EraseSector(FLASH_Sector_5, VoltageRange_3); // 128 Kbyte
	  FLASH_EraseSector(FLASH_Sector_6, VoltageRange_3); // 128 Kbyte
	  FLASH_EraseSector(FLASH_Sector_7, VoltageRange_3); // 128 Kbyte
	  FLASH_EraseSector(FLASH_Sector_8, VoltageRange_3); // 128 Kbyte
	  FLASH_EraseSector(FLASH_Sector_9, VoltageRange_3); // 128 Kbyte
	  FLASH_EraseSector(FLASH_Sector_10, VoltageRange_3); // 128 Kbyte
	  FLASH_EraseSector(FLASH_Sector_11, VoltageRange_3); // 128 Kbyte
//	  FLASH_EraseSector(FLASH_Sector_18, VoltageRange_3); // 128 Kbyte
//	  FLASH_EraseSector(FLASH_Sector_19, VoltageRange_3); // 128 Kbyte
//	  FLASH_EraseSector(FLASH_Sector_20, VoltageRange_3); // 128 Kbyte
//	  FLASH_EraseSector(FLASH_Sector_21, VoltageRange_3); // 128 Kbyte
//	  FLASH_EraseSector(FLASH_Sector_22, VoltageRange_3); // 128 Kbyte
   // FLASH_EraseSector(FLASH_Sector_6, VoltageRange_3); // 128 Kbyte 
   //   FLASH_EraseSector(FlashAddress, VoltageRange_3); // 128 Kbyte 
//    FLASH_EraseSector(FLASH_Sector_8, VoltageRange_3); // 128 Kbyte 
//    FLASH_EraseSector(FLASH_Sector_9, VoltageRange_3); // 128 Kbyte 
//    FLASH_EraseSector(FLASH_Sector_10, VoltageRange_3); // 128 Kbyte 
//    FLASH_EraseSector(FLASH_Sector_11, VoltageRange_3); // 128 Kbyte   //��896KB
  }
  else
  {
    return (1);
  }

  return (0);
}

/**
  * @brief  д������flash,4�ֽڶ���
  * @param  FlashAddress: ��ʼ��ַ;Data:д������ 
  *         DataLength: ���ݳ��� (��λΪ32λ)   
  * @retval 0: д��ɹ�
  *         1: д��ʧ��
  *         2: ����У�����
  */
uint32_t FLASH_If_Write(__IO uint32_t* FlashAddress, uint32_t* Data ,uint16_t DataLength)
{
  uint32_t i = 0;

  for (i = 0; (i < DataLength) && (*FlashAddress <= (USER_FLASH_END_ADDRESS-4)); i++)
  {
    if (FLASH_ProgramWord(*FlashAddress, *(uint32_t*)(Data+i)) == FLASH_COMPLETE)
    {
      if (*(uint32_t*)*FlashAddress != *(uint32_t*)(Data+i)) //�������д���һ����
      {
        return(2);                             
      }
      *FlashAddress += 4;
    }
    else
    {
      return (1);                                            //д�����
    }
  }

  return (0);
}

/**
  * @brief  ��ʼִ��IAP
  * @param  None     
  * @retval 0: �����ɹ�
  *         1: ����ʧ��
  */
u8 IAP_Start(void)
{
	u8 state;
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
	state=FLASH_If_Erase(FLASH_Sector_7);//��������,��128KB
  return state;
}

/**
  * @brief  д��flash
  * @param  add: д��ĵ�ַ  pbuf:ָ�����ݻ�����;size:д������ݴ�С
  * @retval 0: �����ɹ�
  *         1: ����ʧ��
  */
uint32_t IAP_Flash_Write(uint32_t addr,uint8_t *pbuf,uint32_t size)
{
		uint32_t ret;
		ret=FLASH_If_Write(&addr,(uint32_t*)pbuf,size);	
		return ret;
}
