#include "Graphs.h"
#include "Trees.h"
#include "results.h"
#include <ctime>
using namespace std;
#ifdef _Tree
void static_embedding_N(char algorithm)
{
	Tree G;
	G.drawTree();
	//testKSP(G);
	//input
	const int n=6,m=1;
	int numOfreq=1e2,n_test=n;
	// param of physical networks
	float p=0.5,p_minResBw=0,p_maxResBw=1;// with p in [0,1G]
	// param of  VDC
	int numOfVM[n]={2,4,6,8,9,10};
	float minBw=100,maxBw=700;

	//output
	float max_utilization[n]={0},success_rate[n]={0},bandwidth_cost[n]={0};
	// start simulation
	
	time_t Atime,Btime; 
	
	float running_time[n]={0};
	
	for (int i=0;i<n_test;i++)
	{
		time(&Atime); 
		G.SingleRequest(algorithm,numOfreq,
			p,p_minResBw,p_maxResBw,numOfVM[i],minBw,maxBw,
			max_utilization[i],success_rate[i],bandwidth_cost[i]);	
		time(&Btime);
		running_time[i]=1000*(float)(Btime - Atime)/(float)numOfreq;
		cout<<"process time is "<<running_time[i]<<"ms"<<endl;
	}

	switch(algorithm)
	{
	case 'P':
			output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_pertubation.txt");
		break;
	case 'A':		
			output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_allocrange.txt");
		break;

	case 'R':
		
			output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_randomdrop.txt");
		break;
	case 'B':
		
			output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_backtracking.txt");
		break;
	case 'E':
			output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_exhaustive.txt");
		break;
	case 'F':
		 
			output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_firstfit.txt");
		break;
	case 'N':
		 
			output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_nextfit.txt");
		break;
	case 'G':
		 
			output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_bestfit.txt");
		break;
	default:
		break;
	}	
}


void static_embedding_B(char algorithm)
{
	Tree G;
	G.drawTree();
	//testKSP(G);
	//input
	const int n=5;
	int numOfreq=1e2,n_test=n; 
	// param of physical networks
	float p=0.5,p_minResBw=0,p_maxResBw=1; // with p in [500B,1G]
	// param of  VDC
	int numOfVM=8;
	float devBw=100,avgBw[n]={200,250,300,350,400};
	//output
	float max_utilization[n]={0},success_rate[n]={0},bandwidth_cost[n]={0};
	// start simulation
	
	time_t Atime,Btime; 
	
	float running_time[n]={0};

		
		for (int i=0;i<n_test;i++)
		{
			time(&Atime); 
			G.SingleRequest(algorithm,numOfreq,
				p,p_minResBw,p_maxResBw,numOfVM,avgBw[i]-devBw,avgBw[i]+devBw,
				max_utilization[i],success_rate[i],bandwidth_cost[i]);	

			time(&Btime);
			running_time[i]=1000*(float)(Btime - Atime)/(float)(numOfreq*n_test);
			cout<<"process time is "<<running_time[i]<<"ms"<<endl;
		}

		switch(algorithm)
		{
		case 'P':
				output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_pertubation.txt");
			break;
		case 'A':			 
				output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_allocrange.txt");
			break;
		case 'R':			 
				output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_randomdrop.txt");
			break;

		case 'B':
			
				output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_backtracking.txt");
			break;
		case 'E':
				output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_exhaustive.txt");
			break;
		case 'F':
			 
				output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_firstfit.txt");
			break;
		default:
			break;
		}	
	
}



