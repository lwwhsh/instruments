/*
 * temp_gpib device support
 */

#include <epicsStdio.h>
#include <devCommonGpib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <dbDefs.h>
#include <registryFunction.h>
#include <subRecord.h>
#include <epicsExport.h>

/******************************************************************************
 *
 * The following define statements are used to declare the names to be used
 * for the dset tables.   
 *
 * A DSET_AI entry must be declared here and referenced in an application
 * database description file even if the device provides no AI records.
 *
 ******************************************************************************/
#define DSET_AI     devAitemp_gpib
#define DSET_AO     devAotemp_gpib
#define DSET_BI     devBitemp_gpib
#define DSET_BO     devBotemp_gpib
#define DSET_EV     devEvtemp_gpib
#define DSET_LI     devLitemp_gpib
#define DSET_LO     devLotemp_gpib
#define DSET_MBBI   devMbbitemp_gpib
#define DSET_MBBID  devMbbidtemp_gpib
#define DSET_MBBO   devMbbotemp_gpib
#define DSET_MBBOD  devMbbodtemp_gpib
#define DSET_SI     devSitemp_gpib
#define DSET_SO     devSotemp_gpib
#define DSET_WF     devWftemp_gpib

#include <devGpib.h> /* must be included after DSET defines */

#define TIMEOUT     1.0    /* I/O must complete within this time */
#define TIMEWINDOW  1.0    /* Wait this long after device timeout */

/******************************************************************************
 * Array of structures that define all GPIB messages
 * supported for this type of instrument.
 ******************************************************************************/

static int getTemp();
static int getSetPoint();
static int TripSetPoint();
static int SetRemLoc();

static struct gpibCmd gpibCmds[] = {
    /* Param 0 Temperature READ */
    {&DSET_AI,GPIBCVTIO,IB_Q_LOW, NULL,NULL,0,25, getTemp, 0, 0, NULL, NULL, "\r"},
    /* Param 1 Current SET POINT READ */
    {&DSET_AI,GPIBCVTIO,IB_Q_LOW, NULL,NULL,0,25, getSetPoint, 0, 0, NULL, NULL, "\r"},
    /* Param 2 Trip Point SET */
    {&DSET_AO,GPIBCVTIO,IB_Q_LOW, NULL,NULL,25,25, TripSetPoint, 0, 0, NULL, NULL, "\r"},
    /* Param 3 Set REM/LOC: 0=REM */
    {&DSET_BO,GPIBCVTIO,IB_Q_LOW, NULL,NULL,25,25, SetRemLoc, 0, 0, NULL, NULL, "\r"}
};

/* The following is the number of elements in the command array above.  */
#define NUMPARAMS sizeof(gpibCmds)/sizeof(struct gpibCmd)

/******************************************************************************
 * Initialize device support parameters
 *
 *****************************************************************************/
static long init_ai(int parm)
{
    if(parm==0) {
        devSupParms.name = "devtemp_gpib";
        devSupParms.gpibCmds = gpibCmds;
        devSupParms.numparams = NUMPARAMS;
        devSupParms.timeout = TIMEOUT;
        devSupParms.timeWindow = TIMEWINDOW;
        devSupParms.respond2Writes = -1;
    }
    return(0);
}

#define STX  0x02
#define ETX  0x03
#define BCC  0x40
#define NBCC 0x03

#define WEOL
#define REOL NULL
#define REOLLEN 0



/****************************************************************************
**** subRecord definition using E5GN Status Monitoring           ************
*****************************************************************************/
int temp_gpibSubDebug=0;

static unsigned char stsE5GN[3];

typedef long (*processMethod)(subRecord *precord);

static long temp_gpibSubInit(subRecord *precord,processMethod process)
{
    if (temp_gpibSubDebug)
        printf("Record %s called mySubInit(%p, %p)\n",
               precord->name, (void*) precord, (void*) process);
    return(0);
}

