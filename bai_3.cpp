#include <bits/stdc++.h>
#include <fstream>
#include <stdio.h> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <sys/wait.h>
#include <cstring>
  
using namespace std;
const int maxn = 10000;
ifstream fi("file.inp");
ofstream fo("file.out");

struct mesg_buffer { 
    long mesg_type; 
    char content[1000];
} message; 

struct memo{
    int Tong;
    vector <int> X;
    memo(int _Tong, vector <int> _X){
        Tong = _Tong;
        X = _X;
    }
};
  
void toarrint(char str[], vector <int> &v){
    int t = 0;
    str[strlen(str)] = ' ';
    for (int i=0; i<strlen(str); i++){
        if (str[i] == ' '){
            v.push_back(t);
            t = 0;
        }
        else{
            t = t*10 + int(str[i]) - 48;
        }
    }
    
}

void toarrchar(vector <int> v, char str[]){
    string tmp_str;
    for (int i=0; i<v.size(); i++){
        string a;
        while (v[i] != 0){
            a = (char)(v[i]%10 + 48) + a;
            v[i] /= 10;
        }
        tmp_str += a;
        tmp_str += " ";
    }
    strcpy(str, tmp_str.c_str());
}

bool Compare(memo t_1, memo t_2){
    return t_1.Tong < t_2.Tong;
}

int main() 
{ 
    int pid;
    // Fork

	pid = fork();
    if (pid == 0){ // Tien Trinh Con (P1)
        // Nhan Tu MQ

		// Khoi Tao MQ
        key_t key; 
        int msgid;
        key = ftok("progfile", 65); 
        msgid = msgget(key, 0666 | IPC_CREAT);

		 
        vector <memo> Data_In; // Cac Day So
        mesg_buffer message_2;
		
		// Dem So Dong
        ifstream tmp_fi_1("file.inp"); 
        string tmp;
		int n = 0; // So Dong
		while (!tmp_fi_1.eof()){
			getline(tmp_fi_1, tmp);			
			if (tmp != ""){
				n++;
			}
		}
        tmp_fi_1.close();

		// Nhan Xu Lieu Tu MQ
        while (n--){
            cout << "MQ => P1\n";
	        msgrcv(msgid, &message_2, sizeof(message_2), 1, 0);
	        cout << "Data Receive 2: " << message_2.content << '\n';

	        vector <int> a;
	        toarrint(message_2.content, a);
	        int t = 0;
	        for (int i=0; i<a.size(); i++){
	            t += a[i];
	        }
	        sort(a.begin(), a.end()); // Sap Xep Day So Tang Dan
	        Data_In.push_back(memo(t, a)); // Day Day So Vo Data_In
        }
        cout << "Done MQ => P1 !!!\n\n";
        sort(Data_In.begin(), Data_In.end(), Compare); // Sap Xep Tang Dan Theo Tong


		msgctl(msgid, IPC_RMID, NULL);
		key = ftok("progfile", 65); 
        msgid = msgget(key, 0666 | IPC_CREAT); 

		// Gui Vao MQ
    	for (int i=0; i<Data_In.size(); i++){
			cout << "P1 => MQ\n";
			toarrchar(Data_In[i].X, message_2.content);
			message_2.mesg_type = 1; 
			msgsnd(msgid, &message_2, sizeof(message_2), 0);
			cout << "Data Send 2: " << message_2.content << '\n';

		}
        cout << "Done P1 => MQ !!!\n\n";
		return 0; 
    }
    else if (pid > 0){ // Tien Trinh Cha (P0)
        
		// Khoi Tao MQ
        key_t key; 
        int msgid;
        key = ftok("progfile", 65); 
        msgid = msgget(key, 0666 | IPC_CREAT);
        message.mesg_type = 1; 

		int n = 0; // So Dong
        while (!fi.eof()){
			// Doc file.inp
            string temp_content;
            getline(fi, temp_content);
			if (temp_content != ""){
            	cout << "P0 => MQ\n";
		        strcpy(message.content, temp_content.c_str());
		    	n++;
		        // Gui vao MQ  
		        msgsnd(msgid, &message, sizeof(message), 0);
		        cout << "Data Send 1: " << message.content << '\n';
			}
        } 
        cout << "Done P0 => MQ !!!\n\n";
        fi.close();
		
		// Doi Tien Trinh P1 Xu Ly
        wait(NULL);
		
		// Nhan Tu MQ		
        key = ftok("progfile", 65); 
        msgid = msgget(key, 0666 | IPC_CREAT);

        while (n--){
            cout << "P0 => file.out\n";
	        mesg_buffer message_3; 
	        msgrcv(msgid, &message_3, sizeof(message), 1, 0);
	        cout << "Data Receive: " << message_3.content << '\n';

	        // Ghi File
	        fo << message_3.content << '\n';
	        cout << "Write File: " << message_3.content << '\n';
        }
		fo.close();
        cout << "Done P0 => file.out !!!\n\n";
		msgid = msgget(key, 0666 | IPC_CREAT); // Xoa MQ
        wait(NULL);
    }
    cout << "DONE!!!\n";
    return 0; 
} 


/* Example

======= file.inp ========
1000 100 1 10000
4 40000 400 4000
500 5 5000 50000
20000 2 200 2000
300 30000 3 3000

======= file.out ========
1 100 1000 10000 
2 200 2000 20000 
3 300 3000 30000 
4 400 4000 40000 
5 500 5000 50000 

======= Execution ========
ubuntu@ubuntu:~/Desktop$ ./bai_3.out
P0 => MQ
Data Send 1: 1000 100 1 10000
P0 => MQ
Data Send 1: 4 40000 400 4000
P0 => MQ
Data Send 1: 500 5 5000 50000
P0 => MQ
Data Send 1: 20000 2 200 2000
P0 => MQ
Data Send 1: 300 30000 3 3000
Done P0 => MQ !!!

MQ => P1
Data Receive 2: 1000 100 1 10000
MQ => P1
Data Receive 2: 4 40000 400 4000
MQ => P1
Data Receive 2: 500 5 5000 50000
MQ => P1
Data Receive 2: 20000 2 200 2000
MQ => P1
Data Receive 2: 300 30000 3 3000
Done MQ => P1 !!!

P1 => MQ
Data Send 2: 1 100 1000 10000 
P1 => MQ
Data Send 2: 2 200 2000 20000 
P1 => MQ
Data Send 2: 3 300 3000 30000 
P1 => MQ
Data Send 2: 4 400 4000 40000 
P1 => MQ
Data Send 2: 5 500 5000 50000 
Done P1 => MQ !!!

P0 => file.out
Data Receive: 1 100 1000 10000 
Write File: 1 100 1000 10000 
P0 => file.out
Data Receive: 2 200 2000 20000 
Write File: 2 200 2000 20000 
P0 => file.out
Data Receive: 3 300 3000 30000 
Write File: 3 300 3000 30000 
P0 => file.out
Data Receive: 4 400 4000 40000 
Write File: 4 400 4000 40000 
P0 => file.out
Data Receive: 5 500 5000 50000 
Write File: 5 500 5000 50000 
Done P0 => file.out !!!

DONE!!!

*/

