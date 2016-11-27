#include <string.h>
#include <string>
#include <stdio.h>
#include "bsd_string.h"

#include "genesis.h"
#include "cmdbase.h"
#include "extio.h"

#define LOG_ERR(...) {fprintf(stderr,__VA_ARGS__);}
#define LOG_INFO(...) {fprintf(stderr,__VA_ARGS__);}

#ifdef DEBUG
#define LOG_DEBUG(...) {fprintf(stderr,__VA_ARGS__);}
#else
#define LOG_DEBUG(...) {}
#endif

//#define DO_ANNOYING
#ifdef DO_ANNOYING
#define LOG_ANNOYING(...) {fprintf(stderr,__VA_ARGS__);fflush(stderr);}
#else
#define LOG_ANNOYING(...) {}
#endif

static Genesis *mp_genesis = NULL;

static char m_name_str[256];
static char m_model_str[256];
static char m_mode = '\0';
static long m_tune_freq = 1800000;
static long m_lo_freq = 1800000;
//Winrad callback pointer
//void extIOCallback(int cnt, int status, float IQoffs, short IQdata[])
static void (* mp_callback)(int, int, float, void *) = NULL;

class extio_observer : public Genesis_Observer
{
public:
    extio_observer(Genesis *p_genesis)
        : Genesis_Observer()
    {
        LOG_ANNOYING("%s:%d\n",__FUNCTION__,__LINE__);
        if(NULL != p_genesis)
        {
            LOG_ANNOYING("%s:%d this=%p\n",__FUNCTION__,__LINE__,this);
            p_genesis->register_observer(this);
        }
    };

    virtual void OnIdle()
    {
        LOG_ANNOYING("%s:%d\n",__FUNCTION__,__LINE__);
        if(NULL != mp_callback)
        {
            //idle timeout, semi-breakin
            //request rx
            LOG_ANNOYING("%s:%d ecsRXRequest\n",__FUNCTION__,__LINE__);
            mp_callback(-1,ecsRXRequest,0.0,NULL);
        }
    };

    virtual void OnTX(bool on_off)
    {
        LOG_ANNOYING("%s:%d\n",__FUNCTION__,__LINE__);
        if(NULL != mp_callback)
        {
            if (on_off)
            {
                //request tx
                LOG_ANNOYING("%s:%d ecsTXRequest\n",__FUNCTION__,__LINE__);
                mp_callback(-1,ecsTXRequest,0.0,NULL);
            }
            else
            {
                //request rx
                LOG_ANNOYING("%s:%d ecsRXRequest\n",__FUNCTION__,__LINE__);
                mp_callback(-1,ecsRXRequest,0.0,NULL);
            }
        }
    };

    virtual void OnDOT(bool on_off)
    {
        LOG_ANNOYING("%s:%d\n",__FUNCTION__,__LINE__);
        if(NULL != mp_callback)
        {
            if (on_off)
            {
                //request dot on
                LOG_DEBUG("%s:%d ecsDotOnRequest\n",__FUNCTION__,__LINE__);
                mp_callback(-1,ecsDotOnRequest,0.0,NULL);
            }
            else
            {
                //request dot off
                LOG_ANNOYING("%s:%d ecsDotOffRequest\n",__FUNCTION__,__LINE__);
                mp_callback(-1,ecsDotOffRequest,0.0,NULL);
            }
        }
    };

    virtual void OnDASH(bool on_off)
    {
        LOG_ANNOYING("%s:%d\n",__FUNCTION__,__LINE__);
        if(NULL != mp_callback)
        {
            if (on_off)
            {
                //request dash on
                LOG_ANNOYING("%s:%d ecsDashOnRequest\n",__FUNCTION__,__LINE__);
                mp_callback(-1,ecsDashOnRequest,0.0,NULL);
            }
            else
            {
                //request dash off
                LOG_ANNOYING("%s:%d ecsDashOffRequest\n",__FUNCTION__,__LINE__);
                mp_callback(-1,ecsDashOffRequest,0.0,NULL);
            }
        }
    };
};

static extio_observer *mp_extio_observer = NULL;

