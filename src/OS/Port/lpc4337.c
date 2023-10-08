
#include "os_arch_headers.h"
#ifdef LPC4337

#include "os_irq.h"

/* ======================= Private data type ======================== */

/* ================== Private variables declaration ================= */

/* ================== Private functions declaration ================= */

/* ================= Public functions implementation ================ */

void DAC_IRQHandler(void) {
    OS_IRQ_Handler(DAC_IRQn);
}

void M0APP_IRQHandler(void) {
    OS_IRQ_Handler(M0APP_IRQn);
}

void DMA_IRQHandler(void) {
    OS_IRQ_Handler(DMA_IRQn);
}

void FLASH_EEPROM_IRQHandler(void) {
    OS_IRQ_Handler(RESERVED1_IRQn);
}

void ETH_IRQHandler(void) {
    OS_IRQ_Handler(ETHERNET_IRQn);
}

void SDIO_IRQHandler(void) {
    OS_IRQ_Handler(SDIO_IRQn);
}

void LCD_IRQHandler(void) {
    OS_IRQ_Handler(LCD_IRQn);
}

// void USB0_IRQHandler(void) {
//     OS_IRQ_Handler(USB0_IRQn);
// }

void USB1_IRQHandler(void) {
    OS_IRQ_Handler(USB1_IRQn);
}

void SCT_IRQHandler(void) {
    OS_IRQ_Handler(SCT_IRQn);
}

void RIT_IRQHandler(void) {
    OS_IRQ_Handler(RITIMER_IRQn);
}

void TIMER0_IRQHandler(void) {
    OS_IRQ_Handler(TIMER0_IRQn);
}

void TIMER1_IRQHandler(void) {
    OS_IRQ_Handler(TIMER1_IRQn);
}

void TIMER2_IRQHandler(void) {
    OS_IRQ_Handler(TIMER2_IRQn);
}

void TIMER3_IRQHandler(void) {
    OS_IRQ_Handler(TIMER3_IRQn);
}

void MCPWM_IRQHandler(void) {
    OS_IRQ_Handler(MCPWM_IRQn);
}

void ADC0_IRQHandler(void) {
    OS_IRQ_Handler(ADC0_IRQn);
}

void I2C0_IRQHandler(void) {
    OS_IRQ_Handler(I2C0_IRQn);
}

void SPI_IRQHandler(void) {
    OS_IRQ_Handler(I2C1_IRQn);
}

void I2C1_IRQHandler(void) {
    OS_IRQ_Handler(SPI_INT_IRQn);
}

void ADC1_IRQHandler(void) {
    OS_IRQ_Handler(ADC1_IRQn);
}

void SSP0_IRQHandler(void) {
    OS_IRQ_Handler(SSP0_IRQn);
}

void SSP1_IRQHandler(void) {
    OS_IRQ_Handler(SSP1_IRQn);
}

void UART0_IRQHandler(void) {
    OS_IRQ_Handler(USART0_IRQn);
}

void UART1_IRQHandler(void) {
    OS_IRQ_Handler(UART1_IRQn);
}

void UART2_IRQHandler(void) {
    OS_IRQ_Handler(USART2_IRQn);
}

void UART3_IRQHandler(void) {
    OS_IRQ_Handler(USART3_IRQn);
}

void I2S0_IRQHandler(void) {
    OS_IRQ_Handler(I2S0_IRQn);
}

void I2S1_IRQHandler(void) {
    OS_IRQ_Handler(I2S1_IRQn);
}

void SPIFI_IRQHandler(void) {
    OS_IRQ_Handler(RESERVED4_IRQn);
}

void SGPIO_IRQHandler(void) {
    OS_IRQ_Handler(SGPIO_INT_IRQn);
}

void GPIO0_IRQHandler(void) {
    OS_IRQ_Handler(PIN_INT0_IRQn);
}

void GPIO1_IRQHandler(void) {
    OS_IRQ_Handler(PIN_INT1_IRQn);
}

void GPIO2_IRQHandler(void) {
    OS_IRQ_Handler(PIN_INT2_IRQn);
}

void GPIO3_IRQHandler(void) {
    OS_IRQ_Handler(PIN_INT3_IRQn);
}

void GPIO4_IRQHandler(void) {
    OS_IRQ_Handler(PIN_INT4_IRQn);
}

void GPIO5_IRQHandler(void) {
    OS_IRQ_Handler(PIN_INT5_IRQn);
}

void GPIO6_IRQHandler(void) {
    OS_IRQ_Handler(PIN_INT6_IRQn);
}

void GPIO7_IRQHandler(void) {
    OS_IRQ_Handler(PIN_INT7_IRQn);
}

void GINT0_IRQHandler(void) {
    OS_IRQ_Handler(GINT0_IRQn);
}

void GINT1_IRQHandler(void) {
    OS_IRQ_Handler(GINT1_IRQn);
}

void EVRT_IRQHandler(void) {
    OS_IRQ_Handler(EVENTROUTER_IRQn);
}

void CAN1_IRQHandler(void) {
    OS_IRQ_Handler(C_CAN1_IRQn);
}

void ADCHS_IRQHandler(void) {
    OS_IRQ_Handler(ADCHS_IRQn);
}

void ATIMER_IRQHandler(void) {
    OS_IRQ_Handler(ATIMER_IRQn);
}

void RTC_IRQHandler(void) {
    OS_IRQ_Handler(RTC_IRQn);
}

void WDT_IRQHandler(void) {
    OS_IRQ_Handler(WWDT_IRQn);
}

void M0SUB_IRQHandler(void) {
    OS_IRQ_Handler(M0SUB_IRQn);
}

void CAN0_IRQHandler(void) {
    OS_IRQ_Handler(C_CAN0_IRQn);
}

void QEI_IRQHandler(void) {
    OS_IRQ_Handler(QEI_IRQn);
}

#endif // LPC4337
