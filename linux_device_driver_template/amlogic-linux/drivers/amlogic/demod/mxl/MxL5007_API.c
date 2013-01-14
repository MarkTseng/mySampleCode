/*

 Driver APIs for MxL5007 Tuner
 
 Copyright, Maxlinear, Inc.
 All Rights Reserved
 
 File Name:      MxL5007_API.c
 
 Version:    4.1.3
*/


//#include "StdAfx.h"
#include "MxL5007_API.h"
#include "MxL5007.h"


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//																		   //
//							Tuner Functions								   //
//																		   //
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
MxL_ERR_MSG MxL_Set_Register(MxL5007_TunerConfigS* myTuner, UINT8 RegAddr, UINT8 RegData)
{
	UINT32 Status=0;
	UINT8 pArray[2];
	pArray[0] = RegAddr;
	pArray[1] = RegData;
	Status = MxL_I2C_Write(myTuner->I2C_adap, pArray, 2);
	if(Status) return MxL_ERR_SET_REG;

	return MxL_OK;

}

MxL_ERR_MSG MxL_Get_Register(MxL5007_TunerConfigS* myTuner, UINT8 RegAddr, UINT8 *RegData)
{
	if(MxL_I2C_Read(myTuner->I2C_adap, RegAddr, RegData))
		return MxL_ERR_GET_REG;
	return MxL_OK;

}

MxL_ERR_MSG MxL_Soft_Reset(MxL5007_TunerConfigS* myTuner)
{
    // UINT32 Status=0;	
	UINT8 reg_reset;
	reg_reset = 0xFF;
	if(MxL_I2C_Write(myTuner->I2C_adap, &reg_reset, 1))
		return MxL_ERR_OTHERS;

	return MxL_OK;
}

MxL_ERR_MSG MxL_Loop_Through_On(MxL5007_TunerConfigS* myTuner, MxL5007_LoopThru isOn)
{	
	UINT8 pArray[2];	// a array pointer that store the addr and data pairs for I2C write	
	
	pArray[0]=0x04;
	if(isOn)
	 pArray[1]= 0x01;
	else
	 pArray[1]= 0x0;

	if(MxL_I2C_Write(myTuner->I2C_adap, pArray, 2))
		return MxL_ERR_OTHERS;

	return MxL_OK;
}

MxL_ERR_MSG MxL_Stand_By(MxL5007_TunerConfigS* myTuner)
{
	UINT8 pArray[4];	// a array pointer that store the addr and data pairs for I2C write	
	
	pArray[0] = 0x01;
	pArray[1] = 0x0;
	pArray[2] = 0x0F;
	pArray[3] = 0x0;

	if(MxL_I2C_Write(myTuner->I2C_adap, pArray, 4))
		return MxL_ERR_OTHERS;

	return MxL_OK;
}

MxL_ERR_MSG MxL_Wake_Up(MxL5007_TunerConfigS* myTuner)
{
	UINT8 pArray[2];	// a array pointer that store the addr and data pairs for I2C write	
	
	pArray[0] = 0x01;
	pArray[1] = 0x01;

	if(MxL_I2C_Write(myTuner->I2C_adap, pArray, 2))
		return MxL_ERR_OTHERS;

	if(MxL_Tuner_RFTune(myTuner, myTuner->RF_Freq_Hz, myTuner->BW_MHz))
		return MxL_ERR_RFTUNE;

	return MxL_OK;
}

MxL_ERR_MSG MxL_Tuner_Init(MxL5007_TunerConfigS* myTuner)
{	
	UINT8 pArray[MAX_ARRAY_SIZE];	// a array pointer that store the addr and data pairs for I2C write
	UINT32 Array_Size;							// a integer pointer that store the number of element in above array

	//Soft reset tuner
	if(MxL_Soft_Reset(myTuner))
		return MxL_ERR_INIT;

	//perform initialization calculation
	MxL5007_Init(pArray, &Array_Size, (UINT8)myTuner->Mode, myTuner->IF_Diff_Out_Level, (UINT32)myTuner->Xtal_Freq, 
				(UINT32)myTuner->IF_Freq, (UINT8)myTuner->IF_Spectrum, (UINT8)myTuner->ClkOut_Setting, (UINT8)myTuner->ClkOut_Amp);

	//perform I2C write here
	if(MxL_I2C_Write(myTuner->I2C_adap, pArray, Array_Size))
		return MxL_ERR_INIT;

	return MxL_OK;
}


MxL_ERR_MSG MxL_Tuner_RFTune(MxL5007_TunerConfigS* myTuner, UINT32 RF_Freq_Hz, MxL5007_BW_MHz BWMHz)
{
    // UINT32 Status=0;
	UINT8 pArray[MAX_ARRAY_SIZE];	// a array pointer that store the addr and data pairs for I2C write
	UINT32 Array_Size;							// a integer pointer that store the number of element in above array

	//Store information into struc
	myTuner->RF_Freq_Hz = RF_Freq_Hz;
	myTuner->BW_MHz = BWMHz;

	//perform Channel Change calculation
	MxL5007_RFTune(pArray,&Array_Size,RF_Freq_Hz,BWMHz);

	//perform I2C write here
	if(MxL_I2C_Write(myTuner->I2C_adap, pArray, Array_Size))
		return MxL_ERR_RFTUNE;

	//wait for 3ms
	MxL_Delay(3); 

	return MxL_OK;
}

MxL5007_ChipVersion MxL_Check_ChipVersion(MxL5007_TunerConfigS* myTuner)
{	
	UINT8 Data;
	if(MxL_I2C_Read(myTuner->I2C_adap, 0xD3, &Data))
		return MxL_GET_ID_FAIL;
		
	if (Data == 0x41)
	    return 	MxL_5007T_V4;
	else
	    return  MxL_UNKNOWN_ID;

}

MxL_ERR_MSG MxL_RFSynth_Lock_Status(MxL5007_TunerConfigS* myTuner, int * isLock)
{	
	UINT8 Data;
	*isLock = FALSE; 
	if(MxL_I2C_Read(myTuner->I2C_adap, 0xD8, &Data))
		return MxL_ERR_OTHERS;
	Data &= 0x0C;
	if (Data == 0x0C)
		*isLock = TRUE;  //RF Synthesizer is Lock	
	return MxL_OK;
}

MxL_ERR_MSG MxL_REFSynth_Lock_Status(MxL5007_TunerConfigS* myTuner, int * isLock)
{
	UINT8 Data;
	*isLock = FALSE; 
	if(MxL_I2C_Read(myTuner->I2C_adap, 0xD8, &Data))
		return MxL_ERR_OTHERS;
	Data &= 0x03;
	if (Data == 0x03)
		*isLock = TRUE;   //REF Synthesizer is Lock
	return MxL_OK;
}

