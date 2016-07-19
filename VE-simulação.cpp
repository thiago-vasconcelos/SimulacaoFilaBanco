#include<iostream>
#include<functional>
#include<queue>
#include<vector>
#include<math.h>
#include<cstdlib>
#include<ctime>

using namespace std;

typedef enum {
		REGULAR,
		PRIORITY
} ClientType;

typedef enum {
		CAIXA,
		OUTROS
} ServiceType;

class Client {
	private:
		float entranceTime;
		float leaveTime;
		ClientType type;
		ServiceType serviceType;
	public:
		Client(ClientType type, float entranceTime, ServiceType serviceType){
			this->entranceTime = entranceTime;
			this->leaveTime = 0;
			this->type = type;
			this->serviceType = serviceType;
		}

		ClientType GetType(){
			return this->type;
		}

		float GetEntranceTime(){
			return this->entranceTime;
		}

		ServiceType GetServiceType(){
			return this->serviceType;
		}

		void SetLeaveTime(float leaveTime){
			this->leaveTime = leaveTime;
		}

		float GetLeaveTime(){
			return this->leaveTime;
		}

		~Client(){}
};

struct CompareClients : public binary_function<Client*, Client*, bool>{
	bool operator()(Client* client1, Client* client2)
	{
		return client1->GetEntranceTime()>client2->GetEntranceTime();
	}
};

class atendente {
	private:
		float time;
		int id;
	public:
		atendente(int id) {
			this->time = 0;
			this->id = id;
		}

		float GetTime(){
			return this->time;
		}

		void SetTime(float time){
			this->time = time;
		}

		int GetId(){
			return this->id;
		}

		~atendente(){}
};

struct Compareatendentes : public binary_function<atendente*, atendente*, bool>{
	bool operator()(atendente* atendente1, atendente* atendente2)
	{
		return atendente1->GetTime()>atendente2->GetTime();
	}
};

float GenerateRandomNumber(){
	float random = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
return random*sin(random);
}

class Banco {
	private:
		float averageAttendaceTime;
		float PriorityClientsProbability;
		priority_queue<atendente*, vector<atendente*>, Compareatendentes> *atendentes;
		priority_queue<Client*, vector<Client*>, CompareClients> *regularCaixaClients;
		priority_queue<Client*, vector<Client*>, CompareClients> *priorityCaixaClients;
		priority_queue<Client*, vector<Client*>, CompareClients> *regularGerenteClients;
		priority_queue<Client*, vector<Client*>, CompareClients> *priorityGerenteClients;
		int numAtendentes;
		float wait;
		float priorityWait;
		float regularWait;
		float priorityGerenteWait;
		float regularGerenteWait;
		float priorityCaixaWait;
		float regularCaixaWait;
	public:
		Banco(float averageAttendaceTime, int numAtendentes) {
			this->averageAttendaceTime = averageAttendaceTime;
			this->numAtendentes = numAtendentes;
			this->atendentes = new priority_queue<atendente*, vector<atendente*>, Compareatendentes>();
			this->wait = 0;
			this->priorityCaixaWait = 0;
			this->regularCaixaWait = 0;
			this->priorityGerenteWait = 0;
			this->regularGerenteWait = 0;
			for(int i=0; i < numAtendentes; i++) {
				atendente* atendente = new atendente(i);
				atendentes->push(atendente);
			}
			this->regularCaixaClients = new priority_queue<Client*, vector<Client*>, CompareClients>();
			this->priorityCaixaClients = new priority_queue<Client*, vector<Client*>, CompareClients>();
			this->regularGerenteClients = new priority_queue<Client*, vector<Client*>, CompareClients>();
			this->priorityGerenteClients = new priority_queue<Client*, vector<Client*>, CompareClients>();
		}

		void AddClient(Client* newClient){
			switch(newClient->GetServiceType()){
				case OUTROS:
					switch (newClient->GetType()) {
						case PRIORITY:
						(this->priorityCaixaClients)->push(newClient);
						default:
						(this->regularCaixaClients)->push(newClient);
					}
				default:
					switch (newClient->GetType()) {
						case PRIORITY:
						(this->priorityCaixaClients)->push(newClient);
						default:
						(this->regularCaixaClients)->push(newClient);
					}
			}
		}

