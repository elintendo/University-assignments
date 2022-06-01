import re
from collections import defaultdict
from typing import Dict, List, Optional, Tuple


class FSAError(Exception):
    def __init__(self, message: str, num: int):
        self.message = message
        self.num = num

    def __str__(self):
        return "E%d: %s" % (self.num, self.message)


class FSA:
    states: List[str]
    states_nums: Dict[str, int]
    alphabet: List[str]
    initial_state: str
    accepting_states: List[str]
    transitions: List[Tuple[str, str, str]]
    transitions_map: Dict[int, Dict[str, Optional[int]]]

    def __init__(self, description: str):
        try:
            self.try_to_parse_from_description(description)
        except Exception:
            raise FSAError("Input file is malformed", 0)
        self.validate()
        self.numerize_states()
        self.generate_transitions_map()

    def try_to_parse_from_description(self, description: str):
        # Read non-empty lines from the description
        lines = [line.strip() for line in description.split("\n") if line.strip()]

        s = r"[\w\d]+"  # Regexp for state
        a = r"[\w\d_]+"  # Regexp for alphabet
        t = f"{s}>{a}>{s}"
        # Creates regexp for list of regexps
        lst = lambda regex: f"{regex}|(?:{regex},)+{regex}|"

        self.states = re.match(
            rf"^states=\[({lst(s)})]$", lines[0]
        ).group(1).replace(",", " ").split()

        self.alphabet = re.match(
            rf"^alpha=\[({lst(a)})]$", lines[1]
        ).group(1).replace(",", " ").split()

        self.initial_state = re.match(
            rf"^initial=\[({s}|)]$", lines[2]
        ).group(1)

        self.accepting_states = re.match(
            rf"^accepting=\[({lst(s)})]$", lines[3]
        ).group(1).replace(",", " ").split()

        self.transitions = [
            (tr[0], tr[1], tr[2]) for tr in
            map(lambda trans: trans.split(">"), re.match(
                rf"^trans=\[({lst(t)})]$", lines[4]
            ).group(1).replace(",", " ").split())
        ]

        if not self.states and not self.alphabet:
            raise Exception()

    def validate(self):
        self.validate_no_unknown_states()
        self.validate_no_disjoint_states()
        self.validate_no_unknown_transitions()
        self.validate_initial_state()
        self.validate_deterministic()

    def validate_no_unknown_states(self):
        all_states = set()
        available_states = set(self.states)

        if self.initial_state:
            all_states.add(self.initial_state)

        all_states.update(self.accepting_states)
        for s1, _, s2 in self.transitions:
            all_states.add(s1)
            all_states.add(s2)

        for state in all_states:
            if state not in available_states:
                raise FSAError(
                    "A state '%s' is not in the set of states" % state, 1
                )

    def validate_no_disjoint_states(self):
        state_sets = [{s1, s2} for s1, _, s2 in self.transitions]

        was_merging = True
        while was_merging:
            was_merging = False
            unique = []
            while state_sets:
                set1 = state_sets[0]
                others = state_sets[1:]
                state_sets = []
                for set2 in others:
                    if set1.isdisjoint(set2):
                        state_sets.append(set2)
                    else:
                        was_merging = True
                        set1.update(set2)
                unique.append(set1)
            state_sets = unique

        # there are more than one set of states
        if len(state_sets) > 1:
            raise FSAError("Some states are disjoint", 2)

    def validate_no_unknown_transitions(self):
        transition_names = [t[1] for t in self.transitions]
        for name in transition_names:
            if name not in self.alphabet:
                raise FSAError(
                    "A transition '%s' is not represented in the alphabet" % name, 3
                )

    def validate_initial_state(self):
        if not self.initial_state:
            raise FSAError("Initial state is not defined", 4)

    def validate_deterministic(self):
        state_transition_pairs = [(t[0], t[1]) for t in self.transitions]
        unique_state_transition_pairs = set(state_transition_pairs)

        if len(state_transition_pairs) != len(unique_state_transition_pairs):
            raise FSAError("FSA is nondeterministic", 5)

    def numerize_states(self):
        self.states_nums = {self.initial_state: 0}
        n = 1
        for state in self.states:
            if state not in self.states_nums:
                self.states_nums[state] = n
                n += 1

    def generate_transitions_map(self):
        self.transitions_map = defaultdict(lambda: defaultdict(lambda: None))
        for s1, t, s2 in self.transitions:
            n1 = self.states_nums[s1]
            n2 = self.states_nums[s2]
            self.transitions_map[n1][t] = n2

    def generate_initial_regexps(self) -> Dict[Tuple[int, int], str]:
        n = len(self.states)
        initial_regexps = defaultdict(lambda: "{}")

        for s_from in range(n):
            for s_to in range(n):
                pair = s_from, s_to
                transitions = []

                for t in self.alphabet:
                    if self.transitions_map[s_from][t] == s_to:
                        transitions.append(t)

                if s_from == s_to:
                    transitions.append("eps")

                initial_regexps[pair] = "|".join(transitions) or "{}"

        return initial_regexps

    def to_regexp(self) -> str:
        n = len(self.states)
        regexps = [
            # Kleene's algorithm step -1
            self.generate_initial_regexps()
        ]

        # Kleene's algorithm steps (0 to n)
        for k in range(n):
            regexp = defaultdict(lambda: "{}")
            for s_from in range(n):
                for s_to in range(n):
                    pair = s_from, s_to
                    regexp[pair] = "(%s)(%s)*(%s)|(%s)" % (
                        regexps[k][(s_from, k)],
                        regexps[k][(k, k)],
                        regexps[k][(k, s_to)],
                        regexps[k][(s_from, s_to)]
                    )
            regexps.append(regexp)

        # create accepting states numbers in the required order
        accepting_states = set(self.accepting_states)
        accepting_states = [
            self.states_nums[s] for s in self.states if s in accepting_states
        ]
        initial = self.states_nums[self.initial_state]

        # combine all accepting regexps R[k][from (initial) to (accepting)]
        accepting_regexps = [
            regexps[n][(initial, accepting)] for accepting in accepting_states
        ]

        if not accepting_regexps:
            return "{}"

        return "|".join(accepting_regexps)


with open("input.txt", "r") as f:
    fsa_description = f.read()

try:
    output = FSA(fsa_description).to_regexp()
except FSAError as error:
    output = "Error:\n" + str(error)

with open("output.txt", "w") as f:
    f.write(output)
