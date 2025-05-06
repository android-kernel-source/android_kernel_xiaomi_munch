// E404 Manager by Project 113 (kvsnr113)

#include <linux/e404_attributes.h>

struct e404_attributes e404_data = {
    .e404_kernelsu = 0,
    .e404_rom_type = 0,
    .e404_ir_type = 1
    .e404_effcpu = 0,
    .e404_dvq_input_boost = 1,
    .e404_panel_width = 70,
    .e404_panel_height = 155,
    .e404_oem_panel_width = 700,
    .e404_oem_panel_height = 1550
};

static struct kobject *e404_kobj;

int e404_early_kernelsu = 0;
int e404_early_effcpu = 0;
int e404_early_rom_type = 0;
int e404_early_ir_type = 1;

static int __init parse_e404_kernelsu(char *str)
{
    return kstrtoint(str, 10, &e404_early_kernelsu);
}
early_param("e404_kernelsu", parse_e404_kernelsu);

static int __init parse_e404_effcpu(char *str)
{
    return kstrtoint(str, 10, &e404_early_effcpu);
}
early_param("e404_effcpu", parse_e404_effcpu);

static int __init parse_e404_rom_type(char *str)
{
    return kstrtoint(str, 10, &e404_early_rom_type);
}
early_param("e404_rom_type", parse_e404_rom_type);

static int __init parse_e404_ir_type(char *str)
{
    return kstrtoint(str, 10, &e404_ir_rom_type);
}
early_param("e404_ir_type", parse_e404_ir_type);

static void e404_parse_attributes(void) {
    e404_data.e404_kernelsu = e404_early_kernelsu;
    e404_data.e404_effcpu = e404_early_effcpu;
    e404_data.e404_rom_type = e404_early_rom_type;
    e404_data.e404_ir_type = e404_early_ir_type;

    pr_alert("E404 Attr: KernelSU=%d, RomType=%d, IR=%d, EFFCPU=%d, Panel_Width=%d, Panel_Height=%d, OEM_Panel_Width=%d, OEM_Panel_Height=%d, DIBoost=%d\n",
        e404_data.e404_kernelsu,
        e404_data.e404_rom_type,
        e404_data.e404_ir_type,
        e404_data.e404_effcpu,
        e404_data.e404_panel_width,
        e404_data.e404_panel_height,
        e404_data.e404_oem_panel_width,
        e404_data.e404_oem_panel_height,
        e404_data.e404_dvq_input_boost);
}

#define E404_ATTR_RO(name) \
static ssize_t name##_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) { \
    return sprintf(buf, "%d\n", e404_data.name); \
} \
static struct kobj_attribute name##_attr = __ATTR(name, 0444, name##_show, NULL);

#define E404_ATTR_RW(name) \
static ssize_t name##_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) { \
    return sprintf(buf, "%d\n", e404_data.name); \
} \
static ssize_t name##_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) { \
    int ret, val, old_val; \
    ret = kstrtoint(buf, 10, &val); \
    if (ret) return ret; \
    old_val = e404_data.name; \
    e404_data.name = val; \
    pr_alert("E404: %s changed from %d to %d\n", #name, old_val, val); \
    sysfs_notify(e404_kobj, NULL, #name); \
    return count; \
} \
static struct kobj_attribute name##_attr = __ATTR(name, 0664, name##_show, name##_store);

E404_ATTR_RO(e404_rom_type);
E404_ATTR_RO(e404_kernelsu);
E404_ATTR_RO(e404_effcpu);
E404_ATTR_RO(e404_panel_width);
E404_ATTR_RO(e404_panel_height);
E404_ATTR_RO(e404_oem_panel_width);
E404_ATTR_RO(e404_oem_panel_height);
E404_ATTR_RO(e404_ir_type);

E404_ATTR_RW(e404_dvq_input_boost);

static struct attribute *e404_attrs[] = {
    &e404_kernelsu_attr.attr,
    &e404_rom_type_attr.attr,
    &e404_effcpu_attr.attr,
    &e404_ir_type_attr.attr,
    &e404_panel_width_attr.attr,
    &e404_panel_height_attr.attr,
    &e404_oem_panel_width_attr.attr,
    &e404_oem_panel_height_attr.attr,
    &e404_dvq_input_boost_attr.attr,
    NULL,
};

static struct attribute_group e404_attr_group = {
    .attrs = e404_attrs,
};

static int __init e404_init(void) {
    int ret;

    e404_parse_attributes();

    e404_kobj = kobject_create_and_add("e404", kernel_kobj);
    if (!e404_kobj)
        return -ENOMEM;

    ret = sysfs_create_group(e404_kobj, &e404_attr_group);
    if (ret)
        kobject_put(e404_kobj);

    pr_alert("E404: Kernel Module & Sysfs Initialized\n");

    return ret;
}

static void __exit e404_exit(void) {
    kobject_put(e404_kobj);
    pr_alert("E404: Kernel Module & Sysfs Removed\n");
}

core_initcall(e404_init);
module_exit(e404_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kvsnr113");
MODULE_DESCRIPTION("E404 manager via early_param and sysfs");
