#include <iostream>
#include <atlstr.h>

#define NEX_GAMEPAD_DPAD_UP				0x0001
#define NEX_GAMEPAD_DPAD_DOWN			0x0002
#define NEX_GAMEPAD_DPAD_LEFT			0x0004
#define NEX_GAMEPAD_DPAD_RIGHT			0x0008
#define NEX_GAMEPAD_START				0x0010
#define NEX_GAMEPAD_BACK				0x0020
#define NEX_GAMEPAD_LEFT_THUMB			0x0040
#define NEX_GAMEPAD_RIGHT_THUMB			0x0080
#define NEX_GAMEPAD_LEFT_SHOULDER		0x0100
#define NEX_GAMEPAD_RIGHT_SHOULDER		0x0200
#define NEX_GAMEPAD_A					0x1000
#define NEX_GAMEPAD_B					0x2000
#define NEX_GAMEPAD_X					0x4000
#define NEX_GAMEPAD_Y					0x8000

#define NEX_CONTROLLER_WIRED			0
#define NEX_CONTROLLER_WIRELESS			1

#define NEX_INPUT_MAX_COUNT				4

#define ERROR_DEVICE_NOT_CONNECTED		1
#define ERROR_SUCCESS					0

#define NEX_UNKNOWN_CONTROLLER			0

#define MICROSOFT_XBOX_360_CONTROLLER	1
#define MICROSOFT_XBOX_ONE_CONTROLLER	2

#define SONY_DUALSHOCK_3_CONTROLLER		26
#define SONY_DUALSHOCK_4_CONTROLLER		27
#define SONY_DUALSHOCK_5_CONTROLLER		28

#define NINTENDO_SWITCH_PRO_CONTROLLER	51

typedef struct _NEX_INPUT_STATE
{
	WORD								Buttons;
	BYTE								LeftTrigger;
	BYTE								RightTrigger;
	SHORT								AxisLX;
	SHORT								AxisLY;
	SHORT								AxisRX;
	SHORT								AxisRY;
	float								Yaw;
	float								Pitch;
	float								Roll;
} NEX_INPUT_STATE, *PNEX_INPUT_STATE;

typedef struct _NEX_OUTPUT_STATE
{
	WORD								LeftMotorSpeed;
	WORD								RightMotorSpeed;
	BYTE								LEDBrightness;
	BYTE								LEDRed;
	BYTE								LEDGreen;
	BYTE								LEDBlue;
} NEX_OUTPUT_STATE, *PNEX_OUTPUT_STATE;

typedef struct _NEX_CONTROLLER_INFO
{
	WORD								ControllerType;
	BYTE								ConnectType;
	BYTE								BatteryLevel;
	bool								SupportRotation;
} NEX_CONTROLLER_INFO, *PNEX_CONTROLLER_INFO;

typedef DWORD(__stdcall *_NEXInputGetState)(__in DWORD dwUserIndex, __out NEX_INPUT_STATE *pInputState);
typedef DWORD(__stdcall *_NEXInputSetState)(__in DWORD dwUserIndex, __in NEX_OUTPUT_STATE *pOutputState);
typedef DWORD(__stdcall *_NEXInputGetInfo)(__in DWORD dwUserIndex, __out NEX_CONTROLLER_INFO *pControllerInfo);
typedef DWORD(__stdcall *_NEXInputPowerOff)(__in DWORD dwUserIndex);

