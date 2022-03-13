#include "stream_reassembler.hh"

#include <iostream>
#include <functional>
// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&.../* unused */) {}

using namespace std;

inforsubstr::inforsubstr(size_t _first, size_t _last, string _data):first(_first), last(_last), data(_data) {}


StreamReassembler::StreamReassembler(const size_t capacity):private_unassembled_bytes(0),  storelist({}),
 firstindex(0), _output(capacity), _capacity(capacity) {}


//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof)
{

    checkeof = eof == true ? eof : checkeof;
    if (!data.size()) 
    {
        if (checkeof && firstindex == index) // 마지막 
             _output.end_input();

          return; // 필수
           
    }

    size_t sub_begin = index;                     // substring 시작 index
    size_t sub_end = sub_begin + data.size() - 1; // substring 끝 index
    string sub_string = data;

    if (sub_end < firstindex) // 이미 write한 substring일 경우
        return;


    list<inforsubstr>::iterator iter;
    bool is_inserted = false;


    for (iter =storelist.begin(); iter != storelist.end(); iter++){ // first index가 낮은 순으로 list에 insert

        auto temp = *iter;

        if (temp.first + temp.data.size() < sub_begin) continue; //순서대로 넣기 위해 자리를 찾는 과정

        if (sub_end + 1 < temp.first)
        { // 자신이 들어갈 자리 발견하면 insert 하고 끝내기
                is_inserted = true;
                inforsubstr inserttemp(sub_begin, sub_end, sub_string);
                storelist.insert(iter, inserttemp);
                private_unassembled_bytes += sub_string.size(); // 아직 정렬 안된 bytes들
                break;   
        }
        else
        { // overlap -> 합집합을 storelist에 저장해준다.

            list<inforsubstr>::iterator temp2 = iter;
            iter++;
            private_unassembled_bytes -= temp.data.size();
            storelist.erase(temp2);

            if (temp.first <= sub_begin && temp.last >= sub_end) //안에 있을 때
            {
                sub_string = temp.data;
                sub_begin = temp.first;
                sub_end = temp.last;
            }
            else if (temp.first <= sub_begin && temp.last <= sub_end) // temp.first sub.begin temp.last sub.end 순
            {
                sub_string = temp.data + sub_string.substr(temp.last + 1 - sub_begin);
                sub_begin = temp.first;
                // sub end는 그대로
            }
            else if (temp.first >= sub_begin && temp.last >= sub_end) // sub.begin temp.first sub.end temp.last 순
            {
                sub_string = sub_string + temp.data.substr(sub_end + 1 - temp.first);
                sub_end = temp.last;
                // sub begin은 그대로
            }
            iter--;
        }
    }


    if (!is_inserted) //위에서 overlap이 되었을 경우
    { // overlap 처리한 거 insert
        inforsubstr temp(sub_begin, sub_end, sub_string);
        storelist.push_back(temp);
        private_unassembled_bytes += sub_string.size();
    }


    if (storelist.front().first < firstindex)
    { // substring의 앞부분이 overlap 된 경우
        private_unassembled_bytes -= storelist.front().data.size();

        storelist.front().data = storelist.front().data.substr(firstindex - storelist.front().first);
        storelist.front().first = firstindex;

        private_unassembled_bytes += storelist.front().data.size();

    }
    if (storelist.back().last + 1 >= firstindex + _capacity)
    { // 맨 뒤일 경우
        private_unassembled_bytes -= storelist.back().data.size();

        storelist.back().data = storelist.back().data.substr(0, firstindex + _capacity - storelist.back().first);
        storelist.back().last = firstindex + _capacity - 1;

        private_unassembled_bytes += storelist.back().data.size();

    }
    
    if (storelist.front().first == firstindex)
    { // 딱 맞을 때
        size_t rc = _output.remaining_capacity();
        string send;

        if (rc < storelist.front().data.size()) // remaining capacity가 들어갈 data size 보다 작을 경우
        {
            send = storelist.front().data.substr(0, rc); // 보낼 수 있는 만큼만 보낸다
            storelist.front().first += rc;               // front data 보낸거 다음 부터로 시작 위치 바꾼다
            storelist.front().data = storelist.front().data.substr(rc);
            firstindex = storelist.front().first;
        }
        else
        { // capacity 부족 하지 않을 경우
            send = storelist.front().data;
            firstindex = storelist.front().last + 1;
            
            storelist.pop_front();
        }

        private_unassembled_bytes -= send.size();
        _output.write(send);

        if (empty() && checkeof) // end
            _output.end_input();
    }
}


size_t StreamReassembler::unassembled_bytes() const {
     return private_unassembled_bytes;  }

bool StreamReassembler::empty() const { 
    return storelist.size() == 0 && unassembled_bytes() == 0;   }
