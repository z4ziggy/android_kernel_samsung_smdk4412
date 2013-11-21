/*
 * gpu_control.c -- a clock control interface for the sgs2/3
 *
 *  Copyright (C) 2011 Michael Wodkins
 *  twitter - @xdanetarchy
 *  XDA-developers - netarchy
 *  modified by gokhanmoral
 *
 *  Modified by Andrei F. for Galaxy S3 / Perseus kernel (June 2012)
 *
 *  Modified by DerTeufel to make it work with malir3p2 (November 2013)
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of the GNU General Public License as published by the
 *  Free Software Foundation;
 *
 */

#include <linux/platform_device.h>
#include <linux/miscdevice.h>

#include "gpu_control.h"

#define MIN_VOLTAGE_GPU  800000
#define MAX_VOLTAGE_GPU 1200000

#define GPU_MAX_CLOCK 800
#define GPU_MIN_CLOCK 10
#if defined(CONFIG_CPU_EXYNOS4212) || defined(CONFIG_CPU_EXYNOS4412)
#define MALI_STEPS 5
#else
#define MALI_STEPS 3
#endif

typedef struct mali_dvfs_tableTag{
    unsigned int clock;
    unsigned int freq;
    unsigned int vol;
    unsigned int downthreshold;
    unsigned int upthreshold;
}mali_dvfs_table;

extern mali_dvfs_table mali_dvfs[MALI_STEPS];
unsigned int gv[MALI_STEPS];


static ssize_t gpu_voltage_show(struct device *dev, struct device_attribute *attr, char *buf) {
	int i, j = 0;
   	for (i = 0; i < MALI_STEPS; i++)
	{
	    j += sprintf(&buf[j], "Step%d: %d\n", i, mali_dvfs[i].vol);
	}
   return j;
}

static ssize_t gpu_voltage_store(struct device *dev, struct device_attribute *attr, const char *buf,
                                                                        size_t count) {
        unsigned int ret = -EINVAL;
	int i, j = 0;
   	for (i = 0; i < MALI_STEPS; i++)
	{
            ret += sscanf(&buf[j], "%d", &gv[i]);
	}
        if(ret!=MALI_STEPS) return -EINVAL;

        /* safety floor and ceiling - netarchy */
        for( i = 0; i < MALI_STEPS; i++ ) {
                if (gv[i] < MIN_VOLTAGE_GPU) {
                    gv[i] = MIN_VOLTAGE_GPU;
                }
                else if (gv[i] > MAX_VOLTAGE_GPU) {
                    gv[i] = MAX_VOLTAGE_GPU;
                }
                if(ret==MALI_STEPS)
                    mali_dvfs[i].vol=gv[i];
        }
        return count;
}

static ssize_t gpu_clock_show(struct device *dev, struct device_attribute *attr, char *buf) {
	int i, j = 0;
   	for (i = 0; i < MALI_STEPS; i++)
	{
	    j += sprintf(&buf[j], "Step%d: %d\n", i, mali_dvfs[i].clock);
	}

   	for (i = 0; i < MALI_STEPS - 1; i++)
	{
	    j += sprintf(&buf[j], "Threshold%d-%d/up-down: %d%% %d%%\n", i, i+1, mali_dvfs[i].upthreshold, mali_dvfs[i+1].downthreshold);
	}
   return j;
}

unsigned int g[(MALI_STEPS-1)*2];

