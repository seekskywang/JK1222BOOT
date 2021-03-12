/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   LTDC-Һ����ʾӢ�ģ��ֿ����ڲ�FLASH
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:����  STM32 F429 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f4xx.h"
#include "./led/bsp_led.h"
#include "./sdram/bsp_sdram.h"
#include "./lcd/bsp_lcd.h"
#include "./APP/usbh_bsp.h"
#include "bsp_basic_tim.h"
#include "flash_if.h"
#include "pbdata.h"
#include <string.h>
#include "mbedtls/aes.h"

FRESULT result;
FATFS fs;
FIL file;
//IAP
u8 Jumpflag=0;
//�ݹ�ɨ���ļ�ʱʹ�õ�·��
char scan_path[255] = "0:";
const uint8_t ALERT[][21+5]=
{
	{"�����U��"},
	{"�����������벻Ҫ�رյ�Դ"},
	{"�����������Զ��ػ�"},
	{"�ػ�����ε���Դ����"},


};

mbedtls_aes_context aes;
//��Կ
const unsigned char key[16] = "1234567890123456";

//���ܺ������
static unsigned char plain_decrypt[16] = {0};

static union FD FDbuf;


UINT fnum;            					  /* �ļ��ɹ���д���� */
BYTE ReadBuffer[1024]={0};        /* �������� */
BYTE WriteBuffer[] =              /* д������*/
"��ӭʹ��Ұ��STM32 F429������ �����Ǹ������ӣ��½��ļ�ϵͳ�����ļ�\r\n";  

static void read_write_flie(void);
static FRESULT scan_files (char* path) ;
static  u8    filedatabuf[2048];
static  UINT   readcount;

void Delay(__IO u32 nCount); 
void LCD_Test(void);
void GPIO_Configuration(void);
void Lock_On(void);
void Lock_Off(void);


void Beep_On(void)
{
    GPIO_SetBits(GPIOC,GPIO_Pin_10);
   // GPIO_ResetBits(GPIOH,GPIO_Pin_10);

}
void Beep_Off(void)
{
    GPIO_ResetBits(GPIOC,GPIO_Pin_10);

}

void keydelay(u32 n)
{
	while(n!=0)
	{
		n--;
	}
}
void Key_beep(void)
{
    Beep_On();
    keydelay(100000);
    Beep_Off();
}

/**
  * @brief  scan_files �ݹ�ɨ��FatFs�ڵ��ļ�
  * @param  path:��ʼɨ��·��
  * @retval result:�ļ�ϵͳ�ķ���ֵ
  */
static FRESULT scan_files (char* path) 
{ 
  FRESULT res; 		//�����ڵݹ���̱��޸ĵı���������ȫ�ֱ���	
  FILINFO fno; 
  DIR dir; 
  int i;            
  char *fn;        // �ļ���	
	
#if _USE_LFN 
  /* ���ļ���֧�� */
  /* ����������Ҫ2���ֽڱ���һ�����֡�*/
  static char lfn[_MAX_LFN*2 + 1]; 	
  fno.lfname = lfn; 
  fno.lfsize = sizeof(lfn); 
#endif 
  //��Ŀ¼
  res = f_opendir(&dir, path); 
  if (res == FR_OK) 
	{ 
    i = strlen(path); 
    for (;;) 
		{ 
      //��ȡĿ¼�µ����ݣ��ٶ����Զ�����һ���ļ�
      res = f_readdir(&dir, &fno); 								
      //Ϊ��ʱ��ʾ������Ŀ��ȡ��ϣ�����
      if (res != FR_OK || fno.fname[0] == 0) break; 	
#if _USE_LFN 
      fn = *fno.lfname ? fno.lfname : fno.fname; 
#else 
      fn = fno.fname; 
#endif 
      //���ʾ��ǰĿ¼������			
      if (*fn == '.') continue; 	
      //Ŀ¼���ݹ��ȡ      
      if (fno.fattrib & AM_DIR)         
			{ 			
        //�ϳ�����Ŀ¼��        
        sprintf(&path[i], "/%s", fn); 		
        //�ݹ����         
        res = scan_files(path);	
        path[i] = 0;         
        //��ʧ�ܣ�����ѭ��        
        if (res != FR_OK) 
					break; 
      } 
			else 
			{ 
				printf("%s/%s\r\n", path, fn);								//����ļ���	
        /* ������������ȡ�ض���ʽ���ļ�·�� */        
      }//else
    } //for
  } 
  return res; 
}

