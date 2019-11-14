#ifndef DRVAGILENT4UHV_H
#define DRVAGILENT4UHV_H
#include <asynPortDriver.h>

#define P_4uhvIsAliveString			"AGILENT4UHV_ISALIVE"				/* asynInt32, r, 1:connected 0:disconnected */
#define P_4uhvHvCh1String			"AGILENT4UHV_HV_CH1"				/* asynInt32, w, 1:on, 0:off*/
#define P_4uhvHvCh1RbvString		"AGILENT4UHV_HV_CH1_RBV"			/* asynInt32, r, 1:on, 0:off*/
#define P_4uhvHvCh2String			"AGILENT4UHV_HV_CH2"				/* asynInt32, w, 1:on, 0:off */
#define P_4uhvHvCh2RbvString		"AGILENT4UHV_HV_CH2_RBV"			/* asynInt32, r, 1:on, 0:off */
#define P_4uhvHvCh3String			"AGILENT4UHV_HV_CH3"				/* asynInt32, w, 1:on, 0:off */
#define P_4uhvHvCh3RbvString		"AGILENT4UHV_HV_CH3_RBV"			/* asynInt32, r, 1:on, 0:off */
#define P_4uhvHvCh4String			"AGILENT4UHV_HV_CH4"				/* asynInt32, w, 1:on, 0:off */
#define P_4uhvHvCh4RbvString		"AGILENT4UHV_HV_CH4_RBV"			/* asynInt32, r, 1:on, 0:off */
#define P_4uhvVoltageCh1String		"AGILENT4UHV_V_CH1"					/* asynFloat64, r, */
#define P_4uhvCurrentCh1String		"AGILENT4UHV_I_CH1"					/* asynFloat64, r, */
#define P_4uhvPressureCh1String		"AGILENT4UHV_P_CH1"					/* asynFloat64, r, */
#define P_4uhvVoltageCh2String		"AGILENT4UHV_V_CH2"					/* asynFloat64, r, */
#define P_4uhvCurrentCh2String		"AGILENT4UHV_I_CH2"					/* asynFloat64, r, */
#define P_4uhvPressureCh2String		"AGILENT4UHV_P_CH2"					/* asynFloat64, r, */
#define P_4uhvVoltageCh3String		"AGILENT4UHV_V_CH3"					/* asynFloat64, r, */
#define P_4uhvCurrentCh3String		"AGILENT4UHV_I_CH3"					/* asynFloat64, r, */
#define P_4uhvPressureCh3String		"AGILENT4UHV_P_CH3"					/* asynFloat64, r, */
#define P_4uhvVoltageCh4String		"AGILENT4UHV_V_CH4"					/* asynFloat64, r, */
#define P_4uhvCurrentCh4String		"AGILENT4UHV_I_CH4"					/* asynFloat64, r, */
#define P_4uhvPressureCh4String		"AGILENT4UHV_P_CH4"					/* asynFloat64, r, */
#define P_4uhvCommDelayDiagString	"AGILENT4UHV_COMM_DELAY_DIAG"		/* asynFloat64, r, */	
#define P_4uhvCommDelayString		"AGILENT4UHV_COMM_DELAY"			/* asynFloat64, r/w, unit: second */

const char STX =        0x02;
const char COMM_READ =  0x30;
const char COMM_WRITE = 0x31;
const char ETX =   0x03;
const char ACK =   0x06;
const char NACK =  0x15;
const char NOWIN = 0x32;

/* ---------------------------------------------------------------------------------
 * command list 
 * 1: R/W
 * 2: Type
 *		Data Type			Field Length			Valid Characters
 *		Logic(L)			1						0:off, 1:on
 *		Numeric(N)			6						'-', '.', '0'...'9' right justified with '0'
 *		Alphanumeric(A)		10						From blank to '_' (ASCII)
 *		ACK					1						0x06, Execution of the command has been successful.
 *		NACK				1						0x15, Execution of the command has failed.
 *		Unknown Window		1						0x32, command is not valid
 *		Data Type Error		1						0x33,
 *		Out of Range		
 *		Win Disabled		1						The window specified is Read Only or is temporarily disabled.
 * ---------------------------------------------------------------------------------*/

/*			command name	window number							   R/W	Type	Description  */
const char *HV_ONOFF_CH1	= "011";								// R/W,	L
const char *HV_ONOFF_CH2	= "012";								// R/W, L
const char *HV_ONOFF_CH3	= "013";								// R/W, L
const char *HV_ONOFF_CH4	= "014";								// R/W, L
const char *STATUS			= "013";								// R,	N
const char *ERROR_CODE		= "206";								// R,	N
const char *V_CH1			= "810";								// R,	N		[0, 10000] V
const char *I_CH1			= "811";								// R,	A		[1E-10,9E-1] A
const char *P_CH1			= "812";								// R,	A		[X.XE-XX] barr
const char *V_CH2			= "820";								// R,	N		[0, 10000] V
const char *I_CH2			= "821";								// R,	A		[1E-10,9E-1] A
const char *P_CH2			= "822";								// R,	A		[X.XE-XX] barr
const char *V_CH3			= "830";								// R,	N		[0, 10000] V
const char *I_CH3			= "831";								// R,	A		[1E-10,9E-1] A
const char *P_CH3			= "832";								// R,	A		[X.XE-XX] barr
const char *V_CH4			= "840";								// R,	N		[0, 10000] V
const char *I_CH4			= "841";								// R,	A		[1E-10,9E-1] A
const char *P_CH4			= "842";								// R,	A		[X.XE-XX] barr


class drvAgilent4uhv : public asynPortDriver
{
	public:
		drvAgilent4uhv(const char *portName, const char *commPortName, const int nDevAddr, const double dbTimeout, const int nDevIdx);
		~drvAgilent4uhv();

		virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
		virtual asynStatus writeFloat64(asynUser *pasynUser, epicsFloat64 value);

		double  getCommDelay() const;
		void    setAlive(const int value);
		void    setCommDelayDiag(const double tDiff);

		char    *getCRC(char *beg, char *end);
		double  myAtoF(char *buf, size_t len);
		asynStatus getFeatures();
	protected:
		int P_4uhvIsAlive;
#define FIRST_AGILENT4UHV_PARAM P_4uhvIsAlive
		int	P_4uhvHv[4];
		int	P_4uhvHvRbv[4];
		int	P_4uhvVoltage[4];
		int	P_4uhvCurrent[4];
		int	P_4uhvPressure[4];
		int P_4uhvCommDelayDiag;	
		int P_4uhvCommDelay;
#define LAST_AGILENT4UHV_PARAM P_4uhvCommDelay
#define NUM_AGILENT4UHV_PARAMS (&LAST_AGILENT4UHV_PARAM - &FIRST_AGILENT4UHV_PARAM + 1)

	public:
		bool     m_bRun;
	private:
		int      m_nDevIdx;
		double   m_dbCommDelay;
		double   m_dbTimeout;
		asynUser *m_pComm;

		int      m_nDevAddr;
		
		int      m_isHvOn[4];
		double   m_dbV[4];
		double   m_dbI[4];
		double   m_dbP[4];

		static void threadFunc(drvAgilent4uhv *pParam);
		static void agilent4uhvShutdown(void *pParam);

		// comm.
		asynStatus setHv(const int channel, epicsInt32 val);
		asynStatus getHv(const int channel);
		asynStatus getV(const int channel);
		asynStatus getI(const int channel);
		asynStatus getP(const int channel);
		
};

#endif
