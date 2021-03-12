#include "pbdata.h"
#include <math.h>

//==========================================================
//ȫ�ֱ���
Key_TypeDef Keyboard;//����ṹ��

static u8 LastKeyVal;//�ϴμ�ֵ
static u8 LastKeyTicks;//��������
static u8 ContinueKeyTicks;//������������
const u8 Scan_Value[]={0x7f,0xbf,0xdf,0xef,0xf7,0xfb,0xfd,0xfe};
u8 KekValue;
u16 g_enc_fast_flag;
u8 spinflag;
u16 spintest,spinnum;
void Key_Delay(u32 num)
{
    while(num)
    {
        num--;
    }
    

}





 

void Keyboard_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|
	RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOG|RCC_AHB1Periph_GPIOH|RCC_AHB1Periph_GPIOI,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=DIG1_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(DIG1_GPIO_PORT,&GPIO_InitStructure);
    GPIO_SetBits(DIG1_GPIO_PORT,DIG1_PIN);
	
	GPIO_InitStructure.GPIO_Pin=DIG2_PIN;
	GPIO_Init(DIG2_GPIO_PORT,&GPIO_InitStructure);
    GPIO_SetBits(DIG2_GPIO_PORT,DIG2_PIN);
	
	GPIO_InitStructure.GPIO_Pin=DIG3_PIN;
	GPIO_Init(DIG3_GPIO_PORT,&GPIO_InitStructure);
    GPIO_SetBits(DIG3_GPIO_PORT,DIG3_PIN);
	
	GPIO_InitStructure.GPIO_Pin=DIG4_PIN;
	GPIO_Init(DIG4_GPIO_PORT,&GPIO_InitStructure);
    GPIO_SetBits(DIG4_GPIO_PORT,DIG4_PIN);
	
	GPIO_InitStructure.GPIO_Pin=KI1_PIN;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
	GPIO_Init(KI1_GPIO_PORT,&GPIO_InitStructure);

	
	GPIO_InitStructure.GPIO_Pin=KI2_PIN;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
	GPIO_Init(KI2_GPIO_PORT,&GPIO_InitStructure);

	
	GPIO_InitStructure.GPIO_Pin=KI3_PIN;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
	GPIO_Init(KI3_GPIO_PORT,&GPIO_InitStructure);

	
	GPIO_InitStructure.GPIO_Pin=KI4_PIN;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
	GPIO_Init(KI4_GPIO_PORT,&GPIO_InitStructure);

	
	GPIO_InitStructure.GPIO_Pin=KI5_PIN;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
	GPIO_Init(KI5_GPIO_PORT,&GPIO_InitStructure);

	
	GPIO_InitStructure.GPIO_Pin=KI6_PIN;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
	GPIO_Init(KI6_GPIO_PORT,&GPIO_InitStructure);

	
	GPIO_InitStructure.GPIO_Pin=KI7_PIN;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
	GPIO_Init(KI7_GPIO_PORT,&GPIO_InitStructure);

	
}
u8 Key_Identiry(void)
{
	KekValue = KEY_NONE;
	
	DIG1L;
	DIG2H;
	DIG3H;
	DIG4H;	
	delay(10);
	if(READKI1 == 0)
	{
		KekValue = 1;
	}else if(READKI2 == 0){
		KekValue = 2;
	}else if(READKI3 == 0){
		KekValue = 3;
	}else if(READKI4 == 0){
		KekValue = 4;
	}else if(READKI5 == 0){
		KekValue = 5;
	}else if(READKI6 == 0){
		KekValue = 6;
	}else if(READKI7 == 0){
		KekValue = 7;
	}
	
	DIG1H;
	DIG2L;
	DIG3H;
	DIG4H;
	delay(10);
	if(READKI1 == 0)
	{
		KekValue = 8;
	}else if(READKI2 == 0){
		KekValue = 9;
	}else if(READKI3 == 0){
		KekValue = 10;
	}else if(READKI4 == 0){
		KekValue = 11;
	}else if(READKI5 == 0){
		KekValue = 12;
	}else if(READKI6 == 0){
		KekValue = 13;
	}else if(READKI7 == 0){
		KekValue = 14;
	}
	
	DIG1H;
	DIG2H;
	DIG3L;
	DIG4H;	
	delay(10);
	if(READKI1 == 0)
	{
		KekValue = 15;
	}else if(READKI2 == 0){
		KekValue = 16;
	}else if(READKI3 == 0){
		KekValue = 17;
	}else if(READKI4 == 0){
		KekValue = 18;
	}else if(READKI5 == 0){
		KekValue = 19;
	}else if(READKI6 == 0){
		KekValue = 20;
	}else if(READKI7 == 0){
		KekValue = 21;
	}
	
	DIG1H;
	DIG2H;
	DIG3H;
	DIG4L;	
	delay(10);
	if(READKI1 == 0)
	{
		KekValue = 22;
	}else if(READKI2 == 0){
		KekValue = 23;
	}else if(READKI3 == 0){
		KekValue = 24;
	}else if(READKI4 == 0){
		KekValue = 25;
	}else if(READKI5 == 0){
		KekValue = 26;
	}else if(READKI6 == 0){
		KekValue = 27;
	}else if(READKI7 == 0){
		KekValue = 28;
	}
	return (KekValue);
}


