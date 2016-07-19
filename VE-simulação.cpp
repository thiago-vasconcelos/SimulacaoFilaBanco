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

class Atendimento {
	private:
		float time;
		int id;
	public:
		Atendimento(int id) {
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

		~Atendimento(){}
};

struct CompareAtendimentos : public binary_function<Atendimento*, Atendimento*, bool>{
	bool operator()(Atendimento* Atendimento1, Atendimento* Atendimento2)
	{
		return Atendimento1->GetTime()>Atendimento2->GetTime();
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
		priority_queue<Atendimento*, vector<Atendimento*>, CompareAtendimentos> *atendimentos;
		priority_queue<Client*, vector<Client*>, CompareClients> *regularCaixaClients;
		priority_queue<Client*, vector<Client*>, CompareClients> *priorityCaixaClients;
		priority_queue<Client*, vector<Client*>, CompareClients> *regularGerenteClients;
		priority_queue<Client*, vector<Client*>, CompareClients> *priorityGerenteClients;
		int numAtendimentos;
		float wait;
	public:
		Banco(float averageAttendaceTime, int numAtendimentos) {
			this->averageAttendaceTime = averageAttendaceTime;
			this->numAtendimentos = numAtendimentos;
			this->atendimentos = new priority_queue<Atendimento*, vector<Atendimento*>, CompareAtendimentos>();
			this->wait = 0;
			for(int i=0; i < numAtendimentos; i++) {
				Atendimento* atendimento = new Atendimento(i);
				atendimentos->push(atendimento);
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

		Client* SolveClient() {
			Atendimento *atendimento = atendimentos->top();
			atendimentos->pop();
			Client* client = this->GetNextClient(atendimento->GetTime());
			float curTime = client->GetEntranceTime();
			float AtendimentoTime = atendimento->GetTime();
			float newCurTime;
			if (AtendimentoTime > curTime) {
				wait += AtendimentoTime - curTime;
				newCurTime = AtendimentoTime + this->GetAttendanceTime();
			}
			else {
				newCurTime = curTime + this->GetAttendanceTime();
			}
			atendimento->SetTime(newCurTime);
			atendimentos->push(atendimento);
			client->SetLeaveTime(newCurTime);
			return client;
		}

		float GetAttendanceTime(){
			return this->averageAttendaceTime;
		}

		float GetWait(){
			return this->wait;
		}

		~Banco(){
			for(int i=0; i<numAtendimentos; i++) {
				atendimentos->pop();
			}
			delete(atendimentos);
		}
};

class Simulation {
	private:
		float averageAttendanceTime;
		float averageEntrance;
		int numAtendimentos;
		float priorityClientsProbability;
		float runTime;
		float serviceProbability;
	public:
		Simulation(float averageAttendanceTime, float averageEntrance, int numAtendimentos, float priorityClientsProbability, float runTime, float serviceProbability) {
			this->averageAttendanceTime = averageAttendanceTime;
			this->averageEntrance = averageEntrance;
			this->numAtendimentos = numAtendimentos;
			this->priorityClientsProbability = priorityClientsProbability;
			this->runTime = runTime;
			this->serviceProbability = serviceProbability;
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

		int AddClients(Banco* banco){
			int clientsCount = 0;
			for(float curTime = 0; curTime<(this->runTime); curTime += this->GetEntranceTime()){
				Client* newClient = GenerateClient(curTime);
				banco->AddClient(newClient);
				clientsCount++;
			}
			return clientsCount;
		}

		void SolveClients(int numClients, Banco* banco){
			for(int i=0; i< numClients; i++){
				Client* leavingClient = banco->SolveClient();
				delete(leavingClient);
			}
		}

		float Run() {
			float averageWaitTime = 0;
			Banco *banco = new Banco(this->averageAttendanceTime, this->numAtendimentos);
			int numClients = this->AddClients(banco);
			this->SolveClients(numClients, banco);

			if(numClients != 0) {
				averageWaitTime = (banco->GetWait())/numClients;
			}
			delete(banco);
			return averageWaitTime;
		}

		float GetEntranceTime(){
			return this->averageEntrance;
		}

		~Simulation(){}
};

int main() {
	srand (static_cast <unsigned> (time(0)));
	float averageAttendanceTime;
	float averageEntrance;
	int numAtendimentos;
	float priorityClientsProbability;
	float serviceProbability;
	float runTime;
	cin>>averageAttendanceTime>>averageEntrance>>numAtendimentos>>priorityClientsProbability>>runTime>>serviceProbability;
	cout<<"averageAttendanceTime: "<<averageAttendanceTime<<endl;
	cout<<"averageEntrance: "<<averageEntrance<<endl;
	cout<<"numAtendimentos: "<<numAtendimentos<<endl;
	cout<<"priorityClientsProbability: "<<priorityClientsProbability<<endl;
	cout<<"runTime: "<<runTime<<endl;
	cout<<"runTime: "<<serviceProbability<<endl;
	Simulation *simulation = new Simulation(averageAttendanceTime, averageEntrance, numAtendimentos, priorityClientsProbability, runTime, serviceProbability);
	float averageWait = simulation->Run();
	cout<<"averageWait: "<<averageWait<<endl;
	delete(simulation);
	return 0;
}
