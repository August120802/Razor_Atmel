/**********************************************************************************************************************
File: user_app1.c                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
 1. Copy both user_app1.c and user_app1.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app1" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

Description:
This is a user_app1.c file template 

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:


Protected System functions:
void UserApp1Initialize(void)
Runs required initialzation for the task.  Should only be called once in main init section.

void UserApp1RunActiveState(void)
Runs current task state.  Should only be called once in main loop.


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_UserApp1"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                       /* Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */

extern u32 G_u32AntApiCurrentDataTimeStamp;                       
extern AntApplicationMessageType G_eAntApiCurrentMessageClass;   
extern u8 G_au8AntApiCurrentMessageBytes[ANT_APPLICATION_MESSAGE_BYTES];
extern AntExtendedDataType G_sAntApiCurrentMessageExtData;       
/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp_StateMachine;            /* The state machine function pointer */
static u32 UserApp1_u32Timeout;                      /* Timeout counter used across states */
static u32 TIMEOUT_VALUE=3000;
static u32 UserApp_u32DataMsgCount = 0;   /* ANT_DATA packets received */
static u32 UserApp1_u32TickMsgCount = 0;   /* ANT_TICK packets received */

static int a=0;
static u8 u8LastState = 0xff;
  static u8 au8TickMessage[] = "EVENT x\n\r";  /* "x" at index [6] will be replaced by the current code */
  static u8 au8DataContent[] = " ";
 static u8 au8DataContent_B[]= " ";
   static u8 G_au8AntApiCurrentData[] = " ";
  static u8 au8LastAntData[ANT_APPLICATION_MESSAGE_BYTES] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  static u8 au8TestMessage[] = {0, 0, 0, 0, 0xA5, 0, 0, 0};
  bool bGotNewData;
/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
Function: UserApp1Initialize

Description:
Initializes the State Machine and its variables.

Requires:
  -

Promises:
  - 
*/
  void UserApp1Initialize(void)
{

  /* If good initialization, set state to Idle */
 
  
   u8 au8WelcomeMessage[] = "AUTO 401-4 FLOWERS";
  u8 au8Instructions[] = "D.L.G.L";
  
  AntAssignChannelInfoType sAntSetupData;
  
  /* Clear screen and place start messages */
  LCDCommand(LCD_CLEAR_CMD);
  LCDMessage(LINE1_START_ADDR, au8WelcomeMessage); 
  LCDMessage(LINE2_START_ADDR + 10, au8Instructions); 
  LedOff(LCD_RED);
  /* Start with LED0 in RED state = channel is not configured */
  LedOn(RED);
  
 /* Configure ANT for this application */
  sAntSetupData.AntChannel          = ANT_CHANNEL_USERAPP;
  sAntSetupData.AntChannelType      = ANT_CHANNEL_TYPE_USERAPP;
  sAntSetupData.AntChannelPeriodLo  = ANT_CHANNEL_PERIOD_LO_USERAPP;
  sAntSetupData.AntChannelPeriodHi  = ANT_CHANNEL_PERIOD_HI_USERAPP;
  
  sAntSetupData.AntDeviceIdLo       = ANT_DEVICEID_LO_USERAPP;
  sAntSetupData.AntDeviceIdHi       = ANT_DEVICEID_HI_USERAPP;
  sAntSetupData.AntDeviceType       = ANT_DEVICE_TYPE_USERAPP;
  sAntSetupData.AntTransmissionType = ANT_TRANSMISSION_TYPE_USERAPP;
  sAntSetupData.AntFrequency        = ANT_FREQUENCY_USERAPP;
  sAntSetupData.AntTxPower          = ANT_TX_POWER_USERAPP;

  sAntSetupData.AntNetwork = ANT_NETWORK_DEFAULT;
  
  sAntSetupData.AntNetworkKey[0]    = 0xB9;
  sAntSetupData.AntNetworkKey[1]    = 0xA5;
  sAntSetupData.AntNetworkKey[2]    = 0x21;
  sAntSetupData.AntNetworkKey[3]    = 0xFB;
  sAntSetupData.AntNetworkKey[4]    = 0xBD;
  sAntSetupData.AntNetworkKey[5]    = 0x72;
  sAntSetupData.AntNetworkKey[6]    = 0xC3;
  sAntSetupData.AntNetworkKey[7]    = 0x45;

    
  /* If good initialization, set state to Idle */
  if( AntAssignChannel(&sAntSetupData) )
  {
    /* Channel is configured, so change LED to yellow */
    LedOff(RED);
    LedOn(YELLOW);
    UserApp_StateMachine = UserApp1SM_WaitChannelAssign;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    LedBlink(RED, LED_4HZ);
    UserApp_StateMachine = UserApp1SM_Error;
  }

} /* end UserApp1Initialize() */
static void UserApp1SM_WaitChannelAssign(void)          
{
   /* Check if the channel assignment is complete */
  if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_CONFIGURED)
  {
#ifdef EIE1
    LedOff(RED);
    LedOn(YELLOW);
#endif /* EIE1 */
    
#ifdef MPG2
    LedOff(RED0);
    LedOn(GREEN0);
#endif /* MPG2 */

    UserApp_StateMachine = UserAppSM_Idle;
  }
  
  /* Monitor for timeout */
  if( IsTimeUp(&UserApp1_u32Timeout, 5000) )
  {
    DebugPrintf("\n\r***Channel assignment timeout***\n\n\r");
    UserApp_StateMachine = UserApp1SM_Error;
  }
} 
static void UserAppSM_WaitChannelClose(void)          
{
  
}
static void UserAppSM_Idle(void)
{
 /* Look for BUTTON 0 to open channel */
  if(WasButtonPressed(BUTTON0))
  {
    /* Got the button, so complete one-time actions before next state */
    ButtonAcknowledge(BUTTON0);
    
    /* Queue open channel and change LED0 from yellow to blinking green to indicate channel is opening */
    AntOpenChannelNumber(ANT_CHANNEL_USERAPP);

    LedOff(YELLOW);
    LedBlink(GREEN, LED_2HZ);
    
    /* Set timer and advance states */
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp_StateMachine = UserAppSM_WaitChannelOpen;
    
     /* Got the button, so complete one-time actions before next state */ 
   
  }
  
  
} /* end UserApp1SM_Idle() */
  
  

