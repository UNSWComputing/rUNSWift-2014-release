# Prevents a spatial boundary condition from flipping

# boundaries
#   The boundaries around each spatial state.  Must be ordered.  E.g. [ -10, 0, 20 ] defines 4
#   states between each of these values.  

# threshold
#   The amount by which we can step over a boundary while still remaining within a particular state

# state
#   The current state, represented as an integer.  If we are in state n, then we are >= boundaries[n-1]
#   and < boundaries[n]

class SpatialHysteresis(object):

    def __init__(self, boundaries, threshold):
        self.boundaries = boundaries # An array of boundaries
        self.threshold = threshold
        self.state = None

    def reset(self):
        self.state = None
        return self

    def update(self, value):

        # Check whether we're still within the bounds of our threshold
        if (self.state != None
        and (self.state >= len(self.boundaries) or value < self.boundaries[self.state] + self.threshold)
        and (self.state < 0 or value >= self.boundaries[self.state-1] - self.threshold)):
            return self

        # We exceeded the threshold, set the state to its appropriate region
        # This could be a binary search, but hey, fuck it
        self.state = 0
        for index, bound in enumerate(self.boundaries):
            if value >= bound:
                self.state = index + 1
            else:
                break

        return self