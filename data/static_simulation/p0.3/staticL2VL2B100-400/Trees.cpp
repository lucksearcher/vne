#include <ilcplex/ilocplex.h>
#include<ctime>
#include "frandom.h"
#include "elements.h"
#include "Trees.h"
ILOSTLBEGIN
typedef IloArray<IloNumVarArray> NumVarMatrix;
typedef IloArray<NumVarMatrix> NumVarCubic;
#define _server2server
#define _edge_rank
using namespace std;
Tree::Tree()
{
	for( int i = 0; i < Nv; i++ )
	{	
		Table[i].Type=1;
		Table[i].Slot=-1;
		Table[i].Parent=-1;
		Table[i].Degree=0;
	}

}
Tree::~Tree()
{
	for( int i = 0; i < Nv; i++ )
		delete []Table[i].Adj;
}

void Tree::drawTree()
{	
#ifdef _Tree
	float cost=1,line_rate=0;
	//allocate *Adj according to node degree
	// servers
	for (int i=0;i<nServer;i++){
		Table[i].Type=0;  Table[i].Slot=maxSlot;
		Table[i].Adj=NULL;//new Edge[n_server_port];
	}	
	// switches
	for (int i=0;i<nSwitch;i++){
		int switch_index=i+nServer;
		Table[switch_index].Type=1;	Table[switch_index].Slot=0;
		if (i<nToR)
			Table[switch_index].Adj=new Edge[nServerInRack];
		else
			Table[switch_index].Adj=new Edge[H];
	}	
	root=Nv-1;

	//layer 1
	for (int i=0;i<nToR;i++)
	{	int ToR_index=nServer+i;
		for (int j=0;j<nServerInRack;j++)
		{	int Server_index=nServerInRack*i+j;
			AddEdge(ToR_index,Server_index,cost,gbpsCommodity,Server_index);// Edge->Server
			Table[Server_index].Parent=ToR_index; // parent
		}
	}

	//layer 2
	line_rate=gbpsCommodity*H*oversubstription;
	for (int i=0;i<nAggregate;i++)
	{	
		int Agg_index=nServer+nToR+i;
		for (int j=0;j<H;j++)
		{	
			int ToR_index=nServer+i*H+j;
			AddEdge(Agg_index,ToR_index,cost,line_rate,ToR_index);// Aggragate->Edge
			Table[ToR_index].Parent=Agg_index;	 // parent
			
		}
	}
	//layer 3
	line_rate=gbpsCommodity*(H*oversubstription)*H*oversubstription;

	for (int i=0;i<nAggregate;i++)
	{   
		int Agg_index=nServer+nToR+i;
		AddEdge(root,Agg_index,cost,line_rate,Agg_index); // root->Aggragate
		Table[Agg_index].Parent=root; // parent

	}


	Table[root].Parent=-1;

	// cal the cut set
	int child=-1;
	for(int e=0;e<Ne;e++)
		for (int j=0;j<nServer;j++)
			cut[e][j]=0;
	//the servers  is in the subtree of its uplink
	for(int e=0;e<nServer;e++)
		cut[e][e]=1;
	//the server in the subtree of an edge switch
	for(int e=nServer;e<root;e++)// e: a switch and its uplink
	{	
		for (int j=0;j<nServer;j++)	for (int p=0;p<Table[e].Degree;p++){
			child=(Table[e].Adj)[p].Addr;
			if (child!=e)
				cut[e][j]=cut[e][j]||cut[child][j];
		}
	}

#endif
}

// Add the edge ( Source, Dest, cost, Bandwidth ) to the Tree
void Tree::AddEdge( const int & s, const int & d, const float &cost,const float &bandw,const int&addr)
{	
	int i;
	//	e(s,d)
	i=Table[s].Degree;	
	(Table[s].Adj)[i].Assign(d,cost,addr);
	Bandwidth[addr]=bandw;	resBandwidth[addr]=bandw;
	Table[s].Degree++;	
	//	e(d,s)
	/*
	i=Table[d].Degree;
	(Table[d].Adj)[i].Assign(s,cost,addr+Ne/2);
	Bandwidth[addr+Ne/2]=bandw;	resBandwidth[addr+Ne/2]=bandw;
	Table[d].Degree++;
	*/
	//opposite[addr]=addr+Ne/2;	opposite[addr+Ne/2]=addr;
}




