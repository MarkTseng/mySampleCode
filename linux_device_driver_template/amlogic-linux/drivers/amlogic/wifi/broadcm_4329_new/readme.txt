��ΰ�4329�ĳ�moduleģʽ��

1������1���ļ�����kconfig��bool�ĳ�tristate������make menuconfig��
device driver---->amlogic device driver--->AMLOGIC WIFI SUPPORT--->�������4329����ѡ��"M"
�������3������ѡ;

2,�޸�board-8726m-refb03.c

4��make clean��make uImage ;

5,make modules SUBDIRS=drivers/amlogic/wifi/broadcm_4329_new/

1)��drivers/amlogic/wifi/broadcm_4329/������dhd.ko
copy��\\10.28.8.14\nfsroot\sandy.luo\rootfs_out_03\target\product\b03ref\system\lib
��\\10.28.8.14\nfsroot\sandy.luo\rootfs\hardware\amlogic\wifi\dhd
2)��spike����sdio-g-cdc-full11n-reclaim-roml-wme-idsup.bin��nvram.txt
copy��\\10.28.8.14\nfsroot\sandy.luo\rootfs_out_03\target\product\b03ref\system\etc��
��\\10.28.8.14\nfsroot\sandy.luo\rootfs\hardware\amlogic\wifi\dhd

6,init.rc���������
    chmod 0777 /system/FCC_CE_TOOL_013

service FCC_CE_TOOL_013 /system/FCC_CE_TOOL_013

184�иĳ�BT:
    chmod 0777 /sys/class/rfkill/rfkill0/state
    write /sys/class/rfkill/rfkill0/state 1


ע��WIFI�������ϵģ�������˵��sdio-g-cdc-full11n-reclaim-roml-wme-idsup.bin��nvram.txt���ԣ�
�������ļ��������Լ������.


