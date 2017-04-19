// PulserLib.c - The pulse programmer functions
//
// Revision
// 120711	Remove PCI function, assume always 0	C.W.
// 120711	Remove 	UnloadPhyMemDriver();
// 120711	Add viOpen() in loops
// 120711	Add instr in typedef struct
// 120711	Replace LoadPhyMemDriver() with viOpenDefaultRM(&defaultRM)	C.W.
// 120711	Replace ReadPCI() with viIn32()
// 120711	Replace MapPhyMem() with viMapAddress()
// 120711	Replace UnmapPhyMem() with viUnmapAddress()
// 130520	Comment out chi init in mw3init
// 130520	Use SPI to EN instead of CEN

#define PULSERDLL_EXPORTS
#include "PulserDefs.h"
//#include "PulserLib.h"
//#include "PulserDefs.h"

#ifdef PULSER_SIM
static DDSSim DDSmemory[MAXSLOTS];
#endif

static char PulserRoot[MAX_PATH + 1];
static char PulserBin[MAX_PATH + 1];
static PULSERChannelInfo PulserChannel[MAXSLOTS][MAX_CHANNELS_PER_SLOT];
static unsigned NumSlotsPresent;
static FILE *pLogFile;

static	ViSession defaultRM;	//Added 120711

//-------------------------------------------------------------------
//pulserInit - initializes the Pulser environment
//		Returns number of pulser slots present.
//-------------------------------------------------------------------
int pulserInit(void)
{
	int iSlot, ch, bus, dev, func;
	void *virAddr = 0;
//	ULONG dev_ven, devId, venId, *bar0, pxiSlot;
	ULONG dev_ven, devId, venId, bar0, pxiSlot;
	ULONG *cardStatusRegAddr, geoAddr;
	PULSERChannelInfo *chInfo;

	// Open the log file.
	fopen_s(&pLogFile, "PulserLibLog.txt", "w+");
	fwrite("PulserLib Log: ...............\r\n", 1, strlen("PulserLib Log: ...............\r\n"), pLogFile);
	fflush(pLogFile);

#ifndef PULSER_SIM
//	if(!LoadPhyMemDriver()) {
	fwrite("viOpenDefaultRM\r\n", 1, strlen("viOpenDefaultRM\r\n"), pLogFile);
	fflush(pLogFile);
	if (viOpenDefaultRM(&defaultRM) < VI_SUCCESS) {	// Added
		printf("Loading VISA driver failed.\r\n>>");// Modified
//		printf("Loading memory driver failed.\r\n>>");
		fwrite("viOpenDefaultRM failed\r\n", 1, strlen("viOpenDefaultRM failed\r\n"), pLogFile);
		fclose (pLogFile);
		return(-1);
	}
#endif
	// Set PulserRoot and PulserBin directory paths.
	//
	//		PulserRoot
	//		    |
	//			+--- Initial Current Working Directory
	//			|
	//			+--- PulserBin
	//			|
	//			+--- channels
	//			.		|
	//			.		+--- 2-0
	//			.		+--- 2-1
	//			.		......
	//

	GetCurrentDirectory(MAX_PATH - 4, (LPTSTR)PulserRoot);
	strcat_s(PulserRoot, MAX_PATH, "\\..\\");
	SetCurrentDirectory((LPCTSTR)PulserRoot);
	GetCurrentDirectory(MAX_PATH, (LPTSTR)PulserRoot);
	strcat_s(PulserRoot, MAX_PATH, "\\");
	printf("PulserRoot=%s\n", PulserRoot);
	strcat_s(PulserBin, MAX_PATH, PulserRoot);
	strcat_s(PulserBin, MAX_PATH, "bin\\");

	// Initialize the PULSERChannelInfo structure
	NumSlotsPresent = 0;
	for (iSlot = 0; iSlot < MAXSLOTS; iSlot++) {
		for (ch = 0; ch < MW3_MAX_CHANNELS; ch++) {
			PulserChannel[iSlot][ch].slot = 0;	// zero means empty slot
			PulserChannel[iSlot][ch].pciInfo.devId = 0xff;
		}
	}

	// Scan the PCI buses for the vendor and device ID
	//		VID_XILINX (0x10ee)
	//		DID_DDS (0x6014)
	for (bus=0; bus<=255; bus++) {
//		for (dev=0; dev<=31; dev++) {
//			for (func=0; func <=7; func++) {

#ifdef PULSER_SIM
				if (((bus == 7) || (bus == 12)) && (dev == 0) && (func == 0))
					dev_ven = VID_XILINX | (DID_MW3 << 16); 
				else
					continue;
#else
				// Try to read the device/vendor ID
				char chResourceName[80];
				ViSession instr;

				dev = 0;
				func = 0;
				sprintf_s (chResourceName, 80, "PXI%d::%d::INSTR\0", bus, dev);

				if (viOpen(defaultRM, chResourceName, VI_NULL, VI_NULL, &instr) != VI_SUCCESS)
					continue;
				fprintf(pLogFile, "\n%s;", chResourceName);
				fflush(pLogFile);
				
//				if (!ReadPCI(bus, dev, 0, 0, 4, &dev_ven))
				if (viIn32(instr, VI_PXI_CFG_SPACE, 0, &dev_ven) != VI_SUCCESS)
					continue;
				fprintf(pLogFile, "ID%08X - ", dev_ven);
				fflush(pLogFile);

#endif
				// Can read the config registers - check the vendor/dev ID
				devId = HIWORD(dev_ven);
				venId = LOWORD(dev_ven);
//				if ((venId != VID_XILINX) || ((devId != DID_DDS) && (devId != DID_MW3) && (devId != DID_MWVECTOR)))
				if ((venId != VID_XILINX) || (devId != DID_MW3))
					continue;
				fprintf(pLogFile, "Match - ");
				fflush(pLogFile);

#ifndef PULSER_SIM
				// Have a board. Read bar0.
//				ReadPCI(bus, dev, func, 0x10, 4, &bar0);
				viIn32(instr, VI_PXI_CFG_SPACE, 0x10, &bar0);
				fprintf(pLogFile, "BAR0:%08X - ", bar0);
				fflush(pLogFile);

#else
				bar0 = (ULONG *)0xc0000000;
#endif
				// check if bar0 is memory type
				if ((bar0 == 0) || ((ULONG)bar0 & 0x7) != 0) {
//					printf("Bad bar0 0x%x on %d.%d.%d\r\n", bar0, bus, dev, func);
					continue;
				}
				fprintf(pLogFile, "MemoryType - ");
				fflush(pLogFile);

				// Map the interface memory to the host virtual address
#ifndef PULSER_SIM
//				virAddr = MapPhyMem((ULONG)bar0, DDS_PHYSMEM_SIZE4 * 4);			
//				if (virAddr == NULL) {
//				if (viMapAddress(instr, VI_PXI_BAR0_SPACE, 0, DDS_PHYSMEM_SIZE4*4, VI_FALSE, VI_NULL, &virAddr)  != VI_SUCCESS) {
				if (viMapAddress(instr, VI_PXI_BAR0_SPACE, 0, MW3_PHYSMEM_SIZE4*4, VI_FALSE, VI_NULL, &virAddr)  != VI_SUCCESS) {
					printf("Bad address mapping for %d.%d.%d\r\n", bus, dev, func);
					fprintf(pLogFile, "viMapAddress Failed - ");
					fflush(pLogFile);
					continue;
				}
				fprintf(pLogFile, "virAddr:%08X - ", virAddr);
				fflush(pLogFile);
#else
				virAddr = (void *)&DDSmemory[bus - 1];
#endif

				// Get the slot number from the card status register of the board
				pxiSlot = 0;
				//if ((devId & 0xfff0) == (DID_MW3 & 0xfff0)) {
					cardStatusRegAddr = (ULONG *)((char *)virAddr + DDS_INTF_CARD_STATUS_REG_ADDR4 * 4);

					geoAddr = ((*cardStatusRegAddr) >> 1) & 0x1f;
#ifdef PULSER_SIM
					geoAddr = bus;
#endif
					if ((geoAddr > MAXSLOTS) || (geoAddr < 1)) {
//						UnmapPhyMem(virAddr, DDS_PHYSMEM_SIZE4 * 4);
						viUnmapAddress(instr);
						printf("Bad slot %u on %d.%d.%d\r\n", geoAddr, bus, dev, func);
						continue;
					} 
					else 
						pxiSlot = geoAddr;

				//
				// Found a slot. Fill in the channel info structure.
				NumSlotsPresent++;
				iSlot = pxiSlot - 1;	// slot index
				for (ch = 0; ch < MW3_MAX_CHANNELS; ch++) {
					chInfo = _pulserGetChIntf((USHORT)pxiSlot, ch);
					chInfo->instr = instr;								// Added
					chInfo->slot = (USHORT)pxiSlot;
					chInfo->ch = ch;
					chInfo->pciInfo.pciBusNo = bus;
					chInfo->pciInfo.pciDevNo = dev;
					chInfo->pciInfo.pciFuncNo = func;
					chInfo->pciInfo.vendorId = (USHORT)venId;
					chInfo->pciInfo.devId = (USHORT)devId;
					chInfo->pciInfo.barZero = bar0;
					strncpy_s(chInfo->pciInfo.vendorName, PULSER_NAME_SIZE, STR_XILINX, strlen(STR_XILINX)); 
					strncpy_s(chInfo->pciInfo.deviceName, PULSER_NAME_SIZE, STR_DDS, strlen(STR_DDS)); 
					chInfo->progInfo.nEntry = 0;	// No sequencer program loaded yet
					chInfo->progInfo.selectedEntry = -1;
					chInfo->progInfo.length4 = 0;
					chInfo->wavInfo.nEntry = 0;		// No waveform loaded yet.
					chInfo->wavInfo.selectedEntry = -1;
					chInfo->wavInfo.length4 = 0;
					chInfo->cardBaseVirtualAddr = (ULONG *)virAddr;
					chInfo->cardCmdReg = chInfo->cardBaseVirtualAddr + DDS_INTF_CARD_CMD_REG_ADDR4;
					chInfo->cardStatusReg = chInfo->cardBaseVirtualAddr + DDS_INTF_CARD_STATUS_REG_ADDR4;
					chInfo->fromHostBuff = chInfo->cardBaseVirtualAddr + DDS_INTF_CARD_FROMHOST_BUFF_ADDR4;
					chInfo->toHostBuff = chInfo->cardBaseVirtualAddr + DDS_INTF_CARD_TOHOST_BUFF_ADDR4;
					chInfo->chCmdReg = chInfo->cardBaseVirtualAddr + DDS_INTF_CH0_CMD_REG_ADDR4;
					chInfo->chAddr4Reg = chInfo->chCmdReg + 1;
					chInfo->chLen4Reg = chInfo->chCmdReg + 2;
					chInfo->chStatusReg = chInfo->cardBaseVirtualAddr + DDS_INTF_CH0_STATUS_REG_ADDR4;
					chInfo->chIntrCode = chInfo->chStatusReg + 1;
					chInfo->fwVers = *(chInfo->cardBaseVirtualAddr + DDS_INTF_CARD_FWVERS_ADDR4) & 0x0fff;
					// MW3 section
					chInfo->cardSpiWriteData = chInfo->cardBaseVirtualAddr + MW3_INTF_CARD_SPI_WRITE_DATA;
					chInfo->cardSpiReadData = chInfo->cardBaseVirtualAddr + MW3_INTF_CARD_SPI_READ_DATA;
					chInfo->refClockSource = ClockInternal;
					chInfo->refClockFreq = MW3_CLK_INT_MHZ;
					chInfo->refDivider = 1;
					//pulserPrintChInfo(chInfo);
				} //-- for(ch)
//			} //-- for-func
//		} //-- for-dev
	} //-- for-bus
	

	fprintf(pLogFile, "\nPCI Bus Scan Completed\n");
	fflush(pLogFile);

	if (NumSlotsPresent == 0) {
		printf("No Pulser board found.\r\n>>");
		fprintf(pLogFile, "No Pulser board found.\n");
		fflush(pLogFile);
	}
	else{
//	if (NumSlotsPresent == 0) {
//		printf("No Pulser board found.\r\n>>");
		fprintf(pLogFile, "found %d board(s).\n", NumSlotsPresent);
		fflush(pLogFile);
//	}
	}
	// Build waveform and program .bin files and download them.
	//(void)pulserBuildAll();
	//(void)pulserDownloadAll();
	return NumSlotsPresent;
} //--pulserInit()

