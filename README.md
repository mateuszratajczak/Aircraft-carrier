# Aircraft-carrier
A sync problem that uses threads

Aby skompilować projekt wystarczy użyć komendy: gcc -pthread samolot.c -o samolot.out

Następnie aby uruchomić ./samolot.out

Na lotniskowcu lądują i startują samoloty. W tym celu potrzebują wyłącznego dostępu do pasa. Lotniskowiec może pomieścić pewną ustaloną liczbę N samolotów. Jeśli samolotów jest mniej niż K (K < N), wówczas priorytet w dostępie do pasa mają samoloty lądujące.

Idea realizacji:
W moim programie posiadam dwie zmienne określające liczbę samolotów (K) i liczbę miejsc postojowych na lotniskowcu (N). Dodatkowo używam dwóch zmiennych globalnych:

•	czekajaceNaLadowanie – zmienna określa liczbę samolotów oczekujących na lądowanie

•	naLotnisku – zmienna określa liczbę samolotów znajdujących się na lotniskowcu.

W swoim programie użyłem również semaforów i warunków:

•	semafor pas – pozwala na wyłączność uzyskać dostęp do pasa startowego

•	semafor oczekujące – pozwala modyfikować zmienną współdzieloną czekajaceNaLadowanie

•	semafor lotnisko – pozwala modyfikować zmienną naLotnisku

•	warunek warunek_start – warunek na którym zatrzymują się procesy jeśli nie mogą od razu wystartować

•	warunek warunek_ladowania – warunek na którym zatrzymują się procesy jeśli nie mogą od razu wylądować

W części main programu:

Po uruchomieniu programu zeruję liczbę samolotów na lotnisku i czekających na lądowanie. Na potrzeby programu zakładam że wszystkie samoloty na początku są w powietrzu, bo nie jesteśmy w stanie ustawić wszystkich na lotniskowcu, ponieważ samolotów jest więcej niż miejsc. 

Pthread_join  - czekamy aż się zakończą nasze wątki reprezentujące samoloty. 

W programie przyjąłem że priorytet mają samoloty lądujące.

Funkcja myśliwiec:

Jest to główna część programu. Dla każdego samolotu tworzymy wątek wykonujący tę funkcję. Tak jak pisałem wcześniej zaczynam z sytuacją że samolot jest w powietrzu. Następnie przy użyciu funkcji sleep symuluję  czas wykonywania misji przez samolot, aby wyglądało to w miarę naturalnie. Jest to losowa wartość z zakresu od 1 do 10. 

Następnie samolot chce wylądować, dlatego zwiększa wartość zmiennej czekajaceNaLadowanie i próbuje alokować pas startowy. Jeśli mu się udaje to sprawdza czy ma miejsce na lotniskowcu. Jeśli nie ma miejsca, to odwiesza jakiś wątek zawieszony, który chciał wystartować, a sam zawiesza się i zwalnia pas. 

Gdy wyląduje to zwiększa liczbę samolotów na lotnisku i zwalnia pas startowy. Nie potrzebujemy dodatkowego semafora dla zmiennej naLotnisku, ponieważ wyłączny dostęp zapewnia semafor pas. 

Po wylądowaniu zmniejszamy także wartość zmiennej czekająceNaLadowanie oraz sprawdzamy czy ta zmienna jest równa zero. Jeśli nikt nie czeka na lądowanie wówczas możemy wysłać sygnał signal do jakiegoś procesu zawieszonego na warunku_start i umożliwić mu start. 

Po wylądowaniu również symuluję że samolot spędza jakiś czas na lotnisku (np. w celu wykonania napraw czy uzupełnieniu paliwa). Po naszykowaniu samolotu sprawdzam, czy samolot może wystartować. Samolot może wystartować jedynie kiedy nikt nie czeka na lądowanie lub jest pełne lotnisko. W przeciwnym wypadku proces zostaje zawieszony na warunku_start i zwalnia sekcję krytyczną czyli opuszcza semafor oczekujące, pozwalający innym procesom modyfikować zmienną czekająceNaLadowanie. Gdy może wystartować to uzyskuje pas na wyłączność i  startuje.

Po starcie samolot zmniejszamy zmienną mówiącą o ilości samolotów na lotnisku oraz zwalniamy pas. Wcześniej jeszcze sprawdzamy czy jakiś proces nie został zawieszony i nie czeka lądowanie. Jeśli tak to może lądować. Jeśli nikt nie czeka to sprawdzamy wartość zmiennej czekająceNaLadowanie i jeśli jest równa zero to możemy odwiesić jakiś proces czekający na strat. 

W kodzie zawarłem wiele komentarzy, które dokładnie tłumaczą co się dzieje w danym momencie.
