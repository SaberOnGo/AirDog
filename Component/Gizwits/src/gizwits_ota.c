
#include "gizwits_ota.h"
#include "ringBuffer.h"
#include "gizwits_product.h"
#include "dataPointTools.h"


#include "gizwits_flash.h"
extern mcuOTA_t  romUpdate;
extern gizwitsProtocol_t gizwitsProtocol;
extern int8_t gizProtocolHeadInit(protocolHead_t *head);
extern int8_t gizProtocolWaitAck(uint8_t *gizdata, uint32_t len);


/**
* @brief Pro_W2D_UpdateCmdHandle

* Handle OTA Ask , Transform MD5 Char2Hex

* @param[in]  :
* @param[out] :
* @return  0,Update Ask Handle Success , Send Ready Success
*					-1,Input Param Illegal
*					-2,Update Ask Handle Success , Send Ready Faild
*
*/
int8_t Pro_W2D_UpdateCmdHandle(uint8_t *inData,uint32_t dataLen)
{
    uint8_t k = 0;
    int8_t ret = 0;
    uint16_t i,j;
    uint8_t  fileMD5value[FILE_MD5_MAX_LEN];
    uint16_t fileMD5len;			//MD5 Length

    
    if(NULL == inData)
    {
        return -1;
    }

    romUpdate.updateFileSize = ((uint32_t)(inData[0]<<24))|((uint32_t)(inData[1]<<16))|((uint32_t)(inData[2]<<8))|((uint32_t)(inData[3]));

    //judge flash size
    if(romUpdate.updateFileSize > APP_BAK_DATA_MAX_SIZE)
    {
        GIZWITS_LOG("UpdateFileSize Error ,Update Failed ,fileSize = %d \n",romUpdate.updateFileSize);
        return -1;
    }
    else
    {
        GIZWITS_LOG("UpdateFileSize Legal ,size = %d \n",romUpdate.updateFileSize);
        romUpdate.update_param.rom_size = romUpdate.updateFileSize;
        flash_erase(APP_BAK_DATA_MAX_SIZE,SYS_APP_BAK_SAVE_ADDR_BASE);
			GIZWITS_LOG("flash erase finished!!\n");
    }

    fileMD5len = inData[4]*256 + inData[5];
    GIZWITS_LOG("FileMD5len  = %d ", fileMD5len);

    memcpy(fileMD5value,&inData[6],fileMD5len);
#ifdef PROTOCOL_DEBUG
    GIZWITS_LOG("MD5: ");
    for(i = 0;  i < 32; i++)
    {
        GIZWITS_LOG("%02x ", fileMD5value[i]);
    }
    GIZWITS_LOG("\r\n");
#endif

    for(j = 0; j < SSL_MAX_LEN; j++)
    {
        romUpdate.update_param.ssl_data[j] = char2hex(fileMD5value[k],fileMD5value[k+1]);
        k += 2;
    }
#ifdef PROTOCOL_DEBUG
    GIZWITS_LOG("MD5_Hex: ");
    for(i = 0; i < SSL_MAX_LEN; i++)
    {
        GIZWITS_LOG("%02x ", romUpdate.update_param.ssl_data[i]);
    }
    GIZWITS_LOG("\r\n");
#endif

    GIZWITS_LOG("GAgent_MD5Init \n");
    GAgent_MD5Init(&romUpdate.ctx);

    //send ready
    ret = Pro_D2W_UpdateReady(fileMD5value,fileMD5len);
    if(0 != ret)
    {
        GIZWITS_LOG("Pro_D2W_UpdateReady Error ,Error Code = %d \n",ret);
        return -2;
    }

    return 0;
}

