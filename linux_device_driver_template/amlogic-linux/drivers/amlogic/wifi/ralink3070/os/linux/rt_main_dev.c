/*
 *************************************************************************
 * Ralink Tech Inc.
 * 5F., No.36, Taiyuan St., Jhubei City,
 * Hsinchu County 302,
 * Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2007, Ralink Technology, Inc.
 *
 * This program is free software; you can redistribute it and/or modify  * 
 * it under the terms of the GNU General Public License as published by  * 
 * the Free Software Foundation; either version 2 of the License, or     * 
 * (at your option) any later version.                                   * 
 *                                                                       * 
 * This program is distributed in the hope that it will be useful,       * 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        * 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         * 
 * GNU General Public License for more details.                          * 
 *                                                                       * 
 * You should have received a copy of the GNU General Public License     * 
 * along with this program; if not, write to the                         * 
 * Free Software Foundation, Inc.,                                       * 
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             * 
 *                                                                       * 
 *************************************************************************

    Module Name:
    rt_main_dev.c

    Abstract:
    Create and register network interface.

    Revision History:
    Who         When            What
    --------    ----------      ----------------------------------------------
*/

#include "rt_config.h"


#ifdef CONFIG_APSTA_MIXED_SUPPORT
UINT32 CW_MAX_IN_BITS;
#endif // CONFIG_APSTA_MIXED_SUPPORT //

/*---------------------------------------------------------------------*/
/* Private Variables Used                                              */
/*---------------------------------------------------------------------*/

PSTRING mac = "";		   // default 00:00:00:00:00:00
PSTRING hostname = "";		   // default CMPC
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,12)
MODULE_PARM (mac, "s");
#else
module_param (mac, charp, 0);
#endif
MODULE_PARM_DESC (mac, "rt28xx: wireless mac addr");

#ifdef OS_ABL_SUPPORT
#ifdef RTMP_MAC_USB
MODULE_LICENSE("GPL");
#endif // RTMP_MAC_USB //

UCHAR ZERO_MAC_ADDR[MAC_ADDR_LEN]  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#endif // OS_ABL_SUPPORT //


/*---------------------------------------------------------------------*/
/* Prototypes of Functions Used                                        */
/*---------------------------------------------------------------------*/

// public function prototype
int rt28xx_close(IN struct net_device *net_dev);
int rt28xx_open(struct net_device *net_dev);

// private function prototype
static INT rt28xx_send_packets(IN struct sk_buff *skb_p, IN struct net_device *net_dev);




static struct net_device_stats *RT28xx_get_ether_stats(
    IN  struct net_device *net_dev);

