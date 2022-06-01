from logging import warning
from typing import Dict
import re
import sys

WARNING = "Warning"
ERROR = "Error"
REPORT = "Report"


class Log:
    def __init__(self, prefix: str, code: int, codes: Dict[int, str]):
        self.code = code
        self.codes = codes
        self.prefix = prefix

    def __str__(self):
        return "{}{}: {}".format(self.prefix, self.code, self.codes[self.code])

    def __repr__(self):
        return self.__str__()


class Error(Log):

    def __init__(self, code: int, arg=None):
        codes = {
            1: "A state '{}' is not in the set of states".format(arg),
            2: "Some states are disjoint",
            3: "A transition '{}' is not represented in the alphabet".format(arg),
            4: "Initial state is not defined",
            5: "Input file is malformed"
        }
        super().__init__("E", code, codes)


class Report(Log):
    def __init__(self, code: int):
        codes = {
            1: "FSA is complete",
            2: "FSA is incomplete"
        }

        super().__init__("R", code, codes)

    def __str__(self):
        return self.codes[self.code]


class Warning(Log):
    def __init__(self, code: int):
        codes = {
            1: "Accepting state is not defined",
            2: "Some states are not reachable from the initial state",
            3: "FSA is nondeterministic"
        }
        super().__init__("W", code, codes)


class Transition:
    transitions: Dict[str, str]
    name: str

    def __init__(self, accept: str, name: str, output: str):
        self.transitions = dict()
        self.name = name
        self.add_transition(accept, output)

    def add_transition(self, accept: str, output: str):
        self.transitions[accept] = output

    def make_transition(self, accept: str) -> str:
        assert accept in self.transitions
        return self.transitions[accept]


class FSA:
    fields = {
        "states": r'^\[(([a-z0-9]+|[a-z0-9]+(,([a-z0-9])+)+)*)]$',
        "alpha": r'^\[(([a-z0-9_]+|[a-z0-9_]+(,([a-z0-9_])+)+)*)]$',
        "init.st": r'^\[(([a-z0-9])*?)]$',
        "fin.st": r'^\[(([a-z0-9]+|[a-z0-9]+(,([a-z0-9])+)+)*)]$',
        "trans": r'^\[(([a-z0-9]+>[a-z0-9_]+>[a-z0-9]+)|([a-z0-9]+>[a-z0-9_]+>[a-z0-9]+)(,([a-z0-9]+>['
        r'a-z0-9_]+>[a-z0-9]+)+)*?)]$',
    }

    def __init__(self, fsa_description: str):
        self.dict, self.error = self.read(fsa_description)

        if not self.error:
            self.states = self.dict["states"]
            self.alphabet = self.dict["alpha"]
            self.initst = self.dict["init.st"]
            self.finst = self.dict["fin.st"]
            self.trans = self.dict["trans"]

    def read(self, fsa_description: str):
        def register(line: str):  # None, Error; Result (field, value), None
            splitted = line.strip().split("=")
            if len(splitted) != 2:
                return None, Error(5)
            field, value = splitted

            if field not in self.fields:
                return None, Error(5)

            if not re.match(self.fields[field], value):
                return None, Error(5)

            value = value[1:-1].split(",")

            if field == "trans":
                transitions: Dict[str, Transition] = dict()
                for t in value:
                    accept, name, output = t.split(">")

                    if name in transitions:
                        transitions[name].add_transition(accept, output)
                    else:
                        transitions[name] = Transition(accept, name, output)
                value = transitions.values()
            return (field, value), None

        lines = fsa_description.splitlines()

        fsa_dict = dict()

        for line in lines:
            res, error = register(line)
            if error is not None:
                return None, error

            field, value = res
            fsa_dict[field] = value
        return fsa_dict, None

    def validate(self):
        warnings = []

        if self.error is not None:
            return None, None, self.error

        # E4: Initial state is not defined
        if len(self.initst[0].strip()) == 0:
            return None, None, Error(4)

        # W1: Accepting state is not defined
        if len(list(filter(bool, self.finst))) == 0:
            warnings.append(Warning(1))

        # W2: Some states are not reachable from the initial state

        def is_reachable(state1, state2, called=None):
            if called is None:
                called = []
            if state1 in called:
                return False
            if state1 == state2:
                return True
            called.append(state1)

            possible_transitions = [
                t for t in self.trans if state1 in t.transitions.keys()]

            return any(
                is_reachable(t.make_transition(state1), state2, called) for
                t in possible_transitions)

        for s in self.states:
            if not is_reachable(self.initst[0], s):
                warnings.append(Warning(2))
                break

        for t in self.trans:
            for accept, output in t.transitions.items():
                if accept not in self.states:
                    return None, None, Error(1, accept)

                if output not in self.states:
                    return None, None, Error(1, output)

        # E1: A state 's' is not in the set of states
        for state in self.initst + self.finst:
            if state and state not in self.states:
                return None, None, Error(1, state)

        # E2: Some states are disjoint
        for state1 in self.states:
            for state2 in self.states:
                if state1 != state2 and not is_reachable(state1, state2) and not is_reachable(state2, state1):
                    return None, None, Error(2)

        # E3: A transition 'a' is not represented in the alphabet
        for t in self.trans:
            if t.name not in self.alphabet:
                return None, None, Error(3, t.name)

        # W3: FSA is nondeterministic

        def is_nondetermenistic():
            a_states = set(
                t for trans in self.trans for t in trans.transitions)
            return len(a_states) < len(self.trans)

        if is_nondetermenistic():
            warnings.append(Warning(3))

        # FSA is complete / incomplete
        if self.is_complete():
            return Report(1), warnings, None
        return Report(2), warnings, None

    def is_complete(self):
        if len(self.alphabet) != len(self.trans):
            return False

        for t in self.trans:
            if len(set(t.transitions.keys())) != len(self.states):
                return False

        #! Invalid!
        # for state1, state2 in self.states:
        #     if self.trans.transitions[state1] != state2:
        #         return False
        return True


with open('fsa.txt') as f:
    fsa = FSA(f.read())

with open('result.txt', 'w') as f:
    report, warnings, error = fsa.validate()
    sys.stdout = f

    if error is not None:
        print("Error:")
        print(error)
    else:
        print(report)
        if len(warnings):
            print("Warning:")
            print('\n'.join(map(str, warnings)))
