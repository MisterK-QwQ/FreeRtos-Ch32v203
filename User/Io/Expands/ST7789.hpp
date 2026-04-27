#pragma once
#include "../GpioManager.hpp"
#include "../Gpio.hpp"
#include "Expand.hpp"
#include "../../Utils/DisplayGfx.hpp"

#define ST7789_CS_PIN    GPIO_Pin_4
#define ST7789_CS_PORT   GPIOA
#define ST7789_DC_PIN    GPIO_Pin_1
#define ST7789_DC_PORT   GPIOA
#define ST7789_RES_PIN   GPIO_Pin_0
#define ST7789_RES_PORT  GPIOA
#define ST7789_SCK_PIN   GPIO_Pin_5
#define ST7789_SCK_PORT  GPIOA
#define ST7789_MOSI_PIN  GPIO_Pin_7
#define ST7789_MOSI_PORT GPIOA

class ST7789 : public Gpio, public Display::DirtySurface<ST7789> {
  public:
    static constexpr uint16_t PanelWidth = 135;
    static constexpr uint16_t PanelHeight = 240;
    static constexpr uint16_t XOffset = 52;
    static constexpr uint16_t YOffset = 40;

    SPIExpand Data;

    ST7789() : Gpio() {
        RegisterFunc(&oninit);
    }

    static void oninit(ST7789* self) {
        self->InitBus();
    }

    static void Task(ST7789* self) {
        self->HardwareReset();
        self->InitPanel();
        self->Clear(Display::Color565::Black);
        static int16_t box_x = 10;
        while (true) {
            int16_t old_x = box_x;
            box_x += 5;
            if (box_x > 90) box_x = 10;

            self->InvalidateRect(old_x, 40, 20, 20);
            self->InvalidateRect(box_x, 40, 20, 20);

            self->FlushDirty([](ST7789& d, const Display::Rect& r) {
                d.FillRect(r.x, r.y, r.w, r.h, Display::Color565::Black);
                d.DrawRect(box_x, 40, 20, 20, Display::Color565::Yellow);
            });
            
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }

    uint16_t Width() const { return PanelWidth; }
    uint16_t Height() const { return PanelHeight; }

    void DrawPixel(int16_t x, int16_t y, uint16_t color) {
        if (!this->Contains(x, y)) return;
        SetAddressWindow(static_cast<uint16_t>(x), static_cast<uint16_t>(y), 1, 1);
        WriteColor(color, 1);
    }

    void FillRectFast(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t color) {
        if (!ClipRect(x, y, w, h)) return;
        SetAddressWindow(static_cast<uint16_t>(x), static_cast<uint16_t>(y), w, h);
        WriteColor(color, static_cast<uint32_t>(w) * h);
    }

    void DrawImage565(int16_t x, int16_t y, uint16_t w, uint16_t h, const uint16_t* data) {
        if (data == nullptr || !this->Contains(x, y) || x + w > Width() || y + h > Height()) return;
        SetAddressWindow(static_cast<uint16_t>(x), static_cast<uint16_t>(y), w, h);
        Select();
        DataMode();
        for (uint32_t i = 0; i < static_cast<uint32_t>(w) * h; ++i) {
            Write16(data[i]);
        }
        Deselect();
    }

    void InitPanel() {
        static const uint8_t porch_data[] = {0x0C, 0x0C, 0x00, 0x33, 0x33};
        static const uint8_t power_data[] = {0xA4, 0xA1};
        static const uint8_t gamma_p[] = {0xD0, 0x04, 0x0D, 0x11, 0x13, 0x2B, 0x3F, 0x54, 0x4C, 0x18, 0x0D, 0x0B, 0x1F, 0x23};
        static const uint8_t gamma_n[] = {0xD0, 0x04, 0x0C, 0x11, 0x13, 0x2C, 0x3F, 0x44, 0x51, 0x2F, 0x1F, 0x1F, 0x20, 0x23};

        WriteCommand(0x11);
        vTaskDelay(pdMS_TO_TICKS(120));

        WriteRegister(0x36, 0x00);
        WriteRegister(0x3A, 0x55);

        WriteCommand(0xB2);
        WriteDataBuffer(porch_data, sizeof(porch_data));

        WriteRegister(0xB7, 0x35);
        WriteRegister(0xBB, 0x19);
        WriteRegister(0xC0, 0x2C);
        WriteRegister(0xC2, 0x01);
        WriteRegister(0xC3, 0x12);
        WriteRegister(0xC4, 0x20);
        WriteRegister(0xC6, 0x0F);

        WriteCommand(0xD0);
        WriteDataBuffer(power_data, sizeof(power_data));

        WriteCommand(0xE0);
        WriteDataBuffer(gamma_p, sizeof(gamma_p));

        WriteCommand(0xE1);
        WriteDataBuffer(gamma_n, sizeof(gamma_n));

        WriteCommand(0x21);
        WriteCommand(0x29);
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    void HardwareReset() {
        GPIO_SetBits(ST7789_RES_PORT, ST7789_RES_PIN);
        vTaskDelay(pdMS_TO_TICKS(10));
        GPIO_ResetBits(ST7789_RES_PORT, ST7789_RES_PIN);
        vTaskDelay(pdMS_TO_TICKS(10));
        GPIO_SetBits(ST7789_RES_PORT, ST7789_RES_PIN);
        vTaskDelay(pdMS_TO_TICKS(100));
    }

  private:
    bool ClipRect(int16_t& x, int16_t& y, uint16_t& w, uint16_t& h) {
        return this->ClipRectToSurface(x, y, w, h);
    }

    void InitBus() {
        GPIO_InitTypeDef gpio_init = {};
        SPI_InitTypeDef spi_init = {};

        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);

        gpio_init.GPIO_Pin = ST7789_CS_PIN | ST7789_DC_PIN | ST7789_RES_PIN;
        gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
        gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOA, &gpio_init);

        gpio_init.GPIO_Pin = ST7789_SCK_PIN | ST7789_MOSI_PIN;
        gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
        gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOA, &gpio_init);