extern "C"
{
//Foward Decl
int SetHWLO(long LOfreq);
void TuneChanged(long freq);

void __stdcall __declspec(dllexport) ShowGUI()
{
    LOG_DEBUG("%s:%d\n",__FUNCTION__,__LINE__);
}
bool __stdcall __declspec(dllexport) InitHW(char *name, char *model, int& type)
{
    fprintf(stderr,"%s:%d\n",__FUNCTION__,__LINE__);
    bool rtn = false;
    type = 0;
    name = NULL;
    model = NULL;

    //try G59
    mp_genesis =  new G59();
    if (rtn = mp_genesis->Init())
    {
        strlcpy(m_name_str, mp_genesis->GetMake().c_str(), sizeof(m_name_str));
        strlcpy(m_model_str, mp_genesis->GetMake().c_str(), sizeof(m_model_str));
        name = m_name_str;
        model = m_model_str;
        type = 4;
    }
    else
    {
        //get rid of G59 instance
        if (NULL != mp_genesis)
        {
            delete mp_genesis;
            mp_genesis = NULL;
        }

        //try G11
        mp_genesis =  new G11();
        if (rtn = mp_genesis->Init())
        {
            strlcpy(m_name_str, mp_genesis->GetMake().c_str(), sizeof(m_name_str));
            strlcpy(m_model_str, mp_genesis->GetMake().c_str(), sizeof(m_model_str));
            name = m_name_str;
            model = m_model_str;
            type = 4;
        }
        else
        {
            //get rid of G11 instance
            if (NULL != mp_genesis)
            {
                delete mp_genesis;
                mp_genesis = NULL;
            }
        }
    }

    if (NULL != mp_genesis)
    {
        mp_extio_observer = new extio_observer(mp_genesis);
    }
    return rtn;

}
bool __stdcall __declspec(dllexport) OpenHW()
{
    LOG_DEBUG("%s:%d\n",__FUNCTION__,__LINE__);
    SetHWLO(m_lo_freq);
    return true;
}
void __stdcall __declspec(dllexport) CloseHW()
{
    LOG_DEBUG("%s:%d\n",__FUNCTION__,__LINE__);
    if(NULL != mp_genesis)
    {
        mp_genesis->Close();
        delete mp_genesis;
        mp_genesis = NULL;
    }
}
int __stdcall __declspec(dllexport) StartHW(long freq)
{
    LOG_DEBUG("%s:%d new\n",__FUNCTION__,__LINE__);
//  if(NULL != mp_genesis)
//  {
//      SetHWLO(freq);
//      TuneChanged(freq);
//  }
    return 0;
}
void __stdcall __declspec(dllexport) StopHW()
{
    LOG_DEBUG("%s:%d new\n",__FUNCTION__,__LINE__);
//  if(NULL != mp_genesis)
//  {
//      mp_genesis->Close();
//  }
}
int __stdcall __declspec(dllexport) SetHWLO(long LOfreq)
{
    LOG_DEBUG("%s:%d LOfreq: %ld\n",__FUNCTION__,__LINE__, LOfreq);
    if(NULL != mp_genesis)
    {
        mp_genesis->SetLO(LOfreq);
        m_lo_freq = LOfreq;
    }
    else
    {
        LOG_DEBUG("%s:%d ERROR: mp_genesis is NULL\n",__FUNCTION__,__LINE__);
    }
    return 0;
}
int __stdcall __declspec(dllexport) GetStatus()
{
    LOG_DEBUG("%s:%d\n",__FUNCTION__,__LINE__);
    return 0;
}
void __stdcall __declspec(dllexport) SetCallback(void (* Callback)(int, int, float, void *))
{
    fprintf(stderr,"%s:%d\n",__FUNCTION__,__LINE__);
    if (NULL != Callback)
    {
        mp_callback = Callback;
    }
}
// ext routs
long __stdcall __declspec(dllexport) GetHWLO()
{
    LOG_DEBUG("%s:%d\n",__FUNCTION__,__LINE__);
    return m_lo_freq;
}
void __stdcall __declspec(dllexport) TuneChanged(long freq)
{
    LOG_DEBUG("%s:%d freq: %ld\n",__FUNCTION__,__LINE__, freq);
    if(NULL != mp_genesis)
    {
        mp_genesis->SetBand(freq);
        m_tune_freq = freq;
    }
    else
    {
        LOG_DEBUG("%s:%d ERROR: mp_genesis is NULL\n",__FUNCTION__,__LINE__);
    }
}
void __stdcall __declspec(dllexport) IFLimitsChanged(long low, long high)
{
    long LOfreq = (low + high)/2;
    LOG_DEBUG("%s:%d low: %ld, high: %ld, LO = %ld\n",__FUNCTION__,__LINE__, low, high, LOfreq);
    if(NULL != mp_genesis)
    {
        if((0 < LOfreq) && (LOfreq != m_lo_freq))
        {
            mp_genesis->SetLO(LOfreq);
            m_lo_freq = LOfreq;
        }
    }
    else
    {
        LOG_DEBUG("%s:%d ERROR: mp_genesis is NULL\n",__FUNCTION__,__LINE__);
    }
}
long __stdcall __declspec(dllexport) GetTune()
{
    LOG_DEBUG("%s:%d\n",__FUNCTION__,__LINE__);
    return m_tune_freq;
}
char __stdcall __declspec(dllexport) GetMode()
{
    LOG_DEBUG("%s:%d\n",__FUNCTION__,__LINE__);
    return m_mode;
}
void __stdcall __declspec(dllexport) ModeChanged(char mode)
{
    m_mode = mode;
    LOG_DEBUG("%s:%d %c\n",__FUNCTION__,__LINE__,mode);
}
long __stdcall __declspec(dllexport) GetHWSR()
{
    LOG_DEBUG("%s:%d\n",__FUNCTION__,__LINE__);
    return 0;
}
void __stdcall __declspec(dllexport) HideGUI()
{
    LOG_DEBUG("%s:%d\n",__FUNCTION__,__LINE__);
}
void __stdcall __declspec(dllexport) RawDataReady(long samprate, int *Ldata, int *Rdata, int numsamples)
{
    LOG_DEBUG("%s:%d samprate:%ld, numsamples:%d\n",__FUNCTION__,__LINE__, samprate, numsamples);
}
void __stdcall __declspec(dllexport) GetFilters(int& loCut, int& hiCut, int& pitch)
{
    LOG_DEBUG("%s:%d\n",__FUNCTION__,__LINE__);
}

int __stdcall __declspec(dllexport) SetModeRxTx(int modeRxTx)
{
    LOG_DEBUG("%s:%d modeRxTx: %d\n",__FUNCTION__,__LINE__, modeRxTx);
    if(NULL != mp_genesis)
    {
        mp_genesis->SetTx((0!=modeRxTx?true:false));
    }
    else
    {
        LOG_DEBUG("%s:%d ERROR: mp_genesis is NULL\n",__FUNCTION__,__LINE__);
    }
    return 0;
}
int __stdcall __declspec(dllexport) ActivateTx(int magicA, int magicB)
{
    LOG_DEBUG("%s:%d magicA:%d, int magicB:%d\n",__FUNCTION__,__LINE__,magicA, magicB);
    return 0;
}
void __stdcall __declspec(dllexport) VersionInfo(const char *name, int ver_major, int ver_minor)
{
    LOG_DEBUG("%s:%d name:\"%s\", ver_major:%d, ver_minor:%d\n",__FUNCTION__,__LINE__, name, ver_major, ver_minor);
}

void __stdcall __declspec(dllexport) SetPreamp(int db)
{
    LOG_DEBUG("%s:%d db=%d\n",__FUNCTION__,__LINE__, db);
    if(NULL != mp_genesis)
    {
        if (0 > db)
        {
            mp_genesis->SetAtten(true);
            mp_genesis->SetRFPreamp(false);
        }
        else if ( 0 < db)
        {
            mp_genesis->SetAtten(false);
            mp_genesis->SetRFPreamp(true);
        }
        else
        {
            mp_genesis->SetAtten(false);
            mp_genesis->SetRFPreamp(false);
        }
    }
    else
    {
        LOG_DEBUG("%s:%d ERROR: mp_genesis is NULL\n",__FUNCTION__,__LINE__);
    }
}

void __stdcall __declspec(dllexport) SetWpm(int wpm)
{
    LOG_DEBUG("%s:%d wpm=%d\n",__FUNCTION__,__LINE__, wpm);
    if(NULL != mp_genesis)
    {
        mp_genesis->SetWpm(wpm);
    }
    else
    {
        LOG_DEBUG("%s:%d ERROR: mp_genesis is NULL\n",__FUNCTION__,__LINE__);
    }
}

void __stdcall __declspec(dllexport) EnableLineMic(int onoff)
{
    if(NULL != mp_genesis)
    {
        mp_genesis->EnableLineMic((0 == onoff)?false:true);
    }
    else
    {
        LOG_DEBUG("%s:%d ERROR: mp_genesis is NULL\n",__FUNCTION__,__LINE__);
    }
}

void __stdcall __declspec(dllexport) EnablePA(int onoff)
{
    if(NULL != mp_genesis)
    {
        mp_genesis->EnablePA10((0 == onoff)?false:true);
    }
    else
    {
        LOG_DEBUG("%s:%d ERROR: mp_genesis is NULL\n",__FUNCTION__,__LINE__);
    }
}

void __stdcall __declspec(dllexport) SetKeyerRatio(float ratio_dot_to_dash)
{
    if(NULL != mp_genesis)
    {
        mp_genesis->SetKeyerRatio(ratio_dot_to_dash);
    }
    else
    {
        LOG_DEBUG("%s:%d ERROR: mp_genesis is NULL\n",__FUNCTION__,__LINE__);
    }
}

void __stdcall __declspec(dllexport) SetKeyerMode(int keyer_mode)
{
    if(NULL != mp_genesis)
    {
        mp_genesis->SetKeyerMode(keyer_mode);
    }
    else
    {
        LOG_DEBUG("%s:%d ERROR: mp_genesis is NULL\n",__FUNCTION__,__LINE__);
    }
}

} /* extern "C" */