/*
*********************************************************************************************************
*	�� �� ��: read_write_flie
*	����˵��: ��ȡ�ļ�test.txtǰ128���ַ�������ӡ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void read_write_flie(void)
{	

/*----------------------- �ļ�ϵͳ���ԣ�д���� -----------------------------*/
	/* ���ļ�������ļ��������򴴽��� */
	printf("\r\n****** ���������ļ�д�����... ******\r\n");	
	result = f_open(&file, "0:FatFs��д�����ļ�.txt",FA_CREATE_ALWAYS | FA_WRITE );
	if ( result == FR_OK )
	{
		printf("����/����FatFs��д�����ļ�.txt�ļ��ɹ������ļ�д�����ݡ�\r\n");
    /* ��ָ���洢������д�뵽�ļ��� */
		result=f_write(&file,WriteBuffer,sizeof(WriteBuffer),&fnum);
    if(result==FR_OK)
    {
      printf("���ļ�д��ɹ���д���ֽ����ݣ�%d\n",fnum);
      printf("�����ļ�д�������Ϊ��\r\n%s\r\n",WriteBuffer);
    }
    else
    {
      printf("�����ļ�д��ʧ�ܣ�(%d)\n",result);
    }    
		/* ���ٶ�д���ر��ļ� */
    f_close(&file);
	}
	else
	{	
		printf("������/�����ļ�ʧ�ܡ�\r\n");
	}
	
/*------------------- �ļ�ϵͳ���ԣ������� ------------------------------------*/
	printf("****** ���������ļ���ȡ����... ******\r\n");
	result = f_open(&file, "0:FatFs��д�����ļ�.txt", FA_OPEN_EXISTING | FA_READ); 	 
	if(result == FR_OK)
	{
		printf("�����ļ��ɹ���\r\n");
		result = f_read(&file, ReadBuffer, sizeof(ReadBuffer), &fnum); 
    if(result==FR_OK)
    {
      printf("���ļ���ȡ�ɹ�,�����ֽ����ݣ�%d\r\n",fnum);
      printf("����ȡ�õ��ļ�����Ϊ��\r\n%s \r\n", ReadBuffer);	
    }
    else
    {
      printf("�����ļ���ȡʧ�ܣ�(%d)\n",result);
    }		
	}
	else
	{
		printf("�������ļ�ʧ�ܡ�\r\n");
	}
	/* ���ٶ�д���ر��ļ� */
	f_close(&file);	
}

void GetFlashSize(void)
{
    static u16 Stm32_Flash_Size;
    
    Stm32_Flash_Size = (*(__IO u16*)(0x1FFF7A22));//���������Ĵ���
    
    printf("оƬ����������СΪ%dK\r\n",Stm32_Flash_Size);

}