//void Keyboard_Init(void)
//{
//	
//    
//	GPIO_InitTypeDef GPIO_InitStructure;
//    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOI|RCC_AHB1Periph_GPIOD,ENABLE);
//	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9|GPIO_Pin_8|GPIO_Pin_12;
//	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
//    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//	GPIO_Init(GPIOC,&GPIO_InitStructure);
//    GPIO_SetBits(GPIOC,GPIO_Pin_9);
//    
//    
//    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_11;
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
//	GPIO_Init(GPIOC,&GPIO_InitStructure);
//    
//    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;
//	//GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
//	GPIO_Init(GPIOI,&GPIO_InitStructure);
//    
//    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;
//	///GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
//	GPIO_Init(GPIOD,&GPIO_InitStructure);
//	
//	Keyboard.value=KEY_NONE;//��ֵΪ��
//	Keyboard.state=FALSE;	//������Ч
//	Keyboard.continuance=FALSE;//����������Ч
//	
//	LastKeyVal=KEY_NONE;//�ϴμ�ֵ
//	LastKeyTicks=0;//��������
//	ContinueKeyTicks=0;//������������
//}

void _164Delaay(void)
{
	u16 i;
	for(i=0;i<10;i--)
	;

};
void HW_Sendvalueto164(u8 value)
{
	uint8_t i;
	_Hc164CpL;
	for(i=0;i<8;i++)
	{
		if((value&0x80)==0x80)
			_Hc164DH;
		else
			_Hc164DL;
		_Hc164CpH;
		_164Delaay();
		_Hc164CpL;
		value<<=1;
        //_164Delaay();
	}

}

