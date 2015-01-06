from test.WalkAround import WalkAround
from Task import BehaviourTask

class Default(BehaviourTask):

    def init(self):
        self.demo_skill = WalkAround(self.world)

    def tick(self):
        self.demo_skill.tick()