uint8_t File_IAP(void)
{
  FRESULT res;
	void (*pUserApp)(void);
  uint32_t JumpAddress;
	u16 i,j=0;
	u32 wr_state=0;
	static uint32_t fsize;
  
	res=f_open(&file, "0:update.bin",FA_OPEN_EXISTING | FA_READ);
	if(res!=FR_OK)
	{
 //    LCD_ErrLog("f_open->updata.bin->error.\n");
     goto updateerror;
  }
//	LCD_UsrLog("f_open->updata.bin->OK!.\n");
	
//	LCD_UsrLog("f_read->updata.bin->..ing.\n");
	fsize=f_size(&file);//unit byte
//	if(fsize>131072)    //128*1024
//	{
//	 // LCD_ErrLog("This firmware is out of limit\n");   
//		while(1)
//		{
//		            //error
//		}
//	}
	//LCD_UsrLog("fsize:%dKB\n",fsize/1024); //��Ϊֻ����������5,����,�̼����ܳ���128KB
	                                      //���и���̼�����,������������̼�ƥ��Ĵ�С
 // LCD_UsrLog(">Erase.....ing\n");
	if(IAP_Start())
	{ 
	//  LCD_ErrLog(">Erase......error\n");
	  goto updateerror;
	}
//	LCD_UsrLog(">Erase.........ok\n");
	
	while(1)
	{
		do{
		 res=f_read(&file,filedatabuf,2048, &readcount);
		if(res!=FR_OK)
		{
     //  LCD_UsrLog("f_read->updata.bin->error.\n");
			 goto updateerror;
		}
		for(j=0;j<sizeof(FDbuf.filedatabuf)/sizeof(plain_decrypt);j++)
		{
			mbedtls_aes_setkey_dec(&aes, key, 128);//���ý�����Կ
			memset(plain_decrypt,0,sizeof(plain_decrypt));
			mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_DECRYPT, &FDbuf.filedatabuf[j*16], plain_decrypt);//ECB����
			memcpy(&FDbuf.filedatabuf[j*16],plain_decrypt,sizeof(plain_decrypt));
		}
		 wr_state=IAP_Flash_Write(USER_FLASH_FIRST_PAGE_ADDRESS+i*2048,filedatabuf,readcount/4);
		 if(0!=wr_state) 
		 {
			// LCD_UsrLog("Flash.........error\n");
			 goto updateerror;
		 } 
		 i++;
	 }while(f_eof(&file)==0);//�����ļ�ĩβ

	//  LCD_UsrLog("ffok...............%dKB\n",fsize/1024);

   // LCD_UsrLog("ffok............COMPLETE\n");
	 // LCD_UsrLog("Jump to the application...\n");
	  Jumpflag = 55;
//	  I2C_EE_BufferWrite(&Jumpflag, EEP_Firstpage, 1);
	  goto updateerror;
  }

	  updateerror:
	  __asm("CPSID  I");
        
		JumpAddress = *(volatile uint32_t*) (USER_FLASH_FIRST_PAGE_ADDRESS+4);
		pUserApp = (void (*)(void)) JumpAddress;
		TIM_Cmd(BASIC_TIM, DISABLE);	
		TIM_DeInit(TIM2);
		TIM_Cmd(TIM2,DISABLE);
		TIM_DeInit(BASIC_TIM);
		TIM_ITConfig(BASIC_TIM,TIM_IT_Update,DISABLE);
		TIM_Cmd(BASIC_TIM, DISABLE);	
//		USART_DeInit(DEBUG_USART);
//		USART_ITConfig(DEBUG_USART, USART_IT_RXNE, DISABLE);		
//		USART_Cmd(DEBUG_USART,DISABLE);
		RCC_DeInit();
		RCC_RTCCLKCmd(DISABLE);
		EXTI_DeInit();
		SysTick->CTRL = 0;
		RTC_DeInit();
		RTC_ITConfig(RTC_IT_WUT,DISABLE);//�ر�WAKE UP ��ʱ���ж�
		RTC_WakeUpCmd( DISABLE);//�ر�WAKE UP ��ʱ����
//		Disable_Extiint();
		USBH_DeInit(&USB_OTG_Core,&USB_Host);
		__disable_irq();
		NVIC_DisableIRQ(OTG_FS_IRQn);
		NVIC_DisableIRQ(OTG_FS_WKUP_IRQn);
		NVIC_DisableIRQ(OTG_HS_IRQn);
		NVIC_DisableIRQ(OTG_HS_WKUP_IRQn);
		__ASM volatile ("cpsid i");
		/* Initialize user application's Stack Pointer */
		__set_PSP(*(volatile uint32_t*) USER_FLASH_FIRST_PAGE_ADDRESS);
		__set_CONTROL(0);
		__set_MSP(*(volatile uint32_t*) USER_FLASH_FIRST_PAGE_ADDRESS);
		
        
		
