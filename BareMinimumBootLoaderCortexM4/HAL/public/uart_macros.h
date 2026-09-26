#ifndef UART_MACROS_H
#define UART_MACROS_H

/*UART2_clock_enable()
 *Used to enable the clock to the UART peripheral
 */
#define UART2_clock_enable() RCC->rcc_apb1enr = RCC->rcc_apb1enr | (1<<17)

#endif /* UART_MACROS */
