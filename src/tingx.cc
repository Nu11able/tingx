#include<unistd.h>
#include<sys/socket.h>
#include<sys/epoll.h>
#include<arpa/inet.h>
#include<signal.h>
#include<strings.h>

#include<iostream>
#include<vector>
#include<algorithm>
#include<map>
using namespace std;

bool exiting = false;


string GetName(struct sockaddr_in& addr) {
    string ip(20, 0);
    inet_ntop(addr.sin_family, &addr.sin_addr, &ip[0], ip.length());
    string ret(ip.c_str());
    ret.push_back(':');
    ret.append(to_string(addr.sin_port));
    return ret;
}

int main() {
    signal(SIGINT, [](int sig) { exiting = true; });

    int serv_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);

    bzero(&serv_addr, addr_len);
    serv_addr.sin_port = 65535;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(serv_fd, (struct sockaddr*)&serv_addr, addr_len);

    listen(serv_fd, 10);
    
    int epoll_fd = epoll_create(10);
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = serv_fd;

    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, serv_fd, &ev);

    vector<struct epoll_event> events(20);
    vector<int> opened {serv_fd, epoll_fd};
    map<int, string> name;
    string serv_str = GetName(serv_addr);
    cout << "server is working on: " << serv_str << endl;
    name[serv_fd] = serv_str;

    string recvbuf(1024, 0), sendbuf;
    for (;;) {
        int n = epoll_wait(epoll_fd, &events[0], events.size(), 100);

        for (int i = 0; i < n; ++i) {
            if (events[i].data.fd == serv_fd) {
                int clnt_fd = accept(serv_fd, (struct sockaddr*)&clnt_addr, &addr_len);
                string clnt_name = GetName(clnt_addr);
                cout << "connect from: " << clnt_name << endl;

                ev.events = EPOLLIN;
                ev.data.fd = clnt_fd;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, clnt_fd, &ev);
                name[clnt_fd] = clnt_name;
                opened.push_back(clnt_fd);
            } else {
                int n = read(events[i].data.fd, &recvbuf[0], recvbuf.length());
                cout << name[events[i].data.fd] << ">";
                for (int i = 0; i < n; i++)
                    cout << recvbuf[i];
                cout << endl;
                if (n == 0) {
                    cout << name[events[i].data.fd] << " close" << endl;
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, nullptr);
                    close(events[i].data.fd);
                    opened.erase(find(opened.begin(), opened.end(), events[i].data.fd));
                }
                write(events[i].data.fd, &recvbuf[0], n);
            }
        }

        if (exiting) break;
    }

    cout << endl << "exit..." << endl;

    for (auto &iter : opened) 
        close(iter);

    return 0;
}