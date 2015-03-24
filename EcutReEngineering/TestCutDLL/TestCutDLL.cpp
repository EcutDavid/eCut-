#include "stdafx.h"
using namespace std;
#include <iostream> 
#include <windows.h>
using namespace System;
using namespace System::IO;
#include "ecutdevice.h"

eCutDevice eCut;

LPCWSTR stringToLPCWSTR(std::string orig);
eCutPosition GeneratePositon(double x, double y, double z);
eCutCartesian GenerateCartesian(double x, double y, double z);

void CheckResult(int flag)
{
	char charArr[200];
	if (flag == 0 || flag == 3)
	{
		sprintf(charArr, "\ninvoke success");
	}
	else
	{
		sprintf(charArr, "\ninvoke failed");
	}
	OutputDebugString(stringToLPCWSTR(charArr));

}

//0 & 3 Means Success
void TestDepthFunctionWithLine()
{
	char charArr[200];
	int DeviceNums = GetDeviceNum();
	if (DeviceNums)
	{
		//Open ASU
		sprintf(charArr, "Cut exist addcircle test begin");
		OutputDebugString(stringToLPCWSTR(charArr));
		eCutDevice cutHandler = eCutOpen(0);
		eCutError result = eCutConfigDeviceDefault(cutHandler);
		result = eCutSetStepsPerUnitSmoothCoff(cutHandler, 50,
			new int[9]{1600, 1600, 1600},new int[9],16);
		int stepArray[9];
		result = eCutGetSteps(cutHandler, stepArray);
		eCutSetCurrentPostion(cutHandler, &GeneratePositon(0, 0, 0));
		eCutSetCoordinate(cutHandler, new double[9]{0,0,0,0,0,0,0,0});
		//AddLineTest
		for (size_t i = 0; i < 200; i++)
		{
			result = eCutAddLine(cutHandler, &GeneratePositon(20, 0, 0), 5, 5, 5);
			eCutAddCircle(cutHandler, &GeneratePositon(20, 0, 0), &GenerateCartesian(15, 0, 0), &GenerateCartesian(0, 0, 1),0,5,5,5);
			eCutAddLine(cutHandler, &GeneratePositon(20, -20, 0), 5, 5, 5);
			eCutAddLine(cutHandler, &GeneratePositon(0, -20, 0), 5, 5, 5);
			eCutAddLine(cutHandler, &GeneratePositon(0, -0, 0), 5, 5, 5);
		}
		
		while (true)
		{
			int activeDepth = eCutActiveDepth(cutHandler);
			int queueDepth  = eCutQueueDepth(cutHandler);
			eCutGetSteps(cutHandler, stepArray);
			sprintf(charArr, "\nactiveDepth: %d", activeDepth);
			OutputDebugString(stringToLPCWSTR(charArr));
			sprintf(charArr, "\queueDepth: %d",   queueDepth);
			OutputDebugString(stringToLPCWSTR(charArr));
			sprintf(charArr, "\nPosition X:%fX:%fY", stepArray[0] / (1600.0 * 16), stepArray[1] / (1600.0 * 16));
			OutputDebugString(stringToLPCWSTR(charArr));
			Sleep(1500);
			if (queueDepth == 0)
			{
				sprintf(charArr, "\nTestEnd");
				OutputDebugString(stringToLPCWSTR(charArr));
				exit(0);
			}
		}
	}
}

eCutCartesian GenerateCartesian(double x, double y, double z)
{
	eCutCartesian pos;
	pos.x = x;
	pos.y = y;
	pos.z = z;
	return pos;
}

eCutPosition GeneratePositon(double x, double y, double z)
{
	eCutPosition pos;
	pos.a = pos.b = pos.c = pos.u = pos.v = pos.w = 0;
	pos.x = x;
	pos.y = y;
	pos.z = z;
	return pos;
}