/*
========================================================================
Routine Description:
    Close raxx interface.

Arguments:
	*net_dev			the raxx interface pointer

Return Value:
    0					Open OK
	otherwise			Open Fail

Note:
	1. if open fail, kernel will not call the close function.
	2. Free memory for
		(1) Mlme Memory Handler:		MlmeHalt()
		(2) TX & RX:					RTMPFreeTxRxRingMemory()
		(3) BA Reordering: 				ba_reordering_resource_release()
========================================================================
*/
int MainVirtualIF_close(IN struct net_device *net_dev)
{
    RTMP_ADAPTER *pAd = NULL;

	GET_PAD_FROM_NET_DEV(pAd, net_dev);	

	// Sanity check for pAd
	if (pAd == NULL)
		return 0; // close ok

	netif_carrier_off(pAd->net_dev);
	netif_stop_queue(pAd->net_dev);




#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		BOOLEAN 		Cancelled;
#ifdef QOS_DLS_SUPPORT
		// send DLS-TEAR_DOWN message, 
		if (pAd->CommonCfg.bDLSCapable)
		{
			UCHAR i;

			// tear down local dls table entry
			for (i=0; i<MAX_NUM_OF_INIT_DLS_ENTRY; i++)
			{
				if (pAd->StaCfg.DLSEntry[i].Valid && (pAd->StaCfg.DLSEntry[i].Status == DLS_FINISH))
				{
					RTMPSendDLSTearDownFrame(pAd, pAd->StaCfg.DLSEntry[i].MacAddr);
					pAd->StaCfg.DLSEntry[i].Status	= DLS_NONE;
					pAd->StaCfg.DLSEntry[i].Valid	= FALSE;
				}
			}

			// tear down peer dls table entry
			for (i=MAX_NUM_OF_INIT_DLS_ENTRY; i<MAX_NUM_OF_DLS_ENTRY; i++)
			{
				if (pAd->StaCfg.DLSEntry[i].Valid && (pAd->StaCfg.DLSEntry[i].Status == DLS_FINISH))
				{
					RTMPSendDLSTearDownFrame(pAd, pAd->StaCfg.DLSEntry[i].MacAddr);
					pAd->StaCfg.DLSEntry[i].Status = DLS_NONE;
					pAd->StaCfg.DLSEntry[i].Valid	= FALSE;
				}
			}
			RTMP_MLME_HANDLER(pAd);
		}
#endif // QOS_DLS_SUPPORT //

		if (INFRA_ON(pAd) &&
			(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST)))
		{
			MLME_DISASSOC_REQ_STRUCT	DisReq;
			MLME_QUEUE_ELEM *MsgElem = (MLME_QUEUE_ELEM *) kmalloc(sizeof(MLME_QUEUE_ELEM), MEM_ALLOC_FLAG);
    
			if (MsgElem)
			{
			COPY_MAC_ADDR(DisReq.Addr, pAd->CommonCfg.Bssid);
			DisReq.Reason =  REASON_DEAUTH_STA_LEAVING;

			MsgElem->Machine = ASSOC_STATE_MACHINE;
			MsgElem->MsgType = MT2_MLME_DISASSOC_REQ;
			MsgElem->MsgLen = sizeof(MLME_DISASSOC_REQ_STRUCT);
			NdisMoveMemory(MsgElem->Msg, &DisReq, sizeof(MLME_DISASSOC_REQ_STRUCT));

			// Prevent to connect AP again in STAMlmePeriodicExec
			pAd->MlmeAux.AutoReconnectSsidLen= 32;
			NdisZeroMemory(pAd->MlmeAux.AutoReconnectSsid, pAd->MlmeAux.AutoReconnectSsidLen);

			pAd->Mlme.CntlMachine.CurrState = CNTL_WAIT_OID_DISASSOC;
			MlmeDisassocReqAction(pAd, MsgElem);
			kfree(MsgElem);
			}
			
			RTMPusecDelay(1000);
		}

		RTMPCancelTimer(&pAd->StaCfg.StaQuickResponeForRateUpTimer, &Cancelled);
		RTMPCancelTimer(&pAd->StaCfg.WpaDisassocAndBlockAssocTimer, &Cancelled);

#ifdef WPA_SUPPLICANT_SUPPORT
#ifndef NATIVE_WPA_SUPPLICANT_SUPPORT
		// send wireless event to wpa_supplicant for infroming interface down.
		RtmpOSWrielessEventSend(pAd, IWEVCUSTOM, RT_INTERFACE_DOWN, NULL, NULL, 0);
#endif // NATIVE_WPA_SUPPLICANT_SUPPORT //

		if (pAd->StaCfg.pWpsProbeReqIe)
		{
			kfree(pAd->StaCfg.pWpsProbeReqIe);
			pAd->StaCfg.pWpsProbeReqIe = NULL;
			pAd->StaCfg.WpsProbeReqIeLen = 0;
		}

		if (pAd->StaCfg.pWpaAssocIe)
		{
			kfree(pAd->StaCfg.pWpaAssocIe);
			pAd->StaCfg.pWpaAssocIe = NULL;
			pAd->StaCfg.WpaAssocIeLen = 0;
		}
#endif // WPA_SUPPLICANT_SUPPORT //


	}
#endif // CONFIG_STA_SUPPORT //

	VIRTUAL_IF_DOWN(pAd);

	RT_MOD_DEC_USE_COUNT();

	return 0; // close ok
}

