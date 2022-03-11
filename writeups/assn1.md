Assignment 1 Writeup
=============

My name: [Byeon minwoo]

My POVIS ID: [minwoobyeon]

My student IByeon minwoo(numeric): [20200820]

This assignment took me about [12] hours to do (including the time on studying, designing, and writing the code).

Program Structure and Design of the StreamReassembler:
[
    I created another class that can store first index and last index and string of substring.
    And, I used 'list' to remember substrings until they’re ready to be put into the stream.
    I sorted substrings in ascending order based on the first index of the substring.
    Then, if 'firstindex'(where the next substring start) is same with 'storelist.front().first'(first index of first obejct of list),
    it can write a substring of first object of list in output bytestrem.
    I handled storing substrings in list in 'loop('for')' by using iterator, and then I handled about output writing outside the loop

]

Implementation Challenges:
[
    I had difficulty in handling overlapping substirngs. 
    Solution : If the two substrings overlap, the combination of the two makes it a substring.
    And then, delete the overlap before the substring is put. 
]

Remaining Bugs:
[
    well..
]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