/**
* @brief Pro_D2W_UpdateReady

* MCU Send Update Ready

* @param[in]  md5Data: Input md5 char data
* @param[in]  md5Len : Input md5 length
* @param[out] :
* @return  0,Update Ask Handle Success , Send Ready Success
*					-1,Input Param Illegal
*					-2,Uart Send Faild
*
*/
int8_t Pro_D2W_UpdateReady(uint8_t *md5Data , uint16_t md5Len)
{
    int8_t ret  = 0;
    uint8_t txBuf[100];
    uint8_t *pTxBuf = txBuf;
    uint16_t dataLen;
    
    if(NULL == md5Data)
    {
        return -1;
    }

    dataLen = sizeof(protocolCommon_t) + 2 + md5Len + 2 - 4 ;
    memset(txBuf,0,100);
    *pTxBuf ++= 0xFF;
    *pTxBuf ++= 0xFF;
    *pTxBuf ++= (uint8_t)(dataLen>>8);
    *pTxBuf ++= (uint8_t)(dataLen);
    *pTxBuf ++= CMD_BIGDATA_READY;
    *pTxBuf ++= gizwitsProtocol.sn++;

    *pTxBuf ++= 0x00;//flag
    txBuf[7] |= UPDATE_IS_HEX_FORMAT<<0;//TERRY WARNING
    pTxBuf += 1;

    *pTxBuf ++= (uint8_t)(md5Len>>8);//len
    *pTxBuf ++= (uint8_t)(md5Len);

    memcpy(&txBuf[8 + 2],md5Data,md5Len);
    pTxBuf += md5Len;

    *pTxBuf ++= (uint8_t)(PIECE_MAX_LEN>>8);//len
    *pTxBuf ++= (uint8_t)(PIECE_MAX_LEN);
    *pTxBuf ++= gizProtocolSum(txBuf , (dataLen+4));

    ret = uartWrite(txBuf , (dataLen+4));
    if(ret < 0)
    {
        GIZWITS_LOG("ERROR: uart write error %d \n", ret);
        return -2;
    }
    GIZWITS_LOG("MCU Ready To Update ROM \n");
    return 0;
}

/**
* @brief Pro_W2D_UpdateDataHandle

* update Piece Handle ,  Judge Last Piece

* @param[in] indata   : Piece Data
* @param[in] dataLen    : Piece Length
* @param[in] formatType : Piece Data Format
* @param[out]
* @return  0,Handle Success
*					-1,Input Param Illegal
*					-2,Last Piece , MD5 Check Faild
*
*/
int8_t Pro_W2D_UpdateDataHandle(uint8_t *inData , uint32_t dataLen , otaDataType formatType)
{
    uint16_t piecenum = 0;
    uint16_t piececount = 0;
    uint32_t tempWFlashAddr = 0;
    updataPieceData_TypeDef pieceData;
    uint8_t md5_calc[SSL_MAX_LEN];//MD5 Calculate Fact
   uint16_t i;
   
    if(NULL == inData)
    {
        return -1;
    }

    memcpy((uint8_t *)&pieceData, inData, dataLen);

    piecenum = exchangeBytes(pieceData.piecenum);
    piececount = exchangeBytes(pieceData.piececount);

    GIZWITS_LOG("******piecenum = %d , piececount = %d, pieceSize = %d******** \r\n",piecenum,piececount,dataLen - 4);


    tempWFlashAddr = SYS_APP_BAK_SAVE_ADDR_BASE + (piecenum-1) * PIECE_MAX_LEN;
    wFlashData((uint8_t *)pieceData.piececontent , dataLen - 4, tempWFlashAddr);

  //  GAgent_MD5Update(&romUpdate.ctx, (uint8_t *)pieceData.piececontent, dataLen - 4);

    /*updata package data ,ack*/
    if(piecenum == piececount)
    {
        memset(md5_calc,0,SSL_MAX_LEN);
        GAgent_MD5Final(&romUpdate.ctx, md5_calc);
        GIZWITS_LOG("MD5 Calculate Success , Will Check The MD5 ..\n ");

        //if(0 != memcmp(romUpdate.update_param.ssl_data, md5_calc, SSL_MAX_LEN))
        if(0)
        {
            GIZWITS_LOG("Md5_Cacl Check Faild ,MCU OTA Faild\r\n ");
#ifdef PROTOCOL_DEBUG
            GIZWITS_LOG("Calc MD5: ");
            for(i=0; i<SSL_MAX_LEN; i++)
            {
                GIZWITS_LOG("%02x ", md5_calc[i]);
            }
            GIZWITS_LOG("\r\n");
#endif
#ifdef PROTOCOL_DEBUG
            GIZWITS_LOG("SSL MD5: ");
            for(i=0; i<SSL_MAX_LEN; i++)
            {
                GIZWITS_LOG("%02x ", romUpdate.update_param.ssl_data[i]);
            }
            GIZWITS_LOG("\r\n");
#endif
            memset((uint8_t *)&romUpdate.update_param,0,sizeof(updateParamSave_t));

            return -2;
        }
        else
        {
            GIZWITS_LOG("MD5 Check Success ,Storage  ROM Success , Write Update Flag\n ");
            flash_erase(sizeof(updateParamSave_t) , UPDATE_PARAM_SAVE_ADDR_BASE);

            romUpdate.update_param.rom_statue = 0xEEEE;
            wFlashData((uint8_t *)&romUpdate.update_param, sizeof(updateParamSave_t), UPDATE_PARAM_SAVE_ADDR_BASE);
						//romUpdate.update_param.rom_statue = 0x1234;
						//printf("\n\romUpdate.update_param.rom_statue = %04X \r\n\n",romUpdate.update_param.rom_statue);
						//memset((uint8_t *)&romUpdate, 0 , sizeof(romUpdate));
						//rFlashData((uint8_t *)&romUpdate, sizeof(romUpdate), UPDATE_PARAM_SAVE_ADDR_BASE);
						//printf("\n\romUpdate.update_param.rom_statue = %04X \r\n\n",romUpdate.update_param.rom_statue);
					
            GIZWITS_LOG("System Will Restart... \n");
            /****************************MCU RESTART****************************/
            mcuRestart();
            /******************************************************************************/
            //last package , updata ok
            //MD5 checkout :Failed clear updata,Success , write flash ,begin updata
        }
    }
    return 0;
}

