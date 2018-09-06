client:client.cpp client.h client_main.cpp  ../Window/Window.h ../Window/Window.cpp ../comm/UserData/UserData.h ../comm/UserData/UserData.cpp \
	../md5/md5.cpp ../md5/md5.h
	g++ -o $@ $^ -lpthread -ljson -lncursesw
.PHONY:clean
clean:
	rm client
