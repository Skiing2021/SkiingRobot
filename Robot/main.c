#include <stdio.h>
#include "bsp.h"
#include <unistd.h>
#include "sysctl.h"
#include "board_config.h"

#include "uarths.h"
#include "kpu.h"
#include "w25qxx.h"

uint8_t *model_data = NULL;
#define PLL0_OUTPUT_FREQ 800000000UL
#define PLL1_OUTPUT_FREQ 400000000UL
#define MODEL_MEM_ADDRESS 0xE00000

#define CLASS_NUMBER 2

kpu_task_t task;

uint32_t kpu_model_get_size(uint8_t *buffer)
{
    kpu_kmodel_header_t *header = (kpu_kmodel_header_t *)buffer;
    printf("kmodel version: %d\r\n", header->version);
    printf("kmodel flags: %d\r\n", header->flags);
    printf("kmodel layers_length: %d\r\n", header->layers_length);
    printf("kmodel max_start_address: %d\r\n", header->max_start_address);

    // if (header->version != 1)
    //     return -1;
    uint32_t layers_length = header->layers_length;
    // return (header->layers_argument_start + (sizeof(kpu_layer_argument_t) * layers_length));
    return 0;
}

int main()
{
    /* Set CPU and dvp clk */
    sysctl_pll_set_freq(SYSCTL_PLL0, PLL0_OUTPUT_FREQ);
    sysctl_pll_set_freq(SYSCTL_PLL1, PLL1_OUTPUT_FREQ);
    sysctl_clock_enable(SYSCTL_CLOCK_AI);
    uarths_init();

    char* c = NULL;
    scanf("%s", c);
    printf("Starting skiing robot\r\n");

    w25qxx_init(3, 0);
    w25qxx_enable_quad_mode();

    uint8_t model_header[sizeof(kpu_model_header_t) + 1];

    w25qxx_read_data(MODEL_MEM_ADDRESS, model_header, sizeof(kpu_model_header_t), W25QXX_QUAD_FAST);

    uint32_t model_size = kpu_model_get_size(model_header);
    printf("model_size:%d\r\n", model_size);

    model_data = (uint8_t *)malloc(model_size * sizeof(uint8_t));
    if (model_data == NULL)
    {
        printf("malloc error\r\n");
        while (1)
            ;
    }
    printf("Successfully alloc memory for model");

    w25qxx_read_data(MODEL_MEM_ADDRESS, model_data, model_size, W25QXX_QUAD_FAST);
}