void Tree::ClearNetwork()
{
	//renew server space
	for (int i=0;i<nServer;i++){
		Table[i].Slot=maxSlot;
	}
	// renew link capacity
	for (int e=0;e<Ne;e++){	
		resBandwidth[e]=Bandwidth[e];
	}		
}
void Tree::RandomNetwork(float p)
{
	//renew server space
	for (int i=0;i<nServer;i++){
		if (rand_b01(p))
			Table[i].Slot=unif_int(0,maxSlot);
		else
			Table[i].Slot=maxSlot;
	}
	// renew link capacity
	for (int e=0;e<Ne;e++)
	{	
		if (rand_b01(p))
			resBandwidth[e]=unif_int(0,Bandwidth[e])/100*100;
		else
			resBandwidth[e]=Bandwidth[e];
	}

}


void Tree::reserveResource(const Solution&map)
{	
	// bandwidth reservation
    for (int e=0;e<Ne;e++)	
		resBandwidth[e]-=map.Bandwidth[e];
	// VM slot reservation
	for (int i=0;i<nServer;i++)
		Table[i].Slot-=map.Slot[i];
}

void Tree::releaseResource(const Solution&map)
{
	// bandwidth release
    for (int e=0;e<Ne;e++)	
		resBandwidth[e]+=map.Bandwidth[e];

	// VM slot release
	for (int i=0;i<nServer;i++)
		Table[i].Slot+=map.Slot[i];
}

float Tree::ProcessRequest(char algorithm,int numOfreq,float load,
						float &max_utilization,float &success_rate,float &RC)
{
	ClearNetwork();
	init_genrand(0);
	//RandomNetwork(0.5);
	//init_genrand( (unsigned long)time( NULL ));				
	float t=0;

	Cluster req;	// embedding input
	Solution map;	//embedding results

	//performance variables:	
	int n_success=0,n_req=0;
	float revenue=0,cost=0;
	success_rate=0,max_utilization=0,RC=0;
	BinaryHeap<Solution> depart_list(Nvdc);
	while(n_req<numOfreq){
		req.random(load);
		n_req++;
		t=t+req.Arrivaltime;
		// process depart event before next arrival
		while (!depart_list.isEmpty())
		{
			map=depart_list.findMin();
			if (map.Departtime>t)
				break;
			releaseResource(map);
			depart_list.deleteMin();
		}
		bool accept;
		switch(algorithm)
		{
		case 'P':
			accept=Pertubation(req,map);
			break;
		case 'F':
			accept=FirstFit(req,map);
			break;
		case 'B':
			accept=BackTracking(req,map);
			break;
		default:
			break;
		}
		if (accept)
		{   
			reserveResource(map);
			map.Departtime=t+req.Holdtime;
			depart_list.insert(map);
			//performance evaluation
			n_success++;
			float virtual_bandwidth=0,substate_bandwidth=0;
			for (int j=0;j<req.N;j++)
				virtual_bandwidth+=req.B[j];
			revenue=req.N+virtual_bandwidth;
			for (int j=0;j<Ne;j++)
				substate_bandwidth+=map.Bandwidth[j];
			cost=req.N+substate_bandwidth;
			RC+=revenue/cost;
			float max_ut=0;
			for (int e=0;e<Ne;e++)	
			{
				max_ut=max(max_ut,resBandwidth[e]/Bandwidth[e]);
				//max_ut+=(1-resBandwidth[e]/(Table[v].Adj)[p].Bandwidth);
			}
			max_utilization+=max_ut;
		}	
	}
	success_rate=(float)n_success/(float)n_req;
	max_utilization=max_utilization/(float)n_req;
	RC=RC/(float)n_req;
    return success_rate;
}

