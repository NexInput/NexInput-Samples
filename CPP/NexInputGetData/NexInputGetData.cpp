#include <iostream>
#include <atlstr.h>
#include "NexInput.h"

HMODULE hDll;
_NEXInputGetState NEXInputGetState;
_NEXInputSetState NEXInputSetState;
_NEXInputGetInfo NEXInputGetInfo;
_NEXInputPowerOff NEXInputPowerOff;
NEX_INPUT_STATE NexControllerInputState;
NEX_OUTPUT_STATE NexControllerOutputState;
NEX_CONTROLLER_INFO NexControllerInfo;

int NexInputInit() {
	CRegKey key;
	TCHAR NexLibDll[MAX_PATH];

	LONG status = key.Open(HKEY_CURRENT_USER, _T("Software\\NexInput"));
	if (status == ERROR_SUCCESS)
	{
		ULONG regSize = sizeof(NexLibDll);

		#if _M_IX86
				status = key.QueryStringValue(_T("NexInput_v1.0_32"), NexLibDll, &regSize);
		#elif _WIN64
				status = key.QueryStringValue(_T("NexInput_v1.0_64"), NexLibDll, &regSize);
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
			printf(" NEXInputGetState not found\n");
		if (NEXInputSetState == NULL)
			printf(" NEXInputSetState not found\n");
		if (NEXInputGetInfo == NULL)
			printf(" NEXInputGetInfo not found\n");
		if (NEXInputPowerOff == NULL)
			printf(" NEXInputPowerOff not found\n");
		return 0;
	} else {
		printf(" NexInput standard not found.\n");
		if (!PathFileExists(NexLibDll))
			printf(" Dll not found.\n");
		return 1;
	}
}

void NexInputFree() {
	NEXInputGetState = NULL;
	NEXInputSetState = NULL;
	NEXInputGetInfo = NULL;
	NEXInputPowerOff = NULL;

	if (hDll != NULL) {
		FreeLibrary(hDll);
		hDll = nullptr;
	}

	//CoFreeUnusedLibraries();
}

int main()
{
	SetConsoleTitle(_T("NexInput Get Data"));
	
	bool DebugMode = false;

	NexInputInit();

	if (NexInputInit() == 1) {
		system("pause");
		return 1;
	}

	while (true) {
		system("cls");

		if ((GetAsyncKeyState('R') & 0x8000) != 0) {
			NexInputFree();
			hDll = NULL;
			NexInputInit();
			printf("\n Restart gamepads\n");
			Sleep(300);
			continue;
		}

		for (int i = 0; i < NEX_INPUT_MAX_COUNT; i++) {
			if (NEXInputGetState == NULL || NEXInputGetState(i, &NexControllerInputState) == ERROR_DEVICE_NOT_CONNECTED)
				continue;

			printf("\n Gamepad index = %d, name = ", i);

			NEXInputGetInfo(i, &NexControllerInfo);
			switch (NexControllerInfo.ControllerType) {
				case MICROSOFT_XBOX_CONTROLLER:
				{
					printf("MICROSOFT XBOX CONTROLLER\n");
					break;
				}
				case MICROSOFT_XBOX_360_CONTROLLER:
				{
					printf("MICROSOFT XBOX 360 CONTROLLER\n");
					break;
				}
				case MICROSOFT_XBOX_ONE_CONTROLLER:
				{
					printf("MICROSOFT XBOX ONE CONTROLLER\n");
					break;
				}
				case SONY_DUALSHOCK_3_CONTROLLER:
				{
					printf("SONY DUALSHOCK 3 CONTROLLER\n");
					break;
				}
				case SONY_DUALSHOCK_4_CONTROLLER:
				{
					printf("SONY DUALSHOCK 4 CONTROLLER\n");
					break;
				}
				case SONY_DUALSHOCK_5_CONTROLLER:
				{
					printf("SONY DUALSHOCK 5 CONTROLLER\n");
					break;
				}
				case NINTENDO_SWITCH_PRO_CONTROLLER:
				{
					printf("NINTENDO SWITCH PRO CONTROLLER\n");
					break;
				}

				default:
				{
					printf("NEX UNKNOWN CONTROLLER\n");
					break;
				}
			}

			
			if (NexControllerInfo.SupportRotation)
				printf(" Gyroscope support, Yaw=%7.2f, Pitch=%7.2f, Roll=%7.2f\n", NexControllerInputState.Yaw, NexControllerInputState.Pitch, NexControllerInputState.Roll);
			else
				printf(" Gyroscope unsupport \n");
			printf(" AxisLX=%d, AxisLY=%d, AxisRX=%d, AxisRY=%d\n", NexControllerInputState.AxisLX, NexControllerInputState.AxisLY, NexControllerInputState.AxisRX, NexControllerInputState.AxisRY);
			printf(" LeftTrigger=%d, RightTrigger=%d\n", NexControllerInputState.LeftTrigger, NexControllerInputState.RightTrigger);

			printf(" Buttons pressed: ");
			if (DebugMode)
				printf("(%d)", NexControllerInputState.Buttons);

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

			if (NexControllerInputState.Buttons == 0 && DebugMode == false) printf("-");
			

			NEXInputGetInfo(i, &NexControllerInfo);
			if (NexControllerInfo.ConnectType == NEX_CONTROLLER_WIRELESS)
				printf("\n Controller type: %d; connection: wireless, battery capacity: %d.\n", NexControllerInfo.ControllerType, NexControllerInfo.BatteryLevel);
			else
				printf("\n Controller type: %d, connection: wired, battery capacity: %d.\n", NexControllerInfo.ControllerType, NexControllerInfo.BatteryLevel);
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
		if ((GetAsyncKeyState('D') & 0x8000) != 0) DebugMode = true;

		printf("\n Ctrl - feedback (vibration and LED), Escape - exit.\n");

		Sleep(50);
	}

	NexInputFree();

	return 0;
}
