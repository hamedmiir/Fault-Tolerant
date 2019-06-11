#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <map>
#include <fcntl.h>
#include <unistd.h>
using namespace std;

#define PIPE "./pipe"


void checking (int& fd, bool& done, bool& quit, string& td, map<string ,int>& res, char buffer[]) {
    int r;
    while (!done) {
        while ( (r = read(fd, buffer, 1024)) > 0) {
            td = buffer;

            if (td == "done") {
                done = true;
                break;
            }

            if (td == "quit") {
                quit = true ;
                done = true;
                break;
            }

            if (int(td[0]) < 48)
                continue;

            if (res.count(td) > 0)
                res.find(td) -> second++;
            else
                res.insert(pair<string,int> (td,1));
        }
    }
}

int main() {
    int fd;
    bool quit = false , done = false;
    string td;
    string k;
    // int r;
    char buffer[1024];
    int m = 0;

    if ( (fd = open(PIPE, O_RDONLY)) < 0)
        perror("open");
        cout << endl;

    map<string ,int>::iterator it; 
    map<string ,int> res;
    
    while(true) {
        done = false; //control with done flag

        checking(fd,done,quit,td,res,buffer);

        if (res.size() == 0)
            // << "XXXXXXXXXXXXXXXXXXX";
            continue;

        if (quit)
            break;

        m = 0;
        for (it = res.begin(); it != res.end(); it ++)
            if (it -> second >= m) {
                m = it -> second;
                k = it -> first;
            }
        cout << "Answer: " << k << endl;
        res.erase(res.begin(), res.end());
    }
    close(fd);
    return 0;
}