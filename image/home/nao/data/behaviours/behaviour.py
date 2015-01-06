from Constants import LEDColour
from util import FieldGeometry
from util import Timer
from util import Sonar
from Task import BehaviourTask

import skills.HeadSkill
import world
import robot
import traceback
import Log

import pkgutil

# Modules to update on each tick.
from util import TeamStatus
import Global

skill_instance = None

class DummySkill(object):
   def __init__(self, blackboard):
      self.dummy = None
      #TODO(Ritwik): Move back to top when it's converted.

def catch_all(tick):

   def catcher(blackboard):
      try:
         return tick(blackboard)
      except:
         Log.error("Behaviour exception:", exc_info = True)
         raise

   return catcher

@catch_all
def tick(blackboard):

   # Update all blackboard dependent helper modules.
   Global.update(blackboard)
   TeamStatus.update(blackboard)
   FieldGeometry.update(blackboard)
   Timer.update(blackboard)
   Sonar.update(blackboard)

   global skill_instance
   if not skill_instance:
      skill = blackboard.behaviour.skill
      # Load the module and the class we're going to use.
      found_skill = False
      SkillClass = None
      behaviour_packages = [ "roles", "skills", "test" ]
      for package in behaviour_packages:
         if skill not in [name for _, name, _ in pkgutil.iter_modules(["/home/nao/data/behaviours/%s" % package])]:
            Log.info("%s wasn't in %s, skipping import attempt.", skill, package)
            continue
         try:
            skill_module = __import__("%s.%s" % (package, skill), fromlist=[skill])
            # Access the class so we can do some reflection.
            SkillClass = getattr(skill_module, skill)
            found_skill = True
            Log.info("Successfully imported %s from %s.%s", skill, package, skill)
            break
         except ImportError, e:
            Log.error("%s %s", package, e)
            Log.error(traceback.format_exc())

      if not found_skill:
         raise ImportError("Could not find skill: %s in any of our behaviour folders." % skill)

      if issubclass(SkillClass, BehaviourTask):
         new_world = world.World(blackboard) # It's a whole new world.
         skill_instance = SkillClass(new_world)
      else:
         parentSkill = DummySkill(blackboard)
         skill_instance = SkillClass(blackboard, parentSkill)

   if isinstance(skill_instance, BehaviourTask):
      # On every tick of the perception thread, we update the blackboard, tick the skill, and then return the resulting
      # behaviour request.
      skill_instance.world.update(blackboard)
      skill_instance.world.b_request = robot.BehaviourRequest()
      skill_instance.world.b_request.actions = robot.All()
      skill_instance.tick()
      request = skill_instance.world.b_request
   else:
      # Backwards compat for old style skills if called directly via -s.
      request = skill_instance.tick(blackboard)


   headRequest = skills.HeadSkill.instance.tick(blackboard)
   request.actions.head = headRequest.actions.head

   # Eye LED setting.
   if len(blackboard.vision.balls) > 0:
      # Right Eye Red if you can see a ball.
      request.actions.leds.rightEye = LEDColour.red
   else:
      request.actions.leds.rightEye = LEDColour.off

   if Global.amILost():
      # Left Eye Red if you're 'lost', cyan if you're not.
      request.actions.leds.leftEye = LEDColour.red
   else:
      request.actions.leds.leftEye = LEDColour.cyan

   return request