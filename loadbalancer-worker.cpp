#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <sstream>
#include <dirent.h>
//fork
#include <sys/wait.h>
#include <unistd.h>
//pipe
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;

// #define MAINNAMEDPIPE "./mainfifo"
// #define WORKERSPIPE "./workersfifo"

#define PIPE "./pipe"

#define AMPERSAND 38

void extractFiles(string directory ,vector<string> &files)
{
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir (directory.c_str())) != NULL) {
    while ((ent = readdir (dir)) != NULL) {
        string current_file = ent->d_name;
        if (current_file.size() > 4){
            string format = current_file.substr(current_file.size()-4 , current_file.size()-1);
        if (format == ".txt")
            files.push_back(directory + "/" + ent->d_name);
        }
        if (ent->d_type == 4 && (current_file != "." && current_file != ".."))
            extractFiles(directory + "/" + ent->d_name ,files);
    }
        closedir (dir);
  } else {
    perror ("open directory failed!");
  }
}

// void createNamedPipe(){
//     mkfifo(MAINNAMEDPIPE,0666);
//     mkfifo(WORKERSPIPE,0666);
// }

int readFile(string directory ,int s)
{
    ifstream file;
    string config_line;
    file.open(directory ,ios::in);
    int sensorFile, data;
    if (file.is_open()) {
        while (getline(file, config_line))
        {
            istringstream iss(config_line);
            if (!(iss >> sensorFile >> data)) { 
              break; 
            }
            if (sensorFile == s) {
                file.close();
                return data;
            }
        }
        file.close();
    }
    return -1;
}

void handleInput(string& directory ,string& sensorNumber, char msg[])
{
    bool amp = false;
    for(int i = 0; i < 100; i++) {
        if (int(msg[i]) == AMPERSAND )  {
            amp = true;
            continue;
        }
        if (amp)
            directory += msg[i];
        else
            sensorNumber += msg[i];
    }
}

void createWorkers(vector<string> files,vector<int*>& fd, string ss ){
  // int k = 0;
  // vector<int*> fd;
  // cout << ss;
  // string sensorNumber, dir;
  string data;
  int fd2;
  // string val;

  //generate FDs
  for(size_t i = 0; i < files.size(); i++)
    fd.push_back(new int[2]);


  for (int i = 0; i < files.size(); i++){
    if (pipe(fd[i])== -1){
      perror("pipe failed!");
      exit(1);
    }
    int pid = fork();
    if (pid == 0){
      close(fd[i][1]);
      char msg[1000];
      read(fd[i][0], msg, 1000); 
      // close(fd[i][0]);
      //char* argv[3] = {"worker",msg,NULL};
      //execv("./worker", argv);
      close(fd[i][0]);
      string dir, sensorNumber;
      handleInput(dir,sensorNumber,msg);
      int num = readFile(dir,atoi(sensorNumber.c_str()) );
      if (num == -1) 
        exit(0);
      
      string val = to_string(num);
      if ( (fd2 = open(PIPE, O_WRONLY)) < 0)
        cout << "load fifo failed!" << endl;
      
      if ( write(fd2,val.c_str(), val.size()+1) != val.size()+1) {
        close(fd2);
        exit(1);
      }
      close(fd2);
      exit(0);
    }
      close(fd[i][0]);
      data = ss + "&" + files[i];
      write(fd[i][1], data.c_str() , (data.length())+1);
      close(fd[i][1]);
      // exit(1);
  }
    // string data;
    // data = filterString + " @ ";
    // for (int j = 0; j < workerFiles[i]; j++){
    //     data = data + dirPath + "/" + dirFiles[k];
    //     data = data + " ";
    //     k++;
    // }
    //write(fd[i][1], data.c_str() , (data.length())+1);
    //close(fd[i][1]);
}

// void createPresenter(){
//   int pid = fork();
//   if(pid == 0){
//     ifstream fd;
//     fd.open(PIPE, fstream::in);
//     string configLine;
//     getline(fd,configLine);
//     fd.close();
//     execl("./presenter", "presenter", configLine.c_str(), NULL);
//   }
//   else if (pid > 0){
//     int num;
//     int fd = open(PIPE, O_WRONLY);

//     //write(fd,data.c_str(),(data.length())+1);
//     close(fd);
//   }
// }

void quit()
{
    int fd;
    if ( (fd = open(PIPE, O_WRONLY)) < 0)
        cout<<"load fifo failed!" << endl;

    if ( write(fd,"quit", 5) != 5) { 
        close(fd);
        exit(1);
    }
    close(fd);
}

void done()
{
    int d;
    if ((d = open("./pipe" ,O_WRONLY)) < 0) 
      cout<<"load fifo failed!"<<endl;

    if (write(d ,"done" ,5) != 5) {
      close(d);
      exit(1);
    }
    close(d);
}



int main() {
    vector<string> files;
    vector<int*> fd;
    // char msg[1000]; 

    extractFiles("." ,files);
    //cout << "\nThe first element is " << files[0];
    //int c = 5;
    //string cp = to_string(c);
    //cout << "\n" << c;

    mkfifo(PIPE ,0666);

    string ss;
    while(true){
      cin >> ss;
      if (ss == "quit"){ 
        quit();
        break;
      }
      createWorkers(files, fd, ss);
      for(int i = 0; i < files.size(); i++)
        wait(NULL);

      done();
    }
}