void dynamic_embedding(char algorithm)
{
	Tree G;
	G.drawTree();
	//input
	int numOfreq=1e3;
	const int n=4,m=2;
	float load[n]={0.2,0.4,0.6,0.8};
	int n_test=4;
	//output
	float max_utilization[n]={0},success_rate[n]={0},bw_cost[n]={0},RC[n]={0};
	// start simulation	
	time_t Atime,Btime; 
	time(&Atime); 
	float running_time=0;
	
	for (int i=0;i<n_test;i++)
		G.ProcessRequest(algorithm,numOfreq,load[i],max_utilization[i],success_rate[i],bw_cost[i],RC[i]);
	
	time(&Btime);
	running_time=1000*1000*(float)(Btime - Atime)/(float)(numOfreq*n_test);
	cout<<"process time is "<<running_time<<"us"<<endl;
	
	switch(algorithm)
	{
		case 'P':
			output2txt(n_test,running_time,load,max_utilization,success_rate,bw_cost,RC,"tree_pertubation.txt");
			break;
		case 'B':
			output2txt(n_test,running_time,load,max_utilization,success_rate,bw_cost,RC,"tree_backtracking.txt");
			break;
		case 'F':			 
			output2txt(n_test,running_time,load,max_utilization,success_rate,bw_cost,RC,"tree_firstfit.txt");
			break;
		default:
			break;
	}	
}
#endif
void dynamic_embedding(char algorithm,bool enLProuting)
{
	Graph G;
#ifdef _FatTree
	G.drawFattree();
#endif
#ifdef _L2VL2
	G.drawL2VL2();
#endif
#ifdef _VL2
	G.drawVL2();
#endif
#ifdef _Bcube
	G.drawBcube();
#endif

	//testKSP(G);
	//input
	int numOfreq=100;
	const int n=4,m=2;
	float load[n]={0.8,0.6,0.4,0.2};
	int n_test=4;
	//output
	float max_utilization[n]={0},success_rate[n]={0},bw_cost[n]={0},RC[n]={0};
	// start simulation
	
	clock_t  Aclock,Bclock;
	Aclock=clock();
	float running_time=0;
	
	for (int i=0;i<n_test;i++)
		G.ProcessRequest(algorithm,enLProuting,numOfreq,load[i],
						max_utilization[i],success_rate[i],bw_cost[i],RC[i]);
	
	Bclock=clock();
	running_time=1000*(float)(Bclock-Aclock)/(CLOCKS_PER_SEC*(float)numOfreq*n_test);
	cout<<"process time is "<<running_time<<"ms"<<endl;
	
	switch(algorithm)
	{
		case 'P':
			if (enLProuting)
				output2txt(n_test,running_time,load,max_utilization,success_rate,bw_cost,RC,"pertubation_lp.txt");
			else 
				output2txt(n_test,running_time,load,max_utilization,success_rate,bw_cost,RC,"pertubation.txt");
			break;
		case 'L':
			if (enLProuting)
				output2txt(n_test,running_time,load,max_utilization,success_rate,bw_cost,RC,"localsearch_lp.txt");
			else 
				output2txt(n_test,running_time,load,max_utilization,success_rate,bw_cost,RC,"localsearch.txt");
			break;
		case 'E':
				output2txt(n_test,running_time,load,max_utilization,success_rate,bw_cost,RC,"exhaustive.txt");
			break;
		case 'B':
			if (enLProuting)
				output2txt(n_test,running_time,load,max_utilization,success_rate,bw_cost,RC,"backtracking_lp.txt");
			else
				output2txt(n_test,running_time,load,max_utilization,success_rate,bw_cost,RC,"backtracking.txt");
			break;
		case 'F':
			if (enLProuting)
				output2txt(n_test,running_time,load,max_utilization,success_rate,bw_cost,RC,"firstfit_lp.txt");
			else 
				output2txt(n_test,running_time,load,max_utilization,success_rate,bw_cost,RC,"firstfit.txt");
			break;
		default:
			break;
	}	
}

