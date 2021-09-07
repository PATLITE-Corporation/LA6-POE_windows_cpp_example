// LA6-POE_Sample_CPP.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//
#include <winsock2.h>
#include <windows.h>
#include <string>
#include <iostream>
#include <conio.h>

#pragma warning(disable:4996)
#pragma comment( lib, "ws2_32.lib" )

WSADATA wsaData;
SOCKET sock = NULL;

// product category
#define PNS_PRODUCT_ID	0x4142

// PNS command identifier
#define PNS_SMART_MODE_COMMAND			0x54    // smart mode control command
#define PNS_MUTE_COMMAND				0x4D    // mute command
#define PNS_STOP_PULSE_INPUT_COMMAND    0x50    // stop/pulse input command
#define PNS_RUN_CONTROL_COMMAND			0x53 	// operation control command
#define PNS_DETAIL_RUN_CONTROL_COMMAND	0x44 	// detailed operation control command
#define PNS_CLEAR_COMMAND				0x43 	// clear command
#define PNS_REBOOT_COMMAND				0x42 	// reboot command
#define PNS_GET_DATA_COMMAND			0x47 	// get status command
#define PNS_GET_DETAIL_DATA_COMMAND		0x45 	// get detail status command

// Buffer size from product category to data size
#define PNS_COMMAND_HEADER_LENGTH		6

// response data for PNS command
#define PNS_ACK		0x06 	// normal response
#define PNS_NAK		0x15 	// abnormal response

// モード
#define PNS_LED_MODE		0x00 	// signal light mode
#define PNS_SMART_MODE		0x01 	// smart mode

// LED unit for motion control command
#define PNS_RUN_CONTROL_LED_OFF			0x00 	// light off
#define PNS_RUN_CONTROL_LED_ON			0x01 	// light on
#define PNS_RUN_CONTROL_LED_BLINKING    0x02 	// flashing
#define PNS_RUN_CONTROL_LED_NO_CHANGE	0x09    // no change

// buzzer for motion control command
#define PNS_RUN_CONTROL_BUZZER_STOP			0x00 	// stop
#define PNS_RUN_CONTROL_BUZZER_PATTERN1		0x01 	// pattern 1
#define PNS_RUN_CONTROL_BUZZER_PATTERN2		0x02 	// pattern 2
#define PNS_RUN_CONTROL_BUZZER_TONE			0x03 	// buzzer tone for simultaneous buzzer input
#define PNS_RUN_CONTROL_BUZZER_NO_CHANGE    0x09 	// no change

// LED unit for detailed operation control command
#define PNS_DETAIL_RUN_CONTROL_LED_OFF				0x00 	// light off
#define PNS_DETAIL_RUN_CONTROL_LED_COLOR_RED		0x01 	// red
#define PNS_DETAIL_RUN_CONTROL_LED_COLOR_YELLOW		0x02 	// yellow
#define PNS_DETAIL_RUN_CONTROL_LED_COLOR_LEMON		0x03 	// Graduates
#define PNS_DETAIL_RUN_CONTROL_LED_COLOR_GREEN		0x04 	// green
#define PNS_DETAIL_RUN_CONTROL_LED_COLOR_SKY_BLUE	0x05    // sky blue
#define PNS_DETAIL_RUN_CONTROL_LED_COLOR_BLUE		0x06 	// blue
#define PNS_DETAIL_RUN_CONTROL_LED_COLOR_PURPLE		0x07 	// purple
#define PNS_DETAIL_RUN_CONTROL_LED_COLOR_PEACH		0x08 	// peach
#define PNS_DETAIL_RUN_CONTROL_LED_COLOR_WHITE		0x09 	// white

// blinking action for detailed action control command
#define PNS_DETAIL_RUN_CONTROL_BLINKING_OFF		0x00 	    // blinking off
#define PNS_DETAIL_RUN_CONTROL_BLINKING_ON		0x01 	    // blinking ON

// buzzer for detailed action control command
#define PNS_DETAIL_RUN_CONTROL_BUZZER_STOP			0x00 	// stop
#define PNS_DETAIL_RUN_CONTROL_BUZZER_PATTERN1		0x01 	// pattern 1
#define PNS_DETAIL_RUN_CONTROL_BUZZER_PATTERN2		0x02 	// pattern 2
#define PNS_DETAIL_RUN_CONTROL_BUZZER_PATTERN3		0x03 	// pattern 3
#define PNS_DETAIL_RUN_CONTROL_BUZZER_PATTERN4		0x04 	// pattern 4
#define PNS_DETAIL_RUN_CONTROL_BUZZER_PATTERN5		0x05 	// pattern 5
#define PNS_DETAIL_RUN_CONTROL_BUZZER_PATTERN6		0x06 	// pattern 6
#define PNS_DETAIL_RUN_CONTROL_BUZZER_PATTERN7		0x07 	// pattern 7
#define PNS_DETAIL_RUN_CONTROL_BUZZER_PATTERN8		0x08 	// pattern 8
#define PNS_DETAIL_RUN_CONTROL_BUZZER_PATTERN9		0x09 	// pattern 9
#define PNS_DETAIL_RUN_CONTROL_BUZZER_PATTERN10		0x0A 	// pattern 10
#define PNS_DETAIL_RUN_CONTROL_BUZZER_PATTERN11		0x0B 	// pattern 11

// operation control data structure
typedef struct
{
	// 1st LED unit pattern.
    unsigned char led1Pattern;

	// 2nd LED unit pattern.
    unsigned char led2Pattern;

	// 3rd LED unit pattern.
    unsigned char led3Pattern;

	// 4th LED unit pattern.
    unsigned char led4Pattern;

	// 5th LED unit pattern.
    unsigned char led5Pattern;

	// buzzer pattern 1 to 3
    unsigned char buzzerPattern;
}PNS_RUN_CONTROL_DATA;