float Tree::SingleRequest(char algorithm,int numOfreq,float prob,int numOfVM,
						float &max_utilization,float &success_rate,float &bandwidth_cost)
{
	ClearNetwork();
	init_genrand(0);
	//init_genrand( (unsigned long)time( NULL ));				
	float t=0;
	Cluster req;	// embedding input
	Solution map;	//embedding results

	//performance variables:	
	int n_success=0,n_req=0;
	
	success_rate=0,max_utilization=0,bandwidth_cost=0;
	//BinaryHeap<Solution> depart_list(Nvdc);
	while(n_req<numOfreq){
		RandomNetwork(prob);
		req.random(numOfVM);
		n_req++;
		bool accept;
		switch(algorithm)
		{
		case 'P':
			accept=Pertubation(req,map);
			break;
		case 'F':
			accept=FirstFit(req,map);
			break;
		case 'B':
			accept=BackTracking(req,map);
			break;
		default:
			break;
		}
		if (accept)
		{   
			//performance evaluation
			n_success++;
			float max_ut=0;
			for (int e=0;e<Ne;e++)	
			{
				max_ut=max(max_ut,map.Bandwidth[e]/resBandwidth[e]);
			}
			max_utilization+=max_ut;
			float bw_used=0,bw_total=0;
			for (int e=0;e<Ne;e++){
				bw_used+=map.Bandwidth[e];
				bw_total+=resBandwidth[e];
			}
			bandwidth_cost+=bw_used/bw_total;
		}	
	}
	success_rate=(float)n_success/(float)n_req;
	max_utilization=max_utilization/(float)n_req;
	bandwidth_cost=bandwidth_cost/(float)n_req;
    return success_rate;
}


bool Tree::QuickFail(const Cluster& req,float& sumB,float* res_port_B)
{
	bool resource_satisfied=true;
	float res_sumB=0;
	int res_slot=0;
	for (int i=0;i<req.N;i++)// total suscribed bandwidth
		sumB+=req.B[i];
	for(int i=0;i<nServer;i++){
		res_port_B[i]+=resBandwidth[i];
		if (Table[i].Slot>0&&res_port_B[i]>0){
			res_slot+=Table[i].Slot;
			res_sumB+=res_port_B[i];
		}
	}
	if (res_slot<req.N||res_sumB<sumB)
		resource_satisfied=false;
	return resource_satisfied;
}
int Tree::findBottleneck(float *hostBw,const Solution& map)
{	
	int the_server=-1;
	float sum_hostBw=0;
	for (int i=0;i<nServer;i++)
		sum_hostBw+=hostBw[i];
	if (sum_hostBw==0)
		return the_server;
	bool selected[Ne]={0};
	for(int n_selected=0;n_selected<Ne;n_selected++)
	{
		float mlu=0;	int the_link=0;
		for (int e=0;e<Ne;e++){
			if (mlu<map.Bandwidth[e]&&selected[e]==false)
			{
				mlu=map.Bandwidth[e];
				the_link=e; //find the most congested link
			}
		}
		selected[the_link]=true;
		
		float max_traffic=0;
		for (int j=0;j<nServer;j++)
			if (cut[the_link][j]&&hostBw[j]> max_traffic){
				the_server=j;
				max_traffic=hostBw[j];
			}	
		
	}
	return the_server;
}