void static_embedding_N(char algorithm,bool enLProuting)
{
	Graph G;
#ifdef _FatTree
	G.drawFattree();
#endif
#ifdef _L2VL2
	G.drawL2VL2();
#endif
#ifdef _VL2
	G.drawVL2();
#endif
#ifdef _Bcube
	G.drawBcube();
#endif
	//testKSP(G);
	//input
	const int n=6,m=1;
	int numOfreq=1e2,n_test=n,k_paths=2;
	// param of physical networks
	float p=0.5,p_minResBw=0,p_maxResBw=1;// with p in [0,1G]
	// param of  VDC
	int numOfVM[n]={2,4,6,8,9,10};
	float minBw=100,maxBw=700;

	//output
	float max_utilization[n]={0},success_rate[n]={0},bandwidth_cost[n]={0};
	// start simulation
	
	clock_t  Aclock,Bclock;
	
	float running_time[n]={0};
	
	for (int i=0;i<n_test;i++)
	{
		Aclock=clock();
		G.SingleRequest(algorithm,enLProuting,numOfreq,
			p,p_minResBw,p_maxResBw,numOfVM[i],minBw,maxBw,
			max_utilization[i],success_rate[i],bandwidth_cost[i]);	
		Bclock=clock();
		running_time[i]=1000*(float)(Bclock-Aclock)/(CLOCKS_PER_SEC*(float)numOfreq);
		cout<<"process time is "<<running_time[i]<<"ms"<<endl;
	}

	switch(algorithm)
	{
	case 'P':
		if (enLProuting)
			output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_pertubation_lp.txt");
		else 
			output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_pertubation.txt");
		break;
	case 'H':		 
		output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_HVC.txt");
		break;

	case 'R':
		if (enLProuting)
			output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_randomdrop_lp.txt");
		else 
			output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_randomdrop.txt");
		break;
	case 'B':
		if (enLProuting)
			output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_backtracking_lp.txt");
		else 
			output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_backtracking.txt");
		break;
	case 'E':
			output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_exhaustive.txt");
		break;
	case 'F':
		if (enLProuting)
			output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_firstfit_lp.txt");
		else 
			output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_firstfit.txt");
		break;
	case 'N':
		if (enLProuting)
			output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_nextfit_lp.txt");
		else 
			output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_nextfit.txt");
		break;
	case 'G':
		if (enLProuting)
			output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_bestfit_lp.txt");
		else 
			output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_bestfit.txt");
		break;
	default:
		break;
	}	
}


void static_embedding_B(char algorithm,bool enLProuting)
{
	Graph G;
#ifdef _FatTree
	G.drawFattree();
#endif
#ifdef _L2VL2
	G.drawL2VL2();
#endif
#ifdef _VL2
	G.drawVL2();
#endif
#ifdef _Bcube
	G.drawBcube();
#endif
	//testKSP(G);
	//input
	const int n=5,m=1;
	int numOfreq=1e2,n_test=n,k_paths=2; 
	// param of physical networks
	float p=0.5,p_minResBw=0,p_maxResBw=1; // with p in [500B,1G]
	// param of  VDC
	int numOfVM=8;
	float devBw=100,avgBw[n]={200,250,300,350,400};
	//output
	float max_utilization[n]={0},success_rate[n]={0},bandwidth_cost[n]={0};
	// start simulation
	
	
	clock_t  Aclock,Bclock;
	float running_time[n]={0};
		
	for (int i=0;i<n_test;i++)
	{
		Aclock=clock();
		G.SingleRequest(algorithm,enLProuting,numOfreq,
			p,p_minResBw,p_maxResBw,numOfVM,avgBw[i]-devBw,avgBw[i]+devBw,
			max_utilization[i],success_rate[i],bandwidth_cost[i]);	
		Bclock=clock();
		running_time[i]=1000*(float)(Bclock-Aclock)/(CLOCKS_PER_SEC*(float)numOfreq);
		cout<<"process time is "<<running_time[i]<<"ms"<<endl;
	}

	switch(algorithm)
	{
	case 'P':
		if (enLProuting)
			output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_pertubation_lp.txt");
		else 
			output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_pertubation.txt");
		break;
	case 'H':		 
			output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_HVC.txt");
			break;
	case 'R':
		if (enLProuting)
			output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_randomdrop_lp.txt");
		else 
			output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_randomdrop.txt");
		break;
	case 'B':
		if (enLProuting)
			output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_backtracking_lp.txt");
		else 
			output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_backtracking.txt");
		break;
	case 'E':
			output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_exhaustive.txt");
		break;
	case 'F':
		if (enLProuting)
			output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_firstfit_lp.txt");
		else 
			output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_firstfit.txt");
		break;
	case 'G':
		if (enLProuting)
			output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_bestfit_lp.txt");
		else 
			output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_bestfit.txt");
		break;
	case 'N':
		if (enLProuting)
			output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_nextfit_lp.txt");
		else 
			output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_nextfit.txt");
		break;
	default:
		break;
	}	
	
}



