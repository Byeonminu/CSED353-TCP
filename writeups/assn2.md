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

]

Implementation Challenges:
[

    I had difficulty in making unwrap function. I used sigend type at frist. But it made more inefficient codes and logic. 
    So I changed type to unsinged. As a result, I could make more simple logic.

]

Remaining Bugs:
[
    well..
]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
