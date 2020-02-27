# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Rules for generating the source and sink xml wrappers
########################################################################
include(GrPython)

macro(GEN_BLOCK_XML _generator _xml_block)
    set(generator ${CMAKE_CURRENT_SOURCE_DIR}/${_generator})
    set(xml_block ${CMAKE_CURRENT_BINARY_DIR}/${_xml_block})
    list(APPEND xml_blocks ${xml_block})
    add_custom_command(
        DEPENDS ${generator} OUTPUT ${xml_block}
        COMMAND ${PYTHON_EXECUTABLE} ${generator} ${xml_block}
    )
endmacro(GEN_BLOCK_XML)

GEN_BLOCK_XML(gen_uhd_usrp_blocks.py    uhd_usrp_source.xml)
GEN_BLOCK_XML(gen_uhd_usrp_blocks.py    uhd_usrp_sink.xml)

add_custom_target(uhd_grc_xml_blocks ALL DEPENDS ${xml_blocks})

install(FILES
    ${xml_blocks}
    uhd_amsg_source.xml
    uhd_block_tree.xml
    DESTINATION ${GRC_BLOCKS_DIR}
    COMPONENT "uhd_python"
)

if(ENABLE_UHD_RFNOC)
    install(FILES
        rfnoc.domain.yml
        uhd_fpga_clk.domain.yml
        uhd_fpga_ctrl.domain.yml
        uhd_fpga_data.domain.yml
        uhd_fpga_ddc.block.yml
        uhd_fpga_duc.block.yml
        uhd_fpga_fft.block.yml
        uhd_fpga_io_ctrl_port.domain.yml
        uhd_fpga_io_radio.domain.yml
        uhd_fpga_io_time_keeper.domain.yml
        uhd_fpga_radio.block.yml
        uhd_fpga_sep.block.yml
        uhd_fpga_x310.block.yml
        uhd_rfnoc_ddc.block.yml
        uhd_rfnoc_duc.block.yml
        uhd_rfnoc_fft.block.yml
        uhd_rfnoc_graph.block.yml
        uhd_rfnoc_rx_radio.block.yml
        uhd_rfnoc_rx_streamer.block.yml
        uhd_rfnoc_tx_radio.block.yml
        uhd_rfnoc_tx_streamer.block.yml
        DESTINATION ${GRC_BLOCKS_DIR}
    )
endif(ENABLE_UHD_RFNOC)