//-------------------------------------------------------------------
// pulserShutdown() - Releases all resources for program exit
//-------------------------------------------------------------------
void pulserShutdown(void)
{
	USHORT slot, ch;

	for (slot = 1; slot < MAXSLOTS; slot++) {
		for (ch = 0; ch < MW3_MAX_CHANNELS; ch++) {
			if (PulserChannel[slot-1][ch].slot == 0)
				continue;	// unequipped channel
			pulserStop(slot, ch);
			// Release virtual memory
		}
		if (PulserChannel[slot-1][0].cardBaseVirtualAddr != NULL)
//			UnmapPhyMem(PulserChannel[slot-1][0].cardBaseVirtualAddr, DDS_PHYSMEM_SIZE4 * 4);
			viUnmapAddress(PulserChannel[slot-1][0].instr);

	}
//viOpdsdfasdfdfffasdfasdfsdf	UnloadPhyMemDriver();
}

//------------------------------------------------------------
// void pulserStop()
//------------------------------------------------------------
void pulserStop(USHORT slot, USHORT channel)
{
	CardType type = pulserGetCardType(slot);
	if (type == Mw3Type)
		mw3ShutDown(slot);
	else if (type == DdsType)
		_pulserDoCommand(slot, channel, DDS_CMD_RESET, 0, 0);
	else
		;
}

//---------------------------------------------------------------------
// pulserGetCardType() - returns installed card type for the slot
//---------------------------------------------------------------------
CardType pulserGetCardType(USHORT slot)
{
	PULSERChannelInfo *chInfo;
	USHORT devtype;
	chInfo = _pulserGetChIntf(slot, 0);
	if ((chInfo == NULL) || (chInfo->slot != slot))
		return UnknownType;
	devtype = chInfo->pciInfo.devId;
	switch(devtype) {
	case DID_DDS:	return DdsType;
	case DID_MW3:	return Mw3Type;
	case DID_MWVECTOR:	return MwVectorType;
	default:	return UnknownType;
	}
}

//---------------------------------------------------------------------
// pulserGetCardName() - returns card name string
//---------------------------------------------------------------------
char *pulserGetCardName(CardType type)
{
	switch(type) {
	case DdsType:		return STR_DDS;
	case Mw3Type:		return STR_MW3;
	case MwVectorType:	return STR_MWVECTOR;
	default:			return STR_UNKNOWN;
	}
}

//------------------------------------------------------------
ULONG pulserGetFwVersion(USHORT slot)
{
	PULSERChannelInfo *chInfo;
	chInfo = _pulserGetChIntf(slot, 0);
	return chInfo->fwVers;
}

//-------------------------------------------------------
//BOOL pulserGetPulserRootDir(char *pathbuff) {}
//BOOL pulserGetChannelRootDir(char *pathbuff) {}
//BOOL pulserGetBinDir(char *pathbuff) {}

//--------------------------------------------------------
// pulserGotoChDir() - Go to the channel directory.
//--------------------------------------------------------
BOOL pulserGotoChDir(USHORT slot, USHORT ch)
{
	char channelDir[MAX_PATH + 1];
	char buff[MAX_PATH + 1];

	memset(channelDir, '\0', MAX_PATH + 1);
	memset(buff, '\0', MAX_PATH + 1);
	// construct "PulserRoot\channels\sXcY\" string.
	strncpy_s(channelDir, MAX_PATH, PulserRoot, strlen(PulserRoot));
	strncat_s(channelDir, MAX_PATH, "channels\\", strlen("channels\\"));
	sprintf_s(buff, MAX_PATH, "%d-%d\\", slot, ch);
	strncat_s(channelDir, MAX_PATH, buff, MAX_PATH);
	// Go to the channel directory.
	if (SetCurrentDirectory((LPCTSTR)channelDir) == 0) {
		unsigned err = GetLastError();
		printf("SetCurrentDirectory(%s) failed (%d).\r\n", channelDir, err);
		return FALSE;
	}
	return TRUE;
} //--pulserGotoChDir()

//----------------------------------------------------------
// pulserBuild() - Build pulse programs for a channel.
//		Input: .asm and .wfm files
//		Output: seq.bin
//				wfm.bin
//				seq.tab
//				wfm.tab
//		Returns true if success, otherwise returns false.
//----------------------------------------------------------

BOOL pulserBuild(USHORT slot, USHORT ch)
{
	char currDir[MAX_PATH + 1], buff[MAX_PATH + 1];
	unsigned islot = slot - 1;
	HANDLE hChildStd_OUT_Rd = NULL;
	HANDLE hChildStd_OUT_Wr = NULL;
	HANDLE hInputFile = NULL;
	char rdbuff[FILEBUFSIZE];
	ULONG exitCode = 0;
	SECURITY_ATTRIBUTES saAttr;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	FILE *pAsmOut;
	BOOL bSuccess;
	ULONG nread = 0;

	// Save current directory and go to Channels directory
	if (GetCurrentDirectory(MAX_PATH, (LPTSTR)currDir) == 0) {
		printf("GetCurrentDirectory failed.\r\n");
		//return -1;
	}
	if (PulserChannel[islot][ch].slot == 0) {
		printf("Bad slot: islot=%d, ch=%d\n", islot, ch);
		return FALSE;	// unequipped channel
	}
	// Have an equipped channel: go to the channel directory
	if (!pulserGotoChDir(slot, ch)) {
		printf("pulserGotoChDir(%d, %d) failed\r\n", slot, ch);
		return FALSE;
	}

	// Check if the .bin files are up to date.
	// if (pulserUpToDate(islot + 1, ich))
	//		continue;
	// Assemble using MQAsm.exe if not up to date.
	printf("Building [%d-%d]\r\n", slot, ch);

	sprintf_s(buff, MAX_PATH, "%s%s", PulserBin, "MQAsm.exe");

	// Set Sec. Attr. to inherit the pipe handles.
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;
	// Create a pipe for the assembler's STDOUT
	if ( ! CreatePipe(&hChildStd_OUT_Rd, &hChildStd_OUT_Wr,&saAttr, 0) )
		return -1;
	// Ensure the read handle to the pipe for STDOUT is not inherited.
	if ( ! SetHandleInformation(hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0) )
		return -1; 

	// Create the assembler process
	ZeroMemory( &pi, sizeof(PROCESS_INFORMATION) );
	ZeroMemory( &si, sizeof(STARTUPINFO) );
	si.cb = sizeof(STARTUPINFO);
	si.hStdError = hChildStd_OUT_Wr;
	si.hStdOutput = hChildStd_OUT_Wr;
	//si.hStdInput = hChildStd_IN_Rd;
	si.dwFlags |= STARTF_USESTDHANDLES;
	if( !CreateProcess( buff,   // No module name (use command line)
		NULL,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		TRUE,          // Set handle inheritance to FALSE
		CREATE_NO_WINDOW,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi )           // Pointer to PROCESS_INFORMATION structure
		) 
	{
		printf( "CreateProcess failed (%d).\r\n", GetLastError() );
		return FALSE;
	}
	// Read assembler's output and write to asmout.txt.
	fopen_s(&pAsmOut, "asmout.txt", "w");
	if ( !CloseHandle(hChildStd_OUT_Wr) )	// Close the write end.
		return -1;
	bSuccess = FALSE;
	for (;;) {
		bSuccess = ReadFile(hChildStd_OUT_Rd, rdbuff, FILEBUFSIZE, &nread, NULL);
		if ( !bSuccess || (nread == 0))
			break;
		fwrite(rdbuff, 1, nread, pAsmOut);
	}

	// Wait until child process exits.
	WaitForSingleObject( pi.hProcess, INFINITE );
	GetExitCodeProcess(pi.hProcess, &exitCode);
	if (exitCode == 0)
		sprintf_s(rdbuff, sizeof(rdbuff), "Build FAILED.\r\n");
	else
		sprintf_s(rdbuff, sizeof(rdbuff), "Build OK.\r\n");
	fputs(rdbuff, pAsmOut);
	// Close process and thread handles. 
	fclose(pAsmOut);
	CloseHandle(hChildStd_OUT_Rd);
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );
	SetCurrentDirectory((LPTSTR)currDir);
	return (exitCode == 0)? TRUE : FALSE;
} //--pulserBuild()

//----------------------------------------------------------
// pulserDownload() - Downloads both waveform and sequencer 
//					  .bin files for a channel.
//----------------------------------------------------------
BOOL pulserDownload(USHORT slot, USHORT channel)
{
	if (_pulserDownloadBinfile(slot, channel, 0, WfmType))
		if (_pulserDownloadBinfile(slot, channel, 0, SeqType))
			return TRUE;
	return FALSE;
}

//----------------------------------------------------------
// pulserBuildAll() - Build all channels
//----------------------------------------------------------
BOOL pulserBuildAll(void)
{
	char currDir[MAX_PATH + 1];
	unsigned slot, ch;

	// Save current directory and go to Channels directory
	if (GetCurrentDirectory(MAX_PATH + 1, (LPTSTR)currDir) == 0) {
		printf("GetCurrentDirectory failed.\r\n");
		//return -1;
	}

	// Go over all channels and do the work.
	for (slot = 2; slot <= MAXSLOTS; slot++) {
		if (pulserGetCardType(slot) != DdsType)
			continue;
		for (ch = 0; ch < DDS_MAX_CHANNELS; ch++) {
			if (! pulserBuild(slot, ch)) {
				SetCurrentDirectory((LPTSTR)currDir);
				return -1;
			}
		}
	}
	SetCurrentDirectory((LPTSTR)currDir);
	return 0;
} //--pulserBuildAll()	

