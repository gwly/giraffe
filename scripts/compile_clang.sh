cd ..
clang++ -g -v -fsanitize=address -fno-omit-frame-pointer -o monitor monitor.cc utils.cc log.cc capture_net_packet.cc shunt_net_packet.cc combine_dc_packet.cc uncompress_dc_packet.cc luaroutine.cc business_error_info.cc configparser.cc DIDTemplateToLuaStruct.cc bitstream.cpp crc16.cpp mword.cpp extract_dc.cc generalcps.cpp l2compress.cpp -llog4cxx -lpcap -lpthread -llog4cpp -lz -lzmq -lluajit-5.1 -I /usr/include/libxml2 -lxml2 2>&1 | tee scripts/compile_clang.out