/*
========================================================================
Routine Description:
    Open raxx interface.

Arguments:
	*net_dev			the raxx interface pointer

Return Value:
    0					Open OK
	otherwise			Open Fail

Note:
	1. if open fail, kernel will not call the close function.
	2. Free memory for
		(1) Mlme Memory Handler:		MlmeHalt()
		(2) TX & RX:					RTMPFreeTxRxRingMemory()
		(3) BA Reordering: 				ba_reordering_resource_release()
========================================================================
*/
int MainVirtualIF_open(IN struct net_device *net_dev)
{
    RTMP_ADAPTER *pAd = NULL;

	GET_PAD_FROM_NET_DEV(pAd, net_dev);	

	// Sanity check for pAd
	if (pAd == NULL)
		return 0; // close ok

	if (VIRTUAL_IF_UP(pAd) != 0)
		return -1;

	// increase MODULE use count
	RT_MOD_INC_USE_COUNT();

	netif_start_queue(net_dev);
	netif_carrier_on(net_dev);
	netif_wake_queue(net_dev);

	return 0;
}

/*
========================================================================
Routine Description:
    Close raxx interface.

Arguments:
	*net_dev			the raxx interface pointer

Return Value:
    0					Open OK
	otherwise			Open Fail

Note:
	1. if open fail, kernel will not call the close function.
	2. Free memory for
		(1) Mlme Memory Handler:		MlmeHalt()
		(2) TX & RX:					RTMPFreeTxRxRingMemory()
		(3) BA Reordering: 				ba_reordering_resource_release()
========================================================================
*/
int rt28xx_close(IN PNET_DEV dev)
{
	struct net_device * net_dev = (struct net_device *)dev;
    RTMP_ADAPTER	*pAd = NULL;
	BOOLEAN 		Cancelled;
	UINT32			i = 0;
#ifdef RTMP_MAC_USB
	DECLARE_WAIT_QUEUE_HEAD(unlink_wakeup); 
	DECLARE_WAITQUEUE(wait, current);

	//RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_REMOVE_IN_PROGRESS);
#endif // RTMP_MAC_USB //

	GET_PAD_FROM_NET_DEV(pAd, net_dev);	

	DBGPRINT(RT_DEBUG_TRACE, ("===> rt28xx_close\n"));



	Cancelled = FALSE;
	// Sanity check for pAd
	if (pAd == NULL)
		return 0; // close ok



#ifdef WDS_SUPPORT
	WdsDown(pAd);
#endif // WDS_SUPPORT //

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
#ifdef PCIE_PS_SUPPORT
		RTMPPCIeLinkCtrlValueRestore(pAd, RESTORE_CLOSE);
#endif // PCIE_PS_SUPPORT //

		// If dirver doesn't wake up firmware here,
		// NICLoadFirmware will hang forever when interface is up again.
		if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_DOZE))
        {      
		    AsicForceWakeup(pAd, TRUE);
        }

#ifdef RTMP_MAC_USB
	RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_REMOVE_IN_PROGRESS);
#endif // RTMP_MAC_USB //

		MlmeRadioOff(pAd);
	}
#endif // CONFIG_STA_SUPPORT //

	RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS);

	for (i = 0 ; i < NUM_OF_TX_RING; i++)
	{
		while (pAd->DeQueueRunning[i] == TRUE)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("Waiting for TxQueue[%d] done..........\n", i));
			RTMPusecDelay(1000);
		}
	}
	
#ifdef RTMP_MAC_USB
	// ensure there are no more active urbs.
	add_wait_queue (&unlink_wakeup, &wait);
	pAd->wait = &unlink_wakeup;

	// maybe wait for deletions to finish.
	i = 0;
	//while((i < 25) && atomic_read(&pAd->PendingRx) > 0) 
	while(i < 25)
	{
		unsigned long IrqFlags;

		RTMP_IRQ_LOCK(&pAd->BulkInLock, IrqFlags);
		if (pAd->PendingRx == 0)
		{
			RTMP_IRQ_UNLOCK(&pAd->BulkInLock, IrqFlags);
			break;
		}
		RTMP_IRQ_UNLOCK(&pAd->BulkInLock, IrqFlags);
		
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,9)
		msleep(UNLINK_TIMEOUT_MS);	//Time in millisecond
