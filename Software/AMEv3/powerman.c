#include "powerman.h"
#include "stn.h"
#include "spiflash.h"

void PM_Config()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC,&GPIO_InitStructure);
  GPIOC->BSRRH = GPIO_Pin_2;
}

void PM_SetCPUFreq(uint8_t freq)
{
  //PLL_VCO = (HSE_VALUE or HSI_VALUE / PLL_M) * PLL_N
  u8 PLL_M=8;
  u8 PLL_N=120;
  //SYSCLK = PLL_VCO / PLL_P
  u8 PLL_P=2;
  //USB OTG FS, SDIO and RNG Clock =  PLL_VCO / PLLQ
  u8 PLL_Q=5;
  RCC_ClocksTypeDef RCC_Clocks;
  
  //ѡ��HSIΪʱ��Դ
  RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
  RCC->CFGR |= RCC_CFGR_SW_HSI;
  //�ȴ�ʱ��Դ��ѡ��ΪHSI
  while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_HSI);
  if (freq!=16)//�����Ҫʹ��PLL
  {
    PLL_N=freq/2;
    //�ر���PLL
    RCC->CR &= (uint32_t)((uint32_t)~(RCC_CR_PLLON));
    //������PLL
    RCC->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) -1) << 16) |
                 (RCC_PLLCFGR_PLLSRC_HSI) | (PLL_Q << 24);
    //������PLL
    RCC->CR |= RCC_CR_PLLON;
    //�ȴ�PLL׼����
    while((RCC->CR & RCC_CR_PLLRDY) == 0);
    //����FLASHԤ����I/D-Cache������Flash�ȴ�
    FLASH->ACR = FLASH_ACR_PRFTEN |FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_3WS;
    //��ʱ��ԴѡΪPLL
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    //�ȴ���ʱ��Դ���л���PLL
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL);
  }
  else//���ʱ��Ϊ16MHz
  {
    //�ر���PLL
    RCC->CR &= (uint32_t)((uint32_t)~(RCC_CR_PLLON));
    //����FLASHԤ����I/D-Cache������Flash�ȴ�
    FLASH->ACR = FLASH_ACR_PRFTEN |FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_0WS;
  }
  SystemCoreClockUpdate();
  //��������SysTick
  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);
}

void PM_AdcInit()
{
  ADC_InitTypeDef ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //��ADC1ʱ��
  ADC_DeInit();
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;  //����Ϊ12λ           
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;   //ɨ��ת��ģʽʧ��
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;  //����ת��ʹ��
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; //�����ⲿ�������������ת��
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; //�����Ҷ��룬���ֽڶ���
  ADC_InitStructure.ADC_NbrOfConversion = 1;    //�涨��˳����й���ת����ADCͨ������Ŀ
  ADC_Init(ADC1, &ADC_InitStructure);      
  
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent; //����ģʽ
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div8; //��ƵΪ8
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //ʧ��DMA_MODE
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles;//���β������20������
  ADC_CommonInit(&ADC_CommonInitStructure);
  
  ADC_Cmd(ADC1, ENABLE);       //ʹ��ADC1
}

void PM_AdcDeInit()
{
  ADC_Cmd(ADC1, DISABLE);
  ADC_DeInit();
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);
}

uint32_t PM_GetVolt()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  uint32_t vref,volt;
  uint32_t realvolt;
  uint8_t i;
  
  PM_AdcInit();
  ADC_TempSensorVrefintCmd(ENABLE);
  vref=0;
  for(i=0;i<5;i++)
  {
    ADC_RegularChannelConfig(ADC1, ADC_Channel_Vrefint, 1, ADC_SampleTime_480Cycles);
    ADC_SoftwareStartConv(ADC1);
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));
    vref += ADC_GetConversionValue(ADC1);
  }
  ADC_TempSensorVrefintCmd(DISABLE);
  vref=vref/5;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB,&GPIO_InitStructure);
  
  PM_AdcInit();
  volt=0;
  for(i=0;i<5;i++)
  {
    ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_480Cycles);//����ͨ�����ã�1��ʾ���������˳��
    ADC_SoftwareStartConv(ADC1);
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));
    volt += ADC_GetConversionValue(ADC1);
  }
  PM_AdcDeInit();
  volt = volt/5;
  
  realvolt = (volt*1200*2)/vref;
  
  return realvolt;
}

void PM_EnterStopMode()
{
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
}

void PM_EnterStandbyMode()
{
  LCD_PowerSave();
  SPI_Flash_PowerDown();
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
  PWR_WakeUpPinCmd(ENABLE);
  SCB->SCR|=1<<2;//ʹ��SLEEPDEEPλ (SYS->CTRL)
  PWR->CR|=1<<2;           //���Wake-up ��־
  PWR->CR|=1<<1;           //PDDS��λ
  asm("WFI");               //ִ��WFEָ��
}
