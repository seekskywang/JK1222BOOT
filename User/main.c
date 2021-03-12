/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   LTDC-液晶显示英文，字库在内部FLASH
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F429 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
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
//递归扫描文件时使用的路径
char scan_path[255] = "0:";
const uint8_t ALERT[][21+5]=
{
	{"请插入U盘"},
	{"正在升级中请不要关闭电源"},
	{"升级结束后自动关机"},
	{"关机后请拔掉电源重启"},


};

mbedtls_aes_context aes;
//密钥
const unsigned char key[16] = "1234567890123456";

//解密后的明文
static unsigned char plain_decrypt[16] = {0};

static union FD FDbuf;


UINT fnum;            					  /* 文件成功读写数量 */
BYTE ReadBuffer[1024]={0};        /* 读缓冲区 */
BYTE WriteBuffer[] =              /* 写缓冲区*/
"欢迎使用野火STM32 F429开发板 今天是个好日子，新建文件系统测试文件\r\n";  

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
  * @brief  scan_files 递归扫描FatFs内的文件
  * @param  path:初始扫描路径
  * @retval result:文件系统的返回值
  */
static FRESULT scan_files (char* path) 
{ 
  FRESULT res; 		//部分在递归过程被修改的变量，不用全局变量	
  FILINFO fno; 
  DIR dir; 
  int i;            
  char *fn;        // 文件名	
	
#if _USE_LFN 
  /* 长文件名支持 */
  /* 简体中文需要2个字节保存一个“字”*/
  static char lfn[_MAX_LFN*2 + 1]; 	
  fno.lfname = lfn; 
  fno.lfsize = sizeof(lfn); 
#endif 
  //打开目录
  res = f_opendir(&dir, path); 
  if (res == FR_OK) 
	{ 
    i = strlen(path); 
    for (;;) 
		{ 
      //读取目录下的内容，再读会自动读下一个文件
      res = f_readdir(&dir, &fno); 								
      //为空时表示所有项目读取完毕，跳出
      if (res != FR_OK || fno.fname[0] == 0) break; 	
#if _USE_LFN 
      fn = *fno.lfname ? fno.lfname : fno.fname; 
#else 
      fn = fno.fname; 
#endif 
      //点表示当前目录，跳过			
      if (*fn == '.') continue; 	
      //目录，递归读取      
      if (fno.fattrib & AM_DIR)         
			{ 			
        //合成完整目录名        
        sprintf(&path[i], "/%s", fn); 		
        //递归遍历         
        res = scan_files(path);	
        path[i] = 0;         
        //打开失败，跳出循环        
        if (res != FR_OK) 
					break; 
      } 
			else 
			{ 
				printf("%s/%s\r\n", path, fn);								//输出文件名	
        /* 可以在这里提取特定格式的文件路径 */        
      }//else
    } //for
  } 
  return res; 
}

/*
*********************************************************************************************************
*	函 数 名: read_write_flie
*	功能说明: 读取文件test.txt前128个字符，并打印到串口
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void read_write_flie(void)
{	

/*----------------------- 文件系统测试：写测试 -----------------------------*/
	/* 打开文件，如果文件不存在则创建它 */
	printf("\r\n****** 即将进行文件写入测试... ******\r\n");	
	result = f_open(&file, "0:FatFs读写测试文件.txt",FA_CREATE_ALWAYS | FA_WRITE );
	if ( result == FR_OK )
	{
		printf("》打开/创建FatFs读写测试文件.txt文件成功，向文件写入数据。\r\n");
    /* 将指定存储区内容写入到文件内 */
		result=f_write(&file,WriteBuffer,sizeof(WriteBuffer),&fnum);
    if(result==FR_OK)
    {
      printf("》文件写入成功，写入字节数据：%d\n",fnum);
      printf("》向文件写入的数据为：\r\n%s\r\n",WriteBuffer);
    }
    else
    {
      printf("！！文件写入失败：(%d)\n",result);
    }    
		/* 不再读写，关闭文件 */
    f_close(&file);
	}
	else
	{	
		printf("！！打开/创建文件失败。\r\n");
	}
	
/*------------------- 文件系统测试：读测试 ------------------------------------*/
	printf("****** 即将进行文件读取测试... ******\r\n");
	result = f_open(&file, "0:FatFs读写测试文件.txt", FA_OPEN_EXISTING | FA_READ); 	 
	if(result == FR_OK)
	{
		printf("》打开文件成功。\r\n");
		result = f_read(&file, ReadBuffer, sizeof(ReadBuffer), &fnum); 
    if(result==FR_OK)
    {
      printf("》文件读取成功,读到字节数据：%d\r\n",fnum);
      printf("》读取得的文件数据为：\r\n%s \r\n", ReadBuffer);	
    }
    else
    {
      printf("！！文件读取失败：(%d)\n",result);
    }		
	}
	else
	{
		printf("！！打开文件失败。\r\n");
	}
	/* 不再读写，关闭文件 */
	f_close(&file);	
}

