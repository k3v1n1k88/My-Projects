#pragma once
#include "afxsock.h"


 // 
   // HEADER FILE
   // 
   class CTimeOutSocket : public CSocket
   {
   public:
     BOOL SetTimeOut(UINT uTimeOut);
     BOOL KillTimeOut();
   protected:
     virtual BOOL OnMessagePending();
   private:
     int m_nTimerID;
   };
   // 
   // END OF FILE
   // 


   