// detail operation control data structure
typedef struct
{
    // 1st color of LED unit.
    unsigned char led1Color;

    // 2nd color of LED unit.
    unsigned char led2Color;

    // 3rd color of LED unit.
    unsigned char led3Color;

    // 4th color of LED unit.
    unsigned char led4Color;

    // 5th color of LED unit.
    unsigned char led5Color;

    // blinking action
    unsigned char blinkingControl;

    // buzzer pattern 1 to 11
    unsigned char buzzerPattern;
}PNS_DETAIL_RUN_CONTROL_DATA;

// status data when running in signal light mode
typedef struct
{
    // 1st LED unit pattern.
    unsigned char led1Pattern;

    // 2nd LED unit pattern.
    unsigned char led2Pattern;

    // 3rd LED unit pattern.
    unsigned char led3Pattern;

    // 4th LED unit pattern.
    unsigned char led4Pattern;

    // 5th LED unit pattern.
    unsigned char led5Pattern;

    // buzzer patterns 1 through 11
    unsigned char buzzerPattern;
}PNS_LED_MODE_DATA;

// state data when running smart mode
typedef struct
{
    // group number
    unsigned char groupNo;

    // mute
    unsigned char mute;

    // STOP input
    unsigned char stopInput;

    // Pattern No
    unsigned char patternNo;
}PNS_SMART_MODE_DATA;

// status data of operation control
typedef struct
{
    // input 1 to 8
    unsigned char input[8];

    // mode
    unsigned char mode;

    // status data when running signal light mode
    PNS_LED_MODE_DATA ledModeData;

    // status data during smart mode execution
    PNS_SMART_MODE_DATA smartModeData;
}PNS_STATUS_DATA;

// LED unit data
typedef struct
{
    // status
    unsigned char ledPattern;

    // R
    unsigned char red;

    // G
    unsigned char green;

    // B
    unsigned char blue;
}PNS_LED_UNIT_DATA;

// detailed state data when running in signal light mode
typedef struct
{
    // 1st stage of LED unit
    PNS_LED_UNIT_DATA ledUnit1Data;

    // 2nd stage of LED unit
    PNS_LED_UNIT_DATA ledUnit2Data;

    // 3rd stage of LED unit
    PNS_LED_UNIT_DATA ledUnit3Data;

    // 4th stage of LED unit
    PNS_LED_UNIT_DATA ledUnit4Data;

    // 5th stage of LED unit
    PNS_LED_UNIT_DATA ledUnit5Data;

    // buzzer pattern 1 to 11
    unsigned char buzzerPattern;
}PNS_LED_MODE_DETAIL_DATA;

// smart mode status data
typedef struct
{
    // group number
    unsigned char groupNo;

    // mute
    unsigned char mute;

    // STOP input
    unsigned char stopInput;

    // pattern number5
    unsigned char patternNo;

    // last pattern
    unsigned char lastPattern;
}PNS_SMART_MODE_DETAIL_STATE_DATA;

// detail state data for smart mode execution
typedef struct
{
    // smart mode state
    PNS_SMART_MODE_DETAIL_STATE_DATA smartModeData;

    // 1st stage of LED unit
    PNS_LED_UNIT_DATA ledUnit1Data;

    // 2nd stage of LED unit
    PNS_LED_UNIT_DATA ledUnit2Data;

    // 3rd stage of LED unit
    PNS_LED_UNIT_DATA ledUnit3Data;

    // 4th stage of LED unit
    PNS_LED_UNIT_DATA ledUnit4Data;

    // 5th stage of LED unit
    PNS_LED_UNIT_DATA ledUnit5Data;

    // buzzer pattern 1 to 11
    unsigned char buzzerPattern;
}PNS_SMART_MODE_DETAIL_DATA;

// status data of detailed operation control
typedef struct
{
    // MAC address
    unsigned char macAddress[6];

    // Input 1 to 8
    unsigned char input[8];

    // mode
    unsigned char mode;

    // detailed status data when running signal light mode
    PNS_LED_MODE_DETAIL_DATA ledModeDetalData;

    // detailed state data when running in smart mode
    PNS_SMART_MODE_DETAIL_DATA smartModeDetalData;
}PNS_DETAIL_STATUS_DATA;

// PHN command identifier
#define PHN_WRITE_COMMAND       0x57    // write command
#define PHN_READ_COMMAND        0x52    // read command

// response data for PNS command
#define PHN_ACK    { 0x41, 0x43, 0x4B }    // normal response
#define PHN_NAK    { 0x4E, 0x41, 0x4B }    // abnormal response

// action data of PHN command
#define PHN_LED_UNIT1_BLINKING      0x20    // 1st LED unit blinking
#define PHN_LED_UNIT2_BLINKING      0x40    // 2nd LED unit blinking
#define PHN_LED_UNIT3_BLINKING      0x80    // 3rd LED unit blinking
#define PHN_BUZZER_PATTERN1         0x8     // buzzer pattern 1
#define PHN_BUZZER_PATTERN2         0x10    // buzzer pattern 2
#define PHN_LED_UNIT1_LIGHTING      0x1     // 1st LED unit lighting
#define PHN_LED_UNIT2_LIGHTING      0x2     // 2nd LED unit lighting
#define PHN_LED_UNIT3_LIGHTING      0x4     // 3rd LED unit lighting

int SocketOpen(std::string ip, int port);
void SocketClose();
int SendCommand(char* sendData, int sendLength, char* recvData, int recvLength);
int PNS_SmartModeCommand(unsigned char groupNo);
int PNS_MuteCommand(unsigned char mute);
int PNS_StopPulseInputCommand(unsigned char input);
int PNS_RunControlCommand(PNS_RUN_CONTROL_DATA runControlData);
int PNS_DetailRunControlCommand(PNS_DETAIL_RUN_CONTROL_DATA detalRunControlData);
int PNS_ClearCommand();
int PNS_RebootCommand(std::string password);
int PNS_GetDataCommand(PNS_STATUS_DATA* statusData);
int PNS_GetDetailDataCommand(PNS_DETAIL_STATUS_DATA* detailStatusData);
int PHN_WriteCommand(unsigned char runData);
int PHN_ReadCommand(unsigned char* runData);