//----------------------------------------------------------
// pulserDownloadAll() - Download pulse programs and waveforms
//----------------------------------------------------------
BOOL pulserDownloadAll(void)	
{
	char currDir[MAX_PATH + 1];
	unsigned islot, jslot, kslot, ich, jch, kch;
	BOOL rc;
	USHORT slot = 0;
	USHORT ch = 0;
	// Save current directory and go to Channels directory
	if (GetCurrentDirectory(MAX_PATH + 1, (LPTSTR)currDir) == 0) {
		printf("GetCurrentDirectory failed.\r\n");
		//return -1;
	}
	// Set channel parameters for all or one channel.
	if (slot == 0) {	// all channels
		jslot = 0;
		kslot = MAXSLOTS;
		jch = 0;
		kch = DDS_MAX_CHANNELS;
	} else {		// one channel
		jslot = slot - 1;
		kslot = jslot + 1;
		jch = ch;
		kch = ch + 1;
	}
	// Go over the DDS channels and do the download.
	rc = TRUE;
	for (islot = jslot; islot < kslot; islot++) {
		if (pulserGetCardType(islot + 1) != DdsType)
			continue;	// not DDS
		for (ich = jch; ich < kch; ich++) {
			if (!pulserGotoChDir(islot + 1, ich))
				return -1;
			printf("Downloading [%d-%d]\n",islot + 1, ich);
			rc = pulserDownload(islot + 1, ich);
			if (!rc)
				return FALSE;
		}
	}
	SetCurrentDirectory((LPTSTR)currDir);

	return TRUE;
} //-- pulserDownloadAll()

//---------------------------------------------------------------------
// pulserGetSequencerTable() - returns a pointer to the sequencer program table
//---------------------------------------------------------------------
PULSERTab *pulserGetSequncerTable(USHORT slot, USHORT channel)
{
	PULSERChannelInfo *chInfo = _pulserGetChIntf(slot, channel);
	return (&chInfo->progInfo);
}

//---------------------------------------------------------------------
// pulserGetWaveformTable() - returns a pointer to the waveform table
//---------------------------------------------------------------------
PULSERTab *pulserGetWaveformTable(USHORT slot, USHORT channel)
{
	PULSERChannelInfo *chInfo = _pulserGetChIntf(slot, channel);
	return (&chInfo->wavInfo);
}

//BOOL  pulserUploadSeq(USHORT slot, USHORT channel, ULONG addr4) {}
//BOOL  pulserUploadWfm(USHORT slot, USHORT channel, ULONG addr4) {}

//---------------------------------------------------------------------
// pulserPrintChannelInfo() - prints channel data
//---------------------------------------------------------------------
void pulserPrintChannelInfo(USHORT slot, USHORT channel)
{
	PULSERChannelInfo *chInfo = _pulserGetChIntf(slot, channel);
	CardType type = pulserGetCardType(slot);
	printf("\r\nChannel: %d-%d %s\r\n", slot, channel, pulserGetCardName(type));
	if (type == DdsType)
		_pulserPrintChInfo(chInfo);
	else if (type == Mw3Type)
		_mw3PrintChInfo(chInfo);
}

//---------------------------------------------------------------------
void pulserStopSequencer(USHORT slot, USHORT channel)
{
	pulserStop(slot, channel);
}

//---------------------------------------------------------------------
BOOL pulserIsSequencerRunning(USHORT slot, USHORT channel)
{
	PULSERChannelInfo *chInfo = _pulserGetChIntf(slot, channel);
	UINT32 runningbit = (channel == 0)? DDS_SEQ_0_RUNNING : DDS_SEQ_1_RUNNING;
	if (*chInfo->cardStatusReg & runningbit)
		return TRUE;	// running
	else
		return FALSE;	// idle
}

//---------------------------------------------------------------------
//BOOL pulserGroupClear(USHORT group) {}
//BOOL pulserGroupAddChannel(USHORT group, USHORT slot, USHORT channel) {}
//BOOL pulserGroupRemoveChannel(USHORT group, USHORT slot, USHORT channel){}
//BOOL pulserGroupRunSequencer(USHORT group, char *progName) {}
//BOOL pulserGroupStopSequencer(USHORT group, char *progName) {}

//------------------------------------------------------------
// MW3 API functions
//------------------------------------------------------------
void mw3Init(USHORT slot)
{
	PULSERChannelInfo *chInfo;
	chInfo = _pulserGetChIntf(slot, 0);

	// Turn on the CE signals for all three channels.
	//*chInfo->chCmdReg = MW3_CE_ALL;
	// Pulse CE_LATCH bit in the card command register
	//*chInfo->cardCmdReg = 0;
	//*chInfo->cardCmdReg = MW3_CMD_CE_LATCH;
	//Sleep(1L);
	//*chInfo->cardCmdReg = 0;

	// Configure PLL devices with default values.
//	_mw3PllChipEnable(slot, 1, FALSE);

	_mw3PllChipEnable(slot, 2, TRUE);
	_mw3PllChipEnable(slot, 1, TRUE);
	_mw3PllChipEnable(slot, 0, TRUE);

	_mw3InitHmc833(slot, 0);
//	_mw3InitHmc764(slot, 1);
	_mw3InitHmc833(slot, 2);
//	_mw3PllChipEnable(slot, 0, TRUE);
//	_mw3PllChipEnable(slot, 1, TRUE);
//	_mw3PllChipEnable(slot, 2, TRUE);
}

//------------------------------------------------------------
void mw3ShutDown(USHORT slot)
{
	mw3Log("In mw3Shutdown.\n");

//	mw3OutputOff(slot, 0);
//	mw3OutputOff(slot, 1);
//	mw3OutputOff(slot, 2);

//	_mw3PllChipEnable(slot, 0, FALSE);
//	_mw3PllChipEnable(slot, 1, FALSE);
//	_mw3PllChipEnable(slot, 2, FALSE);
}

//------------------------------------------------------------
void mw3Reset(USHORT slot)
{
	char buff[200];
	_mw3DoCommand(slot, 0, DDS_CMD_RESET, 0, 0, NULL);
	sprintf_s(buff, 200, "Slot %d reset.\n", slot);
	fwrite(buff, 1, strlen(buff), pLogFile);
}

//------------------------------------------------------------
BOOL mw3ResetPll(USHORT slot, USHORT channel)
{
	char buff[200];
	sprintf_s(buff, 200, "[%d-%d] Reset PLL device\n", slot, channel);
	fwrite(buff, 1, strlen(buff), pLogFile);

	_mw3PllChipEnable(slot, channel, FALSE);	// power down PLL
	Sleep(2L);									// 2 milliseconds
	_mw3PllChipEnable(slot, channel, TRUE);		// power down PLL
	return TRUE;
}
//------------------------------------------------------------
BOOL mw3OutputOn(USHORT slot, USHORT channel)
{
	ULONG R8;
	BOOL r;

	mw3Log("In mw3OutputOn.\n");

	// Set bit-4 of the Analog EN register (R8)
	R8 = mw3ReadRegister(slot, channel, 8);
	if (R8 == 0xffffffff) {
		fwrite("mw3ReadRegister failed.\r\n", 1, strlen("mw3ReadRegister failed.\r\n"), pLogFile);
		fflush(pLogFile);
		return FALSE;
	}
	R8 |= 0x10;		// set bit-4
	r = mw3WriteRegister(slot, channel, 8, R8);
	//_mw3PllChipEnable(slot, channel, TRUE);	// power up PLL
	if (channel == 1)
		return r;
	// HMC833: write 0x0d88 to R5 (VCO r1)
	mw3WriteRegister(slot, channel, 5, 0x0d88);
	mw3WriteRegister(slot, channel, 5, 0);
	mw3SetOutputFreq(slot, channel, mw3GetOutputFreq(slot, channel));

	mw3Log("Leaving mw3OutputOn.\n");
	return TRUE;
}


//------------------------------------------------------------
BOOL mw3OutputOff(USHORT slot, USHORT channel)
{	
	ULONG R8; 
	BOOL r;

	mw3Log("In mw3OutputOff.\n");

	// Clear bit-4 of the Analog EN register (R8)
	R8 = mw3ReadRegister(slot, channel, 8);
	if (R8 == 0xffffffff) {
		fwrite("mw3ReadRegister failed.\r\n", 1, strlen("mw3ReadRegister failed.\r\n"), pLogFile);
		fflush(pLogFile);
		return FALSE;
	}
	R8 &= ~0x10;	// clear bit-4
	r = mw3WriteRegister(slot, channel, 8, R8);
	if (channel == 1) {
		_mw3PllChipEnable(slot, 1, FALSE);
		mw3Log("Channel 1 Chip Disable.\n");
		return r;
	}
	// HMC833: write 0x0d08 to R5 (VCO r01)
	mw3WriteRegister(slot, channel, 5, 0x0d08);
	mw3WriteRegister(slot, channel, 5, 0);
	mw3WriteRegister(slot, channel, 5, 0x6000);	// Steve (June 11)

	mw3Log("Turn off PLL\n");
	mw3WriteRegister(slot, channel, 2, 0x000000);

	mw3Log("Leaving mw3OutputOff.\n");
	return TRUE;
}

//------------------------------------------------------------
BOOL mw3WriteRegister(USHORT slot, USHORT dev, USHORT reg, ULONG value)
{
	char line[200];
	if (pLogFile) {
		sprintf_s(line, 200, "[%d:%d:%d] 0x%06x\n", slot, dev, reg, value);
		fwrite(line, 1, strlen(line), pLogFile);
		fflush(pLogFile);
	}
	return _mw3DoCommand(slot, dev, MW3_CMD_WRITE_REG, reg, value, 0);
}

//------------------------------------------------------------
ULONG mw3ReadRegister(USHORT slot, USHORT dev, USHORT reg)
{
	ULONG value = 0xffffffff;
	if (_mw3DoCommand(slot, dev, MW3_CMD_READ_REG, reg, 0, &value))
		return value;
	else {
		fprintf(stderr,"mw3ReadRegister: _mw3DoCommand failed.");
		return value;
		//return 0xffffffff;
	}
}

//------------------------------------------------------------
BOOL mw3SelectClock(USHORT slot, ClockType clock, ULONG clockMHz)
{
	PULSERChannelInfo *chInfo;
	double clockFreq;

	// clockFreq is used for external clock only.
	if (clock == ClockPxi)
		clockFreq = MW3_CLK_PXI_MHZ;
	else if (clock == ClockExternal)
		clockFreq = clockMHz;
	else
		clockFreq = MW3_CLK_INT_MHZ;
	chInfo = _pulserGetChIntf(slot, 0);
	chInfo->refClockSource = clock;
	chInfo->refClockFreq = clockFreq;
	return _mw3DoCommand(slot, 0, MW3_CMD_CLK_SELECT, 0, (ULONG)clock, NULL);
}

//------------------------------------------------------------
ClockType mw3GetClockType(USHORT slot)
{
	PULSERChannelInfo *chInfo;
	chInfo = _pulserGetChIntf(slot, 0);
	return chInfo->refClockSource;
}

//------------------------------------------------------------
double mw3GetClockMHz(USHORT slot)
{
	PULSERChannelInfo *chInfo;
	chInfo = _pulserGetChIntf(slot, 0);
	return chInfo->refClockFreq;
}