static void UserAppSM_ChannelOpen(void)
{
if( AntReadAppMessageBuffer() )
  {
    static u8 au8CommonDataPage_B[8]   = {0x46, 0xff, 0xff, 0xff, 0xff, 0x80, 0x07, 0x01};
    AntQueueAcknowledgedMessage(ANT_CHANNEL_0, au8CommonDataPage_B);

    
    /* New data message: check what it is */
    if(G_eAntApiCurrentMessageClass == ANT_DATA)
    {
      UserApp_u32DataMsgCount++;
      
      /* We are synced with a device, so blue is solid */
      LedOff(GREEN);
      LedOn(BLUE);

      /* Check if the new data is the same as the old data and update as we go */
      bGotNewData = FALSE;
      for(u8 i = 0; i < ANT_APPLICATION_MESSAGE_BYTES; i++)
      {
        if(G_au8AntApiCurrentMessageBytes[i] != au8LastAntData[i])
        {
          bGotNewData = TRUE;
          

        }
      }
      
      if(bGotNewData)
      {
        /* We got new data: show on LCD */
        NumberToAscii(G_au8AntApiCurrentMessageBytes[7],au8DataContent);
        LCDClearChars(LINE2_START_ADDR, 20); 
        LCDMessage(LINE2_START_ADDR + 3, "cts/m");
        LCDMessage(LINE2_START_ADDR, au8DataContent); 
       
        NumberToAscii(G_au8AntApiCurrentMessageBytes[1], au8DataContent_B);
        LCDMessage(LINE2_START_ADDR + 13, "%");
        LCDMessage(LINE2_START_ADDR + 10, au8DataContent_B); 
        
         LedOn(LCD_RED);
         LedOn(LCD_GREEN);
         LedOn(LCD_BLUE);
        
       
        /* Update our local message counter and send the message back */
        au8TestMessage[7]++;
        if(au8TestMessage[7] == 0)
        {
          au8TestMessage[6]++;
          if(au8TestMessage[6] == 0)
          {
            au8TestMessage[5]++;
          }
        }
        AntQueueBroadcastMessage(ANT_CHANNEL_USERAPP, au8TestMessage);
      
       if(G_au8AntApiCurrentMessageBytes[1] > 30) 
       {  
        if(G_au8AntApiCurrentMessageBytes[7] >= 120 && G_au8AntApiCurrentMessageBytes[7] <= 150 || G_au8AntApiCurrentMessageBytes[7] <= 40)
        {
          
          LedOn(LCD_RED);
          LedOff(LCD_GREEN);
          LedOff(LCD_BLUE);
          LedOff(RED);
         
        }
        if(G_au8AntApiCurrentMessageBytes[7] >= 150 && G_au8AntApiCurrentMessageBytes[7] <= 200)
        {
          
          LedOn(RED);
          LedOn(LCD_RED);
          LedOff(LCD_GREEN);
          LedOff(LCD_BLUE);
          
          
        }
        if(G_au8AntApiCurrentMessageBytes[7] == 200)
        {
          PWMAudioOn(BUZZER1);
          PWMAudioSetFrequency(BUZZER1, 296);
          if(WasButtonPressed(BUTTON2))
          {
            PWMAudioOff(BUZZER1);
          }
        }
          if(G_au8AntApiCurrentMessageBytes[7] >= 90 && G_au8AntApiCurrentMessageBytes[7] <= 120)
        {
          
          LedOn(LCD_RED);
          LedOff(LCD_GREEN);
          LedOn(LCD_BLUE);
          
        }
        if(G_au8AntApiCurrentMessageBytes[7] <= 90 && G_au8AntApiCurrentMessageBytes[7] >= 65)
        {
          
          LedOff(LCD_RED);
          LedOn(LCD_GREEN);
          LedOff(LCD_BLUE);
          
        }
        if(G_au8AntApiCurrentMessageBytes[7] <= 65 && G_au8AntApiCurrentMessageBytes[7] >= 40)
        {
          
          LedOff(LCD_RED);
          LedOn(LCD_GREEN);
          LedOn(LCD_BLUE);
          
        }
       }
       
       if(G_au8AntApiCurrentMessageBytes[1] <= 30)
        {
               LedOn(YELLOW);
               LedOff(LCD_RED);
               LedOff(LCD_GREEN);
               LedOff(LCD_BLUE);
            
         
               if(IsButtonPressed(BUTTON3))
             {
          
                        ButtonAcknowledge(BUTTON3);
              
                        LedOn(LCD_RED);
                        LedOn(LCD_GREEN);
                        LedOn(LCD_BLUE);
                        
                        a++;
                        if(a >= 30000)
                       {
                         LedOff(LCD_RED);
                         LedOff(LCD_GREEN);
                         LedOff(LCD_BLUE);
                       }
              } 
         }
            
           
             
          
            
                 
      

           

      
      
      
      
      } /* end if(bGotNewData) */
    } /* end if(G_eAntApiCurrentMessageClass == ANT_DATA) */

    else if(G_eAntApiCurrentMessageClass == ANT_TICK)
    {
      UserApp1_u32TickMsgCount++;
      
      /* Look at the TICK contents to check the event code and respond only if it's different */
      if(u8LastState != G_au8AntApiCurrentMessageBytes[ANT_TICK_MSG_EVENT_CODE_INDEX])
      {
        /* The state changed so update u8LastState and queue a debug message */
        u8LastState = G_au8AntApiCurrentMessageBytes[ANT_TICK_MSG_EVENT_CODE_INDEX];
        au8TickMessage[6] = HexToASCIICharUpper(u8LastState);
        DebugPrintf(au8TickMessage);

        /* Parse u8LastState to update LED status */
        switch (u8LastState)
        {
          /* If we are paired but missing messages, blue blinks */
          case EVENT_RX_FAIL:
          {
            LedOff(GREEN);
            LedBlink(BLUE, LED_2HZ);
            break;
          }

          /* If we drop to search, LED is green */
          case EVENT_RX_FAIL_GO_TO_SEARCH:
          {
            LedOff(BLUE);
            LedOn(GREEN);
            break;
          }

          /* If the search times out, the channel should automatically close */
          case EVENT_RX_SEARCH_TIMEOUT:
          {
            DebugPrintf("Search timeout\r\n");
            break;
          }

          default:
          {
            DebugPrintf("Unexpected Event\r\n");
            break;
          }
        } /* end switch (G_au8AntApiCurrentMessageBytes) */
      } /* end if (u8LastState ...) */
    } /* end else if(G_eAntApiCurrentMessageClass == ANT_TICK) */
  } /* end AntReadAppMessageBuffer() */
} 
/*----------------------------------------------------------------------------------------------------------------------
Function UserApp1RunActiveState()

Description:
Selects and runs one iteration of the current state in the state machine.
All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
  - State machine function pointer points at current state

Promises:
  - Calls the function to pointed by the state machine function pointer
*/
void UserApp1RunActiveState(void)
{
  UserApp_StateMachine();

} /* end UserApp1RunActiveState */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */

  



static void UserAppSM_WaitChannelOpen(void)
{
  /* Monitor the channel status to check if channel is opened */
  if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_OPEN)
  {
    LedOn(GREEN);
    UserApp_StateMachine = UserAppSM_ChannelOpen;
    
    LedBlink(GREEN, LED_2HZ);
    LedOff(BLUE);
    
    
    
  }
  
  /* Check for timeout */
  if( IsTimeUp(&UserApp1_u32Timeout, TIMEOUT_VALUE) )
  {
    AntCloseChannelNumber(ANT_CHANNEL_USERAPP);
    LedOff(GREEN);
    LedOn(YELLOW);
    UserApp_StateMachine = UserAppSM_Idle;
  }
  
  
    
} /* end UserAppSM_WaitChannelOpen() */
/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