int main()
{
	SetConsoleTitle(_T("NexInput Get Data"));
	HMODULE hDll;
	_NEXInputGetState NEXInputGetState;
	_NEXInputSetState NEXInputSetState;
	_NEXInputGetInfo NEXInputGetInfo;
	_NEXInputPowerOff NEXInputPowerOff;

	CRegKey key;
	TCHAR NexLibDll[MAX_PATH];

	LONG status = key.Open(HKEY_CURRENT_USER, _T("Software\\NexInput"));
	if (status == ERROR_SUCCESS)
	{
		ULONG regSize = sizeof(NexLibDll);

		#ifdef _WIN64
			status = key.QueryStringValue(_T("NexInput_v1.0_64"), NexLibDll, &regSize);
		#else
			status = key.QueryStringValue(_T("NexInput_v1.0_32"), NexLibDll, &regSize);
		#endif
	}
	key.Close();

	hDll = LoadLibrary(NexLibDll);
	if (hDll != NULL) {
		NEXInputGetState = (_NEXInputGetState)GetProcAddress(hDll, "NEXInputGetState");
		NEXInputSetState = (_NEXInputSetState)GetProcAddress(hDll, "NEXInputSetState");
		NEXInputGetInfo = (_NEXInputGetInfo)GetProcAddress(hDll, "NEXInputGetInfo");
		NEXInputPowerOff = (_NEXInputPowerOff)GetProcAddress(hDll, "NEXInputPowerOff");

		if (NEXInputGetState == NULL)
			printf(" NEXInputGetState not found\r\n");
		if (NEXInputSetState == NULL)
			printf(" NEXInputSetState not found\r\n");
		if (NEXInputGetInfo == NULL)
			printf(" NEXInputGetInfo not found\r\n");
		if (NEXInputPowerOff == NULL)
			printf(" NEXInputPowerOff not found\r\n");
	} 
	else
	{
		printf(" NexInput standard not found.\r\n");
		system("pause");
		return 1;
	}

	NEX_INPUT_STATE NexControllerInputState;
	NEX_OUTPUT_STATE NexControllerOutputState;
	NEX_CONTROLLER_INFO NexControllerInfo;

	while (true) {
		system("cls");

		for (int i = 0; i < NEX_INPUT_MAX_COUNT; i++) {
			if (NEXInputGetState(i, &NexControllerInputState) == ERROR_DEVICE_NOT_CONNECTED)
				continue;


			printf("\r\n Gamepad index = %d, name = ", i);

			NEXInputGetInfo(i, &NexControllerInfo);
			switch (NexControllerInfo.ControllerType) {
			case MICROSOFT_XBOX_360_CONTROLLER:
				{
					printf("MICROSOFT_XBOX_360_CONTROLLER \r\n");
					break;
				}
				case MICROSOFT_XBOX_ONE_CONTROLLER:
				{
					printf("MICROSOFT_XBOX_ONE_CONTROLLER \r\n");
					break;
				}
				case SONY_DUALSHOCK_3_CONTROLLER:
				{
					printf("SONY_DUALSHOCK_3_CONTROLLER \r\n");
					break;
				}
				case SONY_DUALSHOCK_4_CONTROLLER:
				{
					printf("SONY_DUALSHOCK_4_CONTROLLER \r\n");
					break;
				}
				case SONY_DUALSHOCK_5_CONTROLLER:
				{
					printf("SONY_DUALSHOCK_5_CONTROLLER \r\n");
					break;
				}
				case NINTENDO_SWITCH_PRO_CONTROLLER:
				{
					printf("NINTENDO_SWITCH_PRO_CONTROLLER \r\n");
					break;
				}

				default:
				{
					printf("NEX_UNKNOWN_CONTROLLER \r\n");
					break;
				}
			}

			
			if (NexControllerInfo.SupportRotation)
				printf(" Gyroscope support, Yaw=%7.2f, Pitch=%7.2f, Roll=%7.2f\r\n", NexControllerInputState.Yaw, NexControllerInputState.Pitch, NexControllerInputState.Roll);
			else
				printf(" Gyroscope unsupport \r\n");
			printf(" AxisLX=%d, AxisLY=%d, AxisRX=%d, AxisRY=%d\r\n", NexControllerInputState.AxisLX, NexControllerInputState.AxisLY, NexControllerInputState.AxisRX, NexControllerInputState.AxisRY);
			printf(" LeftTrigger=%d, RightTrigger=%d\r\n", NexControllerInputState.LeftTrigger, NexControllerInputState.RightTrigger);

			printf(" Buttons pressed: ");
			if (NexControllerInputState.Buttons & NEX_GAMEPAD_DPAD_UP) printf("DPAD UP ");
			if (NexControllerInputState.Buttons & NEX_GAMEPAD_DPAD_LEFT) printf("DPAD LEFT ");
			if (NexControllerInputState.Buttons & NEX_GAMEPAD_DPAD_RIGHT) printf("DPAD RIGHT ");
			if (NexControllerInputState.Buttons & NEX_GAMEPAD_DPAD_DOWN) printf("DPAD DOWN ");

			if (NexControllerInputState.Buttons & NEX_GAMEPAD_START) printf("START ");
			if (NexControllerInputState.Buttons & NEX_GAMEPAD_BACK) printf("BACK ");

			if (NexControllerInputState.Buttons & NEX_GAMEPAD_LEFT_THUMB) printf("LEFT THUMB ");
			if (NexControllerInputState.Buttons & NEX_GAMEPAD_RIGHT_THUMB) printf("RIGHT THUMB ");

			if (NexControllerInputState.Buttons & NEX_GAMEPAD_LEFT_SHOULDER) printf("LEFT SHOULDER ");
			if (NexControllerInputState.Buttons & NEX_GAMEPAD_RIGHT_SHOULDER) printf("RIGHT SHOULDER ");

			if (NexControllerInputState.Buttons & NEX_GAMEPAD_A) printf("A ");
			if (NexControllerInputState.Buttons & NEX_GAMEPAD_B) printf("B ");
			if (NexControllerInputState.Buttons & NEX_GAMEPAD_X) printf("X ");
			if (NexControllerInputState.Buttons & NEX_GAMEPAD_Y) printf("Y ");

			if (NexControllerInputState.Buttons == 0) printf("-");


			NEXInputGetInfo(i, &NexControllerInfo);
			if (NexControllerInfo.ConnectType == NEX_CONTROLLER_WIRELESS)
				printf("\r\n Controller type: %d; connection: wireless, battery capacity: %d.\r\n", NexControllerInfo.ControllerType, NexControllerInfo.BatteryLevel);
			else
				printf("\r\n Controller type: %d, connection: wired, battery capacity: %d.\r\n", NexControllerInfo.ControllerType, NexControllerInfo.BatteryLevel);
		}

		if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0) {
			NexControllerOutputState.LEDRed = 21;
			NexControllerOutputState.LEDGreen = 199;
			NexControllerOutputState.LEDBlue = 237;
			NexControllerOutputState.LEDBrightness = 170;
			NexControllerOutputState.LeftMotorSpeed = 12000;
			NexControllerOutputState.RightMotorSpeed = 12000;
			for (int i = 0; i < NEX_INPUT_MAX_COUNT; i++)
				NEXInputSetState(i, &NexControllerOutputState);
		}
		if ((GetAsyncKeyState(VK_ESCAPE) & 0x8000) != 0) break;

		printf("\r\n Ctrl - feedback (vibration and LED), Escape - exit.\r\n");

		Sleep(50);
	}

	if (hDll != NULL)
		FreeLibrary(hDll);

	return 0;
}