//------------------------------------------------------------
BOOL mw3Locked(USHORT slot, USHORT channel) {
	if (channel == 1)
		return mw3ReadRegister(slot, channel, 0xf) & 0x01;
	else
		return mw3ReadRegister(slot, channel, 0x12) & 0x02;
}

//------------------------------------------------------------
BOOL mw3SetRefDivider(USHORT slot, USHORT channel, ULONG value)
{
	PULSERChannelInfo *chInfo;
	chInfo = _pulserGetChIntf(slot, channel);
	chInfo->refDivider = value;
	return TRUE;
}

//------------------------------------------------------------
ULONG	mw3GetRefDivider(USHORT slot, USHORT channel)
{
	PULSERChannelInfo *chInfo;
	chInfo = _pulserGetChIntf(slot, channel);
	return chInfo->refDivider;
}


//------------------------------------------------------------
BOOL mw3SetGUIFreq(USHORT slot, USHORT channel, double freqMhz) 
{
	BOOL r;
	char buff[200];
	PULSERChannelInfo *chInfo;
	chInfo = _pulserGetChIntf(slot, channel);
	chInfo->outputFreqMhz = freqMhz;

	if (pLogFile) {
		sprintf_s(buff, 200, "\n[%d-%d] Set freq. to %f MHz\n", slot, channel, freqMhz);
		fwrite(buff, 1, strlen(buff), pLogFile);
		fflush(pLogFile);
	}

	return TRUE;
}

//------------------------------------------------------------
BOOL mw3SetOutputFreq(USHORT slot, USHORT channel, double freqMhz) 
{
	BOOL r;
	char buff[200];
	PULSERChannelInfo *chInfo;
	chInfo = _pulserGetChIntf(slot, channel);
	chInfo->outputFreqMhz = freqMhz;

	if (pLogFile) {
		sprintf_s(buff, 200, "\n[%d-%d] Set freq. to %f MHz\n", slot, channel, freqMhz);
		fwrite(buff, 1, strlen(buff), pLogFile);
		fflush(pLogFile);
	}

	if (channel == 1) {
		_mw3PllChipEnable(slot, 1, TRUE);
		r = _mw3SetHmc764Freq(slot, 1, freqMhz);
	}

	else if ((channel == 0) || (channel == 2))
		r = _mw3SetHmc833Freq(slot, channel, freqMhz);
	else
		r = FALSE;
	return r;
}

//------------------------------------------------------------
double mw3GetOutputFreq(USHORT slot, USHORT channel)
{
	PULSERChannelInfo *chInfo;
	chInfo = _pulserGetChIntf(slot, channel);
	return chInfo->outputFreqMhz;
}

//------------------------------------------------------------
BOOL mw3SetAtten(USHORT slot, USHORT channel, double lossDb)
{
	PULSERChannelInfo *chInfo;
	USHORT dev;
	BOOL r;
	char buff[200];
	double v;

	dev = (channel == 0)? 3 : 4;
	chInfo = _pulserGetChIntf(slot, channel);

	// calculate the control voltage. loss is positive.
	if (lossDb > MW3_ATTEN_LOSS_MAX)
		lossDb = MW3_ATTEN_LOSS_MAX;
	if (lossDb < 0.0)
		lossDb = 0.0;
	v = MW3_ATTEN_VOLT_MAX - 4 * lossDb;

	sprintf_s(buff, 200, "\n[%d-%d] Attn = %.2f dB\n", slot, channel, lossDb);
	if (pLogFile) {
		fwrite(buff, 1, strlen(buff), pLogFile);
		fflush(pLogFile);
	}

	// save attenuation
	chInfo->attenuation = lossDb;
	r = mw3WriteRegister(slot, dev, 0, (ULONG)v);

	sprintf_s(buff, 200, "\n[%d-%d] Attn = %.2f dB (%d)\n", slot, channel, lossDb, r);
	if (pLogFile) {
		fwrite(buff, 1, strlen(buff), pLogFile);
		fflush(pLogFile);
	}
	return r;
}

//------------------------------------------------------------
double mw3GetAtten(USHORT slot, USHORT channel)
{
	PULSERChannelInfo *chInfo;
	chInfo = _pulserGetChIntf(slot, channel);
	return chInfo->attenuation;	// loss in dB
}



//------------------------------------------------------------
void mw3PllChipEnable(USHORT slot, USHORT channel, BOOL enable)
{
	_mw3PllChipEnable(slot, channel,enable);
}



//------------------------------------------------------------
// Internal PulserLib functions
//------------------------------------------------------------

static ULONG _pulserGetAddress(PULSERTab *tab, char *name)
{
	unsigned i, j;
	unsigned len = strlen(name);
	char entered[MAX_PATH], intable[MAX_PATH];
	for (j = 0; j < len; j++)
		entered[j] = toupper(name[j]);
	entered[len] = '\0';
	for (i = 0; i < tab->nEntry; i++) {
		for (j = 0; j < len; j++)
			intable[j] = toupper(tab->entry[i].name[j]);
		intable[len] = '\0';
		if (strcmp(entered, intable) == 0)
			return tab->entry[i].offset4;
	}
	return BADADDR;
}

//---------------------------------------------------------------
//  _pulserDownloadBinfile() - Download a waveform or sequencer .bin file. 
//	slot: slot number
//	channel: channel number
// It is assumed that the current directory has the files to download.
//----------------------------------------------------------------
static BOOL _pulserDownloadBinfile(USHORT slot, USHORT channel, ULONG offset4, FileType filetype)
{
	PULSERChannelInfo *info;
	PULSERTab tab;
	int e, eof;
	FILE *pTabFile, *pBinFile;
	char *tabName;
	char *binName;
	char line[MAX_PATH + 1], *endp;
	errno_t err;
	ULONG  totallen4, txlen4;
	ULONG addr4, len4, maxaddr4;
	size_t i, j, k, left4, txpages, ipage, size;
	ULONG cmd, txaddr4, nread, buff[DDS_INTF_BUFF_SIZE4], *target;

	info = _pulserGetChIntf(slot, channel);
	if (info == NULL) {
		printf("ddsDownload: bad slot or channel %d %d\r\n", slot, channel);
		return FALSE;
	}
	e = pulserGotoChDir(slot, channel);
	// Note: all the files are in the current directory.
	// Open the .tab and .bin files
	tabName = (filetype == WfmType)? "wfm.tab" : "seq.tab";
	binName = (filetype == WfmType)? "wfm.bin" : "seq.bin";
	err = fopen_s(&pTabFile, tabName, "r");
	if (err != 0) {
		printf("Could not open the .tab files. errno=%d\r\n", err);
		return FALSE;
	}

	// Initialize the PULSERTab structure
	memset((char *)&tab, 0, sizeof(PULSERTab));
	tab.selectedEntry = -1;
	//nEntries = 0;
	// read each line of the tab file and process it
	totallen4 = (filetype == WfmType)? 1 : 0;	// 1 for special zero point
	//for (; fgets(line, MAX_PATH, pTabFile) != NULL;) {
	for (eof = 0;;) {
		if (fgets(line, MAX_PATH, pTabFile) == NULL) {
			eof = 1;
			break;
		}
		if (line[0] == '#')
			continue;	// skip comment line
		addr4 = strtoul(line, &endp, 16);	// address in hex
		len4 = strtoul(endp, &endp, 16); // length in hex
		if (len4 == 0) {
			printf("Error in len4\r\n");
			fclose(pTabFile);
			return FALSE;
		}
		maxaddr4 = (filetype == WfmType)? DDS_CH_WAVEMEM_MAX : DDS_CH_WAVEMEM_MAX;
		if ((len4 + addr4) > maxaddr4) {
			printf("Bad address in %s\r\n", line);
			continue;
		}
		if (strlen(endp) == 0) {
			printf("Bad name in %s\r\n", line);
			continue;
		}
		for (k = 0; k < strlen(endp); k++, endp++) {
			if (!isspace(*endp))
				break;
		}
		// Update the table entry
		j = tab.nEntry;
		tab.entry[j].offset4 = addr4;
		tab.entry[j].length4 = len4;
		err = strncpy_s(tab.entry[j].name, strlen(endp), endp, _TRUNCATE);
		tab.nEntry++;
		totallen4 += len4;
		//printf("[%d-%d] %s: line=%s\n", slot, channel, tabName, line);
	}
	tab.length4 = totallen4;
	if (!eof && (tab.nEntry == 0)) {
		printf("%d-%d %d tab data inconsistent\r\n", slot, channel, filetype);
		fclose(pTabFile);
		return FALSE;
	}
	// update PULSERChannelInfo data
	if (filetype == WfmType) {
		info->wavInfo = tab;
		// check data
		if ((info->wavInfo.nEntry == 0) && (info->wavInfo.length4 != 0)) {
			fclose(pTabFile);
			return FALSE;
		}
	}
	else
		info->progInfo = tab;

	fclose(pTabFile);	// close the tab file

	// work on the .bin file
	// obtain file size:
	err = fopen_s(&pBinFile, binName, "rb");
	if (err != 0) {
		printf("Could not open the .bin files. errno=%d\r\n", err);
		return FALSE;
	}
	fseek (pBinFile , 0 , SEEK_END);
	size = ftell (pBinFile);
	rewind (pBinFile);
	if ((size % 4 != 0) || (size > 4 * DDS_CH_PROGMEM_SIZE4) || (size != 4 * totallen4)) {
		printf("Bad file size: file %s size %d\r\n", binName, size);
		fclose(pBinFile);
		info->progInfo.nEntry = info->wavInfo.nEntry = 0;
		fclose(pBinFile);
		return FALSE;
	}

	buff[DDS_INTF_BUFF_SIZE4];
	cmd = (filetype == WfmType)? DDS_CMD_WRWAVE : DDS_CMD_WRPROG;
	txaddr4 = offset4;	// target address
	left4 = size / 4;
	txpages = (size / 4 + DDS_INTF_BUFF_SIZE4 - 1)/DDS_INTF_BUFF_SIZE4;
	for (ipage = 0; ipage < txpages; ipage++, txaddr4 += DDS_INTF_BUFF_SIZE4) {

		// Read one page from the .bin file.
		memset((char *)buff, 0, DDS_INTF_BUFF_SIZE4 * 4);
		txlen4 = (left4 < DDS_INTF_BUFF_SIZE4)? left4 : DDS_INTF_BUFF_SIZE4;
		nread = fread(buff, 4, txlen4, pBinFile);
		if (nread != txlen4) {
			printf("fread failed %ld %ld\r\n", txlen4, nread);
			fclose(pBinFile);
			info->progInfo.nEntry = info->wavInfo.nEntry = 0;
			fclose(pBinFile);
			return FALSE;
		}

		// Write the page to the channel buffer
		target = info->fromHostBuff;
		for (i = 0; i < DDS_INTF_BUFF_SIZE4; i++) {
			*target++ = buff[i];
		}
		left4 -= txlen4;
#ifdef PULSER_SIM  // loop back for testing
		target = info->toHostBuff;
		for (i = 0; i < DDS_INTF_BUFF_SIZE4; i++) {
			*target++ = buff[i];
		}
#endif
		// Do the transfer protocol
		if (_pulserDoCommand(slot, channel, cmd, txaddr4, txlen4) != 0) {
			info->progInfo.nEntry = info->wavInfo.nEntry = 0;
			fclose(pBinFile);
			return FALSE;
		}
	}
	fclose(pBinFile);
	return TRUE;
} //--_pulserDownloadBinFile()