static ssize_t gpu_clock_store(struct device *dev, struct device_attribute *attr,
                               const char *buf, size_t count) {
        unsigned int ret = -EINVAL;
	int i, j = 0;
   	for (i = 0; i < (MALI_STEPS-1)*2; i++)
	{
            ret += sscanf(&buf[j], "%d%%", &g[i]);
	}
	if (ret == (MALI_STEPS-1)*2 ) i=1;

        if(i) {
	    for (i = 0; i < (MALI_STEPS-1)*2; i++)
	    {
		if (g[i] < 0 || g[i] > 100) return -EINVAL;
		
		if (i%2 == 0)
                mali_dvfs[i/2].upthreshold = (int)(g[i]);
		else
                mali_dvfs[(i+1)/2].downthreshold = (int)(g[i]);
	    }	
        } else {

	    ret = -EINVAL;
   	    for (i = 0; i < MALI_STEPS; i++)
	    {
            	ret += sscanf(&buf[j], "%d%%", &g[i]);
	    }
	    if (ret != MALI_STEPS )
                        return -EINVAL;

                /* safety floor and ceiling - netarchy */
                for( i = 0; i < MALI_STEPS; i++ ) {
                        if (g[i] < GPU_MIN_CLOCK) {
                                g[i] = GPU_MIN_CLOCK;
                        }
                        else if (g[i] > GPU_MAX_CLOCK) {
                                g[i] = GPU_MAX_CLOCK;
                        }

                        if(ret==MALI_STEPS)
                                mali_dvfs[i].clock=g[i];
                }
        }

        return count;
}


// Yank555.lu : Add available frequencies sysfs entry (similar to what we have for CPU)
static ssize_t available_frequencies_show(struct device *dev, struct device_attribute *attr, char *buf) {

        int i, len = 0;

        if(buf) {

                for (i = 0; gpu_freq_table[i] != GPU_FREQ_END_OF_TABLE; i++)
                        len += sprintf(buf + len, "%d ", gpu_freq_table[i]);

                len += sprintf(buf + len, "\n"); // Don't forget to go to newline
                
        }

        return len;

}


static DEVICE_ATTR(gpu_voltage_control, S_IRUGO | S_IWUGO, gpu_voltage_show, gpu_voltage_store);
static DEVICE_ATTR(gpu_clock_control, S_IRUGO | S_IWUGO, gpu_clock_show, gpu_clock_store);
static DEVICE_ATTR(available_frequencies, S_IRUGO, available_frequencies_show, NULL);



// Yank555.lu : Add a new sysfs interface, 1 tunable per step, easier to use from an app

// GPU frequency steps

#define expose_gpu_freq(step)                                                                                \
static ssize_t show_gpu_freq_##step                                                                        \
(struct device *dev, struct device_attribute *attr, char *buf) {                                        \
                                                                                                        \
        return sprintf(buf, "%d\n", mali_dvfs[step].clock);                                                \
                                                                                                        \
}                                                                                                        \
                                                                                                        \
static ssize_t store_gpu_freq_##step                                                                        \
(struct device *dev, struct device_attribute *attr,                                                        \
                               const char *buf, size_t count) {                                                \
                                                                                                        \
        unsigned int input;                                                                                \
        int ret;                                                                                        \
        int i=0;                                                                                        \
                                                                                                        \
        ret = sscanf(buf, "%u", &input);                                                                \
        if (ret != 1)                                                                                        \
                return -EINVAL;                                                                                \
                                                                                                        \
        for (i = 0; (gpu_freq_table[i] != GPU_FREQ_END_OF_TABLE); i++)                                        \
                if (gpu_freq_table[i] == input) {                                                        \
                        mali_dvfs[step].clock = input;                                                        \
                        return count;                                                                        \
                }                                                                                        \
                                                                                                        \
        return -EINVAL;                                                                                        \
}                                                                                                        \
                                                                                                        \
static DEVICE_ATTR(gpu_freq_##step                                                                        \
                 , S_IRUGO | S_IWUGO                                                                        \
                 , show_gpu_freq_##step                                                                        \
                 , store_gpu_freq_##step                                                                \
);

expose_gpu_freq(0);
expose_gpu_freq(1);
expose_gpu_freq(2);
expose_gpu_freq(3);
expose_gpu_freq(4);

// New GPU up/down thresholds interface

#define expose_gpu_upthreshold(step)                                                                        \
static ssize_t show_gpu_upthreshold_##step                                                                \
(struct device *dev, struct device_attribute *attr, char *buf) {                                        \
                                                                                                        \
        return sprintf(buf, "%d\n", mali_dvfs[step].upthreshold);                        \
                                                                                                     \
}                                                                                                        \
                                                                                                        \
