#include <mbed.h>
#include <rtos.h>
#include "myDrivers.h"

//DigitalOut g_LED1(LED1);
//DigitalOut g_LED2(LED2);
Serial pc(SERIAL_TX, SERIAL_RX, 115200);

//测试使用bitset、ParallelOutput/vector<uint8_t>/PwmPeriodOutputArray的速度差别
#define TEST_PdmPeriodOutputArrayBitset
//#define TEST_PdmPeriodOutputArrayUint8
//#define TEST_PwmPeriodOutputArray
#define SAMPLE_RATE 1e3f

static void ThreadBody()
{	
	//测试使用bitset、ParallelOutput和使用vector<uint8_t>的速度差别
	Timer timer;
	timer.start();

#ifdef TEST_PdmPeriodOutputArrayBitset
	//使用bitset、ParallelOutput
	//PdmPeriodOutputArrayBitset
	//	construct : 0.002310s
	//	setFrq : 0.013223s
	//	setSins : 0.013010s
	//	setPhases : 0.013513s
	//	setP2ps : 0.013523s
	//	setFrq : 0.027787s
	pc.printf("PdmPeriodOutputArrayBitset\r\n");
	PdmPeriodOutputArrayBitset<3> pdmPeriodOutputArray(
	{
		DigitalOut(LED1),DigitalOut(LED2),DigitalOut(LED3)
	},
		SAMPLE_RATE
	);
	BuzzerArray<3> buzzers(pdmPeriodOutputArray);
#endif // TEST_PdmPeriodOutputArrayBitset
#ifdef TEST_PdmPeriodOutputArrayUint8
	//使用vector<uint8_t>
	//PdmPeriodOutputArrayUint8
	//	construct : 0.002248s
	//	setFrq : 0.011445s
	//	setSins : 0.011405s
	//	setPhases : 0.011950s
	//	setP2ps : 0.011960s
	//	setFrq : 0.023954s
	pc.printf("PdmPeriodOutputArrayUint8\r\n");
	PdmPeriodOutputArrayUint8<3> pdmPeriodOutputArray(
	{
		PdmPeriodOutput(LED1),PdmPeriodOutput(LED2),PdmPeriodOutput(LED3)
	},
		SAMPLE_RATE
	);
	BuzzerArray<3> buzzers(pdmPeriodOutputArray);
#endif // TEST_PdmPeriodOutputArrayUint8
#ifdef TEST_PwmPeriodOutputArray
	//使用PwmPeriodOutputArray
	//PwmPeriodOutputArray
	//	construct : 0.001887s
	//	setFrq : 0.001538s
	//	setSins : 0.001493s
	//	setPhases : 0.001576s
	//	setP2ps : 0.001556s
	//	setFrq : 0.003194s
	pc.printf("PwmPeriodOutputArray\r\n");
	PwmPeriodOutputArray<3> pwmPeriodOutputArray(
	{
		PwmPeriodOutput(PE_9),
		PwmPeriodOutput(PE_11),
		PwmPeriodOutput(PE_13),
	//	PwmPeriodOutput(PE_14),
	//	PwmPeriodOutput(PA_5),
	//	PwmPeriodOutput(PB_15),
	//	PwmPeriodOutput(PB_10),
	//	PwmPeriodOutput(PB_11),
	},
	100e3f,
	SAMPLE_RATE / 8//如果只需要达到3bitDAC效果
	);
	BuzzerArray<3> buzzers(pwmPeriodOutputArray);
#endif // TEST_PwmPeriodOutputArray
	
	pc.printf("construct: %fs\r\n", timer.read());

	timer.reset();
	buzzers.setFrq(2);
	pc.printf("setFrq: %fs\r\n", timer.read());

	timer.reset();
	buzzers.setSins(1, 0);
	pc.printf("setSins: %fs\r\n", timer.read());

	Thread::wait(2000);

	timer.reset();
	buzzers.setPhases({ 0,PI / 3,PI * 2 / 3 });
	pc.printf("setPhases: %fs\r\n", timer.read());

	Thread::wait(2000);

	timer.reset();
	buzzers.setP2ps({ 0.1,0.5,0.9 });
	pc.printf("setP2ps: %fs\r\n", timer.read());

	Thread::wait(2000);

	timer.reset();
	buzzers.setFrq(1);
	pc.printf("setFrq: %fs\r\n", timer.read());



	for (;;)
	{
		//g_LED1 = !g_LED1;
		//pc.printf("%f\r\n", pdmPeriodOutputArray.getActualRate());
		Thread::wait(500);
	}
}

Thread thread(osPriorityNormal, uint32_t(4 * 1024));

int main()
{
	thread.start(callback(ThreadBody));
	
	for (;;)
	{
		//g_LED2 = !g_LED2;
		//pc.printf("%u, %u\r\n", thread.stack_size(), thread.used_stack());
		Thread::wait(300);
	}
}