/// <summary>
/// Main Function
/// </summary>
int main(int argc, char* argv[])
{
    int ret;

    // Connect to LA-POE
    ret = SocketOpen("192.168.10.1", 10000);
    if (ret == -1) {
        return 0;
    }

    // Get the command identifier specified by the command line argument
    char* commandId = NULL;
    if (argc > 1) {
        commandId = argv[1];
    }

    switch (*commandId)
    {
        case 'T':
        {
            // smart mode control command
            if (argc >= 3)
                PNS_SmartModeCommand(atoi(argv[2]));
            break;
        }

        case 'M':
        {
            // mute command
            if (argc >= 3)
                PNS_MuteCommand(atoi(argv[2]));
            break;
        }

        case 'P':
        {
            // stop/pulse input command
            if (argc >= 3)
                PNS_StopPulseInputCommand(atoi(argv[2]));
            break;
        }

        case 'S':
        {
            // operation control command
            if (argc >= 8)
            {
                PNS_RUN_CONTROL_DATA runControlData;
                runControlData.led1Pattern = atoi(argv[2]);
                runControlData.led2Pattern = atoi(argv[3]);
                runControlData.led3Pattern = atoi(argv[4]);
                runControlData.led4Pattern = atoi(argv[5]);
                runControlData.led5Pattern = atoi(argv[6]);
                runControlData.buzzerPattern = atoi(argv[7]);
                PNS_RunControlCommand(runControlData);
            }

            break;
        }

        case 'D':
        {
            // detailed operation control command
            if (argc >= 9)
            {
                PNS_DETAIL_RUN_CONTROL_DATA detalRunControlData;
                detalRunControlData.led1Color = atoi(argv[2]);
                detalRunControlData.led2Color = atoi(argv[3]);
                detalRunControlData.led3Color = atoi(argv[4]);
                detalRunControlData.led4Color = atoi(argv[5]);
                detalRunControlData.led5Color = atoi(argv[6]);
                detalRunControlData.blinkingControl = atoi(argv[7]);
                detalRunControlData.buzzerPattern = atoi(argv[8]);
                PNS_DetailRunControlCommand(detalRunControlData);
            }

            break;
        }

        case 'C':
        {
            // clear command
            PNS_ClearCommand();
            break;
        }

        case 'B':
        {
            // reboot command
            if (argc >= 3)
                PNS_RebootCommand(argv[2]);
            break;
        }

        case 'G':
        {
            // get status command
            PNS_STATUS_DATA statusData;
            ret = PNS_GetDataCommand(&statusData);
            if (ret == 0)
            {
                // Display acquired data
                printf("Response data for status acquisition command\n");
                // Input1
                printf("Input1 : %d\n", statusData.input[0]);
                // Input2
                printf("Input2 : %d\n", statusData.input[1]);
                // Input3
                printf("Input3 : %d\n", statusData.input[2]);
                // Input4
                printf("Input4 : %d\n", statusData.input[3]);
                // Input5
                printf("Input5 : %d\n", statusData.input[4]);
                // Input6
                printf("Input6 : %d\n", statusData.input[5]);
                // Input7
                printf("Input7 : %d\n", statusData.input[6]);
                // Input8
                printf("Input8 : %d\n", statusData.input[7]);
                // mode
                if (statusData.mode == PNS_LED_MODE)
                {
                    // signal light mode
                    printf("signal light mode\n");
                    // 1st LED unit pattern
                    printf("1st LED unit pattern : %d\n", statusData.ledModeData.led1Pattern);
                    // 2nd LED unit pattern
                    printf("2nd LED unit pattern : %d\n", statusData.ledModeData.led2Pattern);
                    // 3rd LED unit pattern
                    printf("3rd LED unit pattern : %d\n", statusData.ledModeData.led3Pattern);
                    // 4th LED unit pattern
                    printf("4th LED unit pattern : %d\n", statusData.ledModeData.led4Pattern);
                    // 5th LED unit pattern
                    printf("5th LED unit pattern : %d\n", statusData.ledModeData.led5Pattern);
                    // buzzer pattern
                    printf("buzzer pattern: %d\n", statusData.ledModeData.buzzerPattern);
                }
                else
                {
                    // smart mode
                    printf("smart mode\n");
                    // group number
                    printf("group number : %d\n", statusData.smartModeData.groupNo);
                    // mute
                    printf("mute : %d\n", statusData.smartModeData.mute);
                    // STOP input
                    printf("STOP input : %d\n", statusData.smartModeData.stopInput);
                    // pattern number
                    printf("pattern number : %d\n", statusData.smartModeData.patternNo);
                }
            }

            break;
        }

        case 'E':
        {
            // get detail status command
            PNS_DETAIL_STATUS_DATA detailStatusData;
            ret = PNS_GetDetailDataCommand(&detailStatusData);
            if (ret == 0)
            {
                // Display acquired data
                printf("Response data for status acquisition command\n");
                // MAC address
                printf("MAC address : %02x-%02x-%02x-%02x-%02x-%02x\n", detailStatusData.macAddress[0]
                                                                      , detailStatusData.macAddress[1]
                                                                      , detailStatusData.macAddress[2]
                                                                      , detailStatusData.macAddress[3]
                                                                      , detailStatusData.macAddress[4]
                                                                      , detailStatusData.macAddress[5]);
                // Input1
                printf("Input1 : %d\n", detailStatusData.input[0]);
                // Input2
                printf("Input2 : %d\n", detailStatusData.input[1]);
                // Input3
                printf("Input3 : %d\n", detailStatusData.input[2]);
                // Input4
                printf("Input4 : %d\n", detailStatusData.input[3]);
                // Input5
                printf("Input5 : %d\n", detailStatusData.input[4]);
                // Input6
                printf("Input6 : %d\n", detailStatusData.input[5]);
                // Input7
                printf("Input7 : %d\n", detailStatusData.input[6]);
                // Input8
                printf("Input8 : %d\n", detailStatusData.input[7]);
                // mode
                if (detailStatusData.mode == PNS_LED_MODE)
                {
                    // signal light mode
                    printf("signal light mode\n");
                    // 1st LED unit
                    printf("1st LED unit\n");
                    // pattern
                    printf("pattern : %d\n", detailStatusData.ledModeDetalData.ledUnit1Data.ledPattern);
                    // R
                    printf("R : %d\n", detailStatusData.ledModeDetalData.ledUnit1Data.red);
                    // G
                    printf("G : %d\n", detailStatusData.ledModeDetalData.ledUnit1Data.green);
                    // B
                    printf("B : %d\n", detailStatusData.ledModeDetalData.ledUnit1Data.blue);
                    // 2nd LED unit
                    printf("2nd LED unit\n");
                    // pattern
                    printf("pattern : %d\n", detailStatusData.ledModeDetalData.ledUnit2Data.ledPattern);
                    // R
                    printf("R : %d\n", detailStatusData.ledModeDetalData.ledUnit2Data.red);
                    // G
                    printf("G : %d\n", detailStatusData.ledModeDetalData.ledUnit2Data.green);
                    // B
                    printf("B : %d\n", detailStatusData.ledModeDetalData.ledUnit2Data.blue);
                    // 3rd LED unit
                    printf("3rd LED unit\n");
                    // pattern
                    printf("pattern : %d\n", detailStatusData.ledModeDetalData.ledUnit3Data.ledPattern);
                    // R
                    printf("R : %d\n", detailStatusData.ledModeDetalData.ledUnit3Data.red);
                    // G
                    printf("G : %d\n", detailStatusData.ledModeDetalData.ledUnit3Data.green);
                    // B
                    printf("B : %d\n", detailStatusData.ledModeDetalData.ledUnit3Data.blue);
                    // 4th LED unit
                    printf("4th LED unit\n");
                    // pattern
                    printf("pattern : %d\n", detailStatusData.ledModeDetalData.ledUnit4Data.ledPattern);
                    // R
                    printf("R : %d\n", detailStatusData.ledModeDetalData.ledUnit4Data.red);
                    // G
                    printf("G : %d\n", detailStatusData.ledModeDetalData.ledUnit4Data.green);
                    // B
                    printf("B : %d\n", detailStatusData.ledModeDetalData.ledUnit4Data.blue);
                    // 5th LED unit
                    printf("5th LED unit\n");
                    // pattern
                    printf("pattern : %d\n", detailStatusData.ledModeDetalData.ledUnit5Data.ledPattern);
                    // R
                    printf("R : %d\n", detailStatusData.ledModeDetalData.ledUnit5Data.red);
                    // G
                    printf("G : %d\n", detailStatusData.ledModeDetalData.ledUnit5Data.green);
                    // B
                    printf("B : %d\n", detailStatusData.ledModeDetalData.ledUnit5Data.blue);
                    // buzzer pattern
                    printf("buzzer pattern: %d\n", detailStatusData.ledModeDetalData.buzzerPattern);
                }
                else
                {
                    // smart mode
                    printf("smart mode\n");
                    // group number
                    printf("group number : %d\n", detailStatusData.smartModeDetalData.smartModeData.groupNo);
                    // mute
                    printf("mute : %d\n", detailStatusData.smartModeDetalData.smartModeData.mute);
                    // STOP input
                    printf("STOP input : %d\n", detailStatusData.smartModeDetalData.smartModeData.stopInput);
                    // pattern number
                    printf("pattern number : %d\n", detailStatusData.smartModeDetalData.smartModeData.patternNo);
                    // last pattern
                    printf("last pattern : %d\n", detailStatusData.smartModeDetalData.smartModeData.lastPattern);
                    // 1st LED unit
                    printf("1st LED unit\n");
                    // pattern
                    printf("pattern : %d\n", detailStatusData.smartModeDetalData.ledUnit1Data.ledPattern);
                    // R
                    printf("R : %d\n", detailStatusData.smartModeDetalData.ledUnit1Data.red);
                    // G
                    printf("G : %d\n", detailStatusData.smartModeDetalData.ledUnit1Data.green);
                    // B
                    printf("B : %d\n", detailStatusData.smartModeDetalData.ledUnit1Data.blue);
                    // 2nd LED unit
                    printf("2nd LED unit\n");
                    // pattern
                    printf("pattern : %d\n", detailStatusData.smartModeDetalData.ledUnit2Data.ledPattern);
                    // R
                    printf("R : %d\n", detailStatusData.smartModeDetalData.ledUnit2Data.red);
                    // G
                    printf("G : %d\n", detailStatusData.smartModeDetalData.ledUnit2Data.green);
                    // B
                    printf("B : %d\n", detailStatusData.smartModeDetalData.ledUnit2Data.blue);
                    // 3rd LED unit
                    printf("3rd LED unit\n");
                    // pattern
                    printf("pattern : %d\n", detailStatusData.smartModeDetalData.ledUnit3Data.ledPattern);
                    // R
                    printf("R : %d\n", detailStatusData.smartModeDetalData.ledUnit3Data.red);
                    // G
                    printf("G : %d\n", detailStatusData.smartModeDetalData.ledUnit3Data.green);
                    // B
                    printf("B : %d\n", detailStatusData.smartModeDetalData.ledUnit3Data.blue);
                    // 4th LED unit
                    printf("4th LED unit\n");
                    // pattern
                    printf("pattern : %d\n", detailStatusData.smartModeDetalData.ledUnit4Data.ledPattern);
                    // R
                    printf("R : %d\n", detailStatusData.smartModeDetalData.ledUnit4Data.red);
                    // G
                    printf("G : %d\n", detailStatusData.smartModeDetalData.ledUnit4Data.green);
                    // B
                    printf("B : %d\n", detailStatusData.smartModeDetalData.ledUnit4Data.blue);
                    // 5th LED unit
                    printf("5th LED unit\n");
                    // pattern
                    printf("pattern : %d\n", detailStatusData.smartModeDetalData.ledUnit5Data.ledPattern);
                    // R
                    printf("R : %d\n", detailStatusData.smartModeDetalData.ledUnit5Data.red);
                    // G
                    printf("G : %d\n", detailStatusData.smartModeDetalData.ledUnit5Data.green);
                    // B
                    printf("B : %d\n", detailStatusData.smartModeDetalData.ledUnit5Data.blue);
                    // buzzer pattern
                    printf("buzzer pattern: %d\n", detailStatusData.smartModeDetalData.buzzerPattern);
                }
            }

            break;
        }

        case 'W':
        {
            // write command
            if (argc >= 3)
                PHN_WriteCommand(atoi(argv[2]));
            break;
        }

        case 'R':
        {
            // read command
            unsigned char runData;
            ret = PHN_ReadCommand(&runData);
            if (ret == 0)
            {
                // Display acquired data
                printf("Response data for read command\n");
                // LED unit flashing
                printf("LED unit flashing\n");
                // 1st LED unit
                printf("1st LED unit : %d\n", ((runData & 0x20) != 0 ? 1 : 0));
                // 2nd LED unit
                printf("2nd LED unit : %d\n", ((runData & 0x40) != 0 ? 1 : 0));
                // 3rd LED unit
                printf("3rd LED unit : %d\n", ((runData & 0x80) != 0 ? 1 : 0));
                // buzzer pattern
                printf("buzzer pattern\n");
                // pattern1
                printf("pattern1 : %d\n", ((runData & 0x8) != 0 ? 1 : 0));
                // pattern2
                printf("pattern2 : %d\n", ((runData & 0x10) != 0 ? 1 : 0));
                // LED unit lighting
                printf("LED unit lighting\n");
                // 1st LED unit
                printf("1st LED unit : %d\n", ((runData & 0x1) != 0 ? 1 : 0));
                // 2nd LED unit
                printf("2nd LED unit : %d\n", ((runData & 0x2) != 0 ? 1 : 0));
                // 3rd LED unit
                printf("3rd LED unit : %d\n", ((runData & 0x4) != 0 ? 1 : 0));
            }

            break;
        }
    }

    // Close the socket
    SocketClose();

    return 0;
}