//----------------------------------------------------------
// _pulserGetChIntf() - returns  pointer or NULL
//----------------------------------------------------------
PULSERChannelInfo * _pulserGetChIntf(USHORT slot, USHORT ch)
{
	USHORT iCh;
	PULSERChannelInfo *info;

	if ((slot > MAXSLOTS) || ((ch > MAX_CHANNELS_PER_SLOT) && (ch != DDS_CH_CARD))) {
		printf("invalid slot or channel %d %d\r\n", slot, ch);
		return NULL;
	}
	iCh = (ch == DDS_CH_CARD)? 0 : ch;
	info = &PulserChannel[slot - 1][iCh];
	return info;

} //--_pulserGetChIntf()

//----------------------------------------------------------
// _pulserDoCommand() - Issues a command to DDS device
//	ch: channel number (0-1) or DDS_CH_CARD
//  All unused arguments should be set to zero.
//	The sequencer start command is considered complete when
//		the sequence "starts", not when it is finished.
//----------------------------------------------------------
int _pulserDoCommand(USHORT slot, USHORT ch, ULONG cmd, ULONG addr4, ULONG len4)
{
	USHORT iCh = (ch == DDS_CH_CARD)? 0 : ch;
	PULSERChannelInfo *info = _pulserGetChIntf(slot, iCh);
	if (info == NULL) {
		return -1;
	}
	// check the reset status bit first???

	// Handle special commands
	if (cmd == DDS_CMD_LOOPBK) {
		*info->cardCmdReg = (DDS_CMD_CH_VAL(ch)) | DDS_CMD_LOOPBK | DDS_CMD_REQ;
		Sleep(1L);
		if (!(*info->cardStatusReg & DDS_CMD_DONE))
			printf("Loop pack command timed out.\r\n\r\n");
		*info->cardCmdReg = 0;	// clear the command register
		return 0;
	}
	else if (cmd == DDS_CMD_RESET) {
		if (ch == DDS_CH_CARD)
			*info->cardCmdReg = DDS_CMD_RESET | (DDS_CMD_CH_VAL(ch)) | DDS_CMD_REQ;
		else {
			*info->chCmdReg = DDS_CMD_RESET;
			*info->cardCmdReg = (DDS_CMD_CH_VAL(ch)) | DDS_CMD_REQ;
		}
		Sleep(5L); // 5 millisecond 
		*info->cardCmdReg = 0;	// clear the command register
		Sleep(45L); // 45 millisecond 
		return 0;		
	}
	if (ch == DDS_CH_CARD) {
		printf("pulserDoCommand: bad channel no. in command 0x%x\r\n", cmd);
		return -1;
	}
	//------------------------------------------------------
	// Normal channel commands.
	// Check the request bit in the card command register
	//------------------------------------------------------
	if (*info->cardCmdReg & DDS_CMD_REQ) {
		printf("pulserDoCommand (0x%x): previous command pending (0x%x)\r\n", cmd, *info->cardCmdReg);
		printf("Resetting the cmd register.\r\n");
		*info->cardCmdReg = 0;
		//return -1;
	}
	// If the command is a "sequencer start", check the sequencer status
	if (cmd == DDS_CMD_SEQ_START) {
		UINT32 runningbit = (ch == 0)? DDS_SEQ_0_RUNNING : DDS_SEQ_1_RUNNING;
		if (*info->cardStatusReg & runningbit) {
			printf("Warning: Channel %d sequencer still running.\r\n", ch);
			//return -1;
		}
	}
	// Write to the channel command, address and length registers first.
	*info->chCmdReg = cmd;
	*info->chAddr4Reg = addr4;
	*info->chLen4Reg = len4;
	// Write to the card command register.
	*info->cardCmdReg = (DDS_CMD_CH_VAL(ch)) | DDS_CMD_REQ;
	// Wait for the DONE bit in the card status register.
#ifdef PULSER_SIM
	*info->cardStatusReg |= DDS_CMD_REQ;	// set the DONE bit
#endif
	if (!(*info->cardStatusReg & DDS_CMD_REQ)) {
		Sleep(1L);	// one millisecond
		if (!(*info->cardStatusReg & DDS_CMD_REQ)) {
			printf("_pulserDoCommand(0x%x): command timed out.\r\n", cmd);
			return -1;
		}
	}
	// Clear the card and channel command registers.
	*info->chCmdReg = 0;
	*info->cardCmdReg = 0;
#ifdef LOUD
	printf("Command [%d-%d] 0x%x OK.\r\n", slot, ch, cmd);
#endif
	return 0;
} //-- pulserDoCommand()


//---------------------------------------------------------------
// int _pulserUploadBinfile() - Uploads a waveform or sequencer to a file. 
//	slot: slot number
//	channel: channel number
//  offset4: address offset of the data
//  len4: number of 4-byte words to uploadF
// The uploaded data is stored in wav_up.bin or seq_up.bin.
//----------------------------------------------------------------
int _pulserUploadBinfile(USHORT slot, USHORT channel, ULONG offset4, ULONG len4, FileType filetype)
{
	PULSERChannelInfo *info;
	FILE *outfile;
	char *filename = (filetype == WfmType)? "wfm_up.bin" : "seq_up.bin";
	ULONG cmd, buff[DDS_INTF_BUFF_SIZE4];
	ULONG rxaddr4, left4, rxpages, ipage, rxlen4, *p;
	size_t i, n;

	// Get the hardware interface.
	info = _pulserGetChIntf(slot, channel);
	if (info == NULL) {
		printf("pulserUpload: bad slot or channel %d %d\r\n", slot, channel);
		return -1;
	}

	// open the output file
	pulserGotoChDir(slot, channel);
	if (fopen_s(&outfile, filename, "wb") != 0) {
		printf("pulserUpload: failed to open %s.\r\n", filename);
		return -1;
	}

	// Iterate over all pages.
	cmd = (filetype == WfmType)? DDS_CMD_READWAVE : DDS_CMD_READPROG;
	rxaddr4 = offset4;	// source address offset
	left4 = len4;
	rxpages = (len4 + DDS_INTF_BUFF_SIZE4 - 1)/DDS_INTF_BUFF_SIZE4;
	for (ipage = 0; ipage < rxpages; ipage++, rxaddr4 += DDS_INTF_BUFF_SIZE4) {
		rxlen4 = (left4 < DDS_INTF_BUFF_SIZE4)? left4 : DDS_INTF_BUFF_SIZE4;
		// Issue a read command to the DDS.
		if (_pulserDoCommand(slot, channel, cmd, rxaddr4, rxlen4) != 0) {
			printf("Upload operation failed.\r\n\r\n");
			fclose(outfile);
			return -1;
		}
		// Transfer one page from the DDS interface buffer.
		memset((char *)buff, 0, DDS_INTF_BUFF_SIZE4 * 4);
		p = info->toHostBuff;
		for (i = 0; i < rxlen4; i++) {
			buff[i] = *p++;
		}
		left4 -= rxlen4;

		// Write one page to the output file.
		n = fwrite(buff, 4, rxlen4, outfile);
		if (n != rxlen4) {
			printf("fwrite failed %ld %ld\r\n", rxlen4, n);
			fclose(outfile);
			return -1;
		}
#ifdef _DEBUG
		for (i = 0; i < rxlen4; i++) {
			printf("0x%08x ", buff[i]);
			if ((i != 0) && (((i+1) % 4) == 0))
				printf("\r\n");
		}
		printf("\r\n");
#endif
	}
	printf("Uploaded to %s\r\n", filename);
	fclose(outfile);
	return 0;
} //--pulserUpload()

//--------------------------------------------------------------------
// Sequencer control API functions
//--------------------------------------------------------------------
BOOL pulserRunSequencer(USHORT slot, USHORT channel, char *progName)
{
	PULSERChannelInfo *chInfo = _pulserGetChIntf(slot, channel);
	ULONG addr4 = _pulserGetAddress(&chInfo->progInfo, progName);
	if (addr4 == BADADDR)
		return FALSE;
	if (_pulserDoCommand(slot, channel, DDS_CMD_SEQ_START, addr4, 0) == 0)
		return TRUE;
	else
		return FALSE;
}

//static BOOL _pulserGroupRunSequencerAddr(USHORT group, ULONG addr4) {}

//----------------------------------------------------------
//	BOOL _FileExists(std::string strFilename)
//----------------------------------------------------------
BOOL _FileExists(char * strFilename) { 
	struct stat stFileInfo; 
	BOOL blnReturn; 
	int intStat; 

	// Attempt to get the file attributes 
	intStat = stat(strFilename,&stFileInfo); 
	if(intStat == 0) { 
		blnReturn = 1; 
	} else { 
		blnReturn = 0; 
	} 
	return(blnReturn); 
}

//---------------------------------------------------------------------
// _pulserPrintChInfo() - Prints PulserChannel data structure for a channel
//---------------------------------------------------------------------
void _pulserPrintChInfo(PULSERChannelInfo *info)
{
	//printf("\r\nPulser Channel: %d-%d\r\n", info->slot, info->ch);
	printf("\r\nWaveforms:");
	_pulserPrintTab(&info->wavInfo);
	printf("\r\nPrograms:");
	_pulserPrintTab(&info->progInfo);

#ifdef LOUD
	_pulserPrintPCIInfo(&info->pciInfo);
	// printf("Sequencer programs: %d    Waveforms %d\r\n", info->progInfo.nEntry, info->wavInfo.nEntry);
	printf("Registers       Host        DDS\r\n");
	printf("cardBaseAddr:   0x%08x  0x%x\r\n", info->cardBaseVirtualAddr, info->cardCmdReg - info->cardBaseVirtualAddr);
	printf("cardCmdReg:     0x%08x  0x%x\r\n", info->cardCmdReg, info->cardCmdReg - info->cardBaseVirtualAddr);
	printf("cardStatusReg:  0x%08x  0x%x\r\n", info->cardStatusReg, info->cardStatusReg - info->cardBaseVirtualAddr);
	printf("fromHostBuff:   0x%08x  0x%x\r\n", info->fromHostBuff, info->fromHostBuff - info->cardBaseVirtualAddr);
	printf("toHostBuff:     0x%08x  0x%x\r\n", info->toHostBuff, info->toHostBuff - info->cardBaseVirtualAddr);
	printf("cmdReg:	        0x%08x  0x%x\r\n", info->chCmdReg, info->chCmdReg - info->cardBaseVirtualAddr);
	printf("addr4Reg:       0x%08x  0x%x\r\n", info->chAddr4Reg, info->chAddr4Reg - info->cardBaseVirtualAddr);
	printf("len4Reg:        0x%08x  0x%x\r\n", info->chLen4Reg, info->chLen4Reg - info->cardBaseVirtualAddr);
	printf("statusReg:      0x%08x  0x%x\r\n", info->chStatusReg, info->chStatusReg - info->cardBaseVirtualAddr);
	printf("intrCode:       0x%08x  0x%x\r\n", info->chIntrCode, info->chIntrCode - info->cardBaseVirtualAddr);
#endif
	return;
}