//==========================================================
//�������ƣ�Key_Identiry
//�������ܣ�����ʶ��
//��ڲ�������
//���ڲ�����U8 ��ֵ
//�������ڣ�2014.09.25 
//�޸����ڣ�2014.09.29 14:45
//��ע˵�������������źţ�0��Ч
//==========================================================
//u8 Key_Identiry(void)
//{
//	u16 key1;
//	u8 key;
//	
//	key1=GPIO_ReadInputData(GPIOG);//GPIOG->ODR;//
//	key1|=0x01ff;
//	if(key1!=0xFFFF)//���������ź���Ч
//	{
//		switch(key1)
//		{
//			case HW_KEY_SET:
//				key=KEY_SET;
//				break;
//			case HW_KEY_UP:
//				key=KEY_UP;
//				break;
//			case HW_KEY_DOWN:
//				key=KEY_DOWN;
//				break;
////			case HW_KEY_LEFT:
////				key=KEY_LEFT;
////				break;
////			case HW_KEY_RIGHT:
////				key=KEY_RIGHT;
////				break;
//			case HW_KEY_ENTER:
//				key=KEY_ENTER;
//				break;
//			case HW_KEY_RESET:
//				key=KEY_RESET;
//				break;
//			case HW_KEY_START:
//				key=KEY_START;
//				break;
//			default:
//				key=KEY_NONE;
//				break;
//		}
//	}
//	else
//	{
//		key=KEY_NONE;
//	}
////	if(key==KEY_NONE)
////	{
////		key2=GPIO_ReadInputData(GPIOC);
////		key2|=0XFFCF;
////		if(key2!=0xFFFF)//���������ź���Ч
////		{
////			switch(key2)
////			{
//////				case HW_KEY_SET:
//////					key=KEY_SET;
//////					break;
//////				case HW_KEY_UP:
//////					key=KEY_UP;
//////					break;
////				case HW_KEY_DOWN:
////					key=KEY_DOWN;
////					break;
//////				case HW_KEY_LEFT:
//////					key=KEY_LEFT;
//////					break;
//////				case HW_KEY_RIGHT:
//////					key=KEY_RIGHT;
//////					break;
//////				case HW_KEY_ENTER:
//////					key=KEY_ENTER;
//////					break;
////				case HW_KEY_RESET:
////					key=KEY_RESET;
////					break;
//////				case HW_KEY_START:
//////					key=KEY_START;
//////					break;
////				default:
////					key=KEY_NONE;
////					break;
////			}
////		}
////		else
////		{
////			key=KEY_NONE;
////		}
////	}
//	return (key);
//}
//==========================================================
//�������ƣ�Key_Init
//�������ܣ������ṹ���ʼ��
//��ڲ�������
//���ڲ�������
//�������ڣ�2014.09.29
//�޸����ڣ�2014.09.29 10:49
//��ע˵������
//==========================================================
//void Key_Init(void)
//{
//    Keyboard_Init();//�����˿ڳ�ʼ��
//	Keyboard.value=KEY_NONE;//��ֵΪ��
//	Keyboard.state=FALSE;	//������Ч
//	Keyboard.continuance=FALSE;//����������Ч
//	
//	LastKeyVal=KEY_NONE;//�ϴμ�ֵ
//	LastKeyTicks=0;//��������
//	ContinueKeyTicks=0;//������������
//}

//==========================================================
//�������ƣ�Key_Read
//�������ܣ���ȡ����
//��ڲ�������
//���ڲ�������
//�������ڣ�2014.09.29
//�޸����ڣ�2014.09.29 10:50
//��ע˵������
//==========================================================
u8 Key_Read(void)
{
	if (Keyboard.state)	//�м�����
	{
		Keyboard.state=FALSE;
		//Beep_One();	//��������һ��
        Key_beep();
        
		return (Keyboard.value);
	}
	//��ֱ�Ӵ����Դ��ͨ�ð���
	return (KEY_NONE);	
}