static ssize_t store_gpu_upthreshold_##step                                                                \
(struct device *dev, struct device_attribute *attr,                                                        \
                               const char *buf, size_t count) {                                                \
                                                                                                        \
        int input;                                                                                        \
                                                                                                        \
        if (sscanf(buf, "%d", &input) != 1)                                                                \
                return -EINVAL;                                                                                \
                                                                                                        \
        if (input < 0 || input > 100)                                                                        \
                return -EINVAL;                                                                                \
                                                                                                        \
        mali_dvfs[step].upthreshold = (int)(input);                                \
        return count;                                                                                        \
}                                                                                                        \
                                                                                                        \
static DEVICE_ATTR(gpu_upthreshold_##step                                                                \
                 , S_IRUGO | S_IWUGO                                                                        \
                 , show_gpu_upthreshold_##step                                                                \
                 , store_gpu_upthreshold_##step                                                                \
);

#define expose_gpu_downthreshold(step)                                                                        \
static ssize_t show_gpu_downthreshold_##step                                                                \
(struct device *dev, struct device_attribute *attr, char *buf) {                                        \
                                                                                                        \
        return sprintf(buf, "%d\n", mali_dvfs[step].downthreshold);                        \
                                                                                                        \
}                                                                                                        \
                                                                                                        \
static ssize_t store_gpu_downthreshold_##step                                                                \
(struct device *dev, struct device_attribute *attr,                                                        \
                               const char *buf, size_t count) {                                                \
                                                                                                        \
        int input;                                                                                        \
                                                                                                        \
        if (sscanf(buf, "%d", &input) != 1)                                                                \
                return -EINVAL;                                                                                \
                                                                                                        \
        if (input < 0 || input > 100)                                                                        \
                return -EINVAL;                                                                                \
                                                                                                        \
        mali_dvfs[step].downthreshold = (int)(input);                                \
        return count;                                                                                        \
}                                                                                                        \
                                                                                                        \
static DEVICE_ATTR(gpu_downthreshold_##step                                                                \
                 , S_IRUGO | S_IWUGO                                                                        \
                 , show_gpu_downthreshold_##step                                                        \
                 , store_gpu_downthreshold_##step                                                        \
);

expose_gpu_upthreshold(0);
expose_gpu_upthreshold(1);
expose_gpu_upthreshold(2);
expose_gpu_upthreshold(3);
expose_gpu_downthreshold(1);
expose_gpu_downthreshold(2);
expose_gpu_downthreshold(3);
expose_gpu_downthreshold(4);

static struct attribute *gpu_control_attributes[] = {
        &dev_attr_gpu_voltage_control.attr,
        &dev_attr_gpu_clock_control.attr,
        &dev_attr_available_frequencies.attr,
        // Yank555.lu : new GPU frequency steps interface
        &dev_attr_gpu_freq_0.attr,
        &dev_attr_gpu_freq_1.attr,
        &dev_attr_gpu_freq_2.attr,
        &dev_attr_gpu_freq_3.attr,
        &dev_attr_gpu_freq_4.attr,
        // Yank555.lu : new GPU up/down thresholds interface
        &dev_attr_gpu_upthreshold_0.attr,
        &dev_attr_gpu_upthreshold_1.attr,
        &dev_attr_gpu_upthreshold_2.attr,
        &dev_attr_gpu_upthreshold_3.attr,
        &dev_attr_gpu_downthreshold_1.attr,
        &dev_attr_gpu_downthreshold_2.attr,
        &dev_attr_gpu_downthreshold_3.attr,
        &dev_attr_gpu_downthreshold_4.attr,
        NULL
};


static struct attribute_group gpu_control_group = {
        .attrs = gpu_control_attributes,
};

static struct miscdevice gpu_control_device = {
        .minor = MISC_DYNAMIC_MINOR,
        .name = "gpu_control",
};



void gpu_control_start()
{
        printk("Initializing gpu control interface\n");

        misc_register(&gpu_control_device);
        if (sysfs_create_group(&gpu_control_device.this_device->kobj,
                                &gpu_control_group) < 0) {
                printk("%s sysfs_create_group failed\n", __FUNCTION__);
                pr_err("Unable to create group for %s\n", gpu_control_device.name);
        }
}