#else
		RTMPusecDelay(UNLINK_TIMEOUT_MS*1000);	//Time in microsecond
#endif
		i++;
	}
	pAd->wait = NULL;
	remove_wait_queue (&unlink_wakeup, &wait); 
#endif // RTMP_MAC_USB //


	// Stop Mlme state machine
	MlmeHalt(pAd);
	
	// Close net tasklets
	RtmpNetTaskExit(pAd);


#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		MacTableReset(pAd);
	}
#endif // CONFIG_STA_SUPPORT //


	MeasureReqTabExit(pAd);
	TpcReqTabExit(pAd);


	// Close kernel threads
	RtmpMgmtTaskExit(pAd);


	// Free IRQ
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE))
	{
		RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE);
	}

#ifdef RESOURCE_PRE_ALLOC
	RTMPResetTxRxRingMemory(pAd);
#else
	// Free Ring or USB buffers
	RTMPFreeTxRxRingMemory(pAd);
#endif // RESOURCE_PRE_ALLOC //

	RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS);

#ifdef DOT11_N_SUPPORT
	// Free BA reorder resource
	ba_reordering_resource_release(pAd);
#endif // DOT11_N_SUPPORT //
	
#ifdef CONFIG_STA_SUPPORT
#endif // CONFIG_STA_SUPPORT //

	RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_START_UP);

/*+++Modify by woody to solve the bulk fail+++*/
#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
	}
#endif // CONFIG_STA_SUPPORT //

#ifdef VENDOR_FEATURE2_SUPPORT
	printk("Number of Packet Allocated = %d\n", pAd->NumOfPktAlloc);
	printk("Number of Packet Freed = %d\n", pAd->NumOfPktFree);
#endif // VENDOR_FEATURE2_SUPPORT //

	DBGPRINT(RT_DEBUG_TRACE, ("<=== rt28xx_close\n"));
	return 0; // close ok
} /* End of rt28xx_close */


