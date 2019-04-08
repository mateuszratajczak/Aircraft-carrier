#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
#include<stdlib.h>
#include<time.h>

#define N 4	 //Liczba miejsc postojowych na lotniskowcu
#define K 10 //Samoloty

 int czekajaceNaLadowanie; 	//liczba samolotów w powietrzu
 int naLotnisku; 				//liczba samolotów na lotniskowcu

 pthread_mutex_t pas = PTHREAD_MUTEX_INITIALIZER;				//blokuje pas startowy
 pthread_mutex_t oczekujace = PTHREAD_MUTEX_INITIALIZER; 		//blokuje dostep do zmiennej wPowietrzuCzekajace
 pthread_mutex_t lotnisko = PTHREAD_MUTEX_INITIALIZER; 
 pthread_cond_t warunek_start = PTHREAD_COND_INITIALIZER;
 pthread_cond_t warunek_ladowania = PTHREAD_COND_INITIALIZER;



void* mysliwiec(void* numer_samolotu)
{
	int numer = *(int*)numer_samolotu;
	
	while(1) //samolot laduje i startuje naprzemiennie
	{
		//zaczynam ze kazdy jest w powietrzu bo k>n wiec wszystkie by sie nie zmiescily na lotniskowcu
		
		
		printf("Mysliwiec %d jest w powietrzu. \n", numer);
		
		int opoznienie = rand()%10;			//sumuluje ze samolot wykonuje misje
		sleep(opoznienie);
		
		//-----------------------------LADOWANIE------------------------------//
		//dajemy sygnal ze chcemy ladowac
		pthread_mutex_lock(&oczekujace);
		czekajaceNaLadowanie+=1;
		pthread_mutex_unlock(&oczekujace);
		
		//teraz chcemy ladowac wiec musimy zablokowac pas
		pthread_mutex_lock(&pas);
		
		//lotnisko ma ograniczona pojemnosc i musimy wystartowac jakis samolot jesli jest pelne
		pthread_mutex_lock(&lotnisko);
		while(naLotnisku == N) 
		{
			pthread_mutex_unlock(&lotnisko);
			pthread_cond_signal(&warunek_start); 			//odwieszamy kogos aby wystartowal
			
			pthread_cond_wait(&warunek_ladowania, &pas); 	//zawieszam się i zwolniam pas aby ktos startowal 
			
			pthread_mutex_lock(&lotnisko);
		}
		pthread_mutex_unlock(&lotnisko);
		
		//-------------------------Uzyskalismy pas i jest miejsce na plycie ------------------------//
		
		printf("Mysliwiec %d przyziemia na pasie... \n", numer);
		
		pthread_mutex_lock(&lotnisko);
		naLotnisku+=1; 					//zwiekszamy liczbe samolotow na lotniskowcu
		pthread_mutex_unlock(&lotnisko);
		
		//------------------------Zwalniamy pas startowy -------------------------------------------//
		pthread_mutex_unlock(&pas);
		
		//zmniejszam liczbe w powietrzu 
		pthread_mutex_lock(&oczekujace);
		czekajaceNaLadowanie-=1;
		
		//jesli nikt nie czeka na ladowanie to moze cos wystartowac
		if(czekajaceNaLadowanie == 0) 
		{
			pthread_cond_signal(&warunek_start);  	//odwieszamy jakis proces co jest zawieszony czekajac na start
		}
		
		pthread_mutex_unlock(&oczekujace);
		
		//.......................................................................................//
		
		printf("Pomyslnie wyladowalem. Jestem na lotniskowcu - Mysliwiec %d \n", numer);
		
		
		int opoznienie2 = rand()%10;			//jestem na lotniskwcu i dokonuje napraw
		sleep(opoznienie2);				
		
		//.............................Mysliwiec chce wystartowac ...............................//
		
		pthread_mutex_lock(&oczekujace); //dostep do zmiennej czekajaceNaLadowanie
		//pierwszenstwo maja samolotty ladujace, wiec mozemy startowac jak nikt nie chce lodowac lub jest pelne lotnisko
		pthread_mutex_lock(&lotnisko);
		while(naLotnisku < N && czekajaceNaLadowanie > 0) 
		{
			pthread_mutex_unlock(&lotnisko);
			pthread_cond_wait(&warunek_start, &oczekujace);
			pthread_mutex_lock(&lotnisko);
		}
		pthread_mutex_unlock(&lotnisko);
		
		pthread_mutex_lock(&pas);
		
		pthread_mutex_unlock(&oczekujace);
		
		//.............................START..........................................//
		
		printf("Start z pasa - Mysliwiec %d \n", numer);
		
		pthread_mutex_lock(&lotnisko);
		naLotnisku-=1;
		pthread_mutex_unlock(&lotnisko);
		
		//...................Zwolnienie pasa po starcie ............................//
		
		pthread_cond_signal(&warunek_ladowania); 		//odwieszam cos co czeka na ladowanie
		pthread_mutex_unlock(&pas); 					//zwalniam pas
		
		//teraz sprawdzam czy przez przypadek jakis jeszcze nie moze wtstartowac, bo jesli tak a nie ma nikogo kto czeka na ladowanie to odwieszam jakis i on moze wystartowac
		pthread_mutex_lock(&oczekujace);
		if(czekajaceNaLadowanie == 0) //jesli nic nie czeka to wystartuj jakis samolot czekajacy na start
		{
			pthread_cond_signal(&warunek_start);
		}
		pthread_mutex_unlock(&oczekujace);
		
	}//while	
	
}//mysliwiec


int main() 
{
	printf("------------- LOTNISKOWIEC -------------  \n");	
	
	
	naLotnisku = 0;					//liczba samolotow na lotnisku
	czekajaceNaLadowanie = 0; 		//liczba samolotow czekajacych na ladowanie
	
	int Samolot[K];			//aby bylo widac co sie dzieje kazdy samolot dostaje swoj numer
	pthread_t watki[K]; 			//jeden watek to jeden samolot
	
	for(int i=0, kk = 100; i<K; i++, kk++)
	{
		Samolot[i] = kk+1;
		pthread_create(&watki[i],NULL, mysliwiec, &Samolot[i]); 		//uruchomienie watku
	}
	
									//czekamy na koniec wątków
	for(int i=0; i<K; i++)
	{
		pthread_join(watki[i],NULL);	
	}
	
	
	printf("------------- SAMOLOTY PRZEPADLY - PROGRAM SIE ZAKONCZYL -------------  \n");
	
	return 0;
}//main
	
