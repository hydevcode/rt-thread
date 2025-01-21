/**
 * Copyright (c) 2016 - 2020, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include <rtthread.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include "nrf.h"
#include "nrfx_usbd.h"
#include "nrfx_clock.h"
#include "nrfx_power.h"

void usb_dc_low_level_post_init(void)
{
    /* Enable interrupt globally */
    NRFX_IRQ_PRIORITY_SET(USBD_IRQn, NRFX_USBD_CONFIG_IRQ_PRIORITY);
    NRFX_IRQ_ENABLE(USBD_IRQn);
}

extern void cherry_usb_hal_nrf_power_event(uint32_t event);
static void power_event_handler(nrfx_power_usb_evt_t event)
{
    cherry_usb_hal_nrf_power_event((uint32_t)event);
}

void usb_dc_low_level_pre_init(void)
{
    uint32_t usb_reg;
    const nrfx_power_usbevt_config_t config = {.handler = power_event_handler};
    nrfx_power_usbevt_init(&config);
    nrfx_power_usbevt_enable();
    usb_reg = NRF_POWER->USBREGSTATUS;

    if (usb_reg & POWER_USBREGSTATUS_VBUSDETECT_Msk)
    {
        cherry_usb_hal_nrf_power_event(NRFX_POWER_USB_EVT_DETECTED);
    }

    if (usb_reg & POWER_USBREGSTATUS_OUTPUTRDY_Msk)
    {
        cherry_usb_hal_nrf_power_event(NRFX_POWER_USB_EVT_READY);
    }
}

void usb_low_clear_pending_irq(void)
{
    NVIC_ClearPendingIRQ(USBD_IRQn);
}

void usb_low_disable_irq(void)
{
    NVIC_DisableIRQ(USBD_IRQn);
}
int cherryusb_protocol_stack_init(void)
{
#ifdef RT_CHERRYUSB_DEVICE_TEMPLATE_CDC_ACM
    extern void cdc_acm_init(void);
    cdc_acm_init();
    rt_kprintf("cdc acm example started. \r\n");
    nrf_delay_ms(5000);
#elif defined RT_CHERRYUSB_DEVICE_TEMPLATE_MSC
    extern void msc_ram_init(void);
    msc_ram_init();
    rt_kprintf("msc ram example started. \r\n");
    nrf_delay_ms(5000);
#elif defined RT_CHERRYUSB_DEVICE_TEMPLATE_HID_KEYBOARD
    extern void hid_keyboard_init(uint8_t busid, uintptr_t reg_base);
    hid_keyboard_init(0,NULL);
    rt_kprintf("hid keyboard example started. \r\n");
    // nrf_delay_ms(5000);
#endif
}
INIT_APP_EXPORT(cherryusb_protocol_stack_init);