//==========================================================
//�������ƣ�Key_Read_WithTimeOut
//�������ܣ�����ʽ��ȡ����
//��ڲ�����ticks:�ȴ�������(50ms����)
//���ڲ���������ֵ
//�������ڣ�2014.09.29
//�޸����ڣ�2014.09.29 10:56
//��ע˵����ֱ��������Ӧ����ʱ��
//==========================================================
u8 Key_Read_WithTimeOut(u8 ticks)
{
   // u8 i=1;
//	if (ticks)//��ʱ�ȴ�����
//		{
			SetSoftTimer(KEYBOARD_SOFTTIMER,ticks);//���ð�����ʱ����(����ʱ������,50ms����)
				
			while ((!GetSoftTimerOut(KEYBOARD_SOFTTIMER))&&(!Keyboard.state)) //��ʱδ�������ް�������
				{
//					RunOnKeyFree();//�ڰ����ȴ������д�������ҵ��
                    //i=GetSoftTimerOut(KEYBOARD_SOFTTIMER);
				}
//		}
//	else//���޵ȴ�����
//		{
//			while (!Keyboard.state) 
//				{
//					RunOnKeyFree();//�ڰ����ȴ������д�������ҵ��
//				}			
//		}
	return Key_Read();//��������
}
////==========================================================
////�������ƣ�Key_Scan
////�������ܣ�����ɨ��
////��ڲ�������
////���ڲ�������
////�������ڣ�2014.09.28 
////�޸����ڣ�2014.09.28 11:01
////��ע˵������10mS�жϵ���
////==========================================================
// u8 Key_Identiry(void)
//{
//    u8 row,i;
////	u8 scanvalue=0;
//	u8 key_value=0xff;
//	u8 value=0xff;
//	HW_Sendvalueto164(0);
//	if(GPIO_ReadInputDataBit(GPIOI, GPIO_Pin_11)==0)
//		row=0;
//	else if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_10)==0)
//		row=1;
//	else if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11)==0)
//		row=2;
//	else if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_2)==0)
//		row=3;
//	else 
//        row=4;
//    if(row<4)
//	{
//		
//		for(i=0;i<8;i++)
//		{
//			HW_Sendvalueto164(Scan_Value[i]);
//            _164Delaay();
//			switch(row)
//			{
//				case 0:
//						
//						if(GPIO_ReadInputDataBit(GPIOI, GPIO_Pin_11)==0)
//						{
//							
//							value=i;
//						}
//						
//					break;
//				case 1:
//					if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_10)==0)
//						value=i;
//					break;
//				case 2:
//					if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11)==0)
//						value=i;
//					break;
//				case 3:
//					if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_2)==0)
//						value=i;
//					break;
//				default:
//					break;
//			}
//            if(value!=0xff)
//			break;
//		}
//        if(value==0xff)
//            return  KEY_NONE;
//		key_value=row*8+value;
//        return key_value;
//	
//	
//	}
//    else
//        if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_13)==0)
//        {
////            return 116;
//        
//        
//        }
//    return  KEY_NONE;
//    
//	
//    
//}

void Key_Scan(void)
{
	u8 keyval;
	
	keyval = Key_Identiry();//����ʶ�𣬷��ؼ�ֵ

	//��������ʶ��
	if (keyval==KEY_NONE)//�޼�
	{
		if (LastKeyVal)	//�ϴ��м����̰����б�
		{
			if ( (LastKeyTicks>SHORTKEYCOUNT) && (LastKeyTicks<LONGKEYCOUNT) )
			{
				Keyboard.value=LastKeyVal;	//��ֵ
				Keyboard.state=TRUE;		//�а���
			}
		}
		LastKeyVal=KEY_NONE;
		LastKeyTicks=0;
		Keyboard.continuance=FALSE; //��������	
	}
	else	//�м�
	{
		if (LastKeyVal==keyval)//�ϴΰ����ͱ��ΰ�����ͬ
		{
			if (LastKeyTicks<LONGKEYCOUNT+1)//������������100x10mS
				LastKeyTicks++;
			
			if (LastKeyTicks==LONGKEYCOUNT)//���ڳ�����������
			{
				ContinueKeyTicks=0;//������������
				//keyval|=LONG_PRESS;	//������
				Keyboard.value=keyval;	//��ֵ
				Keyboard.state=TRUE;	//�а���
//				Keyboard.continuance=FALSE; //��������
				Keyboard.continuance=TRUE; //��������
			}
			else if (LastKeyTicks>LONGKEYCOUNT)//���ڳ�����������
			{
				if(HW_KEYBOARD_CONTINUE_SUPPORT)//����������Ч�б�
				{
					//keyval|=LONG_PRESS;	//������
					Keyboard.value=keyval;//��ֵ
//					Keyboard.state=TRUE;//�а���
					Keyboard.continuance=TRUE; //��������
					ContinueKeyTicks++;
					if(ContinueKeyTicks>CONTINUEKEYCOUNT)//��������������
					{
						ContinueKeyTicks=0;//������������
						if(Keyboard.state==FALSE)//�����Ѷ�ȡ
							Keyboard.state=TRUE;//�а���
					}
				}
			}					
		}
		else//�ϴΰ����ͱ��ΰ�����ͬ
		{
			LastKeyVal=keyval;
			LastKeyTicks=0;
		}
	}
}

