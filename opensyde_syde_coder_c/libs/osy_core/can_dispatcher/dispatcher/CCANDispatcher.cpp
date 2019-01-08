//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       CAN dispatcher class

   Provides a standardized reception mechanism for incoming CAN messages.
   Multiple clients can register in order to receive all incoming CAN traffic.
   Incoming messages will be reported in an installable queue.
   This class also allows to add RX-fiters.

   \implementation
   project     KEFEX
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     14.04.2010  STW/A.Stangl
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------ */
#include "precomp_headers.h" //pre-compiled headers
#ifdef __BORLANDC__          //putting the pragmas in the config-header will not work
#pragma hdrstop
#pragma package(smart_init)
#endif

#include "stwtypes.h"
#include "stwerrors.h"
#include "CCANDispatcher.h"

using namespace stw_types;
using namespace stw_errors;
using namespace stw_can;

/* -- Defines ------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------- */

/* -- Implementation ------------------------------------------------------ */

//-----------------------------------------------------------------------------
/*!
   \brief   constructor

   Init instance.

   \created     14.04.2010  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_CAN_RXFilter::C_CAN_RXFilter(void)
{
   this->PassAll(); //default: let all messages pass
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set filters to let all messages pass.

   Shortcut utility: Set filters to let all messages pass.

   \created     14.04.2010  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
void C_CAN_RXFilter::PassAll(void)
{
   u32_Code = 0U;
   u32_Mask = 0U;

   q_XTDMustMatch = false;
   q_RTRMustMatch = false;

   q_XTD = false;
   q_RTR = false;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set filters to let one ID pass.

   Shortcut utility: Set filters to let exactly one ID pass.

   \param[in]   ou32_ID   ID to let pass
   \param[in]   oq_ExtID  true  -> ID must be 29bit ID
                          false -> ID must be 11bit ID
   \param[in]   oq_RTR    true  -> ID must be an RTR ID
                          false -> ID must not be an RTR ID

   \created     14.04.2010  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
void C_CAN_RXFilter::PassOneID(const uint32 ou32_ID, const bool oq_ExtID, const bool oq_RTR)
{
   u32_Code = ou32_ID;
   u32_Mask = 0xFFFFFFFFUL;

   q_XTD = oq_ExtID;
   q_XTDMustMatch = true;
   q_RTR = oq_RTR;
   q_RTRMustMatch = true;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check whether message can pass filter

   Check whether CAN message can pass the configured filter.

   \param[in]   orc_Message  message to check

   \return
   true    -> message can pass filter      \n
   false   -> message can not pass filter

   \created     14.04.2010  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
bool C_CAN_RXFilter::DoesMessagePass(const T_STWCAN_Msg_RX & orc_Message) const
{
   bool q_Pass;

   q_Pass  = ((orc_Message.u32_ID & u32_Mask) == u32_Code) ? true : false;
   q_Pass = q_Pass && ((q_XTDMustMatch == false) || (static_cast<uint8>(q_XTD) == orc_Message.u8_XTD));
   q_Pass = q_Pass && ((q_RTRMustMatch == false) || (static_cast<uint8>(q_RTR) == orc_Message.u8_RTR));
   return q_Pass;
}

//-----------------------------------------------------------------------------
/*!
   \brief   constructor

   Init instance.

   \created     14.04.2010  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_CAN_RXQueue::C_CAN_RXQueue(void)
{
   //set a default that should be fine for many application cases:
   mu32_MaxSize = mu32_CAN_QUEUE_DEFAULT_MAX_SIZE;
   ms32_Status = C_NO_ERR;
}

//-----------------------------------------------------------------------------
/*!
   \brief   add element to queue

   Add one more element to queue.

   \param[in]   orc_Message   message to add

   \return
   C_NO_ERR    element added                                 \n
   C_OVERFLOW  maximum configured size reached -> not added  \n
   C_NOACT     could not add new element -> not added

   \created     14.04.2010  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_CAN_RXQueue::Push(const T_STWCAN_Msg_RX & orc_Message)
{
   sint32 s32_Return = C_NO_ERR;

   if (mc_Messages.size() >= mu32_MaxSize)
   {
      s32_Return = C_OVERFLOW;
   }
   else
   {
      try
      {
         mc_Messages.push_back(orc_Message);
      }
      catch (...)
      {
         s32_Return = C_NOACT; //probably out of memory
      }
   }
   ms32_Status = s32_Return;
   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   get oldest element from queue

   Get oldest element from queue.

   \param[out]   orc_Message   read message

   \return
   C_NO_ERR    element read                            \n
   C_NOACT     no element available

   \created     14.04.2010  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_CAN_RXQueue::Pop(T_STWCAN_Msg_RX & orc_Message)
{
   sint32 s32_Return = C_NO_ERR;

   if (mc_Messages.size() < 1U)
   {
      s32_Return = C_NOACT;
   }
   else
   {
      orc_Message = mc_Messages.front(); //get element from queue
      mc_Messages.pop_front();           //delete element from queue
   }
   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set maximum queue size

   Set the maximum possible number of queue entries.
   If the queue already contains more entries than the new maximum value it is downsized.

   \param[in]   ou32_MaxSize  maximum number of entries in queue

   \created     14.04.2010  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
void C_CAN_RXQueue::SetMaxSize(const uint32 ou32_MaxSize)
{
   mu32_MaxSize = ou32_MaxSize;
   if (mc_Messages.size() > mu32_MaxSize)
   {
      mc_Messages.resize(mu32_MaxSize);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get maximum queue size

   Get the configured maximum possible number of queue entries.

   \return
   Configured maximum

   \created     14.04.2010  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
uint32 C_CAN_RXQueue::GetMaxSize(void) const
{
   return mu32_MaxSize;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get actual queue size

   Get the actual queue size (i.e. number of entries in the queue).

   \return
   Actual queue size

   \created     14.04.2010  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
uint32 C_CAN_RXQueue::GetSize(void) const
{
   return static_cast<uint32>(mc_Messages.size());
}

//-----------------------------------------------------------------------------
/*!
   \brief   Clear all queue entries

   Clear all queue entries.

   \created     14.04.2010  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
void C_CAN_RXQueue::Clear(void)
{
   mc_Messages.clear();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get queue status

   Read queue status.
   Will reset status to C_NO_ERR.

   \return
   C_NO_ERR   -> no problems          \n
   C_OVERFLOW -> overflow in queue    \n
   C_NOACT    -> internal problem (e.g. out of memory)

   \created     14.04.2010  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_CAN_RXQueue::GetStatus(void)
{
   sint32 s32_Return = ms32_Status;

   ms32_Status = C_NO_ERR;
   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   constructor

   Init instance.

   \created     14.04.2010  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_CAN_Dispatcher::C_CAN_Dispatcher(void) :
   C_CAN_Base()
{
   //nothing to do yet
}

//-----------------------------------------------------------------------------
/*!
   \brief   constructor

   Init instance.

   \param[in]  ou8_CommChannel  communication driver channel

   \created     14.04.2010  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_CAN_Dispatcher::C_CAN_Dispatcher(const uint8 ou8_CommChannel) :
   C_CAN_Base(ou8_CommChannel)
{
   //nothing to do yet
}

//-----------------------------------------------------------------------------
/*!
   \brief   destructor

   Clean up instance.

   \created     14.04.2010  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_CAN_Dispatcher::~C_CAN_Dispatcher(void)
{
   //nothing more than base class destrcutor to do yet ...
   //RX-queue will clean up itself
}

//-----------------------------------------------------------------------------
/*!
   \brief   Reception handler call.

   Check for incoming messages apply RX filters and place messages that pass into registered RX FIFOs.
   Can be called from one central point. But calling it from multiple positions will also not hurt
   (e.g. for one client doing synchronous communication).

   \return
   Number of newly received messages. Caution: unless the caller can be sure it is the only one it shall not rely on
   the return value to report newly received messages as some other client could have called DispatchIncoming()
   in the meantime resulting in messages being placed in the registered RX-Queue even though this function returns 0.

   \created     14.04.2010  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_CAN_Dispatcher::DispatchIncoming(void)
{
   T_STWCAN_Msg_RX t_Msg;
   sint32 s32_Return = C_NO_ERR;
   sint32 s32_NumMessages = 0;
   sint32 s32_Loop;

   while (s32_Return == C_NO_ERR)
   {
      s32_Return = m_CAN_Read_Msg(t_Msg);
      if (s32_Return == C_NO_ERR)
      {
         s32_NumMessages++;
         for (s32_Loop = 0; s32_Loop < mc_InstalledClients.GetLength(); s32_Loop++)
         {
            if (mc_InstalledClients[s32_Loop].c_RXFilter.DoesMessagePass(t_Msg) == true)
            {
               mc_CriticalSection.Acquire();
               (void)mc_InstalledClients[s32_Loop].c_RXQueue.Push(t_Msg);
               mc_CriticalSection.Release();
            }
         }
      }
   }

   return s32_NumMessages;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Register RX client.

   Register interest in received messages. Optionally an RX filter can be specified.
   A queue for the registered client will be added to the list maintained in this class.

   The registered queue will be added to the list maintained in this class.
   Subsequent incoming messages detected in DispatchIncoming() will be added to all registered queues.

   \param[out]    oru16_Handle      Handle to RX queue to be used in subsequent actions
   \param[in]     opc_RXFilter      RX filter configuration (optional)
   \param[in]     oru32_BufferSize  size of buffer for this client

   \return
   C_NO_ERR   -> installed (or reconfigured)
   C_OVERFLOW -> too many clients installed

   \created     14.04.2010  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_CAN_Dispatcher::RegisterClient(uint16 & oru16_Handle, const C_CAN_RXFilter * const opc_RXFilter,
                                        const uint32 & oru32_BufferSize)
{
   uint16 u16_Handle;
   bool q_Found = false;

   if (mc_InstalledClients.GetHigh() >= 0xFFFF)
   {
      return C_OVERFLOW;
   }

   mc_InstalledClients.IncLength(1);
   if (opc_RXFilter != NULL)
   {
      mc_InstalledClients[mc_InstalledClients.GetHigh()].c_RXFilter = *opc_RXFilter;
   }
   else
   {
      mc_InstalledClients[mc_InstalledClients.GetHigh()].c_RXFilter.PassAll();
   }
   mc_InstalledClients[mc_InstalledClients.GetHigh()].c_RXQueue.SetMaxSize(oru32_BufferSize);

   //is there a free one ?
   for (u16_Handle = 0U; u16_Handle < mc_ClientsByHandle.GetLength(); u16_Handle++)
   {
      if (mc_ClientsByHandle[u16_Handle] == NULL)
      {
         q_Found = true;
         break;
      }
   }

   if (q_Found == true)
   {
      //insert here !
      mc_InstalledClients[mc_InstalledClients.GetHigh()].u16_Handle = u16_Handle;
   }
   else
   {
      mc_InstalledClients[mc_InstalledClients.GetHigh()].u16_Handle =
         static_cast<uint16>(mc_ClientsByHandle.GetLength());
   }

   m_ResyncShortcutPointers();
   oru16_Handle = u16_Handle;
   return C_NO_ERR;
}

//-----------------------------------------------------------------------------
/*!
   \brief   De-register RX client.

   Remove client from list of registered clients.

   \param[in]  ou16_Handle   Handle of the client to remove.

   \return
   C_NO_ERR   -> reference to client removed      \n
   C_NOACT    -> client not found -> not removed

   \created     14.04.2010  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_CAN_Dispatcher::RemoveClient(const uint16 ou16_Handle)
{
   uint16 u16_Index;

   if (ou16_Handle >= mc_ClientsByHandle.GetLength())
   {
      return C_NOACT;
   }
   if (mc_ClientsByHandle[ou16_Handle] == NULL)
   {
      //nothing installed there ...
      return C_NOACT;
   }

   //find it in the list:
   for (u16_Index = 0U; u16_Index < mc_InstalledClients.GetLength(); u16_Index++)
   {
      if (mc_ClientsByHandle[ou16_Handle] == &mc_InstalledClients[u16_Index])
      {
         mc_InstalledClients.Delete(u16_Index);
         break;
      }
   }
   m_ResyncShortcutPointers();
   return C_NO_ERR;
}

//-----------------------------------------------------------------------------

void C_CAN_Dispatcher::m_ResyncShortcutPointers(void)
{
   uint16 u16_Index;
   uint16 u16_Handle;
   uint16 u16_Max;

   if (mc_InstalledClients.GetLength() == 0)
   {
      mc_ClientsByHandle.SetLength(0);
      return;
   }

   //get greatest handle number:
   u16_Max = 0U;
   for (u16_Index = 0U; u16_Index < mc_InstalledClients.GetLength(); u16_Index++)
   {
      if (mc_InstalledClients[u16_Index].u16_Handle > u16_Max)
      {
         u16_Max = mc_InstalledClients[u16_Index].u16_Handle;
      }
   }
   mc_ClientsByHandle.SetLength(static_cast<sint32>(u16_Max) + 1);

   //preset all pointers to zero:
   for (u16_Handle = 0U; u16_Handle < mc_ClientsByHandle.GetLength(); u16_Handle++)
   {
      mc_ClientsByHandle[u16_Handle] = NULL;
   }

   for (u16_Index = 0U; u16_Index < mc_InstalledClients.GetLength(); u16_Index++)
   {
      //find client with this handle
      u16_Handle = mc_InstalledClients[u16_Index].u16_Handle;
      if (u16_Handle > mc_ClientsByHandle.GetHigh())
      {
         return; //internal problem
      }
      mc_ClientsByHandle[u16_Handle] = &mc_InstalledClients[u16_Index];
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Read message from queue

   Pop newest message from queue.
   Will not dispatch incoming messages.

   \param[in]  ou16_Handle   Handle of the client
   \param[out] orc_Message   new message


   \return
   C_NO_ERR   -> reference message read      \n
   C_RANGE    -> invalid ou16_Handle         \n
   C_NOACT    -> no new message

   \created     06.05.2010  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_CAN_Dispatcher::ReadFromQueue(const uint16 ou16_Handle, T_STWCAN_Msg_RX & orc_Message)
{
   sint32 s32_Return;

   if (ou16_Handle >= mc_ClientsByHandle.GetLength())
   {
      return C_RANGE;
   }
   if (mc_ClientsByHandle[ou16_Handle] == NULL)
   {
      return C_RANGE;
   }

   mc_CriticalSection.Acquire();
   s32_Return = mc_ClientsByHandle[ou16_Handle]->c_RXQueue.Pop(orc_Message);
   mc_CriticalSection.Release();
   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Clear receive queue

   Dump all messages of receive queue

   \param[in]  ou16_Handle   Handle of the client

   \return
   C_NO_ERR   -> all queue elements dumped   \n
   C_RANGE    -> invalid ou16_Handle

   \created     16.03.2017  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_CAN_Dispatcher::ClearQueue(const uint16 ou16_Handle)
{
   sint32 s32_Return = C_RANGE;

   if ((ou16_Handle < mc_ClientsByHandle.GetLength()) &&
       (mc_ClientsByHandle[ou16_Handle] != NULL))
   {
      s32_Return = C_NO_ERR;
      mc_CriticalSection.Acquire();
      mc_ClientsByHandle[ou16_Handle]->c_RXQueue.Clear();
      mc_CriticalSection.Release();
   }
   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Read message

   Dispatch incoming messages then pop newest message from queue.

   \param[in]  ou16_Handle   Handle of the client
   \param[out] orc_Message   new message


   \return
   C_NO_ERR   -> reference message read      \n
   C_RANGE    -> invalid ou16_Handle         \n
   C_WARN     -> no new message

   \created     06.05.2010  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_CAN_Dispatcher::CAN_Read_Msg(const uint16 ou16_Handle, T_STWCAN_Msg_RX & orc_Message)
{
   this->DispatchIncoming();
   return ReadFromQueue(ou16_Handle, orc_Message);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Read message

   Dispatch a maximum of one incoming message then return it.
   This function is useful for clients that did not explicitely install a queue as they are strictly sequential.

   \param[out] orc_Message   new message

   \return
   C_NO_ERR   -> reference message read      \n
   C_WARN     -> no new message

   \created     12.05.2010  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_CAN_Dispatcher::CAN_Read_Msg(T_STWCAN_Msg_RX & orc_Message)
{
   sint32 s32_Return;
   sint32 s32_Loop;

   s32_Return = m_CAN_Read_Msg(orc_Message);
   if (s32_Return == C_NO_ERR)
   {
      //dispatch to installed clients:
      for (s32_Loop = 0; s32_Loop < mc_InstalledClients.GetLength(); s32_Loop++)
      {
         if (mc_InstalledClients[s32_Loop].c_RXFilter.DoesMessagePass(orc_Message) == true)
         {
            mc_CriticalSection.Acquire();
            (void)mc_InstalledClients[s32_Loop].c_RXQueue.Push(orc_Message);
            mc_CriticalSection.Release();
         }
      }
   }
   else
   {
      s32_Return = C_WARN;
   }

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set a new filter configuration.

   Set a filter to be used for the specified queue.
   The filter will be applied to all subsequently incoming messages.

   \param[in]     ou16_Handle   Client queue to set the filter for.
   \param[in]     orc_RXFilter  RX filter configuration (optional)

   \return
   C_NO_ERR   -> installed (or reconfigured)
   C_RANGE    -> invalid handle

   \created     07.05.2010  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_CAN_Dispatcher::SetRXFilter(const uint16 ou16_Handle, const C_CAN_RXFilter & orc_RXFilter)
{
   if (ou16_Handle >= mc_ClientsByHandle.GetLength())
   {
      return C_RANGE;
   }
   if (mc_ClientsByHandle[ou16_Handle] == NULL)
   {
      return C_RANGE;
   }

   mc_ClientsByHandle[ou16_Handle]->c_RXFilter = orc_RXFilter;
   return C_NO_ERR;
}