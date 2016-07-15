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
	public:
		Client(ClientPriority priority, float entranceTime){
			this->priority = priority;
			this->entranceTime = entranceTime;
		}
		
		ClientPriority GetPriority(){
			return this->priority;
		}
		
		float GetEntranceTime(){
			return this->entranceTime;
		}
		
		~Client(){}
};

struct CompareClients : public binary_function<Client*, Client*, bool>{
	bool operator()(Client* client1, Client* client2) const
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
	bool operator()(Caixa* caixa1, Caixa* caixa2) const
	{
		return caixa1->getTime()>caixa2->getTime();
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
		
		void AddClient(float curTime){
			float probability = GenerateRandomNumber();
			Client* newClient;
			if(probability < this->PriorityClientsProbability){
				newClient = new Client(PRIORITY, curTime);
			}
			else{
				newClient = new Client(REGULAR, curTime);				
			}
			(this->clients)->push(newClient);
		}
		
		Client* SolveClient() {
			Caixa *caixa = caixas->top();
			caixas->pop();
			Client *client = clients->top();
			clients->pop();
			float curTime = client->GetEntranceTime();
			float caixaTime = caixa->GetTime();
			if (caixaTime > curTime) {
				wait += caixaTime - curTime;
				caixa->setTime(caixaTime + this->GetAttendanceTime());
			}
			else {
				caixa->setTime(curTime + this->GetAttendanceTime());
			}
			caixas->push(caixa);
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
		Event(EventType eventType, float time){
			this->eventType = eventType;
			this->time = time;
		}
		
		~Event(){}
};

class Simulation {
	private:
		float averageAttendanceTime;
		float averageEntrance;
		float runTime;
		int numCaixas;
		queue<Event*>* enventQueue;
	public:
		Simulation(float averageAttendanceTime, float averageEntrance, float runTime, int numCaixas) {
			this->averageAttendanceTime = averageAttendanceTime;
			this->averageEntrance = averageEntrance;
			this->runTime = runTime;
			this->numCaixas = numCaixas;
			this->enventQueue = new queue<Event*>();
		}

		float Run() {
			int clients = 0;
			float averageWaitTime = 0;
			Banco *banco = new Banco(this->averageAttendanceTime, this->numCaixas);
			for(int i=0; i<runTime; i += this->GetEntranceTime()){
				banco->AddClient(i);
				clients++;
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
			delete(eventQueue)
		}
};

int main() {
	srand (static_cast <unsigned> (time(0)));
	float averageAttendanceTime;
	float averageEntrance;
	float runTime;
	int numCaixas;
	cin>>averageAttendanceTime>>averageEntrance>>runTime>>numCaixas;
	Simulation *simulation = new Simulation(averageAttendanceTime, averageEntrance, runTime, numCaixas);
	float averageWait = simulation->Run();
	cout<<averageWait;
	delete(simulation);
	return 0;
}
