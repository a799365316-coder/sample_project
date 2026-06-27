/*****************************************************************************
* | File      	:   DEV_Config.c
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2020-02-19
* | Info        :
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#include "DEV_Config.h"

static void GPIO_Config(void)
{
    // 初始化 GPIO 输出引脚
    gpio_config_t io_conf = {
        .pin_bit_mask = 0,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

#if D_9PIN
    io_conf.pin_bit_mask |= (1ULL << EPD_PWR_PIN);
#endif
    io_conf.pin_bit_mask |= (1ULL << EPD_RST_PIN) | (1ULL << EPD_DC_PIN)
                          | (1ULL << EPD_SCK_PIN) | (1ULL << EPD_MOSI_PIN)
                          | (1ULL << EPD_CS_PIN);
    gpio_config(&io_conf);

    // 初始化 GPIO 输入引脚
    io_conf.pin_bit_mask = (1ULL << EPD_BUSY_PIN);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);

    // 设置初始电平
#if D_9PIN
    gpio_set_level((gpio_num_t)EPD_PWR_PIN, 1);
#endif
    gpio_set_level((gpio_num_t)EPD_CS_PIN, 1);
    gpio_set_level((gpio_num_t)EPD_SCK_PIN, 0);
    gpio_set_level((gpio_num_t)EPD_RST_PIN, 1);
    gpio_set_level((gpio_num_t)EPD_DC_PIN, 0);
    gpio_set_level((gpio_num_t)EPD_MOSI_PIN, 0);
}

void GPIO_Mode(UWORD GPIO_Pin, UWORD Mode)
{
    if (Mode == 0) {
        gpio_set_direction((gpio_num_t)GPIO_Pin, GPIO_MODE_INPUT);
    } else {
        gpio_set_direction((gpio_num_t)GPIO_Pin, GPIO_MODE_OUTPUT);
    }
}

/******************************************************************************
function:	毫秒级延时
******************************************************************************/
void DEV_Delay_ms(uint32_t ms)
{
    if (ms == 0) return;
    // vTaskDelay 至少延时 1 个 tick（100Hz = 10ms），短延时也主动让出 CPU 避免看门狗超时
    vTaskDelay(pdMS_TO_TICKS(ms > 0 ? (ms < 10 ? 10 : ms) : 0));
}

/******************************************************************************
function:	Module Initialize
******************************************************************************/
UBYTE DEV_Module_Init(void)
{
    GPIO_Config();
    printf("GPIO init OK\r\n");
    return 0;
}

/******************************************************************************
function:	SPI read and write (软件位绑定)
******************************************************************************/
void DEV_SPI_WriteByte(UBYTE data)
{
    gpio_set_level((gpio_num_t)EPD_CS_PIN, 0);

    for (int i = 0; i < 8; i++) {
        if ((data & 0x80) == 0)
            gpio_set_level((gpio_num_t)EPD_MOSI_PIN, 0);
        else
            gpio_set_level((gpio_num_t)EPD_MOSI_PIN, 1);

        data <<= 1;
        gpio_set_level((gpio_num_t)EPD_SCK_PIN, 1);
        gpio_set_level((gpio_num_t)EPD_SCK_PIN, 0);
    }

    gpio_set_level((gpio_num_t)EPD_CS_PIN, 1);
}

UBYTE DEV_SPI_ReadByte()
{
    UBYTE j = 0xff;
    GPIO_Mode(EPD_MOSI_PIN, 0);
    gpio_set_level((gpio_num_t)EPD_CS_PIN, 0);

    for (int i = 0; i < 8; i++) {
        j = j << 1;
        if (gpio_get_level((gpio_num_t)EPD_MOSI_PIN))
            j = j | 0x01;
        else
            j = j & 0xfe;

        gpio_set_level((gpio_num_t)EPD_SCK_PIN, 1);
        gpio_set_level((gpio_num_t)EPD_SCK_PIN, 0);
    }

    gpio_set_level((gpio_num_t)EPD_CS_PIN, 1);
    GPIO_Mode(EPD_MOSI_PIN, 1);
    return j;
}

void DEV_SPI_Write_nByte(UBYTE *pData, UDOUBLE len)
{
    for (int i = 0; i < len; i++)
        DEV_SPI_WriteByte(pData[i]);
}

void DEV_Module_Exit(void)
{
#if D_9PIN
    gpio_set_level((gpio_num_t)EPD_PWR_PIN, 0);
#endif
    gpio_set_level((gpio_num_t)EPD_RST_PIN, 0);
}
