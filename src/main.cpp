#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <inttypes.h>

#define TIMER_MS   500

static const struct gpio_dt_spec LED1 = GPIO_DT_SPEC_GET(DT_NODELABEL(led0), gpios);
static const struct gpio_dt_spec LED2 = GPIO_DT_SPEC_GET(DT_NODELABEL(led1), gpios);
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(DT_NODELABEL(button0), gpios);

static struct gpio_callback button_cb_data;
static uint64_t start_time;

void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{

    bool pressed = gpio_pin_get_dt(&button);
    if (pressed)
    {
        // start measuring time
        start_time = k_uptime_get();
    }
    else
    {
        uint64_t held_time = k_uptime_get() - start_time;
        // released! start figuring out which button to toggle
        if (held_time < TIMER_MS)
        {
            // turn led1 on/off
            gpio_pin_toggle_dt(&LED1);
            printk("Toggle led1\n");
            printk("Button was held for %" PRIu64 " ms\n", held_time);
    
        }
        else
        {
            gpio_pin_toggle_dt(&LED2);
            printk("Toggle led2\n");
            printk("Button was held for %" PRIu64 " ms\n", held_time);
        }
    }
    
}

int main(void)
{
    int ret;
    // check if gpios are ready
    if (!gpio_is_ready_dt(&LED1)) 
    {
        return -1;
    }

    if (!gpio_is_ready_dt(&button))
    {
        return -1;
    }

    if (!gpio_is_ready_dt(&LED2)) 
    {
        return -1;
    }
    // configure gpios
    ret = gpio_pin_configure_dt(&LED1, GPIO_OUTPUT_HIGH);
    if (ret < 0) 
    {
        return -1;
    }

    ret = gpio_pin_configure_dt(&LED2, GPIO_OUTPUT_HIGH);
    if (ret < 0) 
    {
        return -1;
    }

    ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
    if (ret < 0) 
    {
        return -1;
    }
    // configure button state irs
    ret = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_BOTH);
    if (ret < 0) 
    {
        return -1;
    }

    gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin)); 	
    gpio_add_callback(button.port, &button_cb_data);

    while (1) {

        k_msleep(TIMER_MS);
    }
    return 0;
}
