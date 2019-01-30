#include <bits/stdc++.h>
using namespace std;
unordered_map<int, int> indexes[502];
typedef pair<pair<int, int>,int> piii;
unordered_set<int> s[502];


struct process_info
{
    int total_page,frame_capacity,arrival_time,current_page=0,page_faults=0,st=0,et=0;
    bool previous_page_fault=false;
    vector<int> pages;
    vector<int> time_slice;
};
void get_input_from_file(process_info processes[],int &total_process,int &time_quanta,priority_queue< pair<int, int>,
                         vector< pair<int,int> >, greater< pair<int,int> > > &blocking_queue);
int pageFaults(int page, int process_no, int capacity,int indx);
void round_robin(double fcfs_tat);
void SJF(double fcfc_tat);
double FCFS();
void get_input_from_file(process_info processes[],int &total_process,int &time_quanta,priority_queue< pair<int, int>,
                         vector< pair<int,int> >, greater< pair<int,int> > > &blocking_queue)
{
    ifstream input("in.txt");
    string line, comma;
    int i = 0, total_page, mem_reference;

    getline(input, line);
    stringstream ss(line);
    ss >> total_process >> comma >> time_quanta;

    while (getline(input, line))
    {
        stringstream ss(line);
        ss >> comma >> processes[i].total_page >> comma >> processes[i].arrival_time;
        blocking_queue.push(pair<int, int>(processes[i].arrival_time, i));
        processes[i].frame_capacity=ceil((double)processes[i].total_page/3);
        while (ss >> comma)
        {
            ss >> mem_reference;
            processes[i].pages.push_back(mem_reference / 512);
            processes[i].time_slice.push_back(30);
        }

        i++;
    }

}

int pageFaults(int page, int process_no, int capacity,int indx)
{
    int page_faults = 0;
    if (s[process_no].size() < capacity)
    {
        if (s[process_no].find(page) == s[process_no].end())
        {
            s[process_no].insert(page);
            page_faults++;
        }
        indexes[process_no][page] = indx;
    }

    else
    {
        if (s[process_no].find(page) == s[process_no].end())
        {
            int lru = INT_MAX, val;
            for (auto it = s[process_no].begin(); it != s[process_no].end(); it++)
            {
                if (indexes[process_no][*it] < lru)
                {
                    lru = indexes[process_no][*it];
                    val = *it;
                }
            }

            s[process_no].erase(val);
            s[process_no].insert(page);
            page_faults++;
        }

        indexes[process_no][page] = indx;
    }

    return page_faults;
}