bool Tree::CongestDetect(int s,Cluster& req,float* hostBw,float sumB,Solution& map)
{		
	//Solution oldmap(map);
	bool congest=false;
	
	//whether e(v,w) is congested?
	/*int parent=s,e; // the up_link edge of x
	while(1)
	{	e=parent;
		float B=0; // 
		for (int j=0;j<nServer;j++)
			if (cut[e][j]) B+=hostBw[j];
		map.Bandwidth[e]=min(B,sumB-B);

		if (map.Bandwidth[e]>resBandwidth[e]){
			congest=true;
			//map=oldmap;
			break;
		}
		parent=Table[parent].Parent;	
		if (parent==root) 
			break;
	}	//end for: congestion test for concurrent traffic
	*/

	int _rank[Ne];	float _load[Ne];bool caculated[Ne]={0};
	for( int e=0;e<Ne;e++)
	{	_rank[e]=e;				
		_load[e]=resBandwidth[e]-map.Bandwidth[e];				
	}
	for(int a=0;a<Ne-1;a++)
		for(int b=a;b<Ne;b++)
		{					
			if(_load[a] >_load[b]) //f-sd-e(i,j)
			{	//交换a,b位置
				swap(_load[a],_load[b]);
				swap(_rank[a],_rank[b]);						
			}
		}
	
	for (int j=0;j<Ne;j++)
	{	
		int e=_rank[j];
		if (caculated[e])
			continue;
		float B1=0,B2=0; // 
		for (int j=0;j<nServer;j++)
			if (cut[e][j]) 
				B1+=hostBw[j];
			else
				B2+=hostBw[j];
		
		map.Bandwidth[e]=min(B1,B2);
		caculated[e]=true; 
		if (map.Bandwidth[e]>resBandwidth[e])
		{	//congestion detected!
			congest=true;
			break;
		}		
		
	}	
	return congest;
}
// pertubation only when a VM can not placed
bool Tree::Pertubation(Cluster& req,Solution& map)
{	
	sort(req.B.begin(),req.B.end(),greater<float>());	// sort B1,...BN in descending order	
	float sum_capacity[nServer][nServer];
	
	float hostBw[nServer]={0};	//total bandwidths of a server assigned to the current VDC 
	vector<int> assignment(req.N,-1); //the server index  B1,...BN located in

	vector<vector<bool>> tabu(req.N,vector<bool>(nServer,false));
	//intiate the map variables:
	map.Clear();
	
	//quick fail 
	float res_port_B[nServer]={0},sumB=0;
	if (QuickFail(req,sumB,res_port_B)==false)
		return false;
	//end of quick fail !!
	int numVMembedded=0,maxLoop=req.N*nServer;
	for(int t=0;t<maxLoop+1;t++)
	{	
		// find VM(Bi) with max B, and try to place to server x
		int i,x;	//i: visit the VMs, x: visit the servers
		for (i=0;i<req.N;i++)
			if (assignment[i]==-1)
				break;
		//to place VM(Bi)into the servers, {0,1,...tabu[i]} are not considered		
		int n_usefull_server=0;
		for (x=0;x<nServer;x++)
		{	
			if (tabu[i][x])// this server x is in the tabu list of VM(Bi)
				continue;
			if (Table[x].Slot<=map.Slot[x])// there is no space in server m
				continue;	//try the next server for VM(Bi)
			if(res_port_B[x]<min(hostBw[x]+req.B[i],sumB-hostBw[x]-req.B[i]))
				continue;	 //less ingress bandwidth!!  		
			n_usefull_server++;
			map.Slot[x]+=1;hostBw[x]+=req.B[i];assignment[i]=x;numVMembedded++;
			if (CongestDetect(x,req,hostBw,sumB,map)){
				map.Slot[x]-=1; hostBw[x]-=req.B[i]; assignment[i]=-1; numVMembedded--;	}
			else	
				break;
			//else, try server x+1
		}	//end for server x

		if (numVMembedded==req.N)	
			return true;
		if (x>=nServer)
		{
			if(n_usefull_server==0||numVMembedded==0)	 // running out of VM slots!
				return false;
			// fail due to conestion,try pertubation
			
			int the_server=-1,the_vm=-1;
			//find the bottleneck server to lead to congestion
			the_server=findBottleneck(hostBw,map);
			if (the_server<0) 
				return false; //fail due to bisection traffic, rather than concurrent congestion.
			for(the_vm=req.N-1;the_vm>=0;the_vm--)
				if (assignment[the_vm]==the_server)
					break;	
			//unload a VM from the bottleneck server
			map.Slot[the_server]-=1;	hostBw[the_server]-=req.B[the_vm];
			assignment[the_vm]=-1;		numVMembedded--;
			tabu[the_vm][the_server]=1;
		}
		//else, Let's place the next VM(Bi+1)
	}
	return false;
}


