#include<iostream>
#include<functional>
#include<queue>
#include<vector>
#include<math.h>
#include <cstdlib>
#include <ctime>

using namespace std;
typedef enum{
	PRIORITY,
	REGULAR
} ClientPriority;

typedef enum{
	ADD,
	SOLVE
}EventType;

class Client {
	private:
		ClientPriority priority;
		float entranceTime;
		float leaveTime;
	public:
		Client(ClientPriority priority, float entranceTime){
			this->priority = priority;
			this->entranceTime = entranceTime;
			this->leaveTime = 0;
		}

		ClientPriority GetPriority(){
			return this->priority;
		}

		float GetEntranceTime(){
			return this->entranceTime;
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
		if(client1->GetPriority() != client2->GetPriority()){
			if(client1->GetPriority() == PRIORITY) return true;
			return false;
		}
		return client1->GetEntranceTime()>client2->GetEntranceTime();
	}
};

class Caixa {
	private:
		float time;
		int id;
	public:
		Caixa(int id) {
			time = 0;
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

		~Caixa(){}
};

struct CompareCaixas : public binary_function<Caixa*, Caixa*, bool>{
	bool operator()(Caixa* caixa1, Caixa* caixa2)
	{
		return caixa1->GetTime()>caixa2->GetTime();
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
		priority_queue<Caixa*, vector<Caixa*>, CompareCaixas> *caixas;
		priority_queue<Client*, vector<Client*>, CompareClients> *clients;
		int numCaixas;
		float wait;
	public:
		Banco(float averageAttendaceTime, int numCaixas) {
			this->averageAttendaceTime = averageAttendaceTime;
			this->numCaixas = numCaixas;
			this->caixas = new priority_queue<Caixa*, vector<Caixa*>, CompareCaixas>();
			this->wait = 0;
			for(int i=0; i < numCaixas; i++) {
				Caixa* caixa = new Caixa(i);
				caixas->push(caixa);
			}
			this->clients = new priority_queue<Client*, vector<Client*>, CompareClients>();
		}

		void AddClient(Client* newClient){
			(this->clients)->push(newClient);
		}

		Client* SolveClient() {
			Caixa *caixa = caixas->top();
			caixas->pop();
			Client *client = clients->top();
			clients->pop();
			float curTime = client->GetEntranceTime();
			float caixaTime = caixa->GetTime();
			float newCurTime;
			if (caixaTime > curTime) {
				wait += caixaTime - curTime;
				newCurTime = caixaTime + this->GetAttendanceTime();
			}
			else {
				newCurTime = curTime + this->GetAttendanceTime();
			}
			caixa->SetTime(newCurTime);
			caixas->push(caixa);
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
			for(int i=0; i<numCaixas; i++) {
				caixas->pop();
			}
			delete(caixas);
		}
};

class Events{
	private:
		EventType eventType;
		float time;
	public:
		Events(EventType eventType, float time){
			this->eventType = eventType;
			this->time = time;
		}

		EventType GetEventType(){
			return this->eventType;
		}

		float GetTime(){
			return this->time;
		}

		~Events(){}
};

struct CompareEvents : public binary_function<Events*, Events*, bool>{
	bool operator()(Events* event1, Events* event2)
	{
		return event1->GetTime()>event2->GetTime();
	}
};

class Simulation {
	private:
		float averageAttendanceTime;
		float averageEntrance;
		int numCaixas;
		priority_queue<Events*, vector<Events*>, CompareEvents>* eventQueue;
		float runTime;
		float priorityClientsProbability;
	public:
		Simulation(float averageAttendanceTime, float averageEntrance, int numCaixas, float priorityClientsProbability, float runTime) {
			this->averageAttendanceTime = averageAttendanceTime;
			this->averageEntrance = averageEntrance;
			this->eventQueue = new priority_queue<Events*, vector<Events*>, CompareEvents>();
			this->numCaixas = numCaixas;
			this->priorityClientsProbability = priorityClientsProbability;
			this->runTime = runTime;
		}

		Client* GenerateClient(float curTime){
			float probability = GenerateRandomNumber();
			Client* newClient;
			if(probability < this->priorityClientsProbability){
				newClient = new Client(PRIORITY, curTime);
			}
			else{
				newClient = new Client(REGULAR, curTime);
			}
			return newClient;
		}

		float Run() {
			int clients = 0;
			float averageWaitTime = 0;
			cout<<"curTime"<<endl;
			Banco *banco = new Banco(this->averageAttendanceTime, this->numCaixas);
			Events* firstEvent = new Events(ADD, this->GetEntranceTime());
			eventQueue->push(firstEvent);
			Events* event;
			for(float curTime = 0; !(eventQueue->empty());){
				cout<<curTime<<endl;
				event = eventQueue->top();
				eventQueue->pop();
				switch (event->GetEventType()) {
					case ADD:
					{
						Client* newClient = GenerateClient(curTime);
						float nextClientEntrance = curTime + this->GetEntranceTime();
						if(nextClientEntrance < this->runTime){
							banco->AddClient(newClient);
							clients++;
							Events* newEvent = new Events(ADD, curTime);
							eventQueue->push(newEvent);
						} else{
							delete(newClient);
						}
						break;
					}
					default:
					{
						Client* leavingClient = banco->SolveClient();
						Events* newEvent = new Events(SOLVE, leavingClient->GetLeaveTime());
						eventQueue->push(newEvent);
						delete(leavingClient);
					}
				}
			}

			if(clients != 0) {
				averageWaitTime = (banco->GetWait())/clients;
			}

			delete(banco);
			return averageWaitTime;
		}

		float GetEntranceTime(){
			return this->averageEntrance;
		}

		~Simulation(){
			delete(eventQueue);
		}
};

int main() {
	srand (static_cast <unsigned> (time(0)));
	float averageAttendanceTime;
	float averageEntrance;
	int numCaixas;
	float priorityClientsProbability;
	float runTime;
	cin>>averageAttendanceTime>>averageEntrance>>numCaixas>>priorityClientsProbability>>runTime;
	cout<<"averageAttendanceTime: "<<averageAttendanceTime<<endl;
	cout<<"averageEntrance: "<<averageEntrance<<endl;
	cout<<"numCaixas: "<<numCaixas<<endl;
	cout<<"priorityClientsProbability: "<<priorityClientsProbability<<endl;
	cout<<"runTime: "<<runTime<<endl;
	Simulation *simulation = new Simulation(averageAttendanceTime, averageEntrance, numCaixas, priorityClientsProbability, runTime);
	float averageWait = simulation->Run();
	cout<<averageWait;
	delete(simulation);
	return 0;
}
