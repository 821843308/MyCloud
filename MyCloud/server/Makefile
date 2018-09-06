server:server.h server.cpp server_main.cpp ../comm/transfer/transfer.h ../myDB/myDB.h ../myDB/myDB.cpp ../comm/UserData/UserData.h ../comm/UserData/UserData.cpp
	g++ -o $@ $^ -lpthread -ljson -I/usr/include/mysql -L/usr/lib64/mysql -lmysqlclient
.PHONY:
clean:
	rm server
