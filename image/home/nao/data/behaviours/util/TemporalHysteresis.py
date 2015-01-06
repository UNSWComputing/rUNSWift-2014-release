

class TemporalHysteresis:
   def __init__(self, initialState, historyLength, stateSwitchBonus):
      self.historyLength = historyLength
      self.stateSwitchBonus = stateSwitchBonus

      self.currentState = initialState

      self.stateVotes = []
      self.stateVotes.append(self.currentState)


   def update(self, newState, votesWeight = 1):
      for i in range(0, int(votesWeight)):
         self.stateVotes.insert(0, newState)
      
      while len(self.stateVotes) > self.historyLength:
         self.stateVotes.pop()

      votesTally = {}
      for vote in self.stateVotes:
         if vote in votesTally:
            votesTally[vote] += 1
         else:
            votesTally[vote] = 0

      mostVotes = 0
      bestState = self.currentState

      for state in votesTally:
         if votesTally[state] > mostVotes:
            mostVotes = votesTally[state]
            bestState = state

      if bestState != self.currentState:
         self.currentState = bestState

         self.stateVotes = []
         for i in xrange(0, self.stateSwitchBonus):
            self.stateVotes.append(self.currentState)

      return self.currentState