void GetFlashSize(void)
{
    static u16 Stm32_Flash_Size;
    
    Stm32_Flash_Size = (*(__IO u16*)(0x1FFF7A22));//闪存容量寄存器
    
    printf("芯片闪存容量大小为%dK\r\n",Stm32_Flash_Size);

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
	//LCD_UsrLog("fsize:%dKB\n",fsize/1024); //因为只擦除了扇区5,所以,固件不能超过128KB
	                                      //如有更大固件需求,请擦除扇区至固件匹配的大小
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
			mbedtls_aes_setkey_dec(&aes, key, 128);//设置解密密钥
			memset(plain_decrypt,0,sizeof(plain_decrypt));
			mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_DECRYPT, &FDbuf.filedatabuf[j*16], plain_decrypt);//ECB解密
			memcpy(&FDbuf.filedatabuf[j*16],plain_decrypt,sizeof(plain_decrypt));
		}
		 wr_state=IAP_Flash_Write(USER_FLASH_FIRST_PAGE_ADDRESS+i*2048,filedatabuf,readcount/4);
		 if(0!=wr_state) 
		 {
			// LCD_UsrLog("Flash.........error\n");
			 goto updateerror;
		 } 
		 i++;
	 }while(f_eof(&file)==0);//到达文件末尾

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
		RTC_ITConfig(RTC_IT_WUT,DISABLE);//关闭WAKE UP 定时器中断
		RTC_WakeUpCmd( DISABLE);//关闭WAKE UP 定时器　
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
		RTC_ITConfig(RTC_IT_WUT,DISABLE);//关闭WAKE UP 定时器中断
		RTC_WakeUpCmd( DISABLE);//关闭WAKE UP 定时器　
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
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
	const u8 (*pt)[sizeof(ALERT[0])];
	
	static u16 usbcount;
	static u8 skipkey;
	u8 key;
//	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0xC0000);
//	__enable_irq();
	/* LED 端口初始化 */
	LED_GPIO_Config();	 
	g_LcdDirection=1;
	Keyboard_Init();//按键初始化
//  /*初始化液晶屏*/
//  LCD_Init();
//  LCD_LayerInit();
//  LTDC_Cmd(ENABLE);
	
//	/*把背景层刷黑色*/
//  LCD_SetLayer(LCD_BACKGROUND_LAYER);  
//	LCD_Clear(LCD_COLOR_BLACK);
//	
//  /*初始化后默认使用前景层*/
//	LCD_SetLayer(LCD_FOREGROUND_LAYER); 
//	/*默认设置不透明	，该函数参数为不透明度，范围 0-0xff ，0为全透明，0xff为不透明*/
//  LCD_SetTransparency(0xFF);
//	LCD_Clear(LCD_COLOR_BLACK);
//	/*经过LCD_SetLayer(LCD_FOREGROUND_LAYER)函数后，
//	以下液晶操作都在前景层刷新，除非重新调用过LCD_SetLayer函数设置背景层*/		
//	
//  LED_BLUE;    
	//////////////////////////////////////////液晶初始化
//	__set_PRIMASK(0);
	GPIO_Configuration();
    LCD_Init();
    LCD_LayerInit();
    LTDC_Cmd(ENABLE);
	/*把背景层刷黑色*/
    LCD_SetLayer(LCD_BACKGROUND_LAYER);  
    LCD_Clear(LCD_COLOR_RED);
	
  /*初始化后默认使用前景层*/
	LCD_SetLayer(LCD_FOREGROUND_LAYER); 
//	/*默认设置不透明	，该函数参数为不透明度，范围 0-0xff ，0为全透明，0xff为不透明*/
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
//		key=Key_Read_WithTimeOut(TICKS_PER_SEC_SOFTTIMER/10);//等待按键(100*10ms/10=100ms)

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
	//串口 用于调试
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD/
    RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOF|RCC_AHB1Periph_GPIOH|RCC_AHB1Periph_GPIOI,ENABLE);//
    
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//蜂鸣器
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
    GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13;//蜂鸣器
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
//    GPIO_SetBits(GPIOC,GPIO_Pin_10);
	
    /* 使能 USART 时钟 */
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

//  /*  连接 PXx 到 USARTx__Rx*/
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
    
//    //485方向控制
//    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1;
//    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
//    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
////	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
//	GPIO_Init(GPIOD,&GPIO_InitStructure); 

	
    //4094控制端口
    
//    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_5;
//    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
//    GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
//    GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;//PC5是开机继电器
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
    
    
    //74HC595控制端口
//	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_11|GPIO_Pin_10;
//    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
////	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
//	GPIO_Init(GPIOA,&GPIO_InitStructure);
    
    //ADS1216控制端口
//    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_12|GPIO_Pin_11;
//	GPIO_Init(GPIOB,&GPIO_InitStructure); 
//    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5;
//	GPIO_Init(GPIOI,&GPIO_InitStructure); 
//    GPIO_ResetBits(GPIOI,GPIO_Pin_5);


    
//    //档位控制
//    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3\
//    |GPIO_Pin_4|GPIO_Pin_6;
////	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
//	GPIO_Init(GPIOE,&GPIO_InitStructure); 
//	
//    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;
//    
////	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
//	GPIO_Init(GPIOB,&GPIO_InitStructure);
    
     //ADS1216控制端口输入
    
//    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
// //   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//    
//    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13;
////	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
//    GPIO_Init(GPIOB,&GPIO_InitStructure); 
//    
//	Led_GPIO_Configuration();//LED
////	CPLD_GPIO_Configuration( );//CPLD端口配置
////	V_I_Select_Configuration();//电流电压选择信号
////	Open_flag_Configuration();//开路信号
////	PLC_GPIO_Configuration();//PLC控制信号
//    
//	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4;
//    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
//    GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
//    GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;//PC5是开机继电器
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


void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}
/*********************************************END OF FILE**********************/