static long temp_gpibSubProcess(subRecord *precord)
{
    if (temp_gpibSubDebug)
        printf("Record %s called mySubProcess(%p)\n",
               precord->name, (void*) precord);

    precord->a = stsE5GN[2];  /* E5GN Temperature Sensor Status   : 0=Okay, 1= Sensor Fault */
    precord->b = stsE5GN[1];  /* E5GN ALARM Status                : 0=Okay, 1= Fault(Temp. to HIGH) */
    precord->c = stsE5GN[0];  /* E5GN REMOTE Enable/Disable Status: 0=LOCAL, 1=REMOTE */

    return(0);
}


static int
getTemp(struct gpibDpvt *pdpvt, int P1, int P2, char **P3)
{
    struct aiRecord *pai = ((struct aiRecord *)(pdpvt->precord));

    asynOctet *pasynOctet = pdpvt->pasynOctet;
    void *asynOctetPvt = pdpvt->asynOctetPvt;

    int max;
    char* temp, addrbuf[5];
    static char temp1[30], stsTemp[5];
    char cbuf[30];
	long tempValue;
	static unsigned char t_bcc;
	int i;

    /* Make Address */
    sprintf(addrbuf, "%02d", pai->inp.value.gpibio.addr);

    sprintf(temp1, "@%02sRX01", addrbuf);

    /* Make FCS */
	t_bcc = (unsigned char)temp1[0];
	for(i=1; i < 7; i++)
		t_bcc = (unsigned char)t_bcc ^ (unsigned char)temp1[i];

    sprintf(temp1, "@%02sRX01%x*\0", addrbuf, t_bcc);

    /* Make upper char for FCS */    
    for (i=0; i<9; i++)
    {
       temp1[i] = toupper(temp1[i]);
    }

/*
    printf("===== SEND COMMAND : %s ====\n", temp1);
*/
    printf("******it's write *******\n");

    if (pasynOctet->write(asynOctetPvt,pdpvt->pasynUser,temp1, strlen(temp1),&max) != asynSuccess)
    {
        return -1;
    }
    printf("******it's read *******\n");

    if(pasynOctet->read(asynOctetPvt,pdpvt->pasynUser,cbuf,19,&max,NULL) != asynSuccess)
	{
        printf("----- E5GN Rcv String Error: %s ----\n", cbuf);
        return -1;
	}
    printf("******cbuf******* %s *******\n", cbuf);

	/* sprintf(temp,"%c%c%c%c\0", cbuf[7],cbuf[8],cbuf[9],cbuf[10]);  */
	sprintf(temp,"%c%c%c\0", cbuf[8],cbuf[9],cbuf[10]);

	tempValue = atol(temp); /* Current Temp. */

        /* ADD 2010.06.10 DCM Cryo-cooler temperatures to lessthen zero */
        if(cbuf[7] == 'F') tempValue *= -1;
        if(cbuf[7] == 'A') tempValue = -1000 - tempValue;

    printf("******temp******* %d *******\n", tempValue);


    /************  Status Checking ***********************/
    static unsigned char j =0;
    static unsigned char tempStsData;
 
    stsTemp[0] = cbuf[11]; /* REMOTE/LOCAL STATUS */
    stsTemp[1] = '\0';
    sscanf(stsTemp, "%X", &j);
    if ((j & 0x08) > 0) 
       stsE5GN[2] = 1;
       else stsE5GN[2] = 0;

    stsTemp[0] = cbuf[12]; /* ALARM STATUS */
    stsTemp[1] = '\0';
    sscanf(stsTemp, "%X", &j);
    if ((j & 0x02) > 0) 
       stsE5GN[1] = 1;
       else stsE5GN[1] = 0;

    stsTemp[0] = cbuf[14]; /* SENSOR FAULT STATUS */
    stsTemp[1] = '\0';
    sscanf(stsTemp, "%X", &j);
    if ((j & 0x04) > 0) 
       stsE5GN[0] = 1;
       else stsE5GN[0] = 0;

	pai->val = (float)(tempValue * 0.1);
    pai->udf = 0;
    return 0;
}

