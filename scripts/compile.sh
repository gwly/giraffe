cd ..
g++ -g -O2 -o monitor  monitor.cc utils.cc log.cc capture_net_packet.cc shunt_net_packet.cc combine_dc_packet.cc uncompress_dc_packet.cc luaroutine.cc configparser.cc DIDTemplateToLuaStruct.cc bitstream.cpp crc16.cpp mword.cpp extract_dc.cc generalcps.cpp l2compress.cpp -lpcap -lpthread -llog4cpp -lzmq -lluajit-5.1 -ldl -lz -I /usr/include/libxml2 -lxml2 2>&1 | tee scripts/compile.txt