// greedy pack a server with any fitable VMs
bool Tree::FirstFit(Cluster& req,Solution& map)
{	
	sort(req.B.begin(),req.B.end(),greater<float>());	// sort B1,...BN in descending order	
	float sum_capacity[nServer][nServer];
	
	float hostBw[nServer]={0};	//total bandwidths of a server assigned to the current VDC 
	vector<int> assignment(req.N,-1); //the server index  B1,...BN located in

	//intiate the map variables:
	map.Clear();
	
	//quick fail 
	float res_port_B[nServer]={0},sumB=0;
	if (QuickFail(req,sumB,res_port_B)==false)
		return false;
	//end of quick fail !!

	//i: visit the VMs, j: visit the servers
	int numVMembedded=0,maxLoop=req.N*nServer;;
	for (int i=0;i<req.N;i++)
	{	int x;
		for (x=0;x<nServer;x++)
		{	// packing VMs( i=1:N )into server x
			
			if (Table[x].Slot<=map.Slot[x])// there is no space in server x
				continue;	//open the next server 
			if(res_port_B[x]<min(hostBw[x]+req.B[i],sumB-hostBw[x]-req.B[i]))
				continue;	 //less ingress bandwidth!!  			
			map.Slot[x]+=1;hostBw[x]+=req.B[i];assignment[i]=x;numVMembedded++;
			if (CongestDetect(x,req,hostBw,sumB,map))
			{	map.Slot[x]-=1; hostBw[x]-=req.B[i]; assignment[i]=-1; numVMembedded--;	}
			else
			{
				if (numVMembedded==req.N)	
				{
					return true;
				}
				else
					break;
			}
			//else continue;	//to try the next server
					
		}	//end for server x
		if (x>=nServer)
			return false;
	}	// end for VM(Bi)

	
}


// exhaustive search with load-balanced routing
bool Tree::BackTracking(Cluster &req, Solution &map)
{
	sort(req.B.begin(),req.B.end(),greater<float>());	// sort B1,...BN in descending order	
	float sum_capacity[nServer][nServer];
	
	float hostBw[nServer]={0};	//total bandwidths of a server assigned to the current VDC 
	vector<int> assignment(req.N,-1); //the server index  B1,...BN located in

	//vector<vector<bool>> tabu(req.N,vector<bool>(nServer,false));
	//intiate the map variables:
	map.Clear();
	
		//quick fail 
	float res_port_B[nServer]={0},sumB=0;
	if (QuickFail(req,sumB,res_port_B)==false)
		return false;
	//end of quick fail !!

	//i: visit the VMs, j: visit the servers
	int numVMembedded=0;
	if (recursivePlacement(req, map,assignment,sumB,res_port_B,hostBw,numVMembedded))
	{
		return true;
	}
	else
		return false;

}
// used in backtracking algorihtm
bool Tree::recursivePlacement(Cluster &req, Solution &map,vector<int>& assignment,float sumB,float* res_port_B,
							   float* hostBw,int &numVMembedded)
{
// find VM(Bi) with max B, and try to place to server x
	int i,x;
	for (i=0;i<req.N;i++)
		if (assignment[i]==-1)
			break;
	//to place VM(Bi)into the servers, {0,1,...tabu[i]} are not considered
	for (x=0;x<nServer;x++)
	{	
		if (Table[x].Slot<=map.Slot[x])// there is no space in server m
			continue;	//try the next server for VM(Bi)
		if(res_port_B[x]<min(hostBw[x]+req.B[i],sumB-hostBw[x]-req.B[i]))
			continue;	 //less ingress bandwidth!! 
		map.Slot[x]+=1;hostBw[x]+=req.B[i];assignment[i]=x;numVMembedded++;
		if (CongestDetect(x,req,hostBw,sumB,map))
		{
			//routing failure,try the next server x+1 for VM(Bi)!!
			map.Slot[x]-=1; hostBw[x]-=req.B[i]; assignment[i]=-1; numVMembedded--;	
		}
		else //not congestion
			if (numVMembedded==req.N)
			{
				return true;
			}
			else  //place the next VMs recursively
				if (recursivePlacement(req, map,assignment,sumB,res_port_B,
					hostBw,numVMembedded))
					return true;
				else // back tracking!!
				{	map.Slot[x]-=1;	hostBw[x]-=req.B[i];	assignment[i]=-1; numVMembedded--;
					continue;//try the next server x+1 for VM(Bi)!!
				}

	}//end for server x
	if (x>=nServer)	
	return false;	

}