/*
========================================================================
Routine Description:
    Open raxx interface.

Arguments:
	*net_dev			the raxx interface pointer

Return Value:
    0					Open OK
	otherwise			Open Fail

Note:
========================================================================
*/
int rt28xx_open(IN PNET_DEV dev)
{				 
	struct net_device * net_dev = (struct net_device *)dev;
	PRTMP_ADAPTER pAd = NULL;
	int retval = 0;
 	//POS_COOKIE pObj;

	GET_PAD_FROM_NET_DEV(pAd, net_dev);	

	// Sanity check for pAd
	if (pAd == NULL)
	{
		/* if 1st open fail, pAd will be free;
		   So the net_dev->priv will be NULL in 2rd open */
		return -1;
	}

#ifdef CONFIG_APSTA_MIXED_SUPPORT
	if (pAd->OpMode == OPMODE_AP)
	{
		CW_MAX_IN_BITS = 6;
	}
	else if (pAd->OpMode == OPMODE_STA)
	{
		CW_MAX_IN_BITS = 10;
	}
#endif // CONFIG_APSTA_MIXED_SUPPORT //

#if WIRELESS_EXT >= 12
	if (net_dev->priv_flags == INT_MAIN) 
	{
#ifdef CONFIG_APSTA_MIXED_SUPPORT
		if (pAd->OpMode == OPMODE_AP)
			net_dev->wireless_handlers = (struct iw_handler_def *) &rt28xx_ap_iw_handler_def;
#endif // CONFIG_APSTA_MIXED_SUPPORT //
#ifdef CONFIG_STA_SUPPORT
		if (pAd->OpMode == OPMODE_STA)
			net_dev->wireless_handlers = (struct iw_handler_def *) &rt28xx_iw_handler_def;
#endif // CONFIG_STA_SUPPORT //
	}
#endif // WIRELESS_EXT >= 12 //

	// Request interrupt service routine for PCI device
	// register the interrupt routine with the os
//	RtmpOSIRQRequest(net_dev);

	// Init IRQ parameters stored in pAd
	RTMP_IRQ_INIT(pAd);
	
	// Chip & other init
	if (rt28xx_init(pAd, mac, hostname) == FALSE)
		goto err;

#ifdef RT_CFG80211_SUPPORT
	RT_CFG80211_REINIT(pAd);
	RT_CFG80211_CRDA_REG_RULE_APPLY(pAd);
#endif // RT_CFG80211_SUPPORT //

#ifdef CONFIG_STA_SUPPORT
#endif // CONFIG_STA_SUPPORT //

	// Enable Interrupt
	RTMP_IRQ_ENABLE(pAd);

	// Request interrupt service routine for PCI device
	// register the interrupt routine with the os
	RtmpOSIRQRequest(net_dev);

	// Now Enable RxTx
	RTMPEnableRxTx(pAd);
	RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_START_UP);

	{
	UINT32 reg = 0;
	RTMP_IO_READ32(pAd, 0x1300, &reg);  // clear garbage interrupts
	printk("0x1300 = %08x\n", reg);
	}

	{
//	u32 reg;
//	UINT8  byte;
//	u16 tmp;

//	RTMP_IO_READ32(pAd, XIFS_TIME_CFG, &reg);

//	tmp = 0x0805;
//	reg  = (reg & 0xffff0000) | tmp;
//	RTMP_IO_WRITE32(pAd, XIFS_TIME_CFG, reg);

	}


#ifdef CONFIG_STA_SUPPORT
#ifdef PCIE_PS_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
        RTMPInitPCIeLinkCtrlValue(pAd);
#endif // PCIE_PS_SUPPORT //


#endif // CONFIG_STA_SUPPORT //



#ifdef VENDOR_FEATURE2_SUPPORT
	printk("Number of Packet Allocated in open = %d\n", pAd->NumOfPktAlloc);
	printk("Number of Packet Freed in open = %d\n", pAd->NumOfPktFree);
#endif // VENDOR_FEATURE2_SUPPORT //

	return (retval);

err:
//+++Add by shiang, move from rt28xx_init() to here.
	RtmpOSIRQRelease(net_dev);
//---Add by shiang, move from rt28xx_init() to here.

	return (-1);
} /* End of rt28xx_open */


PNET_DEV RtmpPhyNetDevInit(
	IN RTMP_ADAPTER *pAd,
	IN RTMP_OS_NETDEV_OP_HOOK *pNetDevHook)
{
	struct net_device	*net_dev = NULL;
//	NDIS_STATUS		Status;
	
	net_dev = RtmpOSNetDevCreate(pAd, INT_MAIN, 0, sizeof(PRTMP_ADAPTER), INF_MAIN_DEV_NAME);
	if (net_dev == NULL)
	{
		printk("RtmpPhyNetDevInit(): creation failed for main physical net device!\n");
		return NULL;
	}

	NdisZeroMemory((unsigned char *)pNetDevHook, sizeof(RTMP_OS_NETDEV_OP_HOOK));
	pNetDevHook->open = MainVirtualIF_open;
	pNetDevHook->stop = MainVirtualIF_close;
	pNetDevHook->xmit = rt28xx_send_packets;
#ifdef IKANOS_VX_1X0
	pNetDevHook->xmit = IKANOS_DataFramesTx;
#endif // IKANOS_VX_1X0 //
	pNetDevHook->ioctl = rt28xx_ioctl;
	pNetDevHook->priv_flags = INT_MAIN;
	pNetDevHook->get_stats = RT28xx_get_ether_stats;

	pNetDevHook->needProtcted = FALSE;

#if (WIRELESS_EXT < 21) && (WIRELESS_EXT >= 12)
	pNetDevHook->get_wstats = rt28xx_get_wireless_stats;
#endif

#ifdef CONFIG_STA_SUPPORT
#if WIRELESS_EXT >= 12
	if (pAd->OpMode == OPMODE_STA)
	{
		pNetDevHook->iw_handler = (void *)&rt28xx_iw_handler_def;
	}
#endif //WIRELESS_EXT >= 12
#endif // CONFIG_STA_SUPPORT //

#ifdef CONFIG_APSTA_MIXED_SUPPORT
#if WIRELESS_EXT >= 12
	if (pAd->OpMode == OPMODE_AP)
	{
		pNetDevHook->iw_handler = &rt28xx_ap_iw_handler_def;
	}
#endif //WIRELESS_EXT >= 12
#endif // CONFIG_APSTA_MIXED_SUPPORT //

	RTMP_OS_NETDEV_SET_PRIV(net_dev, pAd);
	pAd->net_dev = net_dev;
	


#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
	SET_MODULE_OWNER(net_dev);
#endif 

	netif_stop_queue(net_dev);

	return net_dev;
	
}


