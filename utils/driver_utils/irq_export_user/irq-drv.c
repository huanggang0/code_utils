#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/types.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/of.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/pm.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/gfp.h>
#include <linux/mm.h>
#include <linux/dma-buf.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/dmaengine.h>
#include <linux/completion.h>
#include <linux/wait.h>
#include <linux/init.h>

#include <linux/sched.h>
#include <linux/pagemap.h>
#include <linux/errno.h>	/* error codes */
#include <linux/clk.h>
#include <linux/interrupt.h>
#include <linux/vmalloc.h>

#include <linux/moduleparam.h>
#include <linux/miscdevice.h>
#include <linux/ioport.h>
#include <linux/notifier.h>
#include <linux/init.h>
#include <linux/pci.h>

#include <linux/time.h>
#include <linux/timer.h>
#include <linux/poll.h>

#define DEVICE_NAME "irq_drv"
static char 			g_devname[16];
static int 				g_major;
static int             	g_minor;
static struct class*	g_cls;
static void __iomem*	g_virt_base;	
static int	            g_irq;
static struct device*	g_dev;
static volatile int 	g_evt_intr = 0;       
static volatile int 	g_irq_is_open = 0;
static struct fasync_struct *g_irq_async;
static DECLARE_WAIT_QUEUE_HEAD(g_intr_waitq);

/******************************user implement start******************************/
#define XILINX_FRMBUF_ISR_OFFSET		(0x0c)
#define XILINX_FRMBUF_ISR_AP_DONE_IRQ	BIT(0)
#define XILINX_FRMBUF_ISR_AP_READY_IRQ	BIT(1)

#define XILINX_FRMBUF_ISR_ALL_IRQ_MASK	\
    (XILINX_FRMBUF_ISR_AP_DONE_IRQ | \
     XILINX_FRMBUF_ISR_AP_READY_IRQ)

#define DEV_PHY_ADDR	(0xa0060000)
#define DEV_PHY_LEN		(0x4c)

static int irq_drv_user_read(struct file *file, char __user *buf, size_t count, loff_t *ppos){
    /* implement by user */
	return 0;
}

static int irq_drv_user_clr_state(void){
    /* implement by user */
    uint32_t status = ioread32(g_virt_base + XILINX_FRMBUF_ISR_OFFSET);
    if (!(status & XILINX_FRMBUF_ISR_ALL_IRQ_MASK))
        return IRQ_NONE;

    iowrite32(status & XILINX_FRMBUF_ISR_ALL_IRQ_MASK,g_virt_base + XILINX_FRMBUF_ISR_OFFSET);
	return 0;
}
/******************************user implement end******************************/

static int irq_drv_open(struct inode *Inode, struct file *File){
    g_irq_is_open = 1;
    return 0;
}

static int irq_drv_release (struct inode *inode, struct file *file){
    g_irq_is_open = 0;
    return 0;
}

static ssize_t irq_drv_read(struct file *file, char __user *buf, size_t count, loff_t *ppos){
    if (buf == NULL)
        return -EINVAL;
    /* block read */
    wait_event_interruptible(g_intr_waitq, g_evt_intr);

    irq_drv_user_read(file,buf,count,ppos);

    g_evt_intr = 0;

    return 0;
}

static ssize_t irq_drv_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos){
    return 0;
}

static unsigned irq_drv_poll(struct file *file, poll_table *wait){
    unsigned int mask = 0;
    poll_wait(file, &g_intr_waitq, wait);

    /* select can read when interrupt */
    if (g_evt_intr)
        mask |= POLLIN | POLLRDNORM;

    return mask;
}

static int irq_drv_fasync (int fd, struct file *filp, int on){
    return fasync_helper(fd, filp, on, &g_irq_async);
}

static struct file_operations irq_fops = {	
    .owner  		= THIS_MODULE,
    .open 			= irq_drv_open,
    .read 			= irq_drv_read, 
    .write 			= irq_drv_write,
    .poll			= irq_drv_poll,
    .fasync		 	= irq_drv_fasync,
    .release		= irq_drv_release,
};

static irqreturn_t irq_interrupt(int irq, void *dev_id){
    if(g_irq_is_open){
        g_evt_intr = 1;
        irq_drv_user_clr_state();
        /* 
         * wake up process blocked in read API 
         * select API can read
         */
        wake_up_interruptible(&g_intr_waitq);

        /* send signal SIGIO to userspace */
        kill_fasync(&g_irq_async, SIGIO, POLL_IN);
    }
    return IRQ_HANDLED;
}

static int irq_probe(struct platform_device *pdev){
    int	err;
    struct device *tmp_dev;
    memset(g_devname,0,16);
    strcpy(g_devname, DEVICE_NAME);
    g_major = register_chrdev(0, g_devname, &irq_fops);

    g_cls = class_create(THIS_MODULE, g_devname);
    g_minor = 1;
    tmp_dev = device_create(g_cls, &pdev->dev, MKDEV(g_major, g_minor), NULL, g_devname);
    if (IS_ERR(tmp_dev)) {
        class_destroy(g_cls);
        unregister_chrdev(g_major, g_devname);
        return 0;
    }

    g_irq = platform_get_irq(pdev,0);
    if (g_irq <= 0)
        return -ENXIO;

    g_dev = &pdev->dev;

    g_virt_base = ioremap(DEV_PHY_ADDR,DEV_PHY_LEN);
    if(g_virt_base == NULL){
        printk(KERN_ALERT "ioremap 0X%08x failed\n", DEV_PHY_ADDR);
        return -ENOMEM;
    }

    err = request_threaded_irq(g_irq, NULL,
            irq_interrupt,
            IRQF_TRIGGER_RISING | IRQF_ONESHOT,
            g_devname, NULL);				   
    if (err) {
        printk(KERN_ALERT "irq_probe irq error=%d\n", err);
        goto fail;
    }else{
        printk(KERN_ERR"irq = %d\n", g_irq);
        printk(KERN_ERR"g_devname = %s\n", g_devname);
    }

    return 0;
fail:
    free_irq(g_irq, NULL);
    device_destroy(g_cls, MKDEV(g_major, g_minor));	
    class_destroy(g_cls);
    unregister_chrdev(g_major, g_devname);

    return -ENOMEM;

}

static int irq_remove(struct platform_device *pdev){
    device_destroy(g_cls, MKDEV(g_major, g_minor));	
    class_destroy(g_cls);
    unregister_chrdev(g_major, g_devname);

    free_irq(g_irq, NULL);
    printk("irq = %d\n", g_irq);
    return 0;
}

static int irq_suspend(struct device *dev){
    return 0;
}

static int irq_resume(struct device *dev){
    return 0;
}

static const struct dev_pm_ops irq_pm_ops = {
    .suspend = irq_suspend,
    .resume  = irq_resume,
};

static const struct of_device_id irq_of_match[] = {
    {.compatible = "ev4,irq" },
    { }
};
MODULE_DEVICE_TABLE(of, irq_of_match);

static struct platform_driver irq_driver = {
    .probe = irq_probe,
    .remove	= irq_remove,
    .driver = {
        .owner   		= THIS_MODULE,
        .name	 		= "irq@0",
        .pm    			= &irq_pm_ops,
        .of_match_table	= irq_of_match,		
    },
};

module_platform_driver(irq_driver);
MODULE_LICENSE("GPL v2");
