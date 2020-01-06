#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/firmware.h>

#define FWFILE "EEPROM_MT7668.bin"
static const struct firmware *fw;

static void cm4release(struct device *dev)
{
	pr_info("releasing firmware device\n");
}

static struct device dev = {
	.release = cm4release
};

static int __init cm4init(void)
{
    int ret;
	dev_set_name(&dev, "my0");
	ret = device_register(&dev);
	pr_info("firmware_example: my device inserted\n");

	if (request_firmware(&fw, FWFILE, &dev)) {
		pr_err("requesting firmware failed\n");
		device_unregister(&dev);
		return -1;
	}
	pr_info("firmware size=%d\n", fw->size);
	pr_info("firmware contents=\n%x\n", fw->data[0]);
	return 0;
}

static void __exit cm4exit(void)
{
	release_firmware(fw);
	device_unregister(&dev);
	pr_info("release firwmare and unregistered device\n");
}

module_init(cm4init);
module_exit(cm4exit);

MODULE_LICENSE("GPL");