//		NVIC_SystemReset();
		pUserApp();
		/*************************Jump to User Application area end***************************/
	
    return 0;

}

void JumpBoot(u8 flag)
{
  	void (*pUserApp)(void);
  uint32_t JumpAddress;
	if(flag==55)
  {		
	__asm("CPSID  I");
        
		JumpAddress = *(volatile uint32_t*) (USER_FLASH_FIRST_PAGE_ADDRESS+4);
		pUserApp = (void (*)(void)) JumpAddress;
		TIM_Cmd(BASIC_TIM, DISABLE);	
		TIM_DeInit(TIM2);
		TIM_Cmd(TIM2,DISABLE);
		TIM_DeInit(BASIC_TIM);
		TIM_ITConfig(BASIC_TIM,TIM_IT_Update,DISABLE);
		TIM_Cmd(BASIC_TIM, DISABLE);	
//		USART_DeInit(DEBUG_USART);
//		USART_ITConfig(DEBUG_USART, USART_IT_RXNE, DISABLE);		
//		USART_Cmd(DEBUG_USART,DISABLE);
		RCC_DeInit();
		RCC_RTCCLKCmd(DISABLE);
		EXTI_DeInit();
		SysTick->CTRL = 0;
		RTC_DeInit();
		RTC_ITConfig(RTC_IT_WUT,DISABLE);//�ر�WAKE UP ��ʱ���ж�
		RTC_WakeUpCmd( DISABLE);//�ر�WAKE UP ��ʱ����
//		Disable_Extiint();
		USBH_DeInit(&USB_OTG_Core,&USB_Host);
		__disable_irq();
		NVIC_DisableIRQ(OTG_FS_IRQn);
		NVIC_DisableIRQ(OTG_FS_WKUP_IRQn);
		NVIC_DisableIRQ(OTG_HS_IRQn);
		NVIC_DisableIRQ(OTG_HS_WKUP_IRQn);
		__ASM volatile ("cpsid i");
		/* Initialize user application's Stack Pointer */
		__set_PSP(*(volatile uint32_t*) USER_FLASH_FIRST_PAGE_ADDRESS);
		__set_CONTROL(0);
		__set_MSP(*(volatile uint32_t*) USER_FLASH_FIRST_PAGE_ADDRESS);
		
        
		
//		NVIC_SystemReset();
		pUserApp();
	}
}


/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
	const u8 (*pt)[sizeof(ALERT[0])];
	
	static u16 usbcount;
	static u8 skipkey;
	u8 key;
//	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0xC0000);
//	__enable_irq();
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();	 
	g_LcdDirection=1;
	Keyboard_Init();//������ʼ��
//  /*��ʼ��Һ����*/
//  LCD_Init();
//  LCD_LayerInit();
//  LTDC_Cmd(ENABLE);
	
//	/*�ѱ�����ˢ��ɫ*/
//  LCD_SetLayer(LCD_BACKGROUND_LAYER);  
//	LCD_Clear(LCD_COLOR_BLACK);
//	
//  /*��ʼ����Ĭ��ʹ��ǰ����*/
//	LCD_SetLayer(LCD_FOREGROUND_LAYER); 
//	/*Ĭ�����ò�͸��	���ú�������Ϊ��͸���ȣ���Χ 0-0xff ��0Ϊȫ͸����0xffΪ��͸��*/
//  LCD_SetTransparency(0xFF);
//	LCD_Clear(LCD_COLOR_BLACK);
//	/*����LCD_SetLayer(LCD_FOREGROUND_LAYER)������
//	����Һ����������ǰ����ˢ�£��������µ��ù�LCD_SetLayer�������ñ�����*/		
//	
//  LED_BLUE;    
	//////////////////////////////////////////Һ����ʼ��
//	__set_PRIMASK(0);
	GPIO_Configuration();
    LCD_Init();
    LCD_LayerInit();
    LTDC_Cmd(ENABLE);
	/*�ѱ�����ˢ��ɫ*/
    LCD_SetLayer(LCD_BACKGROUND_LAYER);  
    LCD_Clear(LCD_COLOR_RED);
	
  /*��ʼ����Ĭ��ʹ��ǰ����*/
	LCD_SetLayer(LCD_FOREGROUND_LAYER); 
//	/*Ĭ�����ò�͸��	���ú�������Ϊ��͸���ȣ���Χ 0-0xff ��0Ϊȫ͸����0xffΪ��͸��*/
    LCD_SetTransparency(0xff);
	LCD_Clear(LCD_COLOR_TEST_BACK);
	TIM6_Configuration();
//  Delay(0xfff);  -
  USBH_Init(&USB_OTG_Core,
			USB_OTG_FS_CORE_ID,
            &USB_Host,
            &USBH_MSC_cb,
            &USR_cb);
     USBH_Process(&USB_OTG_Core, &USB_Host);
  

  while(1)
	{
		pt = ALERT;
		Colour.black=LCD_COLOR_TEST_BACK;
		Colour.Fword=Red;
		WriteString_16(50,50,pt[0],0);
		USBH_Process(&USB_OTG_Core, &USB_Host);
		result = f_mount(&fs,"0:",1);
		if(result == FR_OK && usbcount > 20)
		{
			WriteString_16(50,70,pt[1],0);
			WriteString_16(50,90,pt[2],0);
			WriteString_16(50,110,pt[3],0);
//			File_IAP();			
		}
		usbcount++;
//		key=Key_Read_WithTimeOut(TICKS_PER_SEC_SOFTTIMER/10);//�ȴ�����(100*10ms/10=100ms)

		if(Keyboard.state==TRUE)
        {
			if(skipkey == 0 && Keyboard.continuance == FALSE)
			{
				key=Key_Read();
				if(key!=KEY_NONE)
				switch(key)
				{
						case Key_F1:
							
							
						break;
						case Key_F2:
							
						break;
						case Key_F3:
							
						break;
						case Key_F4:
							
						break;
						case Key_F5:
							
						break;

						case Key_LEFT:
							
						break;
						case Key_RIGHT:
							
						break;

						
						case Key_NUM1:
//							Range++;
//						break;
						case Key_NUM2:
						//break;
						case Key_NUM3:
						//break;
						case Key_NUM4:
						//break;
						case Key_NUM5:
						//break;
						case Key_NUM6:
						//break;
						case Key_NUM7:
						//break;
						case Key_NUM8:
						//break;
						case Key_NUM9:
						//break;
						case Key_NUM0:
//						break;
						case Key_DOT:

						break;

						case Key_REST:
						break;
						case Key_ONOFF:							

						break;
						case Key_SET1:
						{
							JumpBoot(55);
						}break;
						case Key_Ent:
						{
							File_IAP();	
						}break;
						case Key_ESC:
						{
							
						}break;
						case PRESS_SPIN:
						{

						}break;
						default:
						break;
							
					}
				}
         }
//		LCD_Test();
//		LCD_SetColors(LCD_COLOR_BLUE,LCD_COLOR_BLACK);
//		LCD_DrawRect(200,350,50,200);
		Lock_Off();
		Delay(0XFFFFF);
		Lock_On();
		Delay(0XFFFFF);
	}  

}
void Led_GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|
    RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOF|RCC_AHB1Periph_GPIOH,ENABLE);//
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;     //2  PASS
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
    GPIO_Init(GPIOE,&GPIO_InitStructure); 
    
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_14;//10 FAIL  11  LOCK  12 TRIP  14 ON
    GPIO_Init(GPIOH,&GPIO_InitStructure); 
   // RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);//
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3;//OFF
    GPIO_Init(GPIOD,&GPIO_InitStructure); 


}

