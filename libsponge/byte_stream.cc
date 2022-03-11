#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&.../* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity) { max = capacity; }

size_t ByteStream::write(const string &data)
{
    // DUMMY_CODE(data);
    size_t size = remaining_capacity() >= data.size() ? data.size() : remaining_capacity();
    content += data.substr(0, size); // write
    alwritten += size;
    return size;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const
{
    // DUMMY_CODE(len);
    size_t size = buffer_size() >= len ? len : buffer_size();
    std::string outputcopy = content.substr(0, size); // copy
    return outputcopy;
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len)
{
    // DUMMY_CODE(len);
    size_t size = buffer_size() >= len ? len : buffer_size();
    content.erase(0, size); // remove
    alread += size;
    return;
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len)
{
    // DUMMY_CODE(len);
    size_t size = buffer_size() >= len ? len : buffer_size();
    std::string buffercopy = peek_output(size); // copy
    pop_output(size);                           // pop
    return buffercopy;
}

void ByteStream::end_input() { end = true; }

bool ByteStream::input_ended() const { return end; }

size_t ByteStream::buffer_size() const { return content.size(); }

bool ByteStream::buffer_empty() const { return content.empty(); }

bool ByteStream::eof() const { return end && buffer_empty(); }

size_t ByteStream::bytes_written() const { return alwritten; }

size_t ByteStream::bytes_read() const { return alread; }

size_t ByteStream::remaining_capacity() const
{
    size_t size = max - content.size();
    return size;
}