/// <summary>
/// Connect to LA-POE
/// </summary>
/// <param name="ip">IP address</param>
/// <param name="port">port number</param>
/// <returns>success: 0, failure: non-zero</returns>
int SocketOpen(std::string ip, int port)
{
	// Initialize winsock2
    if (WSAStartup(MAKEWORD(2, 0), &wsaData))
    {
        std::cout << "reset winsock failed" << std::endl;
        return -1;
    }

    // Create a socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
    {
        std::cout << "make socket failed" << std::endl;
        return -1;
    }

    // Set the IP address and port
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());

    // Connect to LA-POE
    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)))
    {
        std::cout << "connect failed" << std::endl;
        return -1;
    }

    return 0;
}

/// <summary>
/// Close the socket.
/// </summary>
void SocketClose()
{
	// Close the socket.
    closesocket(sock);

	// Exit process of winsock2
    WSACleanup();
}

/// <summary>
/// Send command
/// </summary>
/// <param name="sendData">send data</param>
/// <param name="recvData">received data</param>
/// <returns>success: 0, failure: non-zero</returns>
int SendCommand(char* sendData, int sendLength, char* recvData, int recvLength)
{
    int ret;

    if (sock == NULL)
    {
        std::cout << "socket is not" << std::endl;
        return -1;
    }

	// Send
    ret = send(sock, sendData, sendLength, 0);
    if (ret < 0)
    {
        std::cout << "failed to send" << std::endl;
        return -1;
    }

	// Receive response data
    std::memset(recvData, 0, recvLength);
    ret = recv(sock, recvData, recvLength, 0);
    if (ret < 0)
    {
        std::cout << "failed to recv" << std::endl;
        return -1;
    }

    return 0;
}

