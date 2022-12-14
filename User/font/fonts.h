/**
  ******************************************************************************
  * @file    fonts.h
  * @author  MCD Application Team
  * @version V5.0.2
  * @date    05-March-2012
  * @brief   Header for fonts.c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FONTS_H
#define __FONTS_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/** @addtogroup Utilities
  * @{
  */
  
/** @addtogroup STM32_EVAL
  * @{
  */ 

/** @addtogroup Common
  * @{
  */

/** @addtogroup FONTS
  * @{
  */ 

/** @defgroup FONTS_Exported_Types
  * @{
  */ 
typedef struct _tFont
{    
  const uint16_t *table;
  uint16_t Width;
  uint16_t Height;
  
} sFONT;

extern sFONT Font16x24;
extern sFONT Font12x12;
extern sFONT Font8x12;
extern sFONT Font8x8;
extern const unsigned char _num[][32];
extern const unsigned char Out_Assic[][40];
extern const uint8_t Out_1224[][16];
extern const unsigned char nBitmapDot[];
 void GetASCIICode(unsigned char* pBuffer,unsigned char ASCII); 
/**
  * @}
  */ 

/** @defgroup FONTS_Exported_Constants
  * @{
  */ 
#define LINE(x) ((x) * (((sFONT *)LCD_GetFont())->Height))



/*******************????********** ???????????????????????? ***************************/
#define      macWIDTH_CH_CHAR		                24	    //???????????? 
#define      macHEIGHT_CH_CHAR		              24		  //???????????? 

//0????????SD????????????????FLASH????,????SD??????????????????????????????

#define GBKCODE_FLASH 						1

#if GBKCODE_FLASH
/*????FLASH????*/
/*??????????????FLASH??????????*/
/*FLASH*/
#define GBKCODE_START_ADDRESS   1360*4096


/*??????????????*/
//??????????????????????????????????ucBuffer??????????????????usChar????????????????????
#define      macGetGBKCode( ucBuffer, usChar )  GetGBKCode_from_EXFlash( ucBuffer, usChar )  
int GetGBKCode_from_EXFlash( uint8_t * pBuffer, uint16_t c);

#else
/*????SD????*/


/*SD??????????*/
#define GBKCODE_FILE_NAME			"0:/Font/GB2312_H2424.FON"


/*??????????????*/
//??????????????????????????????????ucBuffer??????????????????usChar????????????????????

#define macGetGBKCode( ucBuffer, usChar )  GetGBKCode_from_sd( ucBuffer, usChar )
int GetGBKCode_from_sd ( uint8_t * pBuffer, uint16_t c);

#endif

/******************************************************************************/


/**
  * @}
  */ 

/** @defgroup FONTS_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup FONTS_Exported_Functions
  * @{
  */ 
/**
  * @}
  */

#ifdef __cplusplus
}


#endif

#endif /* __FONTS_H */
 
/**
  * @}
  */

/**
  * @}
  */ 

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */      

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
