#ifndef serial_h
#define serial_h

#include <boost/asio.hpp>
#include <boost/thread.hpp>

enum flush_type
{
    flush_receive = TCIFLUSH,
    flush_send = TCOFLUSH,
    flush_both = TCIOFLUSH
};

class ArduinoSerial
{
public:
    using Callback = std::function<void(bool, const std::vector<uint8_t> & buffer)>;

    ArduinoSerial(std::string port, unsigned int baud_rate, Callback callback)
    : serial(io,port), callback(callback)
    {
        using namespace boost::asio;
        serial.set_option(serial_port_base::baud_rate(baud_rate));
        serial.set_option(serial_port_base::flow_control(serial_port_base::flow_control::none));
        serial.set_option(serial_port_base::parity(serial_port_base::parity::none));
        serial.set_option(serial_port_base::stop_bits(serial_port_base::stop_bits::one));
        serial.set_option(serial_port_base::character_size(serial_port_base::character_size::character_size(8U)));

        flush(flush_receive);

        boost::this_thread::sleep(boost::posix_time::seconds(2));
    }

    void flush(flush_type what)
    {
        ::tcflush(serial.lowest_layer().native_handle(), what);
    }

    template<typename T>
    void send(const T & payload)
    {
        send(payload, sizeof(T));
    }

    template<typename T>
    void send(const T & payload, uint8_t length)
    {
        uint8_t header[2] = {0, length};
        boost::asio::write(serial,boost::asio::buffer(header, 2));
        boost::asio::write(serial,boost::asio::buffer(&payload, length));

        uint8_t result[2] = {0};
        boost::asio::read(serial,boost::asio::buffer(result, 2));

        buffer.resize(result[1]);

        if(result[0] == 0)
        {
            callback(false, buffer);
            return;
        }

        if(result[1] == 0)
        {
            callback(true, buffer);
            return;
        }

        boost::asio::read(serial, boost::asio::buffer(buffer));

        callback(true, buffer);
    }

private:
    boost::asio::io_service io;
    boost::asio::serial_port serial;
    std::vector<uint8_t> buffer;
    Callback callback;
};

#endif