/// <summary>
/// Send smart mode control command for PNS command
/// Smart mode can be executed for the number specified in the data area
/// </summary>
/// <param name="groupNo">Group number to execute smart mode (0x01(Group No.1) to 0x1F(Group No.31))</param>
/// <returns>Success: 0, Failure: non-zero</returns>
int PNS_SmartModeCommand(unsigned char groupNo)
{
    int ret;
    char sendData[PNS_COMMAND_HEADER_LENGTH + sizeof(groupNo)];
    char recvData[1];
    std::memset(sendData, 0, sizeof(sendData));
    std::memset(recvData, 0, sizeof(recvData));

    // Product Category (AB)
    sendData[0] = PNS_PRODUCT_ID >> 8;
    sendData[1] = (char)(PNS_PRODUCT_ID | 0xFF00);

    // Command identifier (T)
    sendData[2] = PNS_SMART_MODE_COMMAND;

    // Empty (0)
    sendData[3] = 0;

    // Data size
    sendData[4] = sizeof(groupNo) >> 8;
    sendData[5] = (char)(sizeof(groupNo) | 0xFF00);

    // Data area
    sendData[6] = groupNo;

    // Send PNS command
    ret = SendCommand(sendData, PNS_COMMAND_HEADER_LENGTH + sizeof(groupNo), recvData, sizeof(recvData));
    if (ret != 0)
    {
        std::cout << "failed to send data" << std::endl;
        return -1;
    }

    // check the response data
    if (recvData[0] == PNS_NAK)
    {
        // receive abnormal response
        std::cout << "negative acknowledge" << std::endl;
        return -1;
    }

    return 0;
}

