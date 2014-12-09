
#include "behaviors.h"
#include "system_clock.h"
#include "mGeneral.h" // for NULL

// global
// BehaviorUpdateFunc behaviorUpdateFunc = NULL;
// BehaviorStopFunc behaviorStopFunc = NULL;

CurrentBehaviorInfo currentBehavior = {
  .behavior = NULL,
  .state = 0,
  .startTime = 0
};

// FIXME: These all need to be made fancier to take into account transitions and stuff. Right now they just abruptly switch.

void behaviorStart(const RobotBehavior *behavior)
{
  currentBehavior.startTime = millis();
  currentBehavior.state = 0;
  currentBehavior.behavior = behavior;
}

void behaviorUpdate()
{
  uint32_t behaviorTime = millis() - currentBehavior.startTime;
  if (currentBehavior.behavior != NULL)
    currentBehavior.state = currentBehavior.behavior->update(behaviorTime);
  // switch (robotBehavior)
  // {
  //   case JERBOA_GAIT_RUNNER:
  //     behaviorState = jerboaGaitRunnerUpdate(behaviorTime);
  //     break;
  //   case JERBOA_STATIC_STAND:
  //     behaviorState = jerboaStaticStandUpdate(behaviorTime);
  //     break;
  //   case JERBOA_SIT:
  //     behaviorState = jerboaSitUpdate(behaviorTime);
  //     break;
  //   case JERBOA_DYNAMIC_STAND:
  //     behaviorState = jerboaDynamicStandUpdate(behaviorTime);
  //     break;
  //   case JERBOA_HOP:
  //     behaviorState = jerboaHopUpdate(behaviorTime);
  //     break;
  //   case JERBOA_FREE_FALL:
  //     behaviorState = jerboaFreeFallUpdate(behaviorTime);
  //     break;
  //   case NONE:
  //   default:
  //     // nothing
  //     break;
  // }
}