//---------------------------------------------------------
// _pulserPrintPCIInfo() - Prints PCIInfo data structure
//---------------------------------------------------------
void _pulserPrintPCIInfo(PCIInfo *info)
{
	printf("PCI=%d.%d.%d  BAR0=0x%x ", 
		info->pciBusNo, info->pciDevNo, info->pciFuncNo, info->barZero);
	printf("Vendor=0x%04x(%s) Device=0x%04x(%s)\r\n",
		info->vendorId, info->vendorName, info->devId, info->deviceName);
}

//---------------------------------------------------------
// _pulserPrintTab() - prints PULSERTab data structure
//---------------------------------------------------------
void _pulserPrintTab(PULSERTab *info)
{
	unsigned i;
	printf("(%d entries, 0x%x words)\r\n", info->nEntry, (info->nEntry)? info->length4 : 0);
	for (i = 0; i < info->nEntry; i++) 
		_pulserPrintTabRecord(&info->entry[i]);
}
//---------------------------------------------------------
// _pulserPrintTabRecord() - Prints PULSERTabRecord structure
//---------------------------------------------------------
void _pulserPrintTabRecord(PULSERTabRecord* info)
{
	printf("Address=0x%08x Length=0x%04x %s\r\n", info->offset4, info->length4, info->name);
}

//----------------------------------------------------------
// _pulserPrintBuffer() - Prints toHost buffer
//----------------------------------------------------------
void _pulserPrintBuffer(USHORT slot, USHORT ch)
{
	size_t i;

	PULSERChannelInfo *info = &PulserChannel[slot - 1][ch];
	ULONG *p = info->toHostBuff;
	printf("[%d-%d] %s\r\n", slot, ch);
	for (i = 0; i < DDS_INTF_BUFF_SIZE4; i++) {
		printf("0x%8x ", *p++);
		if ((i != 0) && (((i+1) % 4) == 0))
			printf("\r\n");
	}
	printf("\r\n");
}

//static void _pulserLog(char *logString) {}
//static void _pulserLogOpen(void) {}
//static void _pulserLogClose(void) {}

//------------------------------------------------------------
// MW3 Functions
//------------------------------------------------------------
static void _mw3InitHmc764(USHORT slot, USHORT channel) 
{
	// Initialize HMC764 to 7374 MHz.
	mw3WriteRegister(slot, channel, 5, 0x50894C);
	mw3WriteRegister(slot, channel, 6, 0x702F87);
	mw3WriteRegister(slot, channel, 7, 0x0004FA);
	mw3WriteRegister(slot, channel, 8, 0x0031DF);
	mw3WriteRegister(slot, channel, 9, 0x900380);
	mw3WriteRegister(slot, channel, 11, 0x000072);
//	mw3WriteRegister(slot, channel, 2, 0x000001);

	mw3Log("Turn off PLL Thru SPI\n");
	mw3WriteRegister(slot, channel, 2, 0x000000);
	mw3WriteRegister(slot, channel, 3, 0x000049);
	mw3WriteRegister(slot, channel, 4, 0xBD70A4);
}

//------------------------------------------------------------
static void _mw3InitHmc833(USHORT slot, USHORT channel) 
{
	// Initialize HMC833 to 2000.0 MHz.
	mw3WriteRegister(slot, channel, 5, 0x000d88);
	mw3WriteRegister(slot, channel, 5, 0x008090);
	mw3WriteRegister(slot, channel, 5, 0x002898);
	mw3WriteRegister(slot, channel, 5, 0x0060A0);
	mw3WriteRegister(slot, channel, 5, 0x001628);
	mw3WriteRegister(slot, channel, 5, 0);
	mw3WriteRegister(slot, channel, 6, 0x2003CA);
	mw3WriteRegister(slot, channel, 9, 0x153E7C);
	mw3WriteRegister(slot, channel, 10, 0x002046);
	mw3WriteRegister(slot, channel, 11, 0x07C061);
	mw3WriteRegister(slot, channel, 15, 0x000081);
//	mw3WriteRegister(slot, channel, 2, 0x000001);

	mw3Log("Turn off PLL Thru SPI\n");
	mw3WriteRegister(slot, channel, 2, 0x000000);
	mw3WriteRegister(slot, channel, 3, 0x000028);
	mw3WriteRegister(slot, channel, 4, 0x0);
	//mw3OutputOn(slot, channel);
}

//----------------------------------------------------------------------------
static BOOL _mw3SetHmc764Freq(USHORT slot, USHORT channel, double freqMHz) {
	// Set the frequency of HMC764.
	ULONG u, R, R3, R9, clockMHz, j42;
	double x, xFrac, Icp, B18, B20, B22, J42;

	mw3Log("Turn on PLL\n");
	mw3WriteRegister(slot, channel, 2, 0x000002);

	R = mw3ReadRegister(slot, channel, 2);
	clockMHz = (ULONG)(mw3GetClockMHz(slot) + 0.5);
	if ((R == 0.0) || (clockMHz == 0.0))
		return FALSE;

	// calculate R09
	Icp = 0.002;	// Icp = B19
	B18 = 1.0/(freqMHz * 1.0e6);
	B20 = 1.0e6 * clockMHz / R;
	B22 = (0.4e-8 + 4 * B18) * Icp * B20 / 0.000055;
	u = (ULONG)(B22 + 0.5);
	if (u > 7)
		u = 7;
	J42  = (Icp > 0.002)? 0.002/0.0005 : Icp/0.0005;
	j42 = (ULONG)J42 & 0x7;
	R9 = (u << 7) | (j42 << 21) | (j42 << 18);

	// calculate integer frequency R3
	x = (double)R * freqMHz/(2 * clockMHz);
	R3 = (ULONG)x;	// R3 = N_int

	// calculate the fraction
	xFrac = (x - R3) * (1 << 24) + 0.5;	// R4 = xFrac

	// write to the device

	mw3WriteRegister(slot, channel, 5, 0x50894c);
	mw3WriteRegister(slot, channel, 6, 0x702f87);
	mw3WriteRegister(slot, channel, 7, 0x0004fa);
	mw3WriteRegister(slot, channel, 8, 0x0031df);
	mw3WriteRegister(slot, channel, 9, R9);
	mw3WriteRegister(slot, channel, 0x0b, 0x000072);
	mw3WriteRegister(slot, channel, 2, R);
	mw3WriteRegister(slot, channel, 3, R3);
	mw3WriteRegister(slot, channel, 4, (ULONG)xFrac);
	return TRUE;
}

//------------------------------------------------------------
static BOOL _mw3SetHmc833Freq(USHORT slot, USHORT channel, double freqMHz) {
	// Configure HMC833 for a frequency and attenuation.
	// All frequencies in MHz.

	char buff[200];
	double K, F_xtal, F_frac, B22, B23, B24, B25, x, f;
	ULONG R, F_int, n, cpDnGain, cpUpGain, cpOffset, R6, R9, r2, r3, B31;

	mw3Log("In _mw3SetHmc833Freq\n");
	// calculate K (0.5 for doubler, 1 for fundamental, then 2, 4, ... 62.
	if ((freqMHz < 25.0) || (freqMHz > 6000.0))
		return FALSE;
	if (freqMHz > 3000.0)
		K = 0.5;		// doubler
	else if (freqMHz >= 1500.0)
		K = 1.0;		// fundamental
	else {
		// divider case
		x = (double)1500.0/freqMHz;
		n = (ULONG)x;
		if (n > 31)
			n = 31;
		K = 2 * n;
	}

	sprintf_s(buff, 200, "Hmc833 K=%f\n", K);
	fwrite(buff, 1, strlen(buff), pLogFile);


	mw3Log("Turn on PLL\n");
	mw3WriteRegister(slot, channel, 1, 0x000002); // rdg: changed register 2 to 1

	R = mw3ReadRegister(slot, channel, 2);	// reference divider
	if (R == 0)
		return FALSE;
	F_xtal = mw3GetClockMHz(slot);		// reference freq.
	if (F_xtal < 1.0)
		return FALSE;

	// calculate integer and fractional frequency
	f =  K * R * freqMHz / F_xtal;
	F_int = (ULONG)(f);	
	F_frac = (f - F_int) * (1 << 24);
	// Register 06
	R6 = ((ULONG)F_frac == 0)? 0x2003ca : 0x200b4a;

	// calculate charge pump register (R09)
	B22 = 1.0 / ((double)2250 * (double)1000000L);
	B23 = 0.0025;
	B24 = F_xtal*1000000/R;	// in Hz
	x = (0.0000000025+4*B22)*B24*B23;
	B25 = (x < 0.25*B23)? x : 0.25*B23;	// smaller of the two
	n = (ULONG)(B25/0.000005 + 0.5); 
	cpDnGain = (ULONG)(B23/0.00002);
	cpUpGain = cpDnGain;
	//R9 = 0x150000;  *** where is this number from?
	R9 = 0;
	if (F_frac == 0) {
		cpOffset = 0;
	} else {
		cpOffset = (n > 127)? 127 : n;	// R09[20:14]
		R9 |= 1 << 22;
	}
	R9 |= ((cpOffset & 0x7f) << 14);
	R9 |= ((cpUpGain & 0x7f) << 7);
	R9 |= (cpDnGain & 0x7f);

	sprintf_s(buff, 200, "Hmc833 K=%f cpOffset=0x%x  R9=0x%x\n", 
				K, cpOffset, R9);
	fwrite(buff, 1, strlen(buff), pLogFile);
	// VCO subsystem reg 02 (Biases)
	r2 = 0;
	// bit-8
	if (K <= 2)
		r2 = 0x0100;

	// bit-0
	if (K < 1)
		r2 |= 0x0001;
	else
		r2 |= ((ULONG)K & 0x3f);

	// bit-7&6
	if (freqMHz < 2500.0)
		B31 = 9;
	else if (freqMHz < 4500.0)
		B31 = 3;
	else
		B31 = 0;
	if (B31 == 0)
		r2 |= (0x0003 << 6);
	else if (B31 == 3)
		r2 |= (0x0002 << 6);
	else if (B31 == 6)
		r2 |= (0x0001 << 6);
	else
		r2 |= 0;

	//{sprintf(buff, "[3] r2=0x%x\n", r2); fwrite(buff, 1, strlen(buff), pLogFile);}

	r2 = (r2 << 7) | (2 << 3);		// address = 2

	// r3
	r3 = (K == 0.5)? 0x50 : 0x51;	
	r3 = (r3 << 7) | (3 << 3);		// address = 3

	// write to the device in the following order:
	mw3WriteRegister(slot, channel, 5, r2);
	mw3WriteRegister(slot, channel, 5, r3);
	mw3WriteRegister(slot, channel, 5, 0x60a0);
	mw3WriteRegister(slot, channel, 5, 0x1628);
	mw3WriteRegister(slot, channel, 5, 0);
	mw3WriteRegister(slot, channel, 6, R6);
	mw3WriteRegister(slot, channel, 9, R9);
	mw3WriteRegister(slot, channel, 0xa, 0x002046);
	mw3WriteRegister(slot, channel, 0xb, 0x07C061);
	mw3WriteRegister(slot, channel, 0xf, 0x000081);
	mw3WriteRegister(slot, channel, 2, R);
	mw3WriteRegister(slot, channel, 3, F_int);
	mw3WriteRegister(slot, channel, 4, (ULONG)(F_frac + 0.5));
	sprintf_s(buff, 200, "%d %f\n", F_int, F_frac);
	mw3Log(buff);
	return TRUE;
}