/// <summary>
/// Send mute command for PNS command
/// Can control the buzzer ON/OFF while Smart Mode is running
/// </summary>
/// <param name="mute">Buzzer ON/OFF (ON: 1, OFF: 0)</param>
/// <returns>success: 0, failure: non-zero</returns>
int PNS_MuteCommand(unsigned char mute)
{
    int ret;
    char sendData[PNS_COMMAND_HEADER_LENGTH + sizeof(mute)];
    char recvData[1];
    std::memset(sendData, 0, sizeof(sendData));
    std::memset(recvData, 0, sizeof(recvData));

    // Product Category (AB)
    sendData[0] = PNS_PRODUCT_ID >> 8;
    sendData[1] = (char)(PNS_PRODUCT_ID | 0xFF00);

    // Command identifier (M)
    sendData[2] = PNS_MUTE_COMMAND;

    // Empty (0)
    sendData[3] = 0;

    // Data size
    sendData[4] = sizeof(mute) >> 8;
    sendData[5] = (char)(sizeof(mute) | 0xFF00);

    // Data area
    sendData[6] = mute;

    // Send PNS command
    ret = SendCommand(sendData, PNS_COMMAND_HEADER_LENGTH + sizeof(mute), recvData, sizeof(recvData));
    if (ret != 0)
    {
        std::cout << "failed to send data" << std::endl;
        return -1;
    }

    // check the response data
    if (recvData[0] == PNS_NAK)
    {
        // receive abnormal response
        std::cout << "negative acknowledge" << std::endl;
        return -1;
    }

    return 0;
}

/// <summary>
/// Send stop/pulse input command for PNS command
/// Transmit during time trigger mode operation to control stop/resume of pattern (STOP input)
/// Sending this command during pulse trigger mode operation enables pattern transition (trigger input).
/// </summary>
/// <param name="input">STOP input/trigger input (STOP input ON/trigger input: 1, STOP input: 0)</param>
/// <returns>Success: 0, failure: non-zero</returns>
int PNS_StopPulseInputCommand(unsigned char input)
{
    int ret;
    char sendData[PNS_COMMAND_HEADER_LENGTH + sizeof(input)];
    char recvData[1];
    std::memset(sendData, 0, sizeof(sendData));
    std::memset(recvData, 0, sizeof(recvData));

    // Product Category (AB)
    sendData[0] = PNS_PRODUCT_ID >> 8;
    sendData[1] = (char)(PNS_PRODUCT_ID | 0xFF00);

    // Command identifier (P)
    sendData[2] = PNS_STOP_PULSE_INPUT_COMMAND;

    // Empty (0)
    sendData[3] = 0;

    // Data size
    sendData[4] = sizeof(input) >> 8;
    sendData[5] = (char)(sizeof(input) | 0xFF00);

    // Data area
    sendData[6] = input;

    // Send PNS command
    ret = SendCommand(sendData, PNS_COMMAND_HEADER_LENGTH + sizeof(input), recvData, sizeof(recvData));
    if (ret != 0)
    {
        std::cout << "failed to send data" << std::endl;
        return -1;
    }

    // check the response data
    if (recvData[0] == PNS_NAK)
    {
        // receive abnormal response
        std::cout << "negative acknowledge" << std::endl;
        return -1;
    }

    return 0;
}

/// <summary>
/// Send operation control command for PNS command
/// Each stage of the LED unit and the buzzer (1 to 3) can be controlled by the pattern specified in the data area
/// Operates with the color and buzzer set in the signal light mode
/// </summary>
/// <param name="runControlData">
/// Pattern of the 1st to 5th stage of the LED unit and buzzer (1 to 3)
/// Pattern of LED unit (off: 0, on: 1, blinking: 2, no change: 9)
/// Pattern of buzzer (stop: 0, pattern 1: 1, pattern 2: 2, buzzer tone when input simultaneously with buzzer: 3, no change: 9)
/// </param>
/// <returns>success: 0, failure: non-zero</returns>
int PNS_RunControlCommand(PNS_RUN_CONTROL_DATA runControlData)
{
    int ret;
    char sendData[PNS_COMMAND_HEADER_LENGTH + sizeof(runControlData)];
    char recvData[1];
    std::memset(sendData, 0, sizeof(sendData));
    std::memset(recvData, 0, sizeof(recvData));

    // Product Category (AB)
    sendData[0] = PNS_PRODUCT_ID >> 8;
    sendData[1] = (char)(PNS_PRODUCT_ID | 0xFF00);

    // Command identifier (S)
    sendData[2] = PNS_RUN_CONTROL_COMMAND;

    // Empty (0)
    sendData[3] = 0;

    // Data size
    sendData[4] = sizeof(runControlData) >> 8;
    sendData[5] = (char)(sizeof(runControlData) | 0xFF00);

    // Data area
    std::memcpy(&sendData[6], &runControlData, sizeof(runControlData));

    // Send PNS command
    ret = SendCommand(sendData, PNS_COMMAND_HEADER_LENGTH + sizeof(runControlData), recvData, sizeof(recvData));
    if (ret != 0)
    {
        std::cout << "failed to send data" << std::endl;
        return -1;
    }

    // check the response data
    if (recvData[0] == PNS_NAK)
    {
        // receive abnormal response
        std::cout << "negative acknowledge" << std::endl;
        return -1;
    }

    return 0;
}

