#ifndef EXTIO_H_
#define  EXTIO_H_
extern "C"
{
#include <stdbool.h>
enum ExtIOCallbackStatus {
    // for all extIO's
    ecsChangedSampleRate    = 100   // sampling speed has changed in the external HW
    , ecsChangedLO          = 101   // LO frequency has changed in the external HW
    , ecsLockLO             = 102
    , ecsUnlockLO           = 103
    , ecsChangedLONotTUNE   = 104   // CURRENTLY NOT YET IMPLEMENTED
    // LO freq. has changed, Winrad must keep the Tune freq. unchanged
    // (must immediately call GetHWLO() )
    , ecsChangedTUNE        = 105   // a change of the Tune freq. is being requested.
    // Winrad must call GetTune() to know which value is wanted
    , ecsChangedMODE        = 106   // a change of demod. mode is being requested.
    // Winrad must call GetMode() to know the new mode
    , ecsStart              = 107   // The DLL wants Winrad to Start
    , ecsStop               = 108   // The DLL wants Winrad to Stop
    , ecsChangedFILTER      = 109   // a change in the band limits is being requested
    // Winrad must call GetFilters()

    , ecsTXRequest          = 120   // DLL requests TX mode / User pressed PTT
    //   exciter/transmitter must wait until SetModeRxTx() is called!
    , ecsRXRequest          = 121   // DLL wants to leave TX mode / User released PTT
    //   exciter/transmitter must wait until SetModeRxTx() is called!

    , ecsDotOnRequest       = 520   // DLL requests Dot start mode / User pressed Dot Paddle or Straight key
    , ecsDotOffRequest      = 521   // DLL wants to leave Dot start mode / User released Dot Paddle or Straight key
    , ecsDashOnRequest      = 522   // DLL requests Dash start mode / User pressed Dash Paddle or Straight key
    , ecsDashOffRequest     = 523   // DLL wants to leave Dash start mode / User released Dash Paddle or Straight key
};
void __stdcall __declspec(dllexport) ShowGUI();
bool __stdcall __declspec(dllexport) InitHW(char *name, char *model, int& type);
bool __stdcall __declspec(dllexport) OpenHW();
void __stdcall __declspec(dllexport) CloseHW();
int __stdcall __declspec(dllexport) StartHW(long freq);
void __stdcall __declspec(dllexport) StopHW();
int __stdcall __declspec(dllexport) SetHWLO(long LOfreq);
int __stdcall __declspec(dllexport) GetStatus();
void __stdcall __declspec(dllexport) SetCallback(void (* Callback)(int, int, float, void *));
// ext routs
long __stdcall __declspec(dllexport) GetHWLO();
void __stdcall __declspec(dllexport) TuneChanged(long freq);
void __stdcall __declspec(dllexport) IFLimitsChanged(long low, long high);
long __stdcall __declspec(dllexport) GetTune();
char __stdcall __declspec(dllexport) GetMode();
void __stdcall __declspec(dllexport) ModeChanged(char mode);
long __stdcall __declspec(dllexport) GetHWSR();
void __stdcall __declspec(dllexport) HideGUI();
void __stdcall __declspec(dllexport) RawDataReady(long samprate, int *Ldata, int *Rdata, int numsamples);
void __stdcall __declspec(dllexport) GetFilters(int& loCut, int& hiCut, int& pitch);
// Genesis routs
void __stdcall __declspec(dllexport) EnableLineMic(int onoff);
void __stdcall __declspec(dllexport) EnablePA(int onoff);
void __stdcall __declspec(dllexport) SetKeyerRatio(float ratio_dot_to_dash);
void __stdcall __declspec(dllexport) SetKeyerMode(int keyer_mode);
void __stdcall __declspec(dllexport) SetWpm(int wpm);

//
int __stdcall __declspec(dllexport) SetModeRxTx(int modeRxTx);
int __stdcall __declspec(dllexport) ActivateTx(int magicA, int magicB);
void __stdcall __declspec(dllexport) VersionInfo(const char *name, int ver_major, int ver_minor);
void __stdcall __declspec(dllexport) SetPreamp(int db);
} /* extern "C" */
#endif /*  EXTIO_H_ */