//----------------------------------------------------------
// _mw3DoCommand() - Issues a command to MW3device
//	dev: device number (0-4)
//  cmd:
//		DDS_CMD_RESET
//		DDS_CMD_LOOPBACK
//		MW3_CMD_CLK_SELECT	(data = MW3_CLK_INT, MW3_CLK_ENT or MW3_CLK_PXI10)
//		MW3_CMD_RERAD_REG
//		MW3_CMD_WRITE_REG
//  All unused arguments should be set to zero.
//----------------------------------------------------------
static BOOL _mw3DoCommand(USHORT slot, USHORT dev, ULONG cmd, ULONG addr4, ULONG data, ULONG *pdata)
{
	PULSERChannelInfo *info;
	static ULONG clockSelect;

	if (dev >= (MW3_MAX_CHANNELS * 2 - 1)) {
		return FALSE;
	}
	info = _pulserGetChIntf(slot, 0);
	if (info == NULL) {
		return FALSE;
	}

	if (cmd == DDS_CMD_RESET) {
		*info->cardCmdReg = DDS_CMD_RESET;
		Sleep(5L); // 5 millisecond 
		*info->cardCmdReg = 0;	// clear the command register
		Sleep(45L); // 45 millisecond 
		return 0;		
	}
	//------------------------------------------------------
	// Check the request bit in the card command register
	//------------------------------------------------------
	if (*info->cardCmdReg & DDS_CMD_REQ) {
		printf("pulserDoCommand (0x%x): previous command pending (0x%x)\r\n", cmd, *info->cardCmdReg);
		printf("Resetting the cmd register.\r\n");
		*info->cardCmdReg = 0;
		//return -1;
	}
	// Process the command
	//		MW3_CMD_CLK_SELECT
	//		MW3_CMD_RERAD_REG
	//		MW3_CMD_WRITE_REG
	if (cmd == MW3_CMD_CLK_SELECT) {
		if (data == ClockExternal)
			clockSelect = MW3_CLK_EXT;
		else if (data == ClockPxi)
			clockSelect = MW3_CLK_PXI;
		else
			clockSelect = MW3_CLK_INT;
		*info->chAddr4Reg = 0;
		*info->chLen4Reg = 32;
		*info->chCmdReg = clockSelect;
		// Generate a short pulse of bit-18
		*info->cardCmdReg = MW3_CTRL_CLKSEL0; 
		Sleep(1L);
		*info->cardCmdReg = 0;
		return TRUE;
	} 
	else if (cmd == MW3_CMD_READ_REG) {
		*info->chAddr4Reg = (dev << 16) | addr4;
		*info->chLen4Reg = (dev < MW3_MAX_CHANNELS) ? 32 : 8;
		*info->chCmdReg |= MW3_CMD_RD_FLAG;
		*info->cardCmdReg = DDS_CMD_REQ;
		if (_mw3WaitForDone(info)) {
			*info->cardCmdReg = 0;
			*pdata = *info->cardSpiReadData;
			return TRUE;
		}
		*info->cardCmdReg = 0;
		*pdata = *info->cardSpiReadData;
		return FALSE;
	}
	else if (cmd == MW3_CMD_WRITE_REG) {
		*info->chAddr4Reg = (dev << 16) | addr4;
		*info->cardSpiWriteData = data;
		*info->chLen4Reg = (dev < MW3_MAX_CHANNELS) ? 32 : 8;
		*info->chCmdReg = clockSelect | MW3_CMD_WR_FLAG;
		*info->cardCmdReg = DDS_CMD_REQ;
		if (_mw3WaitForDone(info)) {
			*info->cardCmdReg = 0;
			return TRUE;
		}
		*info->cardCmdReg = 0;
		return FALSE;
	}
	// **** clean up below
	// Write to the channel command, address and length registers first.
	*info->chCmdReg = cmd;
	*info->chAddr4Reg = addr4 | (dev << 16);
	*info->chLen4Reg = (dev < MW3_MAX_CHANNELS)? 32 : 8;
	// Write to the card command register.
	*info->cardCmdReg = DDS_CMD_REQ;
	// Wait for the DONE bit in the card status register.
#ifdef PULSER_SIM
	*info->cardStatusReg |= DDS_CMD_REQ;	// set the DONE bit
#endif
	if (!(*info->cardStatusReg & DDS_CMD_REQ)) {
		Sleep(1L);	// one millisecond
		if (!(*info->cardStatusReg & DDS_CMD_REQ)) {
			printf("ddsDoCommand(0x%x): command timed out.\r\n", cmd);
			return -1;
		}
	}
	// Clear the card and channel command registers.
	*info->chCmdReg = 0;
	*info->cardCmdReg = 0;
#ifdef LOUD
	printf("Command [%d-%d] 0x%x OK.\r\n", slot, ch, cmd);
#endif
	return 0;
} //-- _mw3DoCommand()

//-------------------------------------------------------
static BOOL _mw3WaitForDone(PULSERChannelInfo *info) 
{
	int i;
	for (i = 0; i < 100; i++) {
		if (*info->cardStatusReg & DDS_CMD_REQ)
			break;
		Sleep(10L);		// 10 msec
	}
#ifndef PULSER_SIM
	if (i == 100)
		return FALSE;
#endif
	return TRUE;
}

//---------------------------------------------------------
static void _mw3PrintChInfo(PULSERChannelInfo *info)
{
	char *status = (mw3Locked(info->slot, info->ch))? "Lock OK" : "Lock Lost";
	printf("Frequency = %.5f MHz\r\n", info->outputFreqMhz);
	printf("Attenuation = %.2f dB\r\n", info->attenuation);
	printf("PLL status = %s\r\n", status);
}

//---------------------------------------------------------
static void _mw3PllChipEnable(USHORT slot, USHORT channel, BOOL enable)
{
	PULSERChannelInfo *info = _pulserGetChIntf(slot, 0);
	ULONG ceSel = MW3_CE_ALL;
	char buff[200];

	// latch in all ones first
	*(info->chCmdReg) = ceSel;
	// Generate a short pulse of CE_LATCH bit
	_mw3CmdPulse(slot, MW3_CMD_CE_LATCH);

	if (!enable)	
		ceSel &= ~((MW3_CMD_CE_LATCH) << channel);
	*(info->chCmdReg) = ceSel;

	// Generate a short pulse of CE_LATCH bit
	_mw3CmdPulse(slot, MW3_CMD_CE_LATCH);

	// log channel control register
	sprintf_s(buff, 200, "[%d-%d] PllChipEnable(%s) Reg=0x%x\n",
		slot, channel, (enable)? "enable" : "disable", ceSel);
	fwrite(buff, 1, strlen(buff), pLogFile);
}

//-------------------------------------------------------
// Generate a 1 ms pulse of the card command register.
// bitflag is the image of the card command register
// where it is expected that one bit is set.
//-------------------------------------------------------
static void _mw3CmdPulse(USHORT slot, ULONG bitflag)
{
	PULSERChannelInfo *info = _pulserGetChIntf(slot, 0);
	*info->cardCmdReg = 0; 
	Sleep(2L);
	*info->cardCmdReg = bitflag; 
	Sleep(2L);
	*info->cardCmdReg = 0;
	Sleep(2L);
}

//
// Read nBytes starting byte-offset byteOffset.
//
static BOOL _mw3ReadEeProm(USHORT slot, USHORT byteOffset, USHORT nBytes, UCHAR *buffer)
{
	USHORT dev = 254;
	PULSERChannelInfo *info = _pulserGetChIntf(slot, 0);
	int i;
	ULONG data4;
	for (i = 0; i < nBytes; i++) {
		*info->chAddr4Reg = (dev << 16) + byteOffset + i;
		*info->chLen4Reg = 32;		// 32 bits in an SPI frame
		*info->chCmdReg = MW3_CMD_RD_FLAG;
		*info->cardCmdReg = DDS_CMD_REQ;
		if (_mw3WaitForDone(info)) {
			data4 = *((UCHAR *)info->cardSpiReadData); //%%%
			*(buffer + i) = (UCHAR)data4;
			*info->cardCmdReg = 0;
		}
		else {
			// timeout on DONE flag
			*info->cardCmdReg = 0;
			return FALSE;
		}
	}
	return TRUE;
}

//
// Write nBytes starting byte-offset from the beginning
//
static BOOL _mw3WriteEeProm(USHORT slot, USHORT byteOffset, USHORT nBytes, UCHAR *buffer)
{
	USHORT dev = 254;
	PULSERChannelInfo *info = _pulserGetChIntf(slot, 0);
	int i;
	for (i = 0; i < nBytes; i++) {
		// set the write enable latch first
		_mw3SetEePromWELatch(slot);

		*info->chAddr4Reg = (dev << 16) + byteOffset + i;	
		*info->chLen4Reg = 32;		// 32 bits in an SPI frame
		*(info->cardSpiWriteData) = *(buffer + i);
		*info->chCmdReg = MW3_CMD_WR_FLAG;
		*info->cardCmdReg = DDS_CMD_REQ;
		if (_mw3WaitForDone(info))
			*info->cardCmdReg = 0;
		else {
			// timeout on DONE flag
			*info->cardCmdReg = 0;
			return FALSE;
		}
	}
	return TRUE;
}

static BOOL _mw3SetEePromWELatch(USHORT slot)
{
	USHORT dev = 253;
	PULSERChannelInfo *info = _pulserGetChIntf(slot, 0);

	*info->chAddr4Reg = dev << 16;	
	*info->chLen4Reg = 8;
	*(info->cardSpiWriteData) = EEPROM_CMD_WREN;	//%%%
	*info->chCmdReg = MW3_CMD_WR_FLAG;
	*info->cardCmdReg = DDS_CMD_REQ;
	if (_mw3WaitForDone(info))
		*info->cardCmdReg = 0;
	else {
		// timeout on DONE flag
		*info->cardCmdReg = 0;
		return FALSE;
	}
	return TRUE;
}


BOOL mw3ReadEeProm(USHORT slot, USHORT byteOffset, USHORT nBytes, UCHAR *buffer)
{
	return _mw3ReadEeProm(slot, byteOffset, nBytes, buffer);
}

BOOL mw3WriteEeProm(USHORT slot, USHORT byteOffset, USHORT nBytes, UCHAR *buffer)
{
	return _mw3WriteEeProm(slot, byteOffset, nBytes, buffer);
}

