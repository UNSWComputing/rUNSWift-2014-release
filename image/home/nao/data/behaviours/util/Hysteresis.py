# A utility class to help stop decisions from flicking between alternatives
# Methods are chainable, e.g. hysteresis.adjust(x < 20).is_max()
class Hysteresis(object):

  # Min and max can be set, but the initial value is always 0
  def __init__(self, min_value, max_value, up_amount=1, down_amount=1):
    self.min_value = min_value
    self.max_value = max_value
    self.up_amount = up_amount
    self.down_amount = down_amount
    self.value = 0
    self.true = None

  # Push the value towards the max by 1, or optionally by an amount you set
  def up(self, amount=None):
    if amount == None: amount = self.up_amount
    self.set(self.value + amount)
    return self

  # Push the value towards the min by 1, or optionally by an amount you set
  def down(self, amount=None):
    if amount == None: amount = self.down_amount
    self.set(self.value - amount)
    return self

  # Add a manually calculated value to the hysteresis
  def add(self, amount):
    self.set(self.value + amount)
    return self

  # On a particular condition, cause the Hysteresis to go up or down
  def adjust(self, condition, up_amount=None, down_amount=None):
    if condition:
      self.up(up_amount)
    else:
      self.down(down_amount)
    return self

  # Explicitly set the value of the hysteresis
  def set(self, amount):

    self.value = max(self.min_value, min(self.max_value, amount))

    if self.true is None:
      self.true = amount > 0
    elif self.value == self.max_value:
      self.true = True
    elif self.value == self.min_value:
      self.true = False

    return self

  # Return to 0
  def reset(self):
    self.true = None
    self.set(0)
    return self

  def resetMax(self):
    self.set(self.max_value)
    return self

  def resetMin(self):
    self.set(self.min_value)
    return self

  # Check whether we have hit the maximum value
  def is_max(self):
    return self.value == self.max_value

  # Check whether we have hit the minimum value
  def is_min(self):
    return self.value == self.min_value