#include<iostream>
#include<queue>

using namespace std;

class Caixa {
	private:
		float time;
		int id;
	public:
		Caixa(int id) {
			time = 0;
			this->id = id;
		}

		float getTime(){
			return this->time;
		}

		void setTime(float time){
			this->time = time;
		}
		
		int getId(){
			return this->id;
		}
		
		~Caixa(){}
};

class Banco {
	private:
		float averageAttendaceTime;
		priority_queue<Caixa*> *caixas;
		int numCaixas;
		float wait;
	public:
		Banco(float averageAttendaceTime, int numCaixas) {
			this->averageAttendaceTime = averageAttendaceTime;
			this->numCaixas = numCaixas;
			this->caixas = new priority_queue<Caixa*>();
			this->wait = 0;
			for(int i=0; i < numCaixas; i++) {
				Caixa* caixa = new Caixa(i);
				caixas->push(caixa);
			}
		}

		void addClient(float curTime) {
			Caixa *caixa = caixas->top();
			caixas->pop();
			float caixaTime = caixa->getTime();
			if (caixaTime > curTime) {
				wait += caixaTime - curTime;
				caixa->setTime(caixaTime + this->getAttendanceTime());
			}
			else {
				caixa->setTime(curTime + this->getAttendanceTime());
			}
			caixas->push(caixa);
		}
		
		float getAttendanceTime(){
			return this->averageAttendaceTime;
		}

		float getWait(){
			return this->wait;
		}

		~Banco(){
			for(int i=0; i<numCaixas; i++) {
				caixas->pop();
			}
			delete(caixas);
		}
};

class Simulation {
	private:
		float averageAttendanceTime;
		float averageEntrance;
		float runTime;
		int numCaixas;
	public:
		Simulation(float averageAttendanceTime, float averageEntrance, float runTime, int numCaixas) {
			this->averageAttendanceTime = averageAttendanceTime;
			this->averageEntrance = averageEntrance;
			this->runTime = runTime;
			this->numCaixas = numCaixas;
		}

		float run() {
			int clients = 0;
			float averageWaitTime = 0;
			Banco *banco = new Banco(this->averageAttendanceTime, this->numCaixas);
			for(int i=0; i<runTime; i += this->getEntranceTime()){
				banco->addClient(i);
				clients++;
			}
			
			if(clients != 0) {
				averageWaitTime = (banco->getWait())/clients;
			}
			
			delete(banco);
			return averageWaitTime;
		}
		
		float getEntranceTime(){
			return this->averageEntrance;
		}
		
		~Simulation(){}
};

int main() {
	float averageAttendanceTime;
	float averageEntrance;
	float runTime;
	int numCaixas;
	cin>>averageAttendanceTime>>averageEntrance>>runTime>>numCaixas;
	Simulation *simulation = new Simulation(averageAttendanceTime, averageEntrance, runTime, numCaixas);
	float averageWait = simulation->run();
	cout<<averageWait;
	delete(simulation);
	return 0;
}