void static_RA_N(char algorithm,bool enLProuting)
{
	Graph G;
#ifdef _FatTree
	G.drawFattree();
#endif
#ifdef _L2VL2
	G.drawL2VL2();
#endif
#ifdef _VL2
	G.drawVL2();
#endif
#ifdef _Bcube
	G.drawBcube();
#endif
	//testKSP(G);
	//input
	const int n=6,m=3;
	int numOfreq=1e2,n_test=n,k_paths[]={4,2,1,0}; 

	// param of physical networks
	float p=1,p_minResBw=0.5,p_maxResBw=1;// with p in [0,1G]
	// param of  VDC
	int numOfVM[n]={2,4,6,8,9,10};
	float minBw=100,maxBw=700;

	//output
	float max_utilization[n]={0},success_rate[n]={0},bandwidth_cost[n]={0};
	// start simulation
	clock_t  Aclock,Bclock;
	
	float running_time[n]={0};
	float t_RA[n]={0};
	for (int j=0;j<m;j++)
	{
		if(k_paths[j]==0){
			G.Kmax=Kspt;
			G.routingOption=_ECMP;
		}
		else
		{
			G.Kmax=k_paths[j];
			G.routingOption=_KshortestLB;
		}
		if(enLProuting==1){
			G.Kmax=Kspt;
			G.routingOption=_KshortestLB;
			j=m+1;
		}
		for (int i=0;i<n_test;i++)
		{
			Aclock=clock();
			G.SingleRequest(algorithm,enLProuting,numOfreq,
				p,p_minResBw,p_maxResBw,numOfVM[i],minBw,maxBw,
				max_utilization[i],success_rate[i],bandwidth_cost[i],t_RA[i]);	
			Bclock=clock(); 
			running_time[i]=1000*(float)(Bclock-Aclock)/(CLOCKS_PER_SEC*(float)numOfreq);
			cout<<"process time is "<<running_time[i]<<"ms"<<endl;
		}
	
		if(en_RA_runningtime){
			if (enLProuting)
				output2txt(G,n_test,t_RA,numOfVM,minBw,maxBw,"runtime_optimalRA.txt");
			else 
				output2txt(G,n_test,t_RA,numOfVM,minBw,maxBw,"runtime_loadbalance.txt");
		}

		switch(algorithm)
		{
		case 'P':
			if (enLProuting)
				output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_pertubation_lp.txt");
			else 
				output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_pertubation.txt");
			break;
		case 'H':		 
			output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_HVC.txt");
			break;

		case 'R':
			if (enLProuting)
				output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_randomdrop_lp.txt");
			else 
				output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_randomdrop.txt");
			break;
		case 'B':
			if (enLProuting)
				output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_backtracking_lp.txt");
			else 
				output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_backtracking.txt");
			break;
		case 'E':
				output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_exhaustive.txt");
			break;
		case 'F':
			if (enLProuting)
				output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_firstfit_lp.txt");
			else 
				output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_firstfit.txt");
			break;
		case 'N':
			if (enLProuting)
				output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_nextfit_lp.txt");
			else 
				output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_nextfit.txt");
			break;
		case 'G':
			if (enLProuting)
				output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_bestfit_lp.txt");
			else 
				output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_bestfit.txt");
			break;
		default:
			break;
		}	
	}
}



void static_RA_B(char algorithm,bool enLProuting)
{
	Graph G;
#ifdef _FatTree
	G.drawFattree();
#endif
#ifdef _L2VL2
	G.drawL2VL2();
#endif
#ifdef _VL2
	G.drawVL2();
#endif
#ifdef _Bcube
	G.drawBcube();
#endif
	//testKSP(G);
	//input
	const int n=5,m=3;
	int numOfreq=1e2,n_test=n,k_paths[]={4,2,1,0}; 
	// param of physical networks
	float p=1,p_minResBw=0.5,p_maxResBw=1; // with p in [500B,1G]
	// param of  VDC
	int numOfVM=12;
	float devBw=100,avgBw[n]={200,250,300,350,400};
	//output
	float max_utilization[n]={0},success_rate[n]={0},bandwidth_cost[n]={0};
	// start simulation
	
	
	clock_t  Aclock,Bclock;
	float running_time[n]={0};
	for (int j=0;j<m;j++)
	{
		if(k_paths[j]==0){
			G.Kmax=Kspt;
			G.routingOption=_ECMP;
		}
		else
		{
			G.Kmax=k_paths[j];
			G.routingOption=_KshortestLB;
		}
		if(enLProuting==1){
			G.Kmax=Kspt;
			G.routingOption=_KshortestLB;
			j=m+1;
		}
		for (int i=0;i<n_test;i++)
		{
			Aclock=clock();
			G.SingleRequest(algorithm,enLProuting,numOfreq,
				p,p_minResBw,p_maxResBw,numOfVM,avgBw[i]-devBw,avgBw[i]+devBw,
				max_utilization[i],success_rate[i],bandwidth_cost[i]);	
			Bclock=clock();
			running_time[i]=1000*(float)(Bclock-Aclock)/(CLOCKS_PER_SEC*(float)numOfreq);
			cout<<"process time is "<<running_time[i]<<"ms"<<endl;
		}

		switch(algorithm)
		{
		case 'P':
			if (enLProuting)
				output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_pertubation_lp.txt");
			else 
				output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_pertubation.txt");
			break;
		case 'H':		 
				output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_HVC.txt");
				break;
		case 'R':
			if (enLProuting)
				output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_randomdrop_lp.txt");
			else 
				output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_randomdrop.txt");
			break;
		case 'B':
			if (enLProuting)
				output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_backtracking_lp.txt");
			else 
				output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_backtracking.txt");
			break;
		case 'E':
				output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_exhaustive.txt");
			break;
		case 'F':
			if (enLProuting)
				output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_firstfit_lp.txt");
			else 
				output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_firstfit.txt");
			break;
		case 'G':
			if (enLProuting)
				output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_bestfit_lp.txt");
			else 
				output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_bestfit.txt");
			break;
		case 'N':
			if (enLProuting)
				output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_nextfit_lp.txt");
			else 
				output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_nextfit.txt");
			break;
		default:
			break;
		}	
	}
}