void GPIO_Configuration(void)
{
     GPIO_InitTypeDef GPIO_InitStructure;	
	//���� ���ڵ���
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD/
    RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOF|RCC_AHB1Periph_GPIOH|RCC_AHB1Periph_GPIOI,ENABLE);//
    
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//������
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
    GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13;//������
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
//    GPIO_SetBits(GPIOC,GPIO_Pin_10);
	
    /* ʹ�� USART ʱ�� */
//  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
//      GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//|GPIO_Pin_2;//TX
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//	GPIO_Init(GPIOB,&GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;//|GPIO_Pin_3;//RX
//	GPIO_Init(GPIOB,&GPIO_InitStructure);

////  
//    GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3);

//  /*  ���� PXx �� USARTx__Rx*/
//  GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3);
//  
  
    //RS485_1
    
//    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//|GPIO_Pin_2;//TX
////	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
//	GPIO_Init(GPIOC,&GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;//|GPIO_Pin_3;//RX
////	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
//	GPIO_Init(GPIOC,&GPIO_InitStructure);
//    
//    
//    //RS485_2
//    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12;//|GPIO_Pin_2;//TX
////	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
//	GPIO_Init(GPIOC,&GPIO_InitStructure);
//    
//    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;//|GPIO_Pin_3;//RX
////	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
//	GPIO_Init(GPIOD,&GPIO_InitStructure);
    
//    //485�������
//    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1;
//    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
//    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
////	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
//	GPIO_Init(GPIOD,&GPIO_InitStructure); 

	
    //4094���ƶ˿�
    
//    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_5;
//    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
//    GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
//    GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;//PC5�ǿ����̵���
//    
//    
////	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
//	GPIO_Init(GPIOC,&GPIO_InitStructure);
//    
//    GPIO_SetBits(GPIOC,GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_1);
    
//    
//    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5;
//    GPIO_Init(GPIOH,&GPIO_InitStructure);
//    GPIO_ResetBits(GPIOH,GPIO_Pin_5);
//    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;
//    GPIO_Init(GPIOI,&GPIO_InitStructure);
    
    
    //74HC595���ƶ˿�
//	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_11|GPIO_Pin_10;
//    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
////	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
//	GPIO_Init(GPIOA,&GPIO_InitStructure);
    
    //ADS1216���ƶ˿�
//    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_12|GPIO_Pin_11;
//	GPIO_Init(GPIOB,&GPIO_InitStructure); 
//    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5;
//	GPIO_Init(GPIOI,&GPIO_InitStructure); 
//    GPIO_ResetBits(GPIOI,GPIO_Pin_5);


    
//    //��λ����
//    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3\
//    |GPIO_Pin_4|GPIO_Pin_6;
////	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
//	GPIO_Init(GPIOE,&GPIO_InitStructure); 
//	
//    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;
//    
////	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
//	GPIO_Init(GPIOB,&GPIO_InitStructure);
    
     //ADS1216���ƶ˿�����
    
//    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
// //   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//    
//    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13;
////	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
//    GPIO_Init(GPIOB,&GPIO_InitStructure); 
//    
//	Led_GPIO_Configuration();//LED
////	CPLD_GPIO_Configuration( );//CPLD�˿�����
////	V_I_Select_Configuration();//������ѹѡ���ź�
////	Open_flag_Configuration();//��·�ź�
////	PLC_GPIO_Configuration();//PLC�����ź�
//    
//	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4;
//    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
//    GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
//    GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;//PC5�ǿ����̵���
//	GPIO_Init(GPIOH,&GPIO_InitStructure);
	
	                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   	
}

void Lock_On(void)
{
    GPIO_SetBits(GPIOH,GPIO_Pin_11);
}
void Lock_Off(void)
{
    GPIO_ResetBits(GPIOH,GPIO_Pin_11);
}


void Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����
{
	for(; nCount != 0; nCount--);
}
/*********************************************END OF FILE**********************/

