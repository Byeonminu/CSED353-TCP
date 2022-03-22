Assignment 2 Writeup
=============

My name: [Byeon minwoo]

My POVIS ID: [minwoobyeon]

My student ID (numeric): [20200820]

This assignment took me about [10] hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): []

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Program Structure and Design of the TCPReceiver and wrap/unwrap routines:
[

    I used 'uint64_t', 'uint32_t' and type casting in wrap/unwrap.
    I made private member(isn/fin checker, WrappingInt32) in tcp_receiver.hh to check isn and fin.
    If isn checker is false, it push payload(data) to reassembler with index zero.
    If not, it would find checkpoint in absolute seqno using already wriiten bytes at first. 
    And then it find absolute seqno of header.seqno by using unwrap func to compute index that is closet to the checkpoint.
    Finally, it push payload(data) to reassembler with index this.

]

Implementation Challenges:
[

    I had difficulty in making unwrap function. I used sigend type at frist. But it made more inefficient codes and logic. 
    So I changed type to unsinged. As a result, I could make more simple logic.
    If code like 'uint64_t a = -4' runs, 'a' will have very large positive number.
    By using this property, if the position index of checkpoint is less than the difference of 'n' and (seqno)checkpoint, 
    it returns 'checkpoint + very positive number' to find absolute sequence number that corresponds to n that is closest to the checkpoint.

]

Remaining Bugs:
[
    well..
]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