void print_helper(int total_turn_around_time,int total_burst_time,int total_process,int sAlgo)
{
    if(sAlgo==1)
    {
        printf("RRS: %.2lf %.2lf",(double)(total_turn_around_time-total_burst_time)/
        total_process,(double)total_turn_around_time/total_process);
    }
    else if(sAlgo==2)
    {
         printf("SJF: %.2lf %.2lf",(double)(total_turn_around_time-total_burst_time)/
         total_process,(double)total_turn_around_time/total_process);
    }
    else if(sAlgo==3){
         printf("FCFS: %.2lf %.2lf",(double)(total_turn_around_time-total_burst_time)/
         total_process,(double)total_turn_around_time/total_process);
    }
}
class RoundRobin
{
public:
    void round_robin(double fcfs_tat)
    {
        for(int i=0; i<502; i++)
            s[i].clear(),indexes[i].clear();
        int cur_time=0,blocking_time=0,total_process=0,time_quanta=0,total_burst_time=0;

        process_info processes[502];
        queue<int> ready_queue;
        priority_queue< pair<int, int>, vector< pair<int,int> >, greater< pair<int,int> > > blocking_queue;

        get_input_from_file(processes,total_process,time_quanta,blocking_queue);

        while (!blocking_queue.empty() || !ready_queue.empty())
        {
            int time_slice_left=time_quanta;

            if(ready_queue.empty() && !blocking_queue.empty() && cur_time<blocking_queue.top().first)
            {
                ready_queue.push(blocking_queue.top().second);

                if(blocking_time>0)
                {
                    blocking_time-=min(blocking_queue.top().first-cur_time,blocking_time);
                    //cout<<"blocking time "<<blocking_time<<endl;
                }

                cur_time=blocking_queue.top().first;
                blocking_queue.pop();
            }
            else
            {
                while(!blocking_queue.empty() && cur_time>=blocking_queue.top().first)
                {
                    ready_queue.push(blocking_queue.top().second);
                    blocking_queue.pop();
                }
            }
            int cur_process=ready_queue.front();
            ready_queue.pop();
            for(int cur_page=processes[cur_process].current_page; cur_page<processes[cur_process].pages.size(); cur_page++)
            {

                if(cur_page==0)
                    processes[cur_process].st=cur_time;

                if(processes[cur_process].previous_page_fault==false && pageFaults(processes[cur_process].pages[cur_page],
                        cur_process,processes[cur_process].frame_capacity,cur_page+1))
                {
                    //cout<<"request: page "<<processes[cur_process].pages[cur_page]<<" process "<<cur_process<<endl;
                    processes[cur_process].page_faults++;
                    blocking_queue.push(pair<int, int>(cur_time+blocking_time+60,cur_process));
                    blocking_time+=60;
                    processes[cur_process].current_page=cur_page;
                    processes[cur_process].previous_page_fault=true;
                    break;
                }
                else
                {
                    //cout<<"execution: page "<<processes[cur_process].pages[cur_page]<<" process "<<cur_process<<endl;
                    processes[cur_process].previous_page_fault=false;
                    int execution_time=0;
                    if(processes[cur_process].time_slice[cur_page]>=time_slice_left)
                    {
                        execution_time=time_slice_left;
                        cur_time+=time_slice_left;
                        processes[cur_process].time_slice[cur_page]-=time_slice_left;
                        while(!blocking_queue.empty() && cur_time>=blocking_queue.top().first)
                        {
                            ready_queue.push(blocking_queue.top().second);
                            blocking_queue.pop();
                        }

                        if(processes[cur_process].time_slice[cur_page])
                            processes[cur_process].current_page=cur_page;
                        else processes[cur_process].current_page=cur_page+1;

                        ready_queue.push(cur_process);
                        time_slice_left=0;
                    }
                    else
                    {
                        execution_time=(processes[cur_process].time_slice[cur_page]);
                        cur_time+=execution_time;
                        time_slice_left-=processes[cur_process].time_slice[cur_page];
                        processes[cur_process].time_slice[cur_page]=0;
                    }

                    if(blocking_time>0)
                    {
                        blocking_time-=min(execution_time,blocking_time);
                        //cout<<"blocking time "<<blocking_time<<endl;
                    }
                }

                if(cur_page==processes[cur_process].pages.size()-1 && processes[cur_process].time_slice[cur_page]==0)
                    processes[cur_process].et=cur_time;
                if(time_slice_left==0)
                    break;
            }
        }
        printf("%d\n",cur_time);
        int total_turn_around_time=0;
        for(int i=0; i<total_process; i++)
        {
            total_burst_time+=processes[i].pages.size()*30;
            total_turn_around_time+=processes[i].et-processes[i].arrival_time;
        }

        print_helper(total_turn_around_time,total_burst_time,total_process,1);

        int total_page_faults=0;
        for(int i=0; i<total_process; i++)
            total_page_faults+=processes[i].page_faults;

        printf(" %d",total_page_faults);
        for(int i=0; i<total_process; i++)
            printf(" %d",processes[i].page_faults);

        printf(" %.2lf\n",(double)(fcfs_tat-total_turn_around_time)/total_turn_around_time*100);
    }

};




class ShortestJobFirst
{
public:

