# A timing utility that allows you to measure time and meet time targets with ease.
# blackboard.vision.timestamp is used to measure time, so the time units are nanoseconds.
# The restart(), start() and stop() methods are chainable, e.g.:
#
# countdown_timer = Timer(1000000).start()
# countdown_timer.restart().start()
#

blackboard = None
def update(newBlackboard):
   global blackboard
   blackboard = newBlackboard

class Timer(object):

  def __init__(self, timeTarget=0):

    self.timeTarget = timeTarget
    self.running = False
    self.elapsedTime = 0
    self.startTime = blackboard.vision.timestamp
    self.restart()

  # Resets the timer.  If it's running at the time, it will keep running.
  def restart(self):

    self.elapsedTime = 0
    self.startTime = blackboard.vision.timestamp

    return self

  # Start the timer.  Does nothing if it's already running.
  def start(self):

    if not self.running:
      self.startTime = blackboard.vision.timestamp
      self.running = True

    return self

  # Stops the timer.  Does nothing if it's already stopped.
  def stop(self):

    if self.running:
      self.elapsedTime += blackboard.vision.timestamp - self.startTime
      self.running = False

    return self

  # Returns how much time has elapsed so far.
  def elapsed(self):
    return self.elapsedTime + blackboard.vision.timestamp - self.startTime

  # Returns whether we have reached our target time or not
  def finished(self):
    return self.elapsed() >= self.timeTarget


class WallTimer(object):

  def __init__(self, timeTarget=0):
    self.timeTarget = timeTarget
    self.running = False
    self.startTime = blackboard.vision.timestamp

  def restart(self):
    self.startTime = blackboard.vision.timestamp
    return self

  def elapsed(self):
    return blackboard.vision.timestamp - self.startTime

  def elapsedSeconds(self):
    return self.elapsed() / 1000000.0

  def finished(self):
    return blackboard.vision.timestamp - self.startTime >= self.timeTarget
