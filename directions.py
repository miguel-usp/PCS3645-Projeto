
class DirectionSet():
    def __init__(self, directions):
        self.directions = directions

    def __iter__(self):
        return iter(self.directions)

    def __len__(self):
        return len(self.directions)

    def __add__(self, other):
        return DirectionSet(self.directions + other.directions)

    def __str__(self):
        return str(self.directions)


class AccelerationDS(DirectionSet):
    def __init__(self, duration, steps, finalSpeed=None, finalSpeedLeft=None, finalSpeedRight=None, startSpeed=0, startSpeedLeft=0, startSpeedRight=0):
        if finalSpeed == None:
            directions = list(map(lambda x: (startSpeedLeft + (finalSpeedLeft - startSpeedLeft) * x / (steps-1),
                              startSpeedRight + (finalSpeedRight - startSpeedRight) * x / (steps-1), duration/steps), range(0, steps)))
        else:
            directions = list(map(lambda x: (startSpeed + (finalSpeed - startSpeed) * x / (steps-1),
                              startSpeed + (finalSpeed - startSpeed) * x / (steps-1), duration/steps), range(0, steps)))
        super().__init__(directions)


class ConstantSpeedDS(DirectionSet):
    def __init__(self, duration, speed=None, speedLeft=None, speedRight=None):
        if speed == None:
            super().__init__([(speedLeft, speedRight, duration)])
        else:
            super().__init__([(speed, speed, duration)])