    void SJF(double fcfc_tat)
    {
        for(int i=0; i<502; i++)
            s[i].clear(),indexes[i].clear();

        process_info processes[502];
        int total_process=0,time_quanta,total_burst_time=0;

        priority_queue< pair<int, int>, vector< pair<int,int> >, greater< pair<int,int> > > blocking_queue;
        priority_queue<piii, vector< piii >, greater< piii > > ready_queue;

        get_input_from_file(processes,total_process,time_quanta,blocking_queue);

        for(int i=0; i<total_process; i++)
        {
            for(int j=0; j<processes[i].pages.size(); j++)
            {
                if(pageFaults(processes[i].pages[j],i,processes[i].frame_capacity,j+1))
                    processes[i].page_faults++;
            }
            int completion_time=processes[i].pages.size()*30+processes[i].page_faults*60;
            total_burst_time+=processes[i].pages.size()*30;
            ready_queue.push(piii(pair<int, int>(processes[i].arrival_time, completion_time),i));
        }

        int cur_time=0;
        while (!ready_queue.empty())
        {
            piii p = ready_queue.top();
            ready_queue.pop();

            if(cur_time<p.first.first)
            {
                cur_time=p.first.first;
                cur_time+=p.first.second;
                processes[p.second].et=cur_time;
            }
            else
            {
                cur_time+=p.first.second;
                processes[p.second].et=cur_time;
            }

        }
        printf("%d\n",cur_time);
        int total_turn_around_time=0;
        for(int i=0; i<total_process; i++)
        {
            total_burst_time+=processes[i].pages.size()*30;
            total_turn_around_time+=processes[i].et-processes[i].arrival_time;
        }

        print_helper(total_turn_around_time,total_burst_time,total_process,2);

        int total_page_faults=0;
        for(int i=0; i<total_process; i++)
            total_page_faults+=processes[i].page_faults;

        printf(" %d",total_page_faults);
        for(int i=0; i<total_process; i++)
            printf(" %d",processes[i].page_faults);

        printf(" %.2f\n",(double)(fcfc_tat-total_turn_around_time)/total_turn_around_time*100);
    }
};

int main()
{
    double fcfs_tat=FCFS();
    ShortestJobFirst shortJF;
    shortJF.SJF(fcfs_tat);
    RoundRobin rr;
    rr.round_robin(fcfs_tat);
}



double FCFS()
{

    for(int i=0; i<502; i++)
        s[i].clear(),indexes[i].clear();

    process_info processes[502];
    int total_process=0,time_quanta;

    priority_queue< pair<int, int>, vector< pair<int,int> >, greater< pair<int,int> > > ready_queue;

    int cur_time=0,blocking_time=0,total_burst_time=0;
    get_input_from_file(processes,total_process,time_quanta,ready_queue);


    while (!ready_queue.empty())
    {
        pair<int, int> pii = ready_queue.top();
        ready_queue.pop();
        if(cur_time<pii.first)
        {
            if(blocking_time>0)
                blocking_time-=(pii.first-cur_time);
            cur_time=pii.first;
        }

        for(int cur_page=processes[pii.second].current_page; cur_page<processes[pii.second].pages.size(); cur_page++)
        {
            if(cur_page==0)
                processes[pii.second].st=cur_time;

            if(processes[pii.second].previous_page_fault==false && pageFaults(processes[pii.second].pages[cur_page],
                    pii.second,processes[pii.second].frame_capacity,cur_page+1))
            {
                processes[pii.second].page_faults++;
                ready_queue.push(pair<int, int>(cur_time+blocking_time+60,pii.second));
                blocking_time+=60;
                processes[pii.second].current_page=cur_page;
                processes[pii.second].previous_page_fault=true;
                break;
            }
            else
            {
                //cout<<"execution: page "<<processes[pii.second].pages[cur_page]<<" process "<<pii.second<<"time "<<cur_time<<endl;
                processes[pii.second].previous_page_fault=false;
                cur_time+=30;
                if(blocking_time>0)
                    blocking_time-=30;
            }

            if(cur_page==processes[pii.second].pages.size()-1)
                processes[pii.second].et=cur_time;
        }
    }
    printf("%d\n",cur_time);
    int total_turn_around_time=0;
    for(int i=0; i<total_process; i++)
    {
        total_burst_time+=processes[i].pages.size()*30;
        total_turn_around_time+=processes[i].et-processes[i].arrival_time;
    }

    print_helper(total_turn_around_time,total_burst_time,total_process,3);

    int total_page_faults=0;
    for(int i=0; i<total_process; i++)
        total_page_faults+=processes[i].page_faults;

    printf(" %d",total_page_faults);
    for(int i=0; i<total_process; i++)
        printf(" %d",processes[i].page_faults);

    printf(" %.2f\n",0.0);

    return total_turn_around_time;
}

