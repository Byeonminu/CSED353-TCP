Assignment 3 Writeup
=============

My name: [Byeon minwoo]

My POVIS ID: [minwoobyeon]

My student ID (numeric): [20200820]

This assignment took me about [15] hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): []

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Program Structure and Design of the TCPSender:
[
    I generated Timer class and private/public method to be needed to calculate RTO.
    And 'initial_retransmission_timeout' saves the 'initial value' of the RTO.
    I generated 'unacked' by queue data structure to store unacknowledged segments in order.
    In ack_received method, I chcekcd absolute # of ackno at first. And then, it is compared with unacked.front().
    All segments whose absolute # of header.seqno is less than absolute # of ackno can be poped by loop.

]

Implementation Challenges:
[
    In assignment pdf, there is 'it reads from its input ByteStream and sends as many bytes as possible in the form of TCPSegments, as long as there are new bytes to be read and space available in the window.' sentence. 
    I had a lot of trouble at first. Then, I used syn checker, eof and buffer_empty method of Bytestream, seq #, and 'loop'.
    As a result, I could fill the window with as many bytes as possible.
]

Remaining Bugs:
[
    Sometimes, it failed 17th test case. I don't know the reason.
]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