/// <summary>
/// Send detailed operation control command for PNS command
/// The color and operation pattern of each stage of the LED unit and the buzzer pattern (1 to 11) can be specified and controlled in the data area
/// </summary>
/// <param name="detailRunControlData">
/// Pattern of the 1st to 5th stage of the LED unit, blinking operation and buzzer (1 to 3)
/// Pattern of LED unit (off: 0, red: 1, yellow: 2, lemon: 3, green: 4, sky blue: 5, blue: 6, purple: 7, peach: 8, white: 9)
/// Flashing action (Flashing OFF: 0, Flashing ON: 1)
/// Buzzer pattern (Stop: 0, Pattern 1: 1, Pattern 2: 2, Pattern 3: 3, Pattern 4: 4, Pattern 5: 5, Pattern 6: 6, Pattern 7: 7, Pattern 8: 8, Pattern 9: 9, Pattern 10: 10, Pattern 11: 11)
/// </param>
/// <returns>success: 0, failure: non-zero</returns>
int PNS_DetailRunControlCommand(PNS_DETAIL_RUN_CONTROL_DATA detailRunControlData)
{
    int ret;
    char sendData[PNS_COMMAND_HEADER_LENGTH + sizeof(detailRunControlData)];
    char recvData[1];
    std::memset(sendData, 0, sizeof(sendData));
    std::memset(recvData, 0, sizeof(recvData));

    // Product Category (AB)
    sendData[0] = PNS_PRODUCT_ID >> 8;
    sendData[1] = (char)(PNS_PRODUCT_ID | 0xFF00);

    // Command identifier (D)
    sendData[2] = PNS_DETAIL_RUN_CONTROL_COMMAND;

    // Empty (0)
    sendData[3] = 0;

    // Data size
    sendData[4] = sizeof(detailRunControlData) >> 8;
    sendData[5] = (char)(sizeof(detailRunControlData) | 0xFF00);

    // Data area
    std::memcpy(&sendData[6], &detailRunControlData, sizeof(detailRunControlData));

    // Send PNS command
    ret = SendCommand(sendData, PNS_COMMAND_HEADER_LENGTH + sizeof(detailRunControlData), recvData, sizeof(recvData));
    if (ret != 0)
    {
        std::cout << "failed to send data" << std::endl;
        return -1;
    }

    // check the response data
    if (recvData[0] == PNS_NAK)
    {
        // receive abnormal response
        std::cout << "negative acknowledge" << std::endl;
        return -1;
    }

    return 0;
}

/// <summary>
/// Send clear command for PNS command
/// Turn off the LED unit and stop the buzzer
/// </summary>
/// <returns>success: 0, failure: non-zero</returns>
int PNS_ClearCommand()
{
    int ret;
    char sendData[PNS_COMMAND_HEADER_LENGTH];
    char recvData[1];
    std::memset(sendData, 0, sizeof(sendData));
    std::memset(recvData, 0, sizeof(recvData));

    // Product Category (AB)
    sendData[0] = PNS_PRODUCT_ID >> 8;
    sendData[1] = (char)(PNS_PRODUCT_ID | 0xFF00);

    // Command identifier (C)
    sendData[2] = PNS_CLEAR_COMMAND;

    // Empty (0)
    sendData[3] = 0;

    // Data size
    sendData[4] = 0;
    sendData[5] = 0;

    // Send PNS command
    ret = SendCommand(sendData, PNS_COMMAND_HEADER_LENGTH, recvData, sizeof(recvData));
    if (ret != 0)
    {
        std::cout << "failed to send data" << std::endl;
        return -1;
    }

    // check the response data
    if (recvData[0] == PNS_NAK)
    {
        // receive abnormal response
        std::cout << "negative acknowledge" << std::endl;
        return -1;
    }

    return 0;
}

/// <summary>
/// Send restart command for PNS command
/// LA6-POE can be restarted
/// </summary>
/// <param name="password">Password set in the password setting of Web Configuration</param>.
/// <returns>success: 0, failure: non-zero</returns>
int PNS_RebootCommand(std::string password)
{
    int ret;
    char sendData[PNS_COMMAND_HEADER_LENGTH + 16];
    char recvData[1];
    std::memset(sendData, 0, sizeof(sendData));
    std::memset(recvData, 0, sizeof(recvData));

    // Product Category (AB)
    sendData[0] = PNS_PRODUCT_ID >> 8;
    sendData[1] = (char)(PNS_PRODUCT_ID | 0xFF00);

    // Command identifier (B)
    sendData[2] = PNS_REBOOT_COMMAND;

    // Empty (0)
    sendData[3] = 0;

    // Data size
    sendData[4] = password.length() >> 8;
    sendData[5] = (char)(password.length() | 0xFF00);

    // Data area
    std::memcpy(&sendData[6], password.c_str(), password.length());

    // Send PNS command
    ret = SendCommand(sendData, PNS_COMMAND_HEADER_LENGTH + password.length(), recvData, sizeof(recvData));
    if (ret != 0)
    {
        std::cout << "failed to send data" << std::endl;
        return -1;
    }

    // check the response data
    if (recvData[0] == PNS_NAK)
    {
        // receive abnormal response
        std::cout << "negative acknowledge" << std::endl;
        return -1;
    }

    return 0;
}

/// <summary>
/// Send status acquisition command for PNS command
/// Signal line/contact input status and LED unit and buzzer status can be acquired
/// </summary>
/// <param name="statusData">Received data of status acquisition command (status of signal line/contact input and status of LED unit and buzzer)</param>
/// <returns>Success: 0, failure: non-zero</returns>
int PNS_GetDataCommand(PNS_STATUS_DATA* statusData)
{
    int ret;
    char sendData[PNS_COMMAND_HEADER_LENGTH];
    char recvData[sizeof(PNS_STATUS_DATA)];
    std::memset(sendData, 0, sizeof(sendData));
    std::memset(recvData, 0, sizeof(recvData));
    std::memset(statusData, 0, sizeof(PNS_STATUS_DATA));

    // Product Category (AB)
    sendData[0] = PNS_PRODUCT_ID >> 8;
    sendData[1] = (char)(PNS_PRODUCT_ID | 0xFF00);

    // Command identifier (G)
    sendData[2] = PNS_GET_DATA_COMMAND;

    // Empty (0)
    sendData[3] = 0;

    // Data size
    sendData[4] = 0;
    sendData[5] = 0;

    // Send PNS command
    ret = SendCommand(sendData, PNS_COMMAND_HEADER_LENGTH, recvData, sizeof(recvData));
    if (ret != 0)
    {
        std::cout << "failed to send data" << std::endl;
        return -1;
    }

    // check the response data
    if (recvData[0] == PNS_NAK)
    {
        // receive abnormal response
        std::cout << "negative acknowledge" << std::endl;
        return -1;
    }

    // Input 1 to 8
    std::memcpy(statusData->input, &recvData[0], sizeof(statusData->input));

    // Mode
    statusData->mode = recvData[8];

    // Check the mode
    if (statusData->mode == PNS_LED_MODE)
    {
        // signal light mode
        std::memcpy(&statusData->ledModeData, &recvData[9], sizeof(statusData->ledModeData));
    }
    else
    {
        // smart mode
        std::memcpy(&statusData->smartModeData, &recvData[9], sizeof(statusData->smartModeData));
    }

    return 0;
}

