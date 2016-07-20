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

		void SetEntranceTime(float entranceTime){
			this->entranceTime = entranceTime;
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

class Atendente {
	private:
		float time;
		int id;
	public:
		Atendente(int id) {
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

		~Atendente(){}
};

struct Compareatendentes : public binary_function<Atendente*, Atendente*, bool>{
	bool operator()(Atendente* atendente1, Atendente* atendente2)
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
		float averageTicketTime;
		float PriorityClientsProbability;
		priority_queue<Atendente*, vector<Atendente*>, Compareatendentes> *caixas;
		priority_queue<Atendente*, vector<Atendente*>, Compareatendentes> *gerentes;
		priority_queue<Client*, vector<Client*>, CompareClients> *regularCaixaClients;
		priority_queue<Client*, vector<Client*>, CompareClients> *priorityCaixaClients;
		priority_queue<Client*, vector<Client*>, CompareClients> *regularGerenteClients;
		priority_queue<Client*, vector<Client*>, CompareClients> *priorityGerenteClients;
		int numCaixas;
		int numGerentes;
		float wait;
		float priorityWait;
		float regularWait;
		float priorityGerenteWait;
		float regularGerenteWait;
		float priorityCaixaWait;
		float regularCaixaWait;
		float ticketWait;
		float ticketTime;
	public:
		Banco(float averageAttendaceTime, float averageTicketTime, int numCaixas, int numGerentes) {
			this->averageAttendaceTime = averageAttendaceTime;
			this->averageTicketTime = averageTicketTime;
			this->numCaixas = numCaixas;
			this->numGerentes = numGerentes;
			this->caixas = new priority_queue<Atendente*, vector<Atendente*>, Compareatendentes>();
			this->gerentes = new priority_queue<Atendente*, vector<Atendente*>, Compareatendentes>();
			this->wait = 0;
			this->priorityCaixaWait = 0;
			this->regularCaixaWait = 0;
			this->priorityGerenteWait = 0;
			this->regularGerenteWait = 0;
			this->ticketWait = 0;
			this->ticketTime = 0;
			for(int i=0; i < numCaixas; i++) {
				Atendente* atendente = new Atendente(i);
				caixas->push(atendente);
			}
			for(int i=0; i < numGerentes; i++) {
				Atendente* atendente = new Atendente(i);
				gerentes->push(atendente);
			}
			this->regularCaixaClients = new priority_queue<Client*, vector<Client*>, CompareClients>();
			this->priorityCaixaClients = new priority_queue<Client*, vector<Client*>, CompareClients>();
			this->regularGerenteClients = new priority_queue<Client*, vector<Client*>, CompareClients>();
			this->priorityGerenteClients = new priority_queue<Client*, vector<Client*>, CompareClients>();
		}

		float GetAverageTicketTime(){
			return this->averageTicketTime;
		}

		void AddClient(Client* newClient){
			if(ticketTime>newClient->GetEntranceTime()){
				ticketWait += ticketTime - newClient->GetEntranceTime();
				newClient->SetEntranceTime(ticketTime);
			} else {
				ticketTime = newClient->GetEntranceTime();
				ticketTime+=this->GetAverageTicketTime();
			}
			switch(newClient->GetServiceType()){
				case OUTROS:
					switch (newClient->GetType()) {
						case PRIORITY:
							(this->priorityGerenteClients)->push(newClient);
							break;
						default:
							(this->regularGerenteClients)->push(newClient);
					}
					break;
				default:
					switch (newClient->GetType()) {
						case PRIORITY:
							(this->priorityCaixaClients)->push(newClient);
							break;
						default:
							(this->regularCaixaClients)->push(newClient);
					}
			}
		}

		Client* GetNextCaixaClient(float curTime){
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
			Atendente *atendente = (this->caixas)->top();
			(this->caixas)->pop();
			Client* client = this->GetNextCaixaClient(atendente->GetTime());
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
			caixas->push(atendente);
			client->SetLeaveTime(newCurTime);
			return client;
		}

		Client* GetNextGerenteClient(float curTime){
			if(!((this->priorityGerenteClients)->empty()) && !((this->regularGerenteClients)->empty())) {
				Client* priorityClient = (this->priorityGerenteClients)->top();
				cout<<"***************************"<<endl;
				Client* regularClient = (this->regularGerenteClients)->top();
				if(priorityClient->GetEntranceTime() <= curTime){
					priorityGerenteClients->pop();
					return priorityClient;
				}
				if(priorityClient->GetEntranceTime() < regularClient->GetEntranceTime()){
					priorityGerenteClients->pop();
					return priorityClient;
				}
				regularGerenteClients->pop();
				return regularClient;
			} else if(!((this->priorityGerenteClients)->empty())) {
				Client* priorityClient = (this->priorityGerenteClients)->top();
				priorityGerenteClients->pop();
				return priorityClient;
			} else {
				Client* regularClient = (this->regularGerenteClients)->top();
				regularGerenteClients->pop();
				return regularClient;
			}
		}

		Client* SolveGerenteClient() {
			Atendente *atendente = (this->gerentes)->top();
			(this->gerentes)->pop();
			Client* client = this->GetNextGerenteClient(atendente->GetTime());
			float curTime = client->GetEntranceTime();
			float atendenteTime = atendente->GetTime();
			float newCurTime;
			if (atendenteTime > curTime) {
				wait += atendenteTime - curTime;
				if(client->GetType() == PRIORITY){
					this->priorityGerenteWait += atendenteTime - curTime;
				}
				if(client->GetType() == REGULAR){
					this->regularGerenteWait += atendenteTime - curTime;
				}
				newCurTime = atendenteTime + this->GetAttendanceTime();
			}
			else {
				newCurTime = curTime + this->GetAttendanceTime();
			}
			atendente->SetTime(newCurTime);
			gerentes->push(atendente);
			client->SetLeaveTime(newCurTime);
			return client;
		}

		float GetAttendanceTime(){
			return this->averageAttendaceTime;
		}

		float GetWait(){
			return this->ticketWait + this->wait;
		}

		float GetPriorityGerenteWait(){
			return this->ticketWait + this->priorityGerenteWait;
		}

		float GetPriorityCaixaWait(){
			return this->ticketWait + this->priorityCaixaWait;
		}

		float GetRegularGerenteWait(){
			return this->ticketWait + this->regularGerenteWait;
		}

		float GetRegularCaixaWait(){
			return this->ticketWait + this->regularCaixaWait;
		}

		~Banco(){
			for(int i=0; i<(this->numCaixas); i++) {
				Atendente* atendente = (this->caixas)->top();
				(this->caixas)->pop();
				delete(atendente);
			}
			delete(caixas);
			for(int i=0; i<(this->numGerentes); i++) {
				Atendente* atendente = (this->gerentes)->top();
 				(this->gerentes)->pop();
 				delete(atendente);
			}
			delete(gerentes);
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
		Simulation(float averageAttendanceTime, float averageTicketTime, float averageEntrance, int numCaixas, int numGerentes, float priorityClientsProbability, float runTime, float serviceProbability) {
			this->averageEntrance = averageEntrance;
			this->priorityClientsProbability = priorityClientsProbability;
			this->runTime = runTime;
			this->serviceProbability = serviceProbability;
			this->numPriorityGerenteClients = 0;
			this->numPriorityCaixaClients = 0;
			this->numRegularGerenteClients = 0;
			this->numRegularCaixaClients = 0;
			this->banco = new Banco(averageAttendanceTime, averageTicketTime, numCaixas, numGerentes);
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

		void SolveClients(){
			for(int i=0; i< numRegularCaixaClients+numPriorityCaixaClients; i++){
				Client* leavingClient = (this->banco)->SolveCaixaClient();
				delete(leavingClient);
			}
			for(int i=0; i< numRegularGerenteClients+numPriorityGerenteClients; i++){
				Client* leavingClient = (this->banco)->SolveGerenteClient();
				delete(leavingClient);
			}
		}

		void Run() {
			this->AddClients();
			this->SolveClients();
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
	float averageTicketTime;
	float averageEntrance;
	int numCaixas;
	int numGerentes;
	float priorityClientsProbability;
	float serviceProbability;
	float runTime;
	cin>>averageAttendanceTime>>averageTicketTime>>averageEntrance>>numCaixas>>numGerentes>>priorityClientsProbability>>runTime>>serviceProbability;
	cout<<"averageAttendanceTime: "<<averageAttendanceTime<<endl;
	cout<<"averageTicketTime: "<<averageTicketTime<<endl;
	cout<<"averageEntrance: "<<averageEntrance<<endl;
	cout<<"numCaixas: "<<numCaixas<<endl;
	cout<<"numGerentes: "<<numGerentes<<endl;
	cout<<"priorityClientsProbability: "<<priorityClientsProbability<<endl;
	cout<<"runTime: "<<runTime<<endl;
	cout<<"serviceProbability: "<<serviceProbability<<endl;
	Simulation *simulation = new Simulation(averageAttendanceTime, averageTicketTime, averageEntrance, numCaixas, numGerentes, priorityClientsProbability, runTime, serviceProbability);
	simulation->Run();
	cout<<"averageWait: "<<simulation->GetWait()<<endl;
	cout<<"averagePriorityGerenteWait: "<<simulation->GetAveragePriorityGerenteWait()<<endl;
	cout<<"averagePriorityCaixaWait: "<<simulation->GetAveragePriorityCaixaWait()<<endl;
	cout<<"averageRegularGerenteWait: "<<simulation->GetAverageRegularGerenteWait()<<endl;
	cout<<"averageRegularCaixaWait: "<<simulation->GetAverageRegularCaixaWait()<<endl;
	delete(simulation);
	return 0;
}