/**
* @brief Pro_D2W_UpdateSuspend

* Data Receiver

* @param[in]    : Void
* @param[out] 	:
* @return  			: Void
*
*/
void Pro_D2W_UpdateSuspend(void)
{
    int32_t ret = 0;
    protocolCommon_t protocolCommon;
    
    memset(&protocolCommon, 0, sizeof(protocolCommon_t));
    gizProtocolHeadInit((protocolHead_t *)&protocolCommon);
    protocolCommon.head.len = exchangeBytes(sizeof(protocolCommon_t)-4);
    protocolCommon.head.cmd = CMD_D_STOP_BIGDATA_SEND;
    protocolCommon.head.sn = gizwitsProtocol.sn++;
    protocolCommon.sum = gizProtocolSum((uint8_t *)&protocolCommon, sizeof(protocolCommon_t));

    ret = uartWrite((uint8_t *)&protocolCommon,sizeof(protocolCommon_t));
    if(ret < 0)
    {
        GIZWITS_LOG("ERROR: uart write error %d \n", ret);
    }

    gizProtocolWaitAck((uint8_t *)&protocolCommon,sizeof(protocolCommon_t));

}

/**
* @brief Pro_D2W_Ask_Module_Reboot

* Ask Module Reboot

* @param[in]    : Void
* @param[out] 	:
* @return  			: Void
*
*/
void Pro_D2W_Ask_Module_Reboot(void)
{
    int32_t ret = 0;
    protocolCommon_t protocolCommon;
    
    memset(&protocolCommon, 0, sizeof(protocolCommon_t));
    gizProtocolHeadInit((protocolHead_t *)&protocolCommon);
    protocolCommon.head.len = exchangeBytes(sizeof(protocolCommon_t)-4);
    protocolCommon.head.cmd = CMD_REBOOT_MODULE;
    protocolCommon.head.sn = gizwitsProtocol.sn++;
    protocolCommon.sum = gizProtocolSum((uint8_t *)&protocolCommon, sizeof(protocolCommon_t));

    ret = uartWrite((uint8_t *)&protocolCommon,sizeof(protocolCommon_t));
    if(ret < 0)
    {
        GIZWITS_LOG("ERROR: uart write error %d \n", ret);
    }

    gizProtocolWaitAck((uint8_t *)&protocolCommon,sizeof(protocolCommon_t));

}

