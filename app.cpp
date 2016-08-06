#include "app.h"

#include "Controler.h"

using namespace ev3api;

//Device objects
ColorSensor	gColorSensor(PORT_2);
//GyroSensor	gGyroSensor(PORT_4);
TouchSensor gTouchSensor(PORT_1);
SonarSensor	gSonarSensor(PORT_3);
Motor				gLeftWheel(PORT_C);
Motor				gRightWheel(PORT_B);
Motor				gTailWheel(PORT_D);

/* インスタンスの作成 */
static Controler *gControler;
static Prelude *gPrelude;
static ColorControl *gColorControl;
static ColorJudge *gColorJudge;
static ObjectDetection *gObjectDetection;
static Tracer *gTracer;

static void user_system_create() {
	tslp_tsk(2);

	gPrelude = new Prelude(gTouchSensor,
												 gColorSensor);
	gColorControl = new ColorControl(gColorSensor);
	gColorJudge = new ColorJudge(gColorSensor);
	gObjectDetection = new ObjectDetection(gSonarSensor);
	gTracer = new Tracer(gColorControl, 
											 gLeftWheel, 
											 gRightWheel,
											 gTailWheel);
	gControler = new Controler(gTracer,
														 gPrelude,
														 gObjectDetection,
														 gColorJudge);

	ev3_led_set_color(LED_ORANGE);
}

static void user_system_destroy() {
	gLeftWheel.reset();
	gRightWheel.reset();
	gTailWheel.reset();

	delete gPrelude;
	delete gColorControl;
	delete gColorJudge;
	delete gObjectDetection;
	delete gTracer;
	delete gControler;
}



/* 周期ハンドラ */
void tracer_cyc(intptr_t exinf) {
    act_tsk(TRACER_TASK);
}

/* ロボットを走らせるタスク */
void tracer_task(intptr_t exinf) {
  if (ev3_button_is_pressed(LEFT_BUTTON)) {
    wup_tsk(MAIN_TASK);         // 左ボタン押下でメインを起こす
  } else {
	  gControler->run();               // 走行
	}
  ext_tsk();
}

/* メインタスク */
void main_task(intptr_t unused) {
	user_system_create();
	gControler->init();
	ev3_sta_cyc(TRACER_CYC);
	slp_tsk();                    // 起きたら、走行をやめる
  ev3_stp_cyc(TRACER_CYC);
  gControler->terminate();
	user_system_destroy();
	ext_tsk();
}