/************************/
/* READ SET POINT VALUE */
/************************/
static int
getSetPoint(struct gpibDpvt *pdpvt, int P1, int P2, char **P3)
{
    struct aiRecord *pai = ((struct aiRecord *)(pdpvt->precord));

    asynOctet *pasynOctet = pdpvt->pasynOctet;
    void *asynOctetPvt = pdpvt->asynOctetPvt;

    int max;
    char* temp, addrbuf[5];
    static char temp1[30], stsTemp[5];
    char cbuf[30];
    long tempValue;
    static unsigned char t_bcc;
    int i;

    /* Make Address */
    sprintf(addrbuf, "%02d", pai->inp.value.gpibio.addr);

    sprintf(temp1, "@%02sRS01", addrbuf);

    /* Make FCS */
    t_bcc = (unsigned char)temp1[0];
    for(i=1; i < 7; i++)
        t_bcc = (unsigned char)t_bcc ^ (unsigned char)temp1[i];

    sprintf(temp1, "@%02sRS01%x*\0", addrbuf, t_bcc);

    /* Make upper char for FCS */
    for (i=0; i<9; i++)
    {
       temp1[i] = toupper(temp1[i]);
    }

/*
    printf("===== SEND COMMAND : %s ====\n", temp1);
*/
/*
    if ((pasynOctet->setInputEos(asynOctetPvt,pdpvt->pasynUser,REOL,REOLLEN) != asynSuccess))
        return -1;
    if ((pasynOctet->setOutputEos(asynOctetPvt,pdpvt->pasynUser,REOL,REOLLEN) != asynSuccess))
        return -1;
*/

    if (pasynOctet->write(asynOctetPvt,pdpvt->pasynUser,temp1, strlen(temp1),&max) != asynSuccess)
    {
        return -1;
    }

    if(pasynOctet->read(asynOctetPvt,pdpvt->pasynUser,cbuf,15,&max,NULL) != asynSuccess)
    {
        printf("----- E5GN Rcv String Error in SET POINT READ Value: %s ----\n", cbuf);
        return -1;
    }

    sprintf(temp,"%c%c%c%c\0", cbuf[7],cbuf[8],cbuf[9],cbuf[10]);

    tempValue = atol(temp); /* Current Set Point. */

    pai->val = (float)(tempValue * 0.1);
    pai->udf = 0;
    return 0;
}

/*************************/
/* WRITE SET POINT VALUE */
/*************************/
static int
TripSetPoint(struct gpibDpvt *pdpvt, int P1, int P2, char **P3)
{
    struct aoRecord *pao = ((struct aoRecord *)(pdpvt->precord));

    asynOctet *pasynOctet = pdpvt->pasynOctet;
    void *asynOctetPvt = pdpvt->asynOctetPvt;

    int max;
    char temp[5], addrbuf[5];
    static char temp1[30];
    char cbuf[30];
    static int tempValue;
    static unsigned char t_bcc;
    int i;

    /* Make Address */
    sprintf(addrbuf, "%02d", pao->out.value.gpibio.addr);

    tempValue = (int)(pao->val * 10.0);

    itoa(tempValue, temp, 10);
    
    /* Make Value */
    sprintf(temp1, "@%02sWS01%04s", addrbuf, temp);

    for(i=0; i<11; i++)
    {
        if (isspace(temp1[i])) temp1[i] = 0x30;
    }

    /* Make FCS */
    t_bcc = (unsigned char)temp1[0];
    for(i=1; i < 11; i++)
        t_bcc = (unsigned char)t_bcc ^ (unsigned char)temp1[i];

    sprintf(temp1, "@%02sWS01%04s%x*\0", addrbuf, temp, t_bcc);

    for(i=0; i<13; i++)
    {
       if (isspace(temp1[i])) temp1[i] = 0x30;
       temp1[i] = toupper(temp1[i]);
    }

/*
    if ((pasynOctet->setInputEos(asynOctetPvt,pdpvt->pasynUser,REOL,REOLLEN) != asynSuccess))
        return -1;
    if ((pasynOctet->setOutputEos(asynOctetPvt,pdpvt->pasynUser,REOL,REOLLEN) != asynSuccess))
        return -1;
*/

    if (pasynOctet->write(asynOctetPvt,pdpvt->pasynUser,temp1, strlen(temp1),&max) != asynSuccess)
    {
        printf("===== Errir SEND COMMAND Set Trip Point : %s ====\n", temp1);
        return -1;
    }

    if(pasynOctet->read(asynOctetPvt,pdpvt->pasynUser,cbuf,11,&max,NULL) != asynSuccess)
    {
        printf("----- E5GN Rcv String Error in SET POINT WRITE Value: %s ----\n", cbuf);
        return -1;
    }

    return 0;
}

