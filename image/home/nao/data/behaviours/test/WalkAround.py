# This is a sample behaviour to illustrate how to write your own
# By default, this behaviour will be run, but you can target any
# behaviour by using the -s flag on the command line

from Task import BehaviourTask, TaskState
from util.Timer import Timer
import actioncommand

class WalkAround(BehaviourTask):

    def init(self):
        self.current_state = WalkForwardState(self)

class WalkForwardState(TaskState):

    def init(self):
        self.timer = Timer(3000000).start()

    def transition(self):

        if self.timer.finished():
            return TurnState(self.parent)

        return self

    def tick(self):
        self.world.b_request.actions.body = actioncommand.walk(forward = 200)

class TurnState(TaskState):

    def init(self):
        self.timer = Timer(3000000).start()

    def transition(self):

        if self.timer.finished():
            return WalkForwardState(self.parent)

        return self

    def tick(self):
        self.world.b_request.actions.body = actioncommand.walk(turn = 1)