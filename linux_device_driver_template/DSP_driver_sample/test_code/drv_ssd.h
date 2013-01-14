typedef struct
{
    union
    {
        UINT32 _INF_DEV_STA_REG;//SSD register address = 0x00004
        struct
        {
            UINT32 STDRES    :5;
            UINT32 GST       :1;
            UINT32 GDU       :1;
            UINT32 RESERVED0 :1;
            UINT32 ADU       :1;
            UINT32 AST       :1;
            UINT32 AAMUT     :1;
            UINT32 BPILOT    :1;
            UINT32 SAPDET    :1;
            UINT32 BAMUT     :1;
            UINT32 SAMUT     :1;
            UINT32 VDSP_C    :1;
            UINT32 NICST_C   :1;
            UINT32 NICDU_C   :1;
            UINT32 NAMUT     :1;
            UINT32 RSSF      :1;
            UINT32 INITSTAT  :1;
            UINT32 RESERVED1 :3;
            UINT32 RESERVED2 :8;
        } INF_DEV_STA_REG;
    };

    union
    {
        UINT32 _INF_NIC_STA_REG;//SSD register address = 0x00008
        struct
        {
            UINT32 ERR_OUT   :8;
            UINT32 CFC       :1;
            UINT32 CO_LOCKED :1;
            UINT32 NACB      :4;
            UINT32 VDSP      :1;
            UINT32 NICST     :1;
            UINT32 NICDU     :1;
            UINT32 RESERVED0 :7;
            UINT32 RESERVED1 :8;
        } INF_NIC_STA_REG;
    };

    union
    {
        UINT32 _INF_NIC_ADD_REG;//SSD register address = 0x0000c
        struct
        {
            UINT32 ADW       :11;
            UINT32 RESERVED0 :6;
            UINT32 RESERVED1 :7;
            UINT32 RESERVED2 :8;
        } INF_NIC_ADD_REG;
    };

    union
    {
        UINT32 _INF_LEV_MON_REG;//SSD register address = 0x00018
        struct
        {
            UINT32 MONLEVEL  :24;
            UINT32 RESERVED0 :8;
        } INF_LEV_MON_REG;
    };

    union
    {
        UINT32 _INF_MPX_LEVEL_REG;//SSD register address = 0x0001c
        struct
        {
            UINT32 HW_AST    :1;
            UINT32 HW_ADU    :1;
            UINT32 FI_AST    :1;
            UINT32 FI_ADU    :1;
            UINT32 RESERVED0 :2;
            UINT32 MPXPLEV   :18;
            UINT32 RESERVED1 :8;
        } INF_MPX_LEVEL_REG;
    };

    union
    {
        UINT32 _INF_NOISELEVEL_REG;//SSD register address = 0x00028
        struct
        {
            UINT32 NDETCH_STAT       :1;
            UINT32 NDETPB_STAT       :1;
            UINT32 NOISELEVEL        :22;
            UINT32 RESERVED0         :8;
        } INF_NOISELEVEL_REG;
    };

    union
    {
        UINT32 _INF_OVMADAPT_REG;//SSD register address = 0x0002c
        struct
        {
            UINT32 FILTBW_STAT       :2;
            UINT32 RESERVED0         :10;
            UINT32 OVM_SCALE_STAT    :12;
            UINT32 RESERVED1         :8;
        } INF_OVMADAPT_REG;
    };
} DRV_AudFrontEndReg_t;