void mw3Log(char *strbuff)
{
	fwrite(strbuff, 1, strlen(strbuff), pLogFile);
}
/* **********************************************************

int dds_reset(size_t slot, size_t channel)
{
	if (dds_get_ch_intf(slot, channel) == NULL) {
		printf("dds_reset: bad slot or channel, %d %d\n", slot, channel);
		return -1;
	}

	return dds_command(slot, channel, DDS_CH_RESET);

	/*
	DDSChIntf *pChannel = &(InstalledBoards[slot].ddsmem->chIntf[channel]);
	// check the reset status bit first - it should be set

	pChannel->statusReg |= DDS_CH_RESET;	// ### remove this later

	if ((pChannel->statusReg & DDS_CH_RESET) == 0) {
		printf("dds_reset: reset in progress.\n");
		return -1;	// reset in progress
	}
	// reset the channel -> automatically clears the reset status bit
	pChannel->cmdReg = DDS_CH_RESET;
	// wait for the reset status bit
	pChannel->statusReg |= DDS_CH_RESET;	// ### remove this later
	for (;;) {
		if (pChannel->statusReg & DDS_CH_RESET)
			break;
	} 
	return 0;

}

// read sequencer program or waveform memory to a buffer
int dds_read_mem(size_t slot, size_t channel, DWORD offset4, DWORD size4, DWORD *buf, bool iswave)
{
	DDSChIntf *pChannel = dds_get_ch_intf(slot, channel);
	if (pChannel == NULL) {
		printf("dds_read_mem: bad slot or channel %d %d\n", slot, channel);
		return -1;
	}
	size_t maxsize4 = (iswave)? DDS_CH_WAVEMEM_SIZE4 : DDS_CH_PROGMEM_SIZE4;
	DWORD command = (iswave)? DDS_CH_READWAVE : DDS_CH_READPROG;

	// check the offset and size
	if ((size4 < 1) || ((offset4 + size4) > maxsize4)) {
		printf("dds_read_mem: bad offset4 or size4 0x%x %d\n", offset4, size4);
		return -1;
	}

	// transfer channel memory one page at a time
	DWORD *pbuf4 = buf;
	DWORD rxaddr4 = offset4;	// target address
	int left4 = size4;
	int rxpages = (size4 + DDS_BUFF_SIZE4 - 1)/DDS_BUFF_SIZE4;
	for (int ipage = 0; ipage < rxpages; ipage++, rxaddr4 += DDS_BUFF_SIZE4) {
		int rxlen4 = (left4 < DDS_BUFF_SIZE4)? left4 : DDS_BUFF_SIZE4;
		left4 -= rxlen4;
		// start the transfer protocol
		pChannel->statusReg |= command;	// ### remove this later
		// check the write-program bit
		if ( !(pChannel->statusReg & command) ) {
			printf("dds_read_mem: FPGA not ready.\n");
			return -1;
		}
		// write offset and length to the channel command registers
		pChannel->cmdAddrReg = rxaddr4;
		pChannel->cmdLenReg = rxlen4;
		// read data from the channel output buffer
		for (int i = 0; i < rxlen4; i++, pbuf4++) {	
			pChannel->outData[i] = i;	// ### remove this
			*pbuf4 = pChannel->outData[i];
		}
		// write the command
		pChannel->cmdReg = command;
		pChannel->statusReg |= command;	// ### remove this later
		// wait for the completion report
		if (!(pChannel->statusReg & command)) {
			Sleep(1000L);	// one second sleep
			if (!(pChannel->statusReg & command)) {
				printf("dds_read_mem: timed out at page %d.\n", ipage);
				return -1;
			}
		}
	}
	return 0;
} 


// download a sequencer program or waveform to the channel program memory
int dds_write_mem(size_t slot, size_t channel, DWORD *buf, DWORD offset4, DWORD size4, bool iswave)
{
	DDSChIntf *pChannel = dds_get_ch_intf(slot, channel);
	if (pChannel == NULL) {
		printf("dds_write_mem: bad slot or channel %d %d.\n", slot, channel);
		return -1;
	}

	size_t maxsize4 = (iswave)? DDS_CH_WAVEMEM_SIZE4 : DDS_CH_PROGMEM_SIZE4;
	DWORD command = (iswave)? DDS_CH_WRWAVE : DDS_CH_WRPROG;

	// check the offset and size
	if ((size4 <= 1) || ((offset4 + size4) > maxsize4)) {
		printf("dds_write_mem: bad offset4 or size4 0x%x %d\n", offset4, size4);
		return -1;
	}

	// go over all pages
	DWORD *pbuf4 = buf;
	DWORD txaddr4 = offset4;	// target address
	int left4 = size4;
	int txpages = (size4 + DDS_BUFF_SIZE4 - 1)/DDS_BUFF_SIZE4;
	for (int ipage = 0; ipage < txpages; ipage++, txaddr4 += DDS_BUFF_SIZE4) {
		int txlen4 = (left4 < DDS_BUFF_SIZE4)? left4 : DDS_BUFF_SIZE4;
		left4 -= txlen4;
		// start the transfer protocol
		pChannel->statusReg |= command;	// ### remove this later
		// check the write-program bit
		if ( !(pChannel->statusReg & command) ) {
			printf("dds_write_mem: write-program not ready.\n");
			return -1;	// write in progress
		}
		// write offset and length to the channel command registers
		pChannel->cmdAddrReg = txaddr4;
		pChannel->cmdLenReg = txlen4;
		// write data on the channel input buffer
		for (int i = 0; i < txlen4; i++, pbuf4++) {	
			pChannel->inData[i] = *pbuf4;
		}
		// write the command
		pChannel->cmdReg = command;
		pChannel->statusReg |= command;	// ### remove this later
		// wait for the completion report
		if (!(pChannel->statusReg & command)) {
			Sleep(1000L);	// one second sleep
			if (!(pChannel->statusReg & command)) {
				printf("dds_write_mem: timed out at page %d.\n", ipage);
				return -1;
			}
		}
	}
	return 0;
}

// upload waveforms or sequencer programs to wav.mem or seq.mem file
int dds_upload_prog_wave(size_t slot, size_t channel, DWORD offset4, char *foldername, bool iswave)
{
	DDSChIntf *pChannel = dds_get_ch_intf(slot, channel);
	if (pChannel == NULL) {
		printf("dds_upload_prog_wave: bad slot or channel %d %d\n", slot, channel);
		return -1;
	}

	// file paths
	size_t lenfolder = strlen(foldername);
	if (lenfolder > DDS_NAME_SIZE - 10) {
		printf("Folder name %s too long\n", foldername);
		return -1;
	}
	
	char path[DDS_NAME_SIZE+1];
	char *name = (iswave)? "wav.mem" : "seq.mem";

	path[0] = 0;
	strncat_s(path, DDS_NAME_SIZE, "..\\Debug\\", strlen("..\\Debug\\"));
	strncat_s(path, DDS_NAME_SIZE, foldername, lenfolder);
	strncat_s(path, DDS_NAME_SIZE, "\\", 1);
	strncat_s(path, DDS_NAME_SIZE, name, strlen(name));
	(void)remove(path);
	// open the .mem file for binary write
	FILE *pFile;
	errno_t err = fopen_s(&pFile, (char *)path, "wb+");
	if (err != 0) {
		printf("Failed to create %s, errno=%d\n", path, err);
		return -1;
	}
	
	// allocate memory for the read buffer
	size_t len4 = (iswave)? WaveInfo[slot][channel].length4 : ProgInfo[slot][channel].length4;
	char *pbuf = (char *)malloc(len4 * 4);
	if (pbuf == NULL) {
		printf("malloc %ld failed\n");
		fclose(pFile);
		return -1;
	}
	// read channel memory
	err = dds_read_mem(slot, channel, offset4, len4, (DWORD *)pbuf, iswave);
	if (err != 0) {
		printf("dds_read_mem failed.\n");
		fclose(pFile);
		return -1;
	}
	// write the .mem file in 4 byte units
	size_t n4 = fwrite(pbuf, 4, len4, pFile);
	if (n4 != len4) {
		printf("fwrite failed %ld %ld\n", n4, len4);
		fclose(pFile);
		free(pbuf);
		return -1;
	}
	// clean up and return
	free(pbuf);
	fclose(pFile);
	return 0;
}



// compares two files and prints the first difference
int fcompare(char *path1, char *path2)
{
	FILE *pFile1, *pFile2;
	size_t len1, len2;
	char c1, c2;
	errno_t err;

	err = fopen_s(&pFile1, path1, "rb");	// binary file
	if (err != 0) {
		printf("fcompare: Bad file %s, errno=%d\n", path1, err);
		return -1;
	}
		err = fopen_s(&pFile2, path2, "rb");	// binary file
	if (err != 0) {
		printf("fcompare: Bad file %s, errno=%d\n", path2, err);
		return -1;
	}
	// compare the file sizes
	fseek (pFile1 , 0 , SEEK_END);
	len1 = ftell (pFile1);
	rewind (pFile1);
	fseek (pFile2 , 0 , SEEK_END);
	len2 = ftell (pFile2);
	rewind (pFile2);
	if (len1 != len2) {
		printf("fcompare: File size mismatch: 0x%x 0x%x\n", len1, len2);
		fclose(pFile1);
		fclose(pFile2);
		return -1;
	}
	// read one byte and compare
	for (size_t i = 0; i < len1; i++) {
		size_t n1 = fread(&c1, 1, 1, pFile1);
		size_t n2 = fread(&c2, 1, 1, pFile2);
		if ((n1 != n2) || (c1 != c2)) {
			printf("fcompare: mismatch at %u: 0x%x != 0x%x\n", i, c1, c2);
			fclose(pFile1);
			fclose(pFile2);
			return -1;
		}
	}	
	fclose(pFile1);
	fclose(pFile2);
	return 0;
}

// start a sequencer program

int dds_start_seq(size_t slot, size_t channel, char *name)
{
	if (dds_get_ch_intf(slot, channel) == NULL) {
		printf("dds_seq_start: bad slot or channel %d %d\n", slot, channel);
		return -1;
	}

	size_t addr4 = dds_prog_addr(slot, channel, name);
	if (addr4 == NULL) {
		printf("dds_seq_start: bad name %s\n", name);
		return -1;
	}
	return dds_command(slot, channel, DDS_CH_START);
}


// returns DDS memory offset4 of the named program, or DDS_INVALID_OFFSET4
size_t dds_prog_addr(size_t slot, size_t channel, char *name)
{
	DDSChIntf *pChannel = dds_get_ch_intf(slot, channel);
	if (pChannel == NULL) {
		printf("dds_prog_addr: bad slot or channel %d %d\n", slot, channel);
		return -1;
	}
	size_t n = ProgInfo[slot][channel].nEntry;
	for (size_t i = 0; i < n; i++) {
		if (strncmp(ProgInfo[slot][channel].entry[i].name, name, DDS_NAME_SIZE) == 0)
			 return ProgInfo[slot][channel].entry[i].offset4;
	}
	return DDS_INVALID_OFFSET4;
}
*/
/*
int fcompare(char *path1, char *path2);
int dds_check_slot_channel(size_t slot, size_t channel);
size_t dds_prog_addr(size_t slot, size_t channel, char *name);
*/