/*
========================================================================
Routine Description:
    The entry point for Linux kernel sent packet to our driver.

Arguments:
    sk_buff *skb		the pointer refer to a sk_buffer.

Return Value:
    0					

Note:
	This function is the entry point of Tx Path for Os delivery packet to 
	our driver. You only can put OS-depened & STA/AP common handle procedures 
	in here.
========================================================================
*/
int rt28xx_packet_xmit(struct sk_buff *skb)
{
	struct net_device *net_dev = skb->dev;
	PRTMP_ADAPTER pAd = NULL;
	int status = 0;
	PNDIS_PACKET pPacket = (PNDIS_PACKET) skb;

	GET_PAD_FROM_NET_DEV(pAd, net_dev);	

	/* RT2870STA does this in RTMPSendPackets() */
#ifdef RALINK_ATE
	if (ATE_ON(pAd))
	{
		RELEASE_NDIS_PACKET(pAd, pPacket, NDIS_STATUS_RESOURCES);
		return 0;
	}
#endif // RALINK_ATE //

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		// Drop send request since we are in monitor mode
		if (MONITOR_ON(pAd))
		{
			RELEASE_NDIS_PACKET(pAd, pPacket, NDIS_STATUS_FAILURE);
			goto done;
		}
	}
#endif // CONFIG_STA_SUPPORT //

        // EapolStart size is 18
	if (skb->len < 14)
	{
		//printk("bad packet size: %d\n", pkt->len);
		hex_dump("bad packet", skb->data, skb->len);
		RELEASE_NDIS_PACKET(pAd, pPacket, NDIS_STATUS_FAILURE);
		goto done;
	}



	RTMP_SET_PACKET_5VT(pPacket, 0);
//	MiniportMMRequest(pAd, pkt->data, pkt->len);
#ifdef CONFIG_5VT_ENHANCE
    if (*(int*)(skb->cb) == BRIDGE_TAG) {
		RTMP_SET_PACKET_5VT(pPacket, 1);
    }
#endif



#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{

		STASendPackets((NDIS_HANDLE)pAd, (PPNDIS_PACKET) &pPacket, 1);
	}

#endif // CONFIG_STA_SUPPORT //

	status = 0;
done:
			   
	return status;
}


/*
========================================================================
Routine Description:
    Send a packet to WLAN.

Arguments:
    skb_p           points to our adapter
    dev_p           which WLAN network interface

Return Value:
    0: transmit successfully
    otherwise: transmit fail

Note:
========================================================================
*/
static int rt28xx_send_packets(
	IN struct sk_buff 		*skb_p, 
	IN struct net_device 	*net_dev)
{
	RTMP_ADAPTER *pAd = NULL;

	GET_PAD_FROM_NET_DEV(pAd, net_dev);	

	if (!(RTMP_OS_NETDEV_STATE_RUNNING(net_dev)))
	{
		RELEASE_NDIS_PACKET(pAd, (PNDIS_PACKET)skb_p, NDIS_STATUS_FAILURE);
		return 0;
	}

	NdisZeroMemory((PUCHAR)&skb_p->cb[CB_OFF], 15);
	RTMP_SET_PACKET_NET_DEVICE_MBSSID(skb_p, MAIN_MBSSID);

	MEM_DBG_PKT_ALLOC_INC(pAd);

	return rt28xx_packet_xmit(skb_p);
}


#if WIRELESS_EXT >= 12
// This function will be called when query /proc
struct iw_statistics *rt28xx_get_wireless_stats(
    IN struct net_device *net_dev)
{
	PRTMP_ADAPTER pAd = NULL;

	GET_PAD_FROM_NET_DEV(pAd, net_dev);	


	DBGPRINT(RT_DEBUG_TRACE, ("rt28xx_get_wireless_stats --->\n"));

	//check if the interface is down
	if(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE))
		return NULL;	
	
	pAd->iw_stats.status = 0; // Status - device dependent for now

	// link quality
#ifdef CONFIG_STA_SUPPORT
	if (pAd->OpMode == OPMODE_STA)
		pAd->iw_stats.qual.qual = ((pAd->Mlme.ChannelQuality * 12)/10 + 10);
#endif // CONFIG_STA_SUPPORT //

	if(pAd->iw_stats.qual.qual > 100)
		pAd->iw_stats.qual.qual = 100;

#ifdef CONFIG_STA_SUPPORT
	if (pAd->OpMode == OPMODE_STA)
	{
		pAd->iw_stats.qual.level =
			RTMPMaxRssi(pAd, pAd->StaCfg.RssiSample.LastRssi0,
							pAd->StaCfg.RssiSample.LastRssi1,
							pAd->StaCfg.RssiSample.LastRssi2);
	}
#endif // CONFIG_STA_SUPPORT //

	pAd->iw_stats.qual.noise = pAd->BbpWriteLatch[66]; // noise level (dBm)
	
	pAd->iw_stats.qual.noise += 256 - 143;
	pAd->iw_stats.qual.updated = 1;     // Flags to know if updated
#ifdef IW_QUAL_DBM
	pAd->iw_stats.qual.updated |= IW_QUAL_DBM;	// Level + Noise are dBm
#endif // IW_QUAL_DBM //

	pAd->iw_stats.discard.nwid = 0;     // Rx : Wrong nwid/essid
	pAd->iw_stats.miss.beacon = 0;      // Missed beacons/superframe
	
	DBGPRINT(RT_DEBUG_TRACE, ("<--- rt28xx_get_wireless_stats\n"));
	return &pAd->iw_stats;
}
#endif // WIRELESS_EXT //


void tbtt_tasklet(unsigned long data)
{
//#define MAX_TX_IN_TBTT		(16)

}

INT rt28xx_ioctl(
	IN	PNET_DEV	net_dev, 
	IN	OUT	struct ifreq	*rq, 
	IN	INT					cmd)
{
	RTMP_ADAPTER	*pAd = NULL;
	INT				ret = 0;

	GET_PAD_FROM_NET_DEV(pAd, net_dev);	

	if (pAd == NULL)
	{
		/* if 1st open fail, pAd will be free;
		   So the net_dev->priv will be NULL in 2rd open */
		return -ENETDOWN;
	}


#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		ret = rt28xx_sta_ioctl(net_dev, rq, cmd);
	}
#endif // CONFIG_STA_SUPPORT //

	return ret;
}


