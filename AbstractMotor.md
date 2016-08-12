@class AbstractMotor AbstractMotor.h

### Description of multi-DOF motor coordination

The AbstractMotor base class is useful for coordinating multi-DOF appendages. Some useful derived classes are MinitaurLeg, SagittalPair, etc.

To use these classes:

1. Each motor needs to be intialized (by BlCon34::init(), DxlMotor::init(), etc.) even though they are being coordinated together, just as in step 1 in the [MotorController help page](@ref MotorController)
2. Call the constructor MinitaurLeg::MinitaurLeg() (or whichever AbstractMotor -derived class) with pointers to constituent Motor s
3. Set the direction and zero of each Motor object first (do this before applying power)
4. Call AbstractMotor::enable() (or on derived class)
5. As in the Motor class, AbstractMotor::setOpenLoop(), AbstractMotor::setGain() and AbstractMotor::setPosition() exist, but they have a new first argument. The first argument represents the end-effector coordinate. For example, in MinitaurLeg, the first argument can be `EXTENSION` (defined as 0) or `ANGLE` (defined as 1).
6. Call AbstractMotor::setOpenLoop() with first argument in (0, 1, ...) indicating the end-effector coordinate, and second argument between -1 and 1 to spin freely
7. Call AbstractMotor::setGain() and AbstractMotor::setPosition() to command a position
8. Call AbstractMotor::getPosition() or AbstractMotor::getVelocity() to get back data
9. AbstractMotor::update() must be called frequently (don't call Motor::update() on the constituent motors)

An example for MinitaurLeg is found on its help page.

#### Creating a new multi-DOF appendage (derived class)

1. Implement forward kinematics in AbstractMotor::physicalToAbstract
2. Implement mapping of end effector forces to joint torques in AbstractMotor::abstractToPhysical
