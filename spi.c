#include "stm32f4xx.h"

void SPI1_Pins_Init(void) {
    // Enable clock for GPIOA
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    // Set PA5, PA6, and PA7 to alternate function mode (SPI1 SCK, MISO, MOSI)
    GPIOA->MODER |= GPIO_MODER_MODER5_1 | GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1;
    GPIOA->MODER &= ~(GPIO_MODER_MODER5_0 | GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0);

    // Set alternate function to AF5 (SPI1) for PA5, PA6, and PA7
    GPIOA->AFR[0] |= (0x05 << 20) | (0x05 << 24) | (0x05 << 28);
}

void SPI1_Init(void) {
    // Enable clock access to SPI1 module
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    // Set clock to fPCLK/4
    SPI1->CR1 |= (1U << 3);
    SPI1->CR1 &= ~(1U << 4);
    SPI1->CR1 &= ~(1U << 5);

    // Enable full duplex
    SPI1->CR1 &= ~(1U << 10);

    // Set MSB first
    SPI1->CR1 &= ~(1U << 7);

    // Set mode to MASTER
    SPI1->CR1 |= (1U << 2);

    // Set 8-bit data mode
    SPI1->CR1 &= ~(1U << 11);

    // Select software slave management by setting SSM = 1 and SSI = 1
    SPI1->CR1 |= (1U << 8);
    SPI1->CR1 |= (1U << 9);

    // Enable SPI module
    SPI1->CR1 |= (1U << 6);
}

void spi1_transmit(uint8_t *data, uint32_t size) {
    uint32_t i = 0;
    while (i < size) {
        // Wait until TXE is set
        while (!(SPI1->SR & SPI_SR_TXE)) {}

        // Write the data to the data register
        SPI1->DR = data[i];
        i++;
    }
    // Wait until TXE is set
    while (!(SPI1->SR & SPI_SR_TXE)) {}

    // Wait for BUSY flag to reset
    while (SPI1->SR & SPI_SR_BSY) {}

    // Clear OVR flag
    (void)SPI1->DR;
    (void)SPI1->SR;
}

void spi1_receive(uint8_t *data, uint32_t size) {
    while (size) {
        // Send dummy data
        SPI1->DR = 0;

        // Wait for RXNE flag to be set
        while (!(SPI1->SR & SPI_SR_RXNE)) {}

        // Read data from data register
        *data++ = (uint8_t)SPI1->DR;
        size--;
    }
}