/*
    ========================================================================

    Routine Description:
        return ethernet statistics counter

    Arguments:
        net_dev                     Pointer to net_device

    Return Value:
        net_device_stats*

    Note:

    ========================================================================
*/
static struct net_device_stats *RT28xx_get_ether_stats(
    IN  struct net_device *net_dev)
{
    RTMP_ADAPTER *pAd = NULL;

	if (net_dev)
		GET_PAD_FROM_NET_DEV(pAd, net_dev);	

	if (pAd)
	{

		pAd->stats.rx_packets = pAd->WlanCounters.ReceivedFragmentCount.QuadPart;
		pAd->stats.tx_packets = pAd->WlanCounters.TransmittedFragmentCount.QuadPart;

		pAd->stats.rx_bytes = pAd->RalinkCounters.ReceivedByteCount;
		pAd->stats.tx_bytes = pAd->RalinkCounters.TransmittedByteCount;

		pAd->stats.rx_errors = pAd->Counters8023.RxErrors;
		pAd->stats.tx_errors = pAd->Counters8023.TxErrors;

		pAd->stats.rx_dropped = 0;
		pAd->stats.tx_dropped = 0;

	    pAd->stats.multicast = pAd->WlanCounters.MulticastReceivedFrameCount.QuadPart;   // multicast packets received
	    pAd->stats.collisions = pAd->Counters8023.OneCollision + pAd->Counters8023.MoreCollisions;  // Collision packets

	    pAd->stats.rx_length_errors = 0;
	    pAd->stats.rx_over_errors = pAd->Counters8023.RxNoBuffer;                   // receiver ring buff overflow
	    pAd->stats.rx_crc_errors = 0;//pAd->WlanCounters.FCSErrorCount;     // recved pkt with crc error
	    pAd->stats.rx_frame_errors = pAd->Counters8023.RcvAlignmentErrors;          // recv'd frame alignment error
	    pAd->stats.rx_fifo_errors = pAd->Counters8023.RxNoBuffer;                   // recv'r fifo overrun
	    pAd->stats.rx_missed_errors = 0;                                            // receiver missed packet

	    // detailed tx_errors
	    pAd->stats.tx_aborted_errors = 0;
	    pAd->stats.tx_carrier_errors = 0;
	    pAd->stats.tx_fifo_errors = 0;
	    pAd->stats.tx_heartbeat_errors = 0;
	    pAd->stats.tx_window_errors = 0;

	    // for cslip etc
	    pAd->stats.rx_compressed = 0;
	    pAd->stats.tx_compressed = 0;
		
		return &pAd->stats;
	}
	else
    	return NULL;
}


BOOLEAN RtmpPhyNetDevExit(
	IN RTMP_ADAPTER *pAd, 
	IN PNET_DEV net_dev)
{



#ifdef INF_AMAZON_PPA
	if (ppa_hook_directpath_register_dev_fn && pAd->PPAEnable==TRUE) 
	{
		UINT status;
		status=ppa_hook_directpath_register_dev_fn(&pAd->g_if_id, pAd->net_dev, NULL, PPA_F_DIRECTPATH_DEREGISTER);
		printk("unregister PPA:g_if_id=%d status=%d\n",pAd->g_if_id,status);
	}
	kfree(pAd->pDirectpathCb); 
#endif // INF_AMAZON_PPA //

	// Unregister network device
	if (net_dev != NULL)
	{
		printk("RtmpOSNetDevDetach(): RtmpOSNetDeviceDetach(), dev->name=%s!\n", net_dev->name);
		RtmpOSNetDevDetach(net_dev);
	}

	return TRUE;
	
}


/*******************************************************************************

	Device IRQ related functions.
	
 *******************************************************************************/
int RtmpOSIRQRequest(IN PNET_DEV pNetDev)
{
	struct net_device *net_dev = pNetDev;
	PRTMP_ADAPTER pAd = NULL;
	int retval = 0;
	
	GET_PAD_FROM_NET_DEV(pAd, pNetDev);	
	
	ASSERT(pAd);
	


	return retval; 
	
}


int RtmpOSIRQRelease(IN PNET_DEV pNetDev)
{
	struct net_device *net_dev = pNetDev;
	PRTMP_ADAPTER pAd = NULL;

	GET_PAD_FROM_NET_DEV(pAd, net_dev);	
	
	ASSERT(pAd);
	


	return 0;
}


