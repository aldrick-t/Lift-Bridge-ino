# Lift Bridge
Lift Bridge program for Arduino Mega 2560. Created as final project for IDeSM-S6.



# Version History

V1: ALPHA

  V1.1:
    Original base system. (UNFUNCTIONAL)
    
  V1.2:
    Changes to logic tree, creating 4 seperate conditonal trees.
    
  V1.3:
    Added Chrono lib. to facilitate timed processes without delay.
    
  V1.4:
    After failed testing, decision to move to V2 was made.

V2: BETA
  V2.1:
    After analizing errors from V1, severe changes were made to logic tree.
    Started changing process tree to singular linear system.
  V2.2:
    Test results improved, started search for libraries to create mutltiple process threads.
  V2.3:
    Switched trafficLight_bridge function logic to Chrono lib. for better timekeeping.
  V2.4:
    Adjusted/simplified process tree.
    Switched several conditional processes to while loops to avoid block skipping. (VIEW NECESSITY)
  V2.5:
    Added TaskScheduler lib. to solve V2.2.
    Attempting to create tasks.
    Contemplating change to stepper motor.
  V2.6:
    Purchased wrong stepper motors, cancelled change to steppers.
    Keeping gearmotor-pulley system.
  V2.7:
    Reached point of no end with sensors, started development of V4.
    V2 is ON HOLD.
  V2.8:
    Once V2 is has resumed development.
    Using logic tree from V4.D, adapt for full auto sensor system.
    Move to GAMMA-1.

V3: GAMMA-1
  FULLY AUTOMATIC system, using sensors for both traffic entry/exit.
  Clean program, optimized version after V2 (beta) development stage.
  

V4: GAMMA-2
  FULLY MANUAL system, using IR remote as input.
  Program created as backup if sensor system failed. Allows for 2-way traffic.

  V4.A:
    Standard V4 Program, following logic tree similar to V2.
  V4.B:
    Moved process tree to a task from task TaskScheduler lib..
    Possibly fixes problems from ocupying loop func. with main program.
    Loop func. is kept clean.
    V4.B.1:
      First iteration of V4.B, applying former changes.
    V4.B.2:
      Major changes to process tree.
    V4.B.3:
      Input reading by TaskScheduler lib. is UNFUNCTIONAL in while loops.
      Switched reading to functions.
    V4.B.4:
      First FULLY FUNCTIONAL version. Version V4.B.4 is now V4.D.
      No longer part of V4.B version family.
      After severe development, now moved to dedicated version.
  V4.C:
    Hybrid simplified design, derived from V4.B, keeping TaskScheduler lib. for switch readings.
  V4.D:
    (V4.B.4)
    Fully FUNCTIONAL, derived from V4.B.3 with its change to functions for input readings.
    Improvements were made, bugs were fixed in segments after bridge open stage.
  V4.E: 
    Same logic tree as V4.D, removes TaskScheduler after V4.B.3 made it obsolete.
  V4.F:
    Upgrades V4.E program, adding failsafe function if bridge leaf is ajar.
    Adds semaphore system for road and river traffic.
  V4.G:
    Adapts program for both bridge halves.
    Should achieve absolute functionality.

V5: GAMMA-3

  HYBRID system, using IR remote to trigger bridge lift-->close cycle.
  Automatic bridge close with exit sensors, allows for 1-way traffic only.

V6: DELTA

  Final Programs.
  
  V6.A: 
    Microcontroller versions.
    Optimized for ARDUINO upload.
    
  V6.B: 
    Evaluation versions.
    For viewing and grading.
    
  V6.C: 
    Publish versions.
    For public web upload.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
CURRENT RELEASE VERSION:
  GAMMA-2:
    V4.F

CURRENT DEV VERSION:
  GAMMA-2: 
    V4.G
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
[NOTES]

Logic tree IS FINALIZED, GAMMA-2 is in final stages.
GAMMA-3 is queued for development.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Forecast:
  GAMMA-2 completed after full assembly.
  GAMMA-3 to be developed after GAMMA-2 completion, possibly cancelled.
  Finishing circuitry (monday MAY30).
  Assembly on tight schedule (JUNE1).
  GAMMA-1 cancelled (~~~).

  