        spi_init.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
        spi_init.SPI_Mode = SPI_Mode_Master;
        spi_init.SPI_DataSize = SPI_DataSize_8b;
        spi_init.SPI_CPOL = SPI_CPOL_Low;
        spi_init.SPI_CPHA = SPI_CPHA_1Edge;
        spi_init.SPI_NSS = SPI_NSS_Soft;
        spi_init.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
        spi_init.SPI_FirstBit = SPI_FirstBit_MSB;
        spi_init.SPI_CRCPolynomial = 7;
        SPI_Init(SPI1, &spi_init);
        SPI_Cmd(SPI1, ENABLE);

        Data.Modx = SPI1;
        Data.SPI_InitStructure = spi_init;

        Deselect();
        DataMode();
        GPIO_SetBits(ST7789_RES_PORT, ST7789_RES_PIN);
    }

    void Select() { GPIO_ResetBits(ST7789_CS_PORT, ST7789_CS_PIN); }
    void Deselect() { GPIO_SetBits(ST7789_CS_PORT, ST7789_CS_PIN); }
    void CommandMode() { GPIO_ResetBits(ST7789_DC_PORT, ST7789_DC_PIN); }
    void DataMode() { GPIO_SetBits(ST7789_DC_PORT, ST7789_DC_PIN); }

    void Write8(uint8_t value) {
        while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) {}
        SPI_I2S_SendData(SPI1, value);
        while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) {}
        SPI_I2S_ReceiveData(SPI1);
    }

    void Write16(uint16_t value) {
        Write8(static_cast<uint8_t>(value >> 8));
        Write8(static_cast<uint8_t>(value & 0xFF));
    }

    void WriteCommand(uint8_t cmd) {
        Select();
        CommandMode();
        Write8(cmd);
        Deselect();
    }

    void WriteData8(uint8_t data) {
        Select();
        DataMode();
        Write8(data);
        Deselect();
    }

    void WriteRegister(uint8_t reg, uint8_t value) {
        WriteCommand(reg);
        WriteData8(value);
    }

    void WriteDataBuffer(const uint8_t* data, uint16_t len) {
        if (data == nullptr || len == 0) return;
        Select();
        DataMode();
        for (uint16_t i = 0; i < len; ++i) {
            Write8(data[i]);
        }
        Deselect();
    }

    void SetAddressWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
        const uint16_t x_start = x + XOffset;
        const uint16_t x_end = static_cast<uint16_t>(x_start + w - 1);
        const uint16_t y_start = y + YOffset;
        const uint16_t y_end = static_cast<uint16_t>(y_start + h - 1);

        WriteCommand(0x2A);
        Select();
        DataMode();
        Write16(x_start);
        Write16(x_end);
        Deselect();

        WriteCommand(0x2B);
        Select();
        DataMode();
        Write16(y_start);
        Write16(y_end);
        Deselect();

        WriteCommand(0x2C);
    }

    void WriteColor(uint16_t color, uint32_t count) {
        Select();
        DataMode();
        for (uint32_t i = 0; i < count; ++i) {
            Write16(color);
        }
        Deselect();
    }
};
