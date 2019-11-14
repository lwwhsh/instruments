#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <epicsTypes.h>
#include <epicsTime.h>
#include <epicsMutex.h>
#include <epicsEvent.h>
#include <iocsh.h>
#include <errlog.h>
#include <epicsExport.h>
#include <epicsThread.h>
#include <epicsString.h>
#include <epicsTimer.h>
#include <epicsExit.h>
#include <epicsMutex.h>
#include <cantProceed.h>

#include <asynDriver.h>
#include <asynInt32.h>
#include <asynFloat64.h>
#include <asynInt32Array.h>
#include <asynFloat64Array.h>
#include <asynDrvUser.h>
#include <asynPortClient.h>

#include "drvAgilent4uhv.h"

static const char *driverName = "drvAgilent4uhv";

drvAgilent4uhv::drvAgilent4uhv(const char *myPortName, const char *commPortName, const int nDevAddr, const double dbTimeout, const int nDevIdx)
	: asynPortDriver(myPortName,
			1, /* maxAddr */
			NUM_AGILENT4UHV_PARAMS,
			asynInt32Mask | asynFloat64Mask | asynFloat64ArrayMask | asynOctetMask | asynEnumMask | asynDrvUserMask,
			asynInt32Mask | asynFloat64Mask | asynFloat64ArrayMask | asynOctetMask | asynEnumMask,
			0,
			1,
			0,
			0),
	m_bRun(false),m_dbCommDelay(10),m_pComm(NULL),m_nDevAddr(nDevAddr),m_dbTimeout(dbTimeout)
{
	asynStatus status;
	const char *functionName = "drvAgilent4uhv";

	createParam(P_4uhvIsAliveString,			asynParamInt32,				&P_4uhvIsAlive);
	createParam(P_4uhvHvCh1String,				asynParamInt32,				&P_4uhvHv[0]);
	createParam(P_4uhvHvCh1RbvString,			asynParamInt32,				&P_4uhvHvRbv[0]);
	createParam(P_4uhvHvCh2String,				asynParamInt32,				&P_4uhvHv[1]);
	createParam(P_4uhvHvCh2RbvString,			asynParamInt32,				&P_4uhvHvRbv[1]);
	createParam(P_4uhvHvCh3String,				asynParamInt32,				&P_4uhvHv[2]);
	createParam(P_4uhvHvCh3RbvString,			asynParamInt32,				&P_4uhvHvRbv[2]);
	createParam(P_4uhvHvCh4String,				asynParamInt32,				&P_4uhvHv[3]);
	createParam(P_4uhvHvCh4RbvString,			asynParamInt32,				&P_4uhvHvRbv[3]);
	createParam(P_4uhvVoltageCh1String,			asynParamFloat64,			&P_4uhvVoltage[0]); 	
	createParam(P_4uhvCurrentCh1String,			asynParamFloat64,			&P_4uhvCurrent[0]); 	
	createParam(P_4uhvPressureCh1String,		asynParamFloat64,			&P_4uhvPressure[0]);	
	createParam(P_4uhvVoltageCh2String,			asynParamFloat64,			&P_4uhvVoltage[1]); 	
	createParam(P_4uhvCurrentCh2String,			asynParamFloat64,			&P_4uhvCurrent[1]); 	
	createParam(P_4uhvPressureCh2String,		asynParamFloat64,			&P_4uhvPressure[1]);	
	createParam(P_4uhvVoltageCh3String,			asynParamFloat64,			&P_4uhvVoltage[2]); 	
	createParam(P_4uhvCurrentCh3String,			asynParamFloat64,			&P_4uhvCurrent[2]); 	
	createParam(P_4uhvPressureCh3String,		asynParamFloat64,			&P_4uhvPressure[2]);	
	createParam(P_4uhvVoltageCh4String,			asynParamFloat64,			&P_4uhvVoltage[3]); 	
	createParam(P_4uhvCurrentCh4String,			asynParamFloat64,			&P_4uhvCurrent[3]); 	
	createParam(P_4uhvPressureCh4String,		asynParamFloat64,			&P_4uhvPressure[3]);	
	createParam(P_4uhvCommDelayDiagString,		asynParamFloat64,			&P_4uhvCommDelayDiag);	
	createParam(P_4uhvCommDelayString,			asynParamFloat64,			&P_4uhvCommDelay);

	setIntegerParam(P_4uhvIsAlive, 0);
	setDoubleParam(P_4uhvCommDelay, m_dbCommDelay);

	status = pasynOctetSyncIO->connect(commPortName,0,&m_pComm,NULL);
	if(status != asynSuccess)
		errlogPrintf("%s:%s error calling pasynOctetSyncIO->connect %s\n",m_pComm->errorMessage);

	//char eos[8];
	//sprintf(eos,"%c",ETX);
	//pasynOctetSyncIO->setInputEos(m_pComm,eos,1);
	//pasynOctetSyncIO->setOutputEos(m_pComm,&ETX,1);

	// init
	for(size_t i=0;i!=8;++i)
	{
	}

	m_bRun = true;
	if(epicsThreadCreate("drvAgilent4uhv",
				epicsThreadPriorityHigh,
				epicsThreadGetStackSize(epicsThreadStackMedium),
				(EPICSTHREADFUNC)threadFunc,this) == NULL)
		errlogPrintf("%s:%s Can't create thread...\n",driverName,functionName);

	epicsAtExit(agilent4uhvShutdown,this);
}

void drvAgilent4uhv::threadFunc(drvAgilent4uhv *pParam)
{
	const char *functionName = "threadFunc";
	drvAgilent4uhv *pPvt = (drvAgilent4uhv*)pParam;

	epicsTimeStamp beg,end;

	while(pPvt->m_bRun)
	{
		epicsTimeGetCurrent(&beg);
		if(pPvt->getFeatures() != asynSuccess)
			pPvt->setAlive(0);
		else
			pPvt->setAlive(1);

		epicsThreadSleep(pPvt->getCommDelay());
		epicsTimeGetCurrent(&end);
		pPvt->setCommDelayDiag(epicsTimeDiffInSeconds(&end,&beg));

		pPvt->updateTimeStamp();
		pPvt->callParamCallbacks();
	}
}

void drvAgilent4uhv::setCommDelayDiag(const double tDiff)
{
	setDoubleParam(P_4uhvCommDelayDiag,tDiff);
}

drvAgilent4uhv::~drvAgilent4uhv()
{
}

void drvAgilent4uhv::agilent4uhvShutdown(void *pParam)
{
	const char *functionName = "agilent4uhvShutdown";
	drvAgilent4uhv *pPvt = (drvAgilent4uhv*)pParam;

	pPvt->m_bRun = false;
	epicsThreadSleep(1);

	errlogPrintf("%s:%s shutdown...\n",driverName,functionName);
}


void drvAgilent4uhv::setAlive(const int value)
{
	setIntegerParam(P_4uhvIsAlive,value);
}

double drvAgilent4uhv::getCommDelay() const 
{
	return m_dbCommDelay;
}


asynStatus drvAgilent4uhv::writeInt32(asynUser *pasynUser, epicsInt32 value)
{
	int function = pasynUser->reason;
	asynStatus status = asynSuccess;
	epicsInt32 rbv;
	const char *paramName;
	const char *functionName = "writeInt32";

	int nCmdIdx = -1;

	status = setIntegerParam(function,value);

	for(int i=0;i!=4;++i)
		if(function == P_4uhvHv[i])
		{
			setHv(i,value);
			getHv(i);
		}
	callParamCallbacks();
}

asynStatus drvAgilent4uhv::writeFloat64(asynUser *pasynUser, epicsFloat64 value)
{
	int function = pasynUser->reason;
	asynStatus status = asynSuccess;
	epicsInt32 rbv;
	const char *paramName;
	const char *functionName = "writeFloat32";
	
	status = setDoubleParam(function,value);

	if(function == P_4uhvCommDelay)
		m_dbCommDelay = value;

	callParamCallbacks();
}

char *drvAgilent4uhv::getCRC(char *beg, char *end)
{
	char crc = 0;
	char str[8];
	while(beg!=end)
	{
		crc ^= (0xff & *beg);
		++beg;
	}

	sprintf(str,"%X",(0xff&crc));
	return str;
}

asynStatus drvAgilent4uhv::getFeatures()
{
	asynStatus status = asynSuccess;
	for(int i=0;i!=4;++i)
	{
		status = getHv(i);
		if(status != asynSuccess) return status;
		
		status = getV(i);
		if(status != asynSuccess) return status;
		
		status = getI(i);
		if(status != asynSuccess) return status;
		
		status = getP(i);
		if(status != asynSuccess) return status;
	}

	/*
	for(int i=0;i!=4;++i)
		epicsPrintf("[%d] HV:%d, V:%f, I: %.2le, P: %.2le\n",i,m_isHvOn[i],m_dbV[i],m_dbI[i],m_dbP[i]);
	epicsPrintf("\n");
	*/
}

asynStatus drvAgilent4uhv::setHv(const int channel, epicsInt32 val)
{
	asynStatus status = asynSuccess;
	char recvBuf[32];
	char sendDummy[32];
	char sendBuf[32];
	size_t nSendBytes;
	size_t nRecvBytes;
	int eomReason;
	char * pWin;

	// STX, ADDR, WIN, COM, DATA, ETX, CRC
	switch(channel)
	{
		case 0: pWin = (char*)HV_ONOFF_CH1; break;
		case 1: pWin = (char*)HV_ONOFF_CH2; break;
		case 2: pWin = (char*)HV_ONOFF_CH3; break;
		case 3: pWin = (char*)HV_ONOFF_CH4; break;
	}
	sprintf(sendDummy,"%c%c%s%c%1d%c",STX,m_nDevAddr,pWin,COMM_WRITE,val,ETX);
	sprintf(sendBuf,"%s%s",sendDummy,getCRC(&sendDummy[1],&sendDummy[strlen(sendDummy)]));
	
	status = pasynOctetSyncIO->writeRead(m_pComm,
			sendBuf,
			strlen(sendBuf),
			recvBuf,
			6,
			m_dbTimeout,
			&nSendBytes,
			&nRecvBytes,
			&eomReason);
	
	if(status != asynSuccess)
		errlogPrintf("SendAndReceive error calling writeRead, output=%s, status=%d, error=%s\n",
				sendBuf,status,m_pComm->errorMessage);
	
	if(recvBuf[2] != ACK)
		return asynError;

	return status;
}

asynStatus drvAgilent4uhv::getHv(const int channel)
{
	asynStatus status = asynSuccess;
	char recvBuf[32];
	char sendDummy[32];
	char sendBuf[32];
	size_t nSendBytes;
	size_t nRecvBytes;
	int eomReason;
	char * pWin;

	// STX, ADDR, WIN, COM, DATA, ETX, CRC
	switch(channel)
	{
		case 0: pWin = (char*)HV_ONOFF_CH1; break;
		case 1: pWin = (char*)HV_ONOFF_CH2; break;
		case 2: pWin = (char*)HV_ONOFF_CH3; break;
		case 3: pWin = (char*)HV_ONOFF_CH4; break;
	}
	sprintf(sendDummy,"%c%c%s%c%c",STX,m_nDevAddr,pWin,COMM_READ,ETX);
	sprintf(sendBuf,"%s%s",sendDummy,getCRC(&sendDummy[1],&sendDummy[strlen(sendDummy)]));
	
	status = pasynOctetSyncIO->writeRead(m_pComm,
			sendBuf,
			strlen(sendBuf),
			recvBuf,
			10,
			m_dbTimeout,
			&nSendBytes,
			&nRecvBytes,
			&eomReason);
	
	if(status != asynSuccess)
		errlogPrintf("SendAndReceive error calling writeRead, output=%s, status=%d, error=%s\n",
				sendBuf,status,m_pComm->errorMessage);
	
	if(strncmp(&sendDummy[2],&recvBuf[2],3) != 0)
		return asynError;

	if(recvBuf[6] == '0') 
		m_isHvOn[channel] = 0;
	else if(recvBuf[6] == '1')
		m_isHvOn[channel] = 1;

	setIntegerParam(P_4uhvHvRbv[channel],m_isHvOn[channel]);
	return status;
}

asynStatus drvAgilent4uhv::getV(const int channel)
{
	asynStatus status = asynSuccess;
	char recvBuf[32];
	char sendDummy[32];
	char sendBuf[32];
	size_t nSendBytes;
	size_t nRecvBytes;
	int eomReason;
	char * pWin;

	// STX, ADDR, WIN, COM, DATA, ETX, CRC
	switch(channel)
	{
		case 0: pWin = (char*)V_CH1; break;
		case 1: pWin = (char*)V_CH2; break;
		case 2: pWin = (char*)V_CH3; break;
		case 3: pWin = (char*)V_CH4; break;
	}
	sprintf(sendDummy,"%c%c%s%c%c",STX,m_nDevAddr,pWin,COMM_READ,ETX);
	sprintf(sendBuf,"%s%s",sendDummy,getCRC(&sendDummy[1],&sendDummy[strlen(sendDummy)]));
	
	status = pasynOctetSyncIO->writeRead(m_pComm,
			sendBuf,
			strlen(sendBuf),
			recvBuf,
			15,
			m_dbTimeout,
			&nSendBytes,
			&nRecvBytes,
			&eomReason);
	
	if(status != asynSuccess)
		errlogPrintf("SendAndReceive error calling writeRead, output=%s, status=%d, error=%s\n",
				sendBuf,status,m_pComm->errorMessage);
	
	if(strncmp(&sendDummy[2],&recvBuf[2],3) != 0)
		return asynError;

	m_dbV[channel] = myAtoF(&recvBuf[6],6) / 1000.0;
	setDoubleParam(P_4uhvVoltage[channel],m_dbV[channel]);

	return status;
}

asynStatus drvAgilent4uhv::getI(const int channel)
{
	asynStatus status = asynSuccess;
	char recvBuf[32];
	char sendDummy[32];
	char sendBuf[32];
	size_t nSendBytes;
	size_t nRecvBytes;
	int eomReason;
	char * pWin;

	// STX, ADDR, WIN, COM, DATA, ETX, CRC
	switch(channel)
	{
		case 0: pWin = (char*)I_CH1; break;
		case 1: pWin = (char*)I_CH2; break;
		case 2: pWin = (char*)I_CH3; break;
		case 3: pWin = (char*)I_CH4; break;
	}
	sprintf(sendDummy,"%c%c%s%c%c",STX,m_nDevAddr,pWin,COMM_READ,ETX);
	sprintf(sendBuf,"%s%s",sendDummy,getCRC(&sendDummy[1],&sendDummy[strlen(sendDummy)]));
	
	status = pasynOctetSyncIO->writeRead(m_pComm,
			sendBuf,
			strlen(sendBuf),
			recvBuf,
			19,
			m_dbTimeout,
			&nSendBytes,
			&nRecvBytes,
			&eomReason);
	
	if(status != asynSuccess)
		errlogPrintf("SendAndReceive error calling writeRead, output=%s, status=%d, error=%s\n",
				sendBuf,status,m_pComm->errorMessage);
	
	if(strncmp(&sendDummy[2],&recvBuf[2],3) != 0)
		return asynError;

	m_dbI[channel] = myAtoF(&recvBuf[6],10);
	setDoubleParam(P_4uhvCurrent[channel],m_dbI[channel]);

	return status;
}

asynStatus drvAgilent4uhv::getP(const int channel)
{
	asynStatus status = asynSuccess;
	char recvBuf[32];
	char sendDummy[32];
	char sendBuf[32];
	size_t nSendBytes;
	size_t nRecvBytes;
	int eomReason;
	char * pWin;

	// STX, ADDR, WIN, COM, DATA, ETX, CRC
	switch(channel)
	{
		case 0: pWin = (char*)P_CH1; break;
		case 1: pWin = (char*)P_CH2; break;
		case 2: pWin = (char*)P_CH3; break;
		case 3: pWin = (char*)P_CH4; break;
	}
	sprintf(sendDummy,"%c%c%s%c%c",STX,m_nDevAddr,pWin,COMM_READ,ETX);
	sprintf(sendBuf,"%s%s",sendDummy,getCRC(&sendDummy[1],&sendDummy[strlen(sendDummy)]));
	
	status = pasynOctetSyncIO->writeRead(m_pComm,
			sendBuf,
			strlen(sendBuf),
			recvBuf,
			19,
			m_dbTimeout,
			&nSendBytes,
			&nRecvBytes,
			&eomReason);
	
	if(status != asynSuccess)
		errlogPrintf("SendAndReceive error calling writeRead, output=%s, status=%d, error=%s\n",
						sendBuf,status,m_pComm->errorMessage);
	
	if(strncmp(&sendDummy[2],&recvBuf[2],3) != 0)
		return asynError;

	m_dbP[channel] = myAtoF(&recvBuf[6],10);
	setDoubleParam(P_4uhvPressure[channel],m_dbP[channel]);

	return status;
}

double drvAgilent4uhv::myAtoF(char *buf, size_t len)
{
	char str[32];
	memset(str,0x00,32);

	strncpy(str,buf,len);

//	epicsPrintf("%s -> %s -> %.2le\n",buf,str,atof(str));

	return atof(str);
}


////////////////////////////////////////////////////////////////////////////////
//// EPICS FUNC
//////////////////////////////////////////////////////////////////////////////////
extern "C"{
int drvAgilent4uhvConfigure(const char *myPortName,const char *commPortName, const int nDevAddr, const double dbTimeout, const int nDevIdx)
{
	drvAgilent4uhv *agilent4uhv = new drvAgilent4uhv(myPortName,commPortName,nDevAddr, dbTimeout,nDevIdx);
	agilent4uhv = NULL;
	return asynSuccess;
}

static const iocshArg drvAgilent4uhvConfigureArg0 = {"myPortName",iocshArgString};
static const iocshArg drvAgilent4uhvConfigureArg1 = {"commPortName",iocshArgString};
static const iocshArg drvAgilent4uhvConfigureArg2 = {"Device Address",iocshArgInt};
static const iocshArg drvAgilent4uhvConfigureArg3 = {"comm timeout",iocshArgDouble};
static const iocshArg drvAgilent4uhvConfigureArg4 = {"device index",iocshArgInt};

static const iocshArg * const drvAgilent4uhvConfigureArgs[] = {
	&drvAgilent4uhvConfigureArg0, 
	&drvAgilent4uhvConfigureArg1, 
	&drvAgilent4uhvConfigureArg2,
	&drvAgilent4uhvConfigureArg3,
	&drvAgilent4uhvConfigureArg4};

static const iocshFuncDef drvAgilent4uhvConfigureDef = {"drvAgilent4uhvConfigure",5,drvAgilent4uhvConfigureArgs};

static void drvAgilent4uhvConfigureCallFunc(const iocshArgBuf * args)
{
	drvAgilent4uhvConfigure(args[0].sval,args[1].sval,args[2].ival,args[3].dval,args[4].ival);
}

void drvAgilent4uhvConfigureRegister(void)
{
	iocshRegister(&drvAgilent4uhvConfigureDef,drvAgilent4uhvConfigureCallFunc);
}

epicsExportRegistrar(drvAgilent4uhvConfigureRegister);
} // extern "C"