		Client* GetNextClient(float curTime){
			if(!((this->priorityCaixaClients)->empty()) && !((this->regularCaixaClients)->empty())) {
				Client* priorityClient = (this->priorityCaixaClients)->top();
				Client* regularClient = (this->regularCaixaClients)->top();
				if(priorityClient->GetEntranceTime() <= curTime){
					priorityCaixaClients->pop();
					return priorityClient;
				}
				if(priorityClient->GetEntranceTime() < regularClient->GetEntranceTime()){
					priorityCaixaClients->pop();
					return priorityClient;
				}
				regularCaixaClients->pop();
				return regularClient;
			} else if(!((this->priorityCaixaClients)->empty())) {
				Client* priorityClient = (this->priorityCaixaClients)->top();
				priorityCaixaClients->pop();
				return priorityClient;
			} else {
				Client* regularClient = (this->regularCaixaClients)->top();
				regularCaixaClients->pop();
				return regularClient;
			}
		}

		Client* SolveCaixaClient() {
			atendente *atendente = atendentes->top();
			atendentes->pop();
			Client* client = this->GetNextClient(atendente->GetTime());
			float curTime = client->GetEntranceTime();
			float atendenteTime = atendente->GetTime();
			float newCurTime;
			if (atendenteTime > curTime) {
				wait += atendenteTime - curTime;
				if(client->GetType() == PRIORITY){
					this->priorityCaixaWait += atendenteTime - curTime;
				}
				if(client->GetType() == REGULAR){
					this->regularCaixaWait += atendenteTime - curTime;
				}
				newCurTime = atendenteTime + this->GetAttendanceTime();
			}
			else {
				newCurTime = curTime + this->GetAttendanceTime();
			}
			atendente->SetTime(newCurTime);
			atendentes->push(atendente);
			client->SetLeaveTime(newCurTime);
			return client;
		}

		float GetAttendanceTime(){
			return this->averageAttendaceTime;
		}

		float GetWait(){
			return this->wait;
		}

		float GetPriorityGerenteWait(){
			return this->priorityGerenteWait;
		}

		float GetPriorityCaixaWait(){
			return this->priorityCaixaWait;
		}

		float GetRegularGerenteWait(){
			return this->regularGerenteWait;
		}

		float GetRegularCaixaWait(){
			return this->regularCaixaWait;
		}

		~Banco(){
			for(int i=0; i<numAtendentes; i++) {
				atendentes->pop();
			}
			delete(atendentes);
		}
};

class Simulation {
	private:
		float averageEntrance;
		float priorityClientsProbability;
		float runTime;
		float serviceProbability;
		int numClients;
		int numPriorityGerenteClients;
		int numPriorityCaixaClients;
		int numRegularGerenteClients;
		int numRegularCaixaClients;
		Banco *banco;
	public:
		Simulation(float averageAttendanceTime, float averageEntrance, int numAtendentes, float priorityClientsProbability, float runTime, float serviceProbability) {
			this->averageEntrance = averageEntrance;
			this->priorityClientsProbability = priorityClientsProbability;
			this->runTime = runTime;
			this->serviceProbability = serviceProbability;
			this->numPriorityGerenteClients = 0;
			this->numPriorityCaixaClients = 0;
			this->numRegularGerenteClients = 0;
			this->numRegularCaixaClients = 0;
			this->banco = new Banco(averageAttendanceTime, numAtendentes);
		}

		Client* GenerateClient(float curTime){
			float probability = GenerateRandomNumber();
			ServiceType serviceType;
			ClientType clientType;

			if(probability < this->serviceProbability){
				serviceType = CAIXA;
			} else{
				serviceType = OUTROS;
			}

			if(probability < this->priorityClientsProbability){
				clientType = PRIORITY;
			} else{
				clientType = REGULAR;
			}
			return new Client(clientType, curTime, serviceType);
		}

