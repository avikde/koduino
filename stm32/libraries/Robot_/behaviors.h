#ifndef _behaviors_h_
#define _behaviors_h_

#include <stdint.h>
/*
Behavior documentation
----------------------
All behaviors come with a 
1. behaviorNameUpdate(behaviorTime) function. Should return 0 if it is continuing, 1 if it is completed.
2. behaviorNameStop() function: should block (calling update() itself) till it is safe to exit that behavior--basically go to a stand state or something? E.g. a sit behavior (after it is complete) should just return immediately. Should return not 0xff if it is continuing, 0xff if it is completed.
*/

// #include <robot/jerboa/jerboa_sit_stand.h>
// #include <robot/jerboa/jerboa_gait_runner.h>
// #include <robot/jerboa/jerboa_hop.h>
// #include <robot/jerboa/jerboa_free_fall.h>

// TODO: implement with function pointers for it to be "plugin-capable", i.e. robot_behaviors.c will not have to be modified to add behaviors
typedef uint8_t (*BehaviorUpdateFunc)(uint32_t);
// typedef uint8_t (*BehaviorStopFunc)(void);
extern BehaviorUpdateFunc behaviorUpdateFunc;
// extern BehaviorStopFunc behaviorStopFunc;

typedef struct {
  BehaviorUpdateFunc update;
} RobotBehavior;

typedef struct {
  const RobotBehavior *behavior;
  uint8_t state;
  uint32_t startTime;
} CurrentBehaviorInfo;
extern CurrentBehaviorInfo currentBehavior;

// typedef enum {
//   NONE, // behaviorUpdate() does nothing
//   JERBOA_SIT, JERBOA_STATIC_STAND, JERBOA_DYNAMIC_STAND, // jerboa_sit_stand.c
//   JERBOA_GAIT_RUNNER, // jerboa_gait_runner.c
//   JERBOA_HOP, // jerboa_hop.c
//   JERBOA_FREE_FALL, // jerboa_free_fall.c
// } RobotBehavior;

// Mostly for debugging. This should never need to be called directly; behaviorStart() is supposed to handle transitions better.
// extern RobotBehavior robotBehavior;
// Behaviors can optionally set the state. 0 is starting, 0xff is done, all the rest are free to use. This is the return value of

// Stops the previous behavior (waiting till it is ok), and then starts the new one. Also checks that the transitions make sense. E.g. from sit you can only go to some kind of stand, and running / hopping etc. can only be started from stand.
// NULL for no behavior
void behaviorStart(const RobotBehavior *behavior);
void behaviorUpdate();

#endif
