import robot
import Global
import actioncommand

from Task import BehaviourTask

# Sample behaviour to demonstrate how behaviour to motion interface works
# Switches between walk on the spot, kick, and dribble

class WalkKickDribble(BehaviourTask):
   
   def init(self):
      self.isActive = False
      self.walkFrames = 0
      self.kickOrDribble = 0

   def tick(self):
      # this tells localisation that we are lining up and to give us a more
      # stable ball position
      if Global.ballRelPos().x < 300 :
         self.world.b_request.doingBallLineUp = True
      
      active = self.world.blackboard.motion.active.body.actionType
      
      # walk on the spot for 2 seconds (behaviour runs at 30fps)
      if self.walkFrames < 60 :
         self.walkFrames += 1
         self.walk()
      elif self.walkFrames == 60 :
         if self.kickOrDribble == 0 :
            if self.isActive and active != robot.ActionType.KICK :
               # switch back to walk, dribble when walk finishes
               self.kickOrDribble = 1
               self.walkFrames = 0
               self.walk()
            else :
               self.world.b_request.actions.body = actioncommand.kick(turn=Global.ballHeading())
         else :
            if self.isActive and active != robot.ActionType.DRIBBLE :
               # switch back to walk, kick when walk finishes
               self.kickOrDribble = 0
               self.walkFrames = 0
               self.walk()
            else :
               self.world.b_request.actions.body = actioncommand.dribble(turn=Global.ballHeading())

      # motion will attempt to line up precisely to the ball before performing
      # kick or dribble. The active actionType will be LINE_UP if still lining up.
      # Motion will set active actionType to KICK or DRIBBLE when finished lining
      # up, and performance kick or dribble.
      # When kick or dribble finishes, motion will set active actionType back to WALK.
      if active == robot.ActionType.KICK or active == robot.ActionType.DRIBBLE :
         self.isActive = True

   def walk(self):
      self.world.b_request.actions.body = actioncommand.walk(turn=Global.ballHeading())
      self.isActive = False