		void AddClients(){
			for(float curTime = 0; curTime<(this->runTime); curTime += this->GetEntranceTime()){
				Client* newClient = GenerateClient(curTime);
				(this->banco)->AddClient(newClient);
				this->numClients +=1;
				if(newClient->GetType() == PRIORITY && newClient->GetServiceType() == OUTROS){
					this->numPriorityGerenteClients += 1;
				}
				if(newClient->GetType() == PRIORITY && newClient->GetServiceType() == CAIXA){
					this->numPriorityCaixaClients += 1;
				}
				if(newClient->GetType() == REGULAR && newClient->GetServiceType() == OUTROS){
					this->numRegularGerenteClients += 1;
				}
				if(newClient->GetType() == REGULAR && newClient->GetServiceType() == CAIXA){
					this->numRegularCaixaClients += 1;
				}
			}
		}

		void SolveClients(int numClients){
			for(int i=0; i< numClients; i++){
				cout<<"***********************************************"<<endl;
				Client* leavingClient = (this->banco)->SolveClient();
				delete(leavingClient);
			}
		}

		void Run() {
			this->AddClients();
			this->SolveClients(this->numClients);
		}

		float GetWait(){
			float averageWaitTime = 0;
			if(this->numClients !=0 )
				averageWaitTime = ((this->banco)->GetWait())/this->numClients;
			return averageWaitTime;
		}

		float GetAveragePriorityGerenteWait(){
			float averagePriorityGerenteWait = 0;
			if(this->numPriorityGerenteClients !=0 )
				averagePriorityGerenteWait = ((this->banco)->GetPriorityGerenteWait())/this->numPriorityGerenteClients;
			return averagePriorityGerenteWait;
		}

		float GetAveragePriorityCaixaWait(){
			float averagePriorityCaixaWait = 0;
			if(this->numPriorityCaixaClients !=0 )
				averagePriorityCaixaWait = ((this->banco)->GetPriorityCaixaWait())/this->numPriorityCaixaClients;
			return averagePriorityCaixaWait;
		}

		float GetAverageRegularGerenteWait(){
			float averageRegularGerenteWait = 0;
			if(this->numRegularGerenteClients !=0 )
				averageRegularGerenteWait = ((this->banco)->GetRegularGerenteWait())/this->numRegularGerenteClients;
			return averageRegularGerenteWait;
		}

		float GetAverageRegularCaixaWait(){
			float averageRegularCaixaWait = 0;
			if(this->numRegularCaixaClients !=0 )
				averageRegularCaixaWait = ((this->banco)->GetRegularCaixaWait())/this->numRegularCaixaClients;
			return averageRegularCaixaWait;
		}

		float GetEntranceTime(){
			return this->averageEntrance;
		}

		~Simulation(){
			delete(banco);
		}
};

int main() {
	srand (static_cast <unsigned> (time(0)));
	float averageAttendanceTime;
	float averageEntrance;
	int numAtendentes;
	float priorityClientsProbability;
	float serviceProbability;
	float runTime;
	cin>>averageAttendanceTime>>averageEntrance>>numAtendentes>>priorityClientsProbability>>runTime>>serviceProbability;
	cout<<"averageAttendanceTime: "<<averageAttendanceTime<<endl;
	cout<<"averageEntrance: "<<averageEntrance<<endl;
	cout<<"numAtendentes: "<<numAtendentes<<endl;
	cout<<"priorityClientsProbability: "<<priorityClientsProbability<<endl;
	cout<<"runTime: "<<runTime<<endl;
	cout<<"serviceProbability: "<<serviceProbability<<endl;
	Simulation *simulation = new Simulation(averageAttendanceTime, averageEntrance, numAtendentes, priorityClientsProbability, runTime, serviceProbability);
	simulation->Run();
	cout<<"averageWait: "<<simulation->GetWait()<<endl;
	cout<<"averagePriorityGerenteWait: "<<simulation->GetAveragePriorityGerenteWait()<<endl;
	cout<<"averagePriorityCaixaWait: "<<simulation->GetAveragePriorityCaixaWait()<<endl;
	cout<<"averageRegularGerenteWait: "<<simulation->GetAverageRegularGerenteWait()<<endl;
	cout<<"averageRegularCaixaWait: "<<simulation->GetAverageRegularCaixaWait()<<endl;
	delete(simulation);
	return 0;
}
