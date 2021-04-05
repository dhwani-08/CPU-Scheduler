#include <bits/stdc++.h>
using namespace std;

struct bursts {
    string burstType;
    int burstVal;
    int remainingBurst;
};

struct process {
    int p_id;
    int arrival_time;
    int priority;
    int share;
    queue < struct bursts> burst;
    int turnaround_time;
    int waiting_time;
    int response_time;
    int start_time = -1;
    int completion_time;
    int burst_time = 0;
    bool operator==(struct process p){
        return p.p_id;
    }
};

void removeProcess(vector<struct process> & cpu, int idx) {
    cpu.erase(
        remove_if(cpu.begin(), cpu.end(), [&](struct process const & p) {
            return p.p_id == idx;
        }),
        cpu.end());
}

struct find_id : std::unary_function<struct process, bool> {
    int id;
    find_id(int id):id(id) { }
    bool operator()(struct process const& p) const {
        return p.p_id == id;
    }
};

int main(){
    FILE *fp = fopen("p.dat", "r");
    if(fp==NULL)
        printf("Error in opening file");
    else{
        char * line = NULL;
        size_t len = 0;
        ssize_t read;
        int n;
        if((read = getline(&line, &len, fp)) != -1)
            n = atoi(line);
        struct process p[n];
        int i=0;
        while ((read = getline(&line, &len, fp)) != -1) {
            char* token = strtok(line, " ");
            p[i].p_id = atoi(token);
            token = strtok(NULL, " ");
            p[i].arrival_time = atoi(token);
            token = strtok(NULL, " ");
            p[i].priority = atoi(token);
            token = strtok(NULL, " ");
            p[i].share = atoi(token);
            token = strtok(NULL, " ");
            while(token != NULL && atoi(token)!=-1){
                struct bursts b;
                b.burstType = token;
                token = strtok(NULL, " ");
                b.burstVal = atoi(token);
                token = strtok(NULL, " ");
                b.remainingBurst = b.burstVal;
                p[i].burst.push(b);
                p[i].burst_time = p[i].burst_time + b.burstVal;
            }
            i++;
        } 
        fclose(fp);
        int current_time = 0;
        int completed = 0;
        bool mark[n];
        for(int i=0; i<n; i++)
            mark[i]=false; 
        float total_turnaround_time = 0;
        float total_waiting_time = 0;
        float total_response_time = 0;
        vector <struct process> cpu;
        queue <struct process> io;
        queue <int> occur;
        queue <int> iooccur;
        int tickets[100];
        int x=0;
        for(i=0; i<n; i++){
            for(int j=0; j<p[i].share; j++)
                tickets[x++]=i;
        }
        for(int i=0; i<n; i++){
            if(p[i].arrival_time == 0 && mark[i]==false){
                mark[i]=true;
                cpu.push_back(p[i]);
            }
        }
        while(completed != n ){
            bool cpu_occupied = false;
            bool io_occupied = false;
            int cpid = -1;
            for(int i=0; i<n; i++){
                if(current_time>=p[i].arrival_time && mark[i]==false){
                    mark[i]=true;
                    cpu.push_back(p[i]);
                }
            }
            current_time++;
            if(!cpu.empty()){
                cpu_occupied = true;
                bool gotTicket = false;
                int idx;
                while(gotTicket == false){
                    x = rand()%100;
                    idx = tickets[x];
                    if(find_if(cpu.begin(), cpu.end(), find_id(idx+1)) != cpu.end() && p[idx].burst.front().remainingBurst!=0)
                        gotTicket = true;
                }
                cpid = idx+1;
                if(p[idx].start_time == -1)
                    p[idx].start_time = current_time-1;
                occur.push(idx+1);
                p[idx].burst.front().remainingBurst--;
                if(p[idx].burst.front().remainingBurst == 0){
                    p[idx].burst.pop();
                    removeProcess(cpu, idx+1);
                    if(p[idx].burst.empty()){
                        p[idx].completion_time = current_time;
                        p[idx].turnaround_time = p[idx].completion_time - p[idx].arrival_time;
                        p[idx].waiting_time = p[idx].turnaround_time - p[idx].burst_time;
                        p[idx].response_time = p[idx].start_time - p[idx].arrival_time;
                        total_turnaround_time += p[idx].turnaround_time;
                        total_waiting_time += p[idx].waiting_time;
                        total_response_time += p[idx].response_time;
                        completed++;
                    }
                    else 
                        io.push(p[idx]);
                }
            }
            if(cpu_occupied == false)
                occur.push(0);
            if(!io.empty() && cpid != io.front().p_id){
                int idx = io.front().p_id-1;
                if(p[idx].start_time == -1)
                    p[idx].start_time = current_time-1;
                iooccur.push(idx+1);
                io_occupied = true;
                p[idx].burst.front().remainingBurst--;
                if(p[idx].burst.front().remainingBurst == 0){
                    p[idx].burst.pop();
                    io.pop();
                    if(p[idx].burst.empty()){
                        p[idx].completion_time = current_time;
                        p[idx].turnaround_time = p[idx].completion_time - p[idx].arrival_time;
                        p[idx].waiting_time = p[idx].turnaround_time - p[idx].burst_time;
                        p[idx].response_time = p[idx].start_time - p[idx].arrival_time;
                        total_turnaround_time += p[idx].turnaround_time;
                        total_waiting_time += p[idx].waiting_time;
                        total_response_time += p[idx].response_time;
                        completed++;
                    }
                    else 
                        cpu.push_back(p[idx]);
                }
            }
            if(io_occupied == false)
                iooccur.push(0); 
        } 
        printf("CPU GHANT CHART PROCESS ORDER\n");
        printf("\n|");
        while(!occur.empty()){
            if(occur.front() == 0)
                printf("    |");
            else 
                printf(" P%d |", occur.front());
            occur.pop();
        }
        printf("\n\nIO GHANT CHART PROCESS ORDER\n");
        printf("\n|");
        while(!iooccur.empty()){
            if(iooccur.front() == 0)
                printf("    |");
            else 
                printf(" P%d |", iooccur.front());
            iooccur.pop();
        }
        printf("\n\nPROCESS WISE DETAILS:\n\n");
        printf("+-----+---------------+--------------+-----------------+\n");
        printf("| PID | Response Time | Waiting Time | Turnaround Time |\n");
        printf("+-----+---------------+--------------+-----------------+\n");
        for(int i=0; i<n; i++){
            printf("| %2d  |       %2d      |      %2d      |        %2d       |\n", p[i].p_id, p[i].response_time, p[i].waiting_time, p[i].turnaround_time);
            printf("+-----+---------------+--------------+-----------------+\n");
        }
        printf("| TOT |      %0.1f     |     %0.1f     |       %0.1f      |\n",  total_response_time, total_waiting_time, total_turnaround_time);
        printf("+-----+---------------+--------------+-----------------+\n");
        printf("| AVG |       %0.1f     |     %0.1f     |       %0.1f      |\n",  total_response_time/n, total_waiting_time/n, total_turnaround_time/n);
        printf("+-----+---------------+--------------+-----------------+\n\n");
    } 
    return 0;
} 