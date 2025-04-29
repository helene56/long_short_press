#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

/* 1000 msec = 1 sec */
#define TIMER_MS   1000


// tasks
// 1. Turn on LED1 for a short press (<1s).
// 2. Turn on LED2 for a long press (≥1s).

// todo's
// 1. get button.                                                    [x]
// 2. configure button.                                              [x]
// 3. get led0.                                                      [x]
// 4. configure led0.                                                [x]
// 5. get led1.                                                      [x]
// 6. configure led1.                                                [x]
// 7. configure isr (interrupt service) callback function for button,[x] 
//    register when pressed.
// 8. get output (1 (pressed) or 0 (not pressed)) from button.       [x]
// 9. calculate time between pressed and not pressed.                [x]
// 10. turn led0 on if <1s pressed, turn led1 on if >1s pressed.     [x]



/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec LED1 = GPIO_DT_SPEC_GET(DT_NODELABEL(led0), gpios);
static const struct gpio_dt_spec LED2 = GPIO_DT_SPEC_GET(DT_NODELABEL(led1), gpios);
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(DT_NODELABEL(button0), gpios);

static struct gpio_callback button_cb_data;
static uint64_t time_stamp;
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
		// released! start figuring out which button to toggle
		if (k_uptime_get() - start_time <= TIMER_MS)
		{
			// turn led1 on/off
			gpio_pin_toggle_dt(&LED1);
		}
		else
		{
			gpio_pin_toggle_dt(&LED2);
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

	ret = gpio_pin_configure_dt(&LED1, GPIO_OUTPUT_LOW);
	if (ret < 0) 
	{
		return -1;
	}

	ret = gpio_pin_configure_dt(&LED2, GPIO_OUTPUT_LOW);
	if (ret < 0) 
	{
		return -1;
	}

	// configure button state
	ret = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_BOTH);
	if (ret < 0) 
	{
		return -1;
	}

	gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin)); 	
	gpio_add_callback(button.port, &button_cb_data);

	while (1) {
		// ret = gpio_pin_toggle_dt(&led);
		// if (ret < 0) {
		// 	return 0;
		// }

		// led_state = !led_state;
		// printf("LED state: %s\n", led_state ? "ON" : "OFF");
		k_msleep(TIMER_MS);
	}
	return 0;
}
