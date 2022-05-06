Assignment 5 Writeup
=============

My name: [Byeon minwoo]

My POVIS ID: [minwoobyeon]

My student ID (numeric): [20200820]

This assignment took me about [10] hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): []

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Program Structure and Design of the NetworkInterface:
[
    I made new struct for ARP and Ethernet.
    And I generated two lists for unreplied ARP request and not yet sent datagrams.
    also, I used 'map' data structure to map IP with Ethernet address. 
]

Implementation Challenges:
[
    it remains bug becaues of tcpsender(assn3), tcpconnection(assn4)
    Although I tried debuging by using assn1, assn2, assn3 and assn4 bestcode, I could not fix unclean shutdown.
    I don't know what is wrong
]

Remaining Bugs:
[
    unclean shutdown
    31 - t_webget (Failed)
]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