void GerserialNumber()
{
	char serialNumber[12];
	eCutError result = GetDeviceInfo(0, serialNumber);//������Ϻ󼴿���serialNumber�õ��忨���к�
}

void SetStepsPerUnitSmoothCoff()
{
	//�������巽����ʱ50,����ϸ����32, X,Y,Z,A������ÿ���׽�Ϊ400����/����
	eCutError result = eCutSetStepsPerUnitSmoothCoff(eCut, 50, new int[9]{400, 400, 400, 400}, new int[9], 32);
}

void SmoothCoffStepsPerUnit()
{
	unsigned int smoothCoff;
	//eCutError result = eCutGetSmoothCoff(eCut, &smoothCoff);

	int stepsPerUnitArray[9];
	eCutError result = eCutGetStepsPerUnit(eCut, stepsPerUnitArray);
}

void SetCoordinate()
{
	double posArr[9] = { 30, 40, 50 };
	//���� X���е����30mm Y���е����40mm Z���е����50mm
	eCutError result = eCutSetCoordinate(eCut, posArr);
}

void MaxSpeedAndSoftLimit()
{
	//double maxSpeedArr[9];
	//eCutError result = eCutGetMaxSpeed(eCut, maxSpeedArr);

	double maxSpeedArr[9] = {100, 100, 100, 100};
	double minSoftLimit[9] = { -100, -100, -100, -100 };
	eCutError result = eCutSetSoftLimit(eCut, maxSpeedArr, minSoftLimit);

	double acceleration[9] = {5, 5, 5, 5};
	double maxSpeed[9] = {10, 10, 10, 10};
	result = eCutSetAccelarationMaxSpeed(eCut, acceleration, maxSpeed);
}

void Move()
{
	char charArr[200];
	//X,Y,Z,A�������˶�
	eCutMoveAtSpeed(eCut, 15, new double[9]{5, 5, 5, 5, 5}, new double[9]{5, 5, 5, 5, 5});
	//Y�������˶�
	eCutMoveAtSpeed(eCut, 2, new double[9]{5, 5, 5, 5, 5}, new double[9]{5, 5, 5, 5, 5});

	eCutJogOn(eCut, 1,new double[9]{0, 50});
	eCutMoveAbsolute(eCut, 15, new double[9]{0, 50, 50, 50});
	while (true)
	{
		int steps[4];
		eCutGetSteps(eCut, steps);
		sprintf(charArr, "\n steps of x: %d y: %d b: %d", steps[0], steps[1], steps[4]);
		OutputDebugString(stringToLPCWSTR(charArr));
		Sleep(500);
	}
}

void CutPCMoveDone()
{
	eCutError result = eCutIsDone(eCut);
	eCutPosition Position;
	Position.x = 0; Position.y = 0; Position.z = 0;
	Position.a = 0; Position.b = 0; Position.c = 0;
	Position.u = 0; Position.v = 0; Position.w = 0;
	eCutSetCurrentPostion(eCut, &Position);
	Position.x = -5000; Position.y = 5; Position.z = 0;
	Position.a = 0; Position.b = 0; Position.c = 0;
	Position.u = 0; Position.v = 0; Position.w = 0;
	result = eCutAddLine(eCut, &Position, 5, 5, 5);
    result = eCutIsDone(eCut);
}

void GetInput()
{
	//��I5�����룬����I������������inputArr[0]�õ�32
	unsigned short inputArr[1];
	eCutGetInputIO(eCut, inputArr);
}

//lib�������µĵ��´˴������޷�����
//void SetOutput()
//{
//	//����OUT5 ��OUT6���Ϊ��
//	eCutSetOutput(eCut, 0, new short[4], new unsigned short[4]{32 + 64});
//}

