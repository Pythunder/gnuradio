/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "tcp_connection.h"
#include <gnuradio/basic_block.h>
#include <gnuradio/blocks/pdu.h>

namespace gr {
  namespace blocks {

    tcp_connection::sptr tcp_connection::make(boost::asio::io_service& io_service, int MTU/*= 10000*/, bool no_delay/*=false*/)
    {
      return sptr(new tcp_connection(io_service, MTU, no_delay));
    }

    tcp_connection::tcp_connection(boost::asio::io_service& io_service, int MTU/*= 10000*/, bool no_delay/*=false*/)
      : d_socket(io_service)
      , d_block(NULL)
      , d_no_delay(no_delay)
    {
      d_buf.resize(MTU);
      try {
        d_socket.set_option(boost::asio::ip::tcp::no_delay(no_delay));
      }
      catch (...) {
        // Silently ignore failure (socket might be current in accept stage) and try again in 'start'
      }
    }

    void
    tcp_connection::send(pmt::pmt_t vector)
    {
      size_t len = pmt::length(vector);
      size_t offset = 0;
      std::vector<char> txbuf(std::min(len, d_buf.size()));
      while (offset < len) {
        size_t send_len = std::min((len - offset), txbuf.size());
        memcpy(&txbuf[0], pmt::uniform_vector_elements(vector, offset), send_len);
        offset += send_len;
        boost::asio::async_write(d_socket, boost::asio::buffer(txbuf, send_len),
			       boost::bind(&tcp_connection::handle_write, this,
					   boost::asio::placeholders::error,
					   boost::asio::placeholders::bytes_transferred));
      }
    }

    void
    tcp_connection::start(gr::basic_block *block)
    {
      d_block = block;
      d_socket.set_option(boost::asio::ip::tcp::no_delay(d_no_delay));
      d_socket.async_read_some(boost::asio::buffer(d_buf),
        boost::bind(&tcp_connection::handle_read, this, 
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
    }

    void
    tcp_connection::handle_read(const boost::system::error_code& error, size_t bytes_transferred)
    {
      if (!error) {
        if (d_block) {
          pmt::pmt_t vector = pmt::init_u8vector(bytes_transferred, (const uint8_t*)&d_buf[0]);
          pmt::pmt_t pdu = pmt::cons(pmt::PMT_NIL, vector);

          d_block->message_port_pub(PDU_PORT_ID, pdu);
        }

        d_socket.async_read_some(boost::asio::buffer(d_buf),
          boost::bind(&tcp_connection::handle_read, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
      }
    }
  } /* namespace blocks */
}/* namespace gr */