void static_embedding_oversub(char algorithm,bool enLProuting)
{	
	//testKSP(G);
	//input
	const int n=4,m=1;
	int numOfreq=1e2,n_test=n;
	// param of physical networks
	float oversub_tor[]={1,1,0.5,0.4},oversub_agg[]={1,0.5,0.5,0.5};
	//float oversub_tor[]={0.4,0.5,1,1},oversub_agg[]={0.5,0.5,0.5,1};
	float p=1,p_minResBw=0.8,p_maxResBw=1;// with p in [0,1G]
	// param of  VDC
	//int numOfVM[n]={2,4,6,8,9,10};
	int min_numOfVM=8,max_numOfVM=8;
	float minBw=100,maxBw=700;
	
	//output
	float max_utilization[n]={0},success_rate[n]={0},bandwidth_cost[n]={0};
	// start simulation
	
	clock_t  Aclock,Bclock;
	
	float running_time[n]={0};
	
	for (int i=0;i<n_test;i++)
	{	Graph G;
		G.drawVL2(oversub_tor[i],oversub_agg[i]);
		Aclock=clock();
		G.SingleRequest(algorithm,enLProuting,numOfreq,
			p,p_minResBw,p_maxResBw,min_numOfVM,max_numOfVM,minBw,maxBw,
			max_utilization[i],success_rate[i],bandwidth_cost[i]);	
		Bclock=clock();
		running_time[i]=1000*(float)(Bclock-Aclock)/(CLOCKS_PER_SEC*(float)numOfreq);
		cout<<"process time is "<<running_time[i]<<"ms"<<endl;
	}

	switch(algorithm)
	{
	case 'P':
		if (enLProuting)
			output2txt(n_test,running_time,oversub_tor,oversub_agg,min_numOfVM,max_numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_pertubation_lp.txt");
		else 
			output2txt(n_test,running_time,oversub_tor,oversub_agg,min_numOfVM,max_numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_pertubation.txt");
		break;
	case 'H':		 
		output2txt(n_test,running_time,oversub_tor,oversub_agg,min_numOfVM,max_numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_HVC.txt");
		break;

	case 'R':
		if (enLProuting)
			output2txt(n_test,running_time,oversub_tor,oversub_agg,min_numOfVM,max_numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_randomdrop_lp.txt");
		else 
			output2txt(n_test,running_time,oversub_tor,oversub_agg,min_numOfVM,max_numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_randomdrop.txt");
		break;
	case 'B':
		if (enLProuting)
			output2txt(n_test,running_time,oversub_tor,oversub_agg,min_numOfVM,max_numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_backtracking_lp.txt");
		else 
			output2txt(n_test,running_time,oversub_tor,oversub_agg,min_numOfVM,max_numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_backtracking.txt");
		break;
	case 'E':
			output2txt(n_test,running_time,oversub_tor,oversub_agg,min_numOfVM,max_numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_exhaustive.txt");
		break;
	case 'F':
		if (enLProuting)
			output2txt(n_test,running_time,oversub_tor,oversub_agg,min_numOfVM,max_numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_firstfit_lp.txt");
		else 
			output2txt(n_test,running_time,oversub_tor,oversub_agg,min_numOfVM,max_numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_firstfit.txt");
		break;
	case 'N':
		if (enLProuting)
			output2txt(n_test,running_time,oversub_tor,oversub_agg,min_numOfVM,max_numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_nextfit_lp.txt");
		else 
			output2txt(n_test,running_time,oversub_tor,oversub_agg,min_numOfVM,max_numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_nextfit.txt");
		break;
	case 'G':
		if (enLProuting)
			output2txt(n_test,running_time,oversub_tor,oversub_agg,min_numOfVM,max_numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_bestfit_lp.txt");
		else 
			output2txt(n_test,running_time,oversub_tor,oversub_agg,min_numOfVM,max_numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_bestfit.txt");
		break;
	default:
		break;
	}	
}


void static_RA_oversub(char algorithm,bool enLProuting)
{	
	//testKSP(G);
	//input
	const int n=4,m=3;
	int numOfreq=1e2,n_test=n,k_paths[]={4,2,1,0};
	// param of physical networks
	float oversub_tor[]={1,1,0.5,0.4},oversub_agg[]={1,0.5,0.5,0.5};
	float p=1,p_minResBw=0.8,p_maxResBw=1;// with p in [0,1G]
	// param of  VDC
	int numOfVM=8,min_numOfVM=numOfVM,max_numOfVM=numOfVM;
	float minBw=100,maxBw=700;
	//output
	float max_utilization[n]={0},success_rate[n]={0},bandwidth_cost[n]={0};
	// start simulation
	
	clock_t  Aclock,Bclock;
	
	float running_time[n]={0};
	for (int j=0;j<m;j++){	
		for (int i=0;i<n_test;i++)
		{	
			Graph G;
			G.drawVL2(oversub_tor[i],oversub_agg[i]);
		
			if(k_paths[j]==0){
				G.Kmax=Kspt;
				G.routingOption=_ECMP;
			}
			else
			{
				G.Kmax=k_paths[j];
				G.routingOption=_KshortestLB;
			}
			if(enLProuting==1){
				G.Kmax=Kspt;
				G.routingOption=_KshortestLB;
				j=m;
			}
			Aclock=clock();
			G.SingleRequest(algorithm,enLProuting,numOfreq,
				p,p_minResBw,p_maxResBw,min_numOfVM,max_numOfVM,minBw,maxBw,
				max_utilization[i],success_rate[i],bandwidth_cost[i]);	
			Bclock=clock();
			running_time[i]=1000*(float)(Bclock-Aclock)/(CLOCKS_PER_SEC*(float)numOfreq);
			cout<<"process time is "<<running_time[i]<<"ms"<<endl;
		}

		switch(algorithm)
		{
		case 'P':
			if (enLProuting)
				output2txt(Kspt,n_test,running_time,oversub_tor,oversub_agg,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_pertubation_lp.txt");
			else 
				output2txt(k_paths[j],n_test,running_time,oversub_tor,oversub_agg,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_pertubation.txt");
			break;
		case 'H':		 
			output2txt(k_paths[j],n_test,running_time,oversub_tor,oversub_agg,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_HVC.txt");
			break;

		case 'R':
			if (enLProuting)
				output2txt(Kspt,n_test,running_time,oversub_tor,oversub_agg,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_randomdrop_lp.txt");
			else 
				output2txt(k_paths[j],n_test,running_time,oversub_tor,oversub_agg,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_randomdrop.txt");
			break;
		case 'B':
			if (enLProuting)
				output2txt(Kspt,n_test,running_time,oversub_tor,oversub_agg,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_backtracking_lp.txt");
			else 
				output2txt(k_paths[j],n_test,running_time,oversub_tor,oversub_agg,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_backtracking.txt");
			break;
		case 'E':
				output2txt(k_paths[j],n_test,running_time,oversub_tor,oversub_agg,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_exhaustive.txt");
			break;
		case 'F':
			if (enLProuting)
				output2txt(Kspt,n_test,running_time,oversub_tor,oversub_agg,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_firstfit_lp.txt");
			else 
				output2txt(k_paths[j],n_test,running_time,oversub_tor,oversub_agg,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_firstfit.txt");
			break;
		case 'N':
			if (enLProuting)
				output2txt(Kspt,n_test,running_time,oversub_tor,oversub_agg,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_nextfit_lp.txt");
			else 
				output2txt(k_paths[j],n_test,running_time,oversub_tor,oversub_agg,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_nextfit.txt");
			break;
		case 'G':
			if (enLProuting)
				output2txt(Kspt,n_test,running_time,oversub_tor,oversub_agg,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_bestfit_lp.txt");
			else 
				output2txt(k_paths[j],n_test,running_time,oversub_tor,oversub_agg,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_bestfit.txt");
			break;
		default:
			break;
		}	
	}
}


void Homo_static_embedding_N(char algorithm)
{
	Graph G;
#ifdef _FatTree
	G.drawFattree();
#endif
#ifdef _L2VL2
	G.drawL2VL2();
#endif
#ifdef _VL2
	G.drawVL2();
#endif
#ifdef _Bcube
	G.drawBcube();
#endif
	//testKSP(G);
	//input
	const int n=6,m=1;
	int numOfreq=2e2,n_test=n;
	// param of physical networks
	float p=0.5,p_minResBw=0,p_maxResBw=1;// with p in [0,1G]
	// param of  VDC
	int numOfVM[n]={2,4,6,8,9,10};
	float minBw=100,maxBw=700;
	//always using optimal routing
	bool enLProuting=1;
	//output
	float max_utilization[n]={0},success_rate[n]={0},bandwidth_cost[n]={0};
	// start simulation
	
	clock_t  Aclock,Bclock;
	
	float running_time[n]={0};
	
	G.Kmax=Kspt;
	for (int i=0;i<n_test;i++)
	{
		Aclock=clock();
		G.HomoRequest(algorithm,enLProuting,numOfreq,
			p,p_minResBw,p_maxResBw,numOfVM[i],minBw,maxBw,
			max_utilization[i],success_rate[i],bandwidth_cost[i]);	
		Bclock=clock();
		running_time[i]=1000*(float)(Bclock-Aclock)/(CLOCKS_PER_SEC*(float)numOfreq);
		cout<<"process time is "<<running_time[i]<<"ms"<<endl;
	}

	switch(algorithm)
	{
	case 'P':
		if (enLProuting)
			output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_pertubation_lp.txt");
		else 
			output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_pertubation.txt");
		break;
	case 'H':		 
		output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_HVC.txt");
		break;

	case 'R':
		if (enLProuting)
			output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_randomdrop_lp.txt");
		else 
			output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_randomdrop.txt");
		break;
	case 'B':
		if (enLProuting)
			output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_backtracking_lp.txt");
		else 
			output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_backtracking.txt");
		break;
	case 'E':
			output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_exhaustive.txt");
		break;
	case 'F':
		if (enLProuting)
			output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_firstfit_lp.txt");
		else 
			output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_firstfit.txt");
		break;
	case 'N':
		if (enLProuting)
			output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_nextfit_lp.txt");
		else 
			output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_nextfit.txt");
		break;
	case 'G':
		if (enLProuting)
			output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_bestfit_lp.txt");
		else 
			output2txt(G,n_test,running_time,numOfVM,minBw,maxBw,max_utilization,success_rate,bandwidth_cost,"s_bestfit.txt");
		break;
	default:
		break;
	}	
	
}


void Homo_static_embedding_B(char algorithm)
{
	Graph G;
#ifdef _FatTree
	G.drawFattree();
#endif
#ifdef _L2VL2
	G.drawL2VL2();
#endif
#ifdef _VL2
	G.drawVL2();
#endif
#ifdef _Bcube
	G.drawBcube();
#endif
	//testKSP(G);
	//input
	const int n=5,m=4;
	int numOfreq=2e2,n_test=n; 
	// param of physical networks
	float p=0.5,p_minResBw=0,p_maxResBw=1; // with p in [500B,1G]
	// param of  VDC
	int numOfVM=8;
	float devBw=100,avgBw[n]={200,250,300,350,400};
	//output
	float max_utilization[n]={0},success_rate[n]={0},bandwidth_cost[n]={0};	
	//always using optimal routing
	bool enLProuting=1;

	// start simulation
	clock_t  Aclock,Bclock;
	
	float running_time[n]={0};
	G.Kmax=Kspt;
	for (int i=0;i<n_test;i++)
	{
		Aclock=clock();
		G.HomoRequest(algorithm,enLProuting,numOfreq,
			p,p_minResBw,p_maxResBw,numOfVM,avgBw[i]-devBw,avgBw[i]+devBw,
			max_utilization[i],success_rate[i],bandwidth_cost[i]);	
		Bclock=clock();
		running_time[i]=1000*(float)(Bclock-Aclock)/(CLOCKS_PER_SEC*(float)numOfreq);
		cout<<"process time is "<<running_time[i]<<"ms"<<endl;
	}

	switch(algorithm)
	{
	case 'P':
		if (enLProuting)
			output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_pertubation_lp.txt");
		else 
			output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_pertubation.txt");
		break;
	case 'H':		 
			output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_HVC.txt");
			break;
	case 'R':
		if (enLProuting)
			output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_randomdrop_lp.txt");
		else 
			output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_randomdrop.txt");
		break;
	case 'B':
		if (enLProuting)
			output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_backtracking_lp.txt");
		else 
			output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_backtracking.txt");
		break;
	case 'E':
			output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_exhaustive.txt");
		break;
	case 'F':
		if (enLProuting)
			output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_firstfit_lp.txt");
		else 
			output2txt(G,n_test,running_time,numOfVM,avgBw,max_utilization,success_rate,bandwidth_cost,"s_firstfit.txt");
		break;
	default:
		break;
	}	
	
}



int main()
{		
	//P: Pertubation; B:Backtracking; F:FirstFit
#ifndef _Tree	
	bool enLProuting=0;
	//single request simulation	
	if(1){			

//simulation of placement algorithms for N=2..10,B=100-700
	//static_embedding_N('P',enLProuting);
	//static_embedding_N('N',enLProuting);
	//static_embedding_N('G',enLProuting);
	//static_embedding_N('F',enLProuting);
	//static_embedding_N('B',enLProuting);

//simulation of placement algorithms for B=200-400,N=8
	//static_embedding_B('P',enLProuting);
	//static_embedding_B('N',enLProuting);
	//static_embedding_B('G',enLProuting);
	//static_embedding_B('F',enLProuting);
	//static_embedding_B('B',enLProuting);
//simulation of routing algorithms for B=200-400,N=12
		//enLProuting=0;static_RA_B('P',enLProuting);	
		//enLProuting=1;static_RA_B('P',enLProuting);
//running time of routing algorithms for N=2..10,B=100-700
		//enLProuting=0;static_RA_N('P',enLProuting);	
		//enLProuting=1;static_RA_N('P',enLProuting);

//simulation of placement algorithms under oversubscription for N=8,B=100-700
		//enLProuting=0;static_embedding_oversub('P',enLProuting);
		//static_embedding_oversub('F',enLProuting);
		//static_embedding_oversub('G',enLProuting);
		//static_embedding_oversub('N',enLProuting);
		//static_embedding_oversub('B',enLProuting);

//simulation of routing algorithms under oversubscription for N=8,B=100-700
	//enLProuting=1;static_RA_oversub('P',enLProuting);
	//enLProuting=0;static_RA_oversub('P',enLProuting);

//compare limited version of pertubation and backtracking 
		//enLProuting=0;static_embedding_N('P',enLProuting);	
		//enLProuting=0;static_embedding_N('B',enLProuting);



		//special case B1=B2...=BN		
		
		//Homo_static_embedding_N('H');
		//Homo_static_embedding_N('P');
		//Homo_static_embedding_B('H');
		Homo_static_embedding_B('P');

	}				
	// dynamic simulation
	else{
	dynamic_embedding('F',enLProuting);
		dynamic_embedding('P',enLProuting);
			dynamic_embedding('B',enLProuting);
				}	
#else
	//simulation of placement algorithms for N=2..10,B=100-700
	//	static_embedding_N('A');
	//static_embedding_N('P');
		//static_embedding_N('N');
		//static_embedding_N('G');
		//static_embedding_N('F');
			//static_embedding_N('R');
				static_embedding_N('B');

		//static_embedding_B('A');
		//static_embedding_B('P');
		//static_embedding_B('N');
		//static_embedding_B('G');
		//static_embedding_B('F');
		//static_embedding_B('R');
		static_embedding_B('B');

#endif
	// finish
	char c;
	cin.get(c);
	cout<<"\n"<<"end of Main"<<"\n"<<"Type any key to continue..";
	
}