void AxisOutputConfig()
{
	//����X,Y�ȸ������������X,Y�ȸ�������������������ʹ��X,Y,Z,A����������������岻����
	eCutSetAxisOutputConfig(eCut, new INT8U[11]{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 0x60, 0x62}, 
		new INT8U[11]{ 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x61, 0x63},
		new BOOLEAN[9]{true, true, true, true}, 0, 0);

	//����1�������ǣ�X��Y�����ʵ�����Ž���
	eCutSetAxisOutputConfig(eCut, new INT8U[11]{ 1, 0, 2, 3, 4, 5, 6, 7, 8, 0x60, 0x62},
		new INT8U[11]{ 0x11, 0x10, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x61, 0x63},
		new BOOLEAN[9]{true, true, true, true}, 0, 0);

	//����1�������ǣ�X��Y�����壬�����������
	eCutSetAxisOutputConfig(eCut, new INT8U[11]{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 0x60, 0x62},
		new INT8U[11]{ 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x61, 0x63},
		new BOOLEAN[9]{true, true, true, true}, 3, 3);
}

void SetInputIOEngineDir()
{
	uINT64U inputIOEnable;
	inputIOEnable.all = 3;
	uINT64U inputIONeg;
	inputIONeg.all = 0;
	//ʹ��X����������λ���ܣ��ߵ�ƽ������X++ ��������ΪIN1 X--��������ΪIN2
	eCutSetInputIOEngineDir(eCut, inputIOEnable, inputIONeg, new INT8U[30]{1,2},new INT8S[10]);
}

int _tmain(int argc, _TCHAR* argv[])
{
	char charArr[200];
	//TestDepthFunctionWithLine();
	eCutDevice cutHandler;
	eCut = eCutOpen(0);
	eCutConfigDeviceDefault(eCut);
	GetInput();
	eCutPosition Position;
	Position.x = 0; Position.y = 0; Position.z = 0;
	Position.a = 0; Position.b = 0; Position.c = 0;
	Position.u = 0; Position.v = 0; Position.w = 0;
	eCutSetCurrentPostion(eCut, new eCutPosition{-5000});
	Position.x = -5000; Position.y = 5; Position.z = 0;
	Position.a = 0 ; Position.b = 0; Position.c = 0;
	Position.u = 0 ; Position.v = 0; Position.w = 0;
	eCutAddLine(eCut, &Position, 5.0, 5.0, 5.0);
	//X�˶���PC�滮��������ϵ-5000���������˶���0
	eCutAddLine(eCut, new eCutPosition{ -5000 }, 5, 5, 5);
	//X�˶���PC�滮��������ϵ-5000��Y 100,�������˶���0
	eCutAddLine(eCut, new eCutPosition{ 50,100 }, 5, 5, 5);

	//���赱ǰλ��ΪPC�滮��������ϵ����㣬��(25,0,0)ΪԲ�ģ��˶���(50,0,0)
	eCutAddCircle(eCut, new eCutPosition{ 50 }, new eCutCartesian{ 25 }, new eCutCartesian{0, 0, 1}, 0, 5, 5, 5);

	while (true)
	{
		int steps[4];
		eCutGetSteps(eCut, steps);
		sprintf(charArr, "\n steps of x: %d %d", steps[0], eCutIsDone(eCut));
		OutputDebugString(stringToLPCWSTR(charArr));
		Sleep(500);
	}
	for (size_t i = 0; i < 15; i++)
	{
		int pulseArr[9];
		Sleep(2000);
		//eCutGetSteps(eCut, pulseArr);
		/*for (size_t j = 0; j < 4; j++)
		{
			cout << "+" << pulseArr[j];
		}
		cout << endl;*/
	}
}


LPCWSTR stringToLPCWSTR(string orig)
{
	size_t origsize = orig.length() + 1;
	const size_t newsize = 100;
	size_t convertedChars = 0;
	wchar_t *wcstring = (wchar_t *)malloc(sizeof(wchar_t)*(orig.length() - 1));
	mbstowcs_s(&convertedChars, wcstring, origsize, orig.c_str(), _TRUNCATE);
	return wcstring;
}