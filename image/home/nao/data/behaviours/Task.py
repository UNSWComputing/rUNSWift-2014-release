import Global
import Log

class BehaviourTask(object):
   """Behaviour Task.

   General class for an option/action in our behaviour model. Each task directly performs some action or delegates
   to a subtask. It can also have complex states (TaskState's) if needed.
   """

   def __init__(self, world=None, *args, **kwargs):

      self.world = world
      self.current_state = None
      self.prev_state = None

      self.init(*args, **kwargs)

   def printStateChange(self, prevState, nextState):
      """Print State Change.

      Prints the change in state to terminal in our common format if the state changes.
      """

      if prevState != nextState:
         Log.info(
            "%20s: %20s -> %20s (%ld | %ld)",
            self.__class__.__name__,
            prevState.__class__.__name__,
            nextState.__class__.__name__,
            Global.currentVisionTime(),
            Global.ticksSinceStart()
         )

   def transition(self):
      """Transition.

      By default this transitions the current_state into the next, but this can be overridden to support more
      complex (or more simple) behaviour
      """
      next_state = self.current_state.transition()
      self.printStateChange(self.current_state, next_state)
      self.current_state = next_state

   def init(self):
      """Init.

      This is called after initialising the BehaviourTask. Here you should set the current_state variable to the 
      initial state of the machine.  Override this if you want to add instance variables, rather
      than __init__(). If you do override __init__, make sure to call the super __init__ or define the variables
      set up in this class's __init__.
      """

      pass


   def tick(self, *args, **kwargs):
      """Tick.

      Gets called once on each frame. If you don't want to manage calling transitions, just override _tick() instead
      which this one will call after calling transition().
      """

      # TODO(Ritwik): Add debugging.

      # Transition into whatever state.
      self.transition()

      self._tick(*args, **kwargs)

   def _tick(self):
      """_Tick.

      Function to be overriden by concrete classes to perform some action in each frame. Note, you don't return
      a behaviour request. Just set the b_request variable.  By default this just runs the tick() of the current_state. 
      """
      self.current_state.tick()


class TaskState(object):

   # constructor, called on entry
   def __init__(self, parent, *args, **kwargs):
      
      self.parent = parent
      self.world = parent.world

      self.init(*args, **kwargs)

   # Here any state configuration variables can be set
   def init(self):
      pass

   # call each cycle we are in this state.  Modifies the world.b_request object as needed.  
   def tick(self):
      raise NotImplementedError()

   # returns the next state
   def transition(self):
      return self

   # called when moving to a different state
   def leave(self):
      raise NotImplementedError()

   def enter(self):
      raise NotImplementedError()