/***************************************************************************************
 * SET REMOTE / LOCAL
****************************************************************************************/
static int
SetRemLoc(struct gpibDpvt *pdpvt, int P1, int P2, char **P3)
{
    struct boRecord *pbo = ((struct boRecord *)(pdpvt->precord));

    asynOctet *pasynOctet = pdpvt->pasynOctet;
    void *asynOctetPvt = pdpvt->asynOctetPvt;

    unsigned char temp1[20], temp[20];
    static unsigned char cbuf[50], addrbuf[4], t_bcc;
    int max, i;

    temp1[0] = NULL;
    cbuf[0] = NULL;

    /* Make Address */
    sprintf(addrbuf, "%02d", pbo->out.value.gpibio.addr);
    /* Make Value */
    sprintf(temp, "%04d", pbo->val);
    
    sprintf(temp1, "@%02sMB01%04s", addrbuf, temp);

    /* Make FCS */
    t_bcc = (unsigned char)temp1[0];
    for(i=1; i < 11; i++)
        t_bcc = (unsigned char)t_bcc ^ (unsigned char)temp1[i];

    sprintf(temp1, "@%02sMB01%04s%x*\0", addrbuf, temp, t_bcc);

    for(i=0; i<13; i++)
    {
       if (isspace(temp1[i])) temp1[i] = 0x30;
       temp1[i] = toupper(temp1[i]);
    }


/*
    if ((pasynOctet->setInputEos(asynOctetPvt,pdpvt->pasynUser,REOL,REOLLEN) != asynSuccess))
        return -1;

    if ((pasynOctet->setOutputEos(asynOctetPvt,pdpvt->pasynUser,REOL,REOLLEN) != asynSuccess))
        return -1;
*/
    if (pasynOctet->write(asynOctetPvt,pdpvt->pasynUser,temp1, strlen(temp1), &max) != asynSuccess)
    {
        printf("=====  E5GN Set to REM/LOC SEND COMMAND Error: %s ====\n", temp1);
        return -1;
    }

    if(pasynOctet->read(asynOctetPvt,pdpvt->pasynUser,cbuf, 11, &max,NULL) != asynSuccess)
    {
        return -1;
    }

    return 0;
}


/**
 * Ansi C "itoa" based on Kernighan & Ritchie's "Ansi C"
 * with slight modification to optimize for specific architecture:
 */
void strreverse(char* begin, char* end) {
	char aux;
	
	while(end>begin)
		aux=*end, *end--=*begin, *begin++=aux;
}
	
void itoa(int value, char* str, int base) {
	
	static char num[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	char* wstr=str;
	int sign;
	div_t res;

	/* Validate base */
	if (base<2 || base>35){ *wstr='\0'; return; }

	/* Take care of sign */	
	if ((sign=value) < 0) value = -value;
	
	/* Conversion. Number is reversed. */
	do {
		res = div(value,base);
		*wstr++ = num[res.rem];
	}while(value=res.quot);
	
	if(sign<0) *wstr++='-';
	*wstr='\0';

	/* Reverse string */
	strreverse(str,wstr-1);
}

epicsExportAddress(int, temp_gpibSubDebug);
epicsRegisterFunction(temp_gpibSubInit);
epicsRegisterFunction(temp_gpibSubProcess);