/// <summary>
/// Send command to get detailed status of PNS command
/// Signal line/contact input status, LED unit and buzzer status, and color information for each stage can be acquired
/// </summary>
/// <param name="detailStatusData">Received data of detail status acquisition command (status of signal line/contact input, status of LED unit and buzzer, and color information of each stage)</param>
/// <returns>Success: 0, failure: non-zero</returns>
int PNS_GetDetailDataCommand(PNS_DETAIL_STATUS_DATA* detailStatusData)
{
    int ret;
    char sendData[PNS_COMMAND_HEADER_LENGTH];
    char recvData[sizeof(PNS_DETAIL_STATUS_DATA)];
    std::memset(sendData, 0, sizeof(sendData));
    std::memset(recvData, 0, sizeof(recvData));
    std::memset(detailStatusData, 0, sizeof(PNS_DETAIL_STATUS_DATA));

    // Product Category (AB)
    sendData[0] = PNS_PRODUCT_ID >> 8;
    sendData[1] = (char)(PNS_PRODUCT_ID | 0xFF00);

    // Command identifier (E)
    sendData[2] = PNS_GET_DETAIL_DATA_COMMAND;

    // Empty (0)
    sendData[3] = 0;

    // Data size
    sendData[4] = 0;
    sendData[5] = 0;

    // Send PNS command
    ret = SendCommand(sendData, PNS_COMMAND_HEADER_LENGTH, recvData, sizeof(recvData));
    if (ret != 0)
    {
        std::cout << "failed to send data" << std::endl;
        return -1;
    }

    // check the response data
    if (recvData[0] == PNS_NAK)
    {
        // receive abnormal response
        std::cout << "negative acknowledge" << std::endl;
        return -1;
    }

    // MAC Address
    std::memcpy(detailStatusData->macAddress, &recvData[0], sizeof(detailStatusData->macAddress));

    // Input 1 to 8
    std::memcpy(detailStatusData->input, &recvData[6], sizeof(detailStatusData->input));

    // Mode
    detailStatusData->mode = recvData[14];

    // Check the mode
    if (detailStatusData->mode == PNS_LED_MODE)
    {
        // signal light mode
        std::memcpy(&detailStatusData->ledModeDetalData, &recvData[19], sizeof(detailStatusData->ledModeDetalData));
    }
    else
    {
        // smart mode
        std::memcpy(&detailStatusData->smartModeDetalData, &recvData[19], sizeof(detailStatusData->smartModeDetalData));
    }

    return 0;
}

/// <summary>
/// Send PHN command write command
/// Can control the lighting and blinking of LED units 1 to 3 stages, and buzzer patterns 1 and 2
/// </summary>
/// <param name="data">
/// Operation data for lighting and blinking of LED unit 1 to 3 stages, and buzzer pattern 1 and 2
/// bit7: 3rd LED unit blinking (OFF: 0, ON: 1)
/// bit6: 2nd LED unit blinking (OFF: 0, ON: 1)
/// bit5: 1st LED unit blinking (OFF: 0, ON: 1)
/// bit4: Buzzer pattern 2 (OFF: 0, ON: 1)
/// bit3: Buzzer pattern 1 (OFF: 0, ON: 1)
/// bit2: 3rd LED unit lighting (OFF: 0, ON: 1)
/// bit1: 2nd LED unit lighting (OFF: 0, ON: 1)
/// bit0: 1st LED unit lighting (OFF: 0, ON: 1)
/// </param>
/// <returns>success: 0, failure: non-zero</returns>
int PHN_WriteCommand(unsigned char runData)
{
    int ret;
    char nak[3] = PHN_NAK;
    char sendData[2];
    char recvData[3];
    std::memset(sendData, 0, sizeof(sendData));
    std::memset(recvData, 0, sizeof(recvData));

    // Command identifier (W)
    sendData[0] = PHN_WRITE_COMMAND;

    // Operation data
    sendData[1] = runData;

    // Send PHN command
    ret = SendCommand(sendData, sizeof(sendData), recvData, sizeof(recvData));
    if (ret != 0)
    {
        std::cout << "failed to send data" << std::endl;
        return -1;
    }

    // check the response data
    if (std::memcmp(&recvData[0], nak, sizeof(nak)) == 0)
    {
        // receive abnormal response
        std::cout << "negative acknowledge" << std::endl;
        return -1;
    }

    return 0;
}

/// <summary>
/// Send command to read PHN command
/// Get information about LED unit 1 to 3 stage lighting and blinking, and buzzer pattern 1 and 2
/// </summary>
/// <param name="runData">Received data of read command (operation data of LED unit 1 to 3 stages lighting and blinking, buzzer pattern 1,2)</param>
/// <returns>Success: 0, failure: non-zero</returns>
int PHN_ReadCommand(unsigned char* runData)
{
    int ret;
    char sendData[2];
    char recvData[2];
    std::memset(sendData, 0, sizeof(sendData));
    std::memset(recvData, 0, sizeof(recvData));

    // Command identifier (R)
    sendData[0] = PHN_READ_COMMAND;

    // Send PHN command
    ret = SendCommand(sendData, sizeof(sendData), recvData, sizeof(recvData));
    if (ret != 0)
    {
        std::cout << "failed to send data" << std::endl;
        return -1;
    }

    // check the response data
    if (recvData[0] != PHN_READ_COMMAND)
    {
        // receive abnormal response
        std::cout << "negative acknowledge" << std::endl;
        return -1;
    }

    // Response data
    *runData = recvData[1];

    return 0;
}
