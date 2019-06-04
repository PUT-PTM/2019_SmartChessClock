# 2019_SmartChessClock


# Inteligenty zegar do szachów szybkich

Zegar został stworzony z wykorzystaniem płytki STM32L100CDISCOVERY, kilku przycisków, dwóch wyświetlaczy siedmiosegmentowych oraz modułu bluetooth.
Zasilany jest domyślnie przez wbudowany w płytkę port micro-usb, a do sterowania nim wykorzystywane są 3 przyciski -> Dwa przyciski graczy oraz przycisk pauzy.
Wykorzystywana płytka z mikrokontrolerem, będąca sercem zegara umożliwia wysoką precyzję pomiaru czasu (1ms) oraz responsywność przycisków (czas reakcji 5ms).
Zegar można sparować ze smartfonem poprzez dedykowaną aplikację korzystając z modułu bluetooth aby dodatkowo rozszerzyć jego funkcjonalność.


# Charakterystyka zegara
-	Zegar posiada pulę wbudowanych presetów czasowych. Zawiera ona wszystkie najpopularniejsze formaty czasowe wykorzystywane w szachach szybkich. Kontrole czasowe takie jak Hyper Bullet czy Ultra Bullet nie zostały zaimplementowane z uwagi na bycie zbyt szybkimi i nie praktycznymi do grania na fizycznej szachownicy.
-	Istnieje możliwość dokładnej konfiguracji presetu czasowego, a następnie wgranie go na zegar poprzez moduł bluetooth z poziomu aplikacji android. Taki preset należy wgrać zawsze po uruchomieniu zegara i wybraniu odpowiedniego trybu.
-	Zegar posiada 2 liczniki czasu dla każdego z graczy, które zliczają upływający czas z dokładnością do jednej milisekundy.
-	Wyświetlacze są odświeżane z częstotliwością 100Hz oraz posiadają migające dwukropki odzielające liczbę minut od liczby sekund danego gracza.
-	Jak w każdym zegarze szachowym instieje możliwość zmiany gracza, któremu obecnie ma upływać czas podczas pauzy.
-	W odróżnieniu od wiodących zegarów elektronicznych po upływie czasu jednego z graczy zegar "blokuje" się na nim i nie pozwala na włączenie upływu czasu drugiego gracza. Jest to duże udogodnienie, ponieważ nie istnieje możliwość że obu graczom "skończy" się czas.
-	Przyciski graczy w porównaniu do innych zegarów elektronicznych korzystających z klasycznych przycisków do przełączania czasu są duże, mają wyczuwalny ale nie zbyt głęboki skok, oraz posiadają wbudowane diody led, które informują któremu z graczy obecnie upływa czas.

# Charakterystyka aplikacji na urządzenia z systemem Android
- Aplikacja umożliwia sparowanie oraz nawiązanie połączenia między urządzeniem i zegarem. W tym trybie działania czas wyświetlany na ekranie jest z sychnronizowany z zegarem. 
- Oprogramowanie rozszerza funkcjonalność zegara o takie funkcjonalności jak informacja o długości poprzedniego ruchu dla każdego z graczy, wyświetlanie czasu z dokładnością do 10ms oraz tworzenie własnych formatów czasowych.
- Aplikacja może również działać w trybie "StandAlone" (bez zegara) zachowując pełną funkcjonalność.
- W przypadku wyczerpania czasu jednego z graczy pojawia się stosowna informacja. Jest to pomocna funkcja w sytuacjach ciężkich do rozstrzygnięcia.

# Presety czasowe i ich wybór
Po uruchomieniu zegara należy wybrać preset czasowy poruszając się przyciskami graczy. Na lewym wyświetlaczu widnieje numer presetu, na prawym odpowiednio przed dwukropkiem liczba minut, a za dwukropkiem, liczba dodawanych sekund co ruch.
Wybór presetu potwierdzamy przyciskiem pauzy. Wybór ostatniego presetu powoduje, że zegar oczekuje na przesłanie mu skonfigurowanego presetu z aplikacji android. Edytor formatów czasowych na urządzeniu mobilnym można uruchomić poprzez naciśnięcie obszaru presetem w oknie głównym. Zatwierdzenie nowego presetu wysyła odpowiednią informację jeżeli połączenie zostało wcześniej nawiązane.
Po wybraniu presetu wystarczy wcisnąć przycisk pauzy. Od tego momentu czas zacznie upływać graczowi, którego przycisk jest zapalony.

# Rozgrywka
Gracze na zmianę po wykonanym na szachownicy ruchu wciskają swój przycisk (wedle zasad turniejowych tą samą ręką co wykonali ruch!) co powoduje, że przeciwnikowi zaczyna upływać czas.
Gra toczy się do momentu, aż gra nie roztrzygnie się na szachownicy (wtedy zegar już nie jest potrzebny), gracze zgodzą się na remis lub jeden z nich zrezygnuje, lub gdy jednemu z graczy skończy się czas.

# Koniec czasu
Po upływie czasu jednego z graczy zegar zostaje "zablokowany". Nie można zmienić obenego gracza, oraz przycisk gracza, któremu skończył się czas zostaje zapalony.
Jak we wszystkich elektronicznych zegarach aby zagrać ponownie, należy zresetować zegar (odłączyć i podączyć zasilanie).

# Inkrementacja
Niektóre presety czasowe oferują dodawanie czasu gracza po wykonanym ruchu. Naturalnie dzięki inkrementacji gracz może po chwili mieć więcej czasu niż kiedy rozpoczęto rozgrywkę.
Limitem jest liczba 99 minut i 99 sekund. Oczywiście zegar zapamięta czas powyżej tej wartości, ale wyświetlacze nie będą go poprawnie wyświetlały zanim spadnie poniżej 100 minut.
Taka sytuacja jest jednakże trudno osiągalna i wymaga niezmiernie szybkich palcy oraz wiele samozapału.

# Wykorzystane narzędzia
## STM32
- Płytka STM32L100CDISCOVERY
- IDE STM32 System Workbench
- Generator kodu inicjalizującego STM32CubeMX
## Android
- Android Studio 3.3.1 (Build #AI-182.5107.16.33.5264788, built on January 29, 2019)
- JRE: 1.8.0_152-release-1248-b01 amd64
- JVM: OpenJDK 64-Bit Server VM by JetBrains s.r.o

# Aspekty czysto techniczne - część programowa zegara
Logika zegara została napisana w języku C. Sam zegar śmiga na zawrotnych 500 liniach kodu, nie licząc biblioteki do obsługi wyświetlaczy. Przechowywanie danych o czasach obu graczy,
oraz ich inkrementacja/dekrementacja zostały oparte o struktury i tablice. Program pamięta osobno liczbę minut, sekund i milisekund dla każdego z graczy co czyni kod nieco czytelniejszym i łatwiejszym w rozwoju.
Płytka ma na tyle dużą moc obliczeniową, że w jednym przerwaniu timera może obsłużyć całą logikę związaną z upłynięciem jednej milisekundy (a jest tego sporo) zanim faktycznie milisekunda minie.

# Aspekty czysto techniczne - płytka STM32L100CDISCOVERY
-	Nad poprawnym działaniem zegara czuwają 4 timery. Po jednym na zliczanie czasu każdego z graczy, timer odpowiedzialny za debouncing przycisków oraz timer odpowiedzialny za odświeżanie wyświetlaczy.
-	Wyświetlacze są obsługiwane przez 2 interfejsy I2C z wykorzystaniem open-source'owej bliblioteki przeznaczonej do układu cyfrowego umieszczonego w wyświetlaczach.
-	Wyświetlacze są zasilane napięciem 3V.
-	Moduł bluetooth obsługuje interfejs UART.
-	Moduł bluetooth również jest zasilany napięciem 3V.
-	3 przyciski sterujące posiadają zaimplementowany debouncing, który opóźnia ich czas reakcji do 5ms w zamian za dokładny pomiar stanu na pinach GPIO
-	Z uwagi na dużą liczbę potrzebnych pinów dla przycisków sterujących (po cztery na każdy przycisk graczy oraz 2 na przycisk pauzy) wszystkie przyciski sterujące posiadają wspólną masę.

# Aspekty czysto techniczne - część programowa aplikacji Android
Cała aplikacja została przygotowana w programie Android Studio z wykorzystaniem języka programowania Java (logika aplikacji) oraz 
plików XML (oprawa graficzna). 
Oprogramowanie przechowuje czas obu graczy w postaci ilości milisekund, a za przedstawianie go w czytelniejszej dla człowieka formie odpowiedzialna jest odpowiednia funkcja.
Presety czasowe są reprezentowane za pomocą obiektów autorskiej klasy TimePreset, co umożliwiło stworzenie prostego i intuicyjnego edytora formatów.
W celu zapewnienia synchronicznej pracy aplikacji, każda funkcjonalność jest realizowana na innym wątku.

# Jak skompilować
- Podłączyć wszystkie piny według schematu zawartego w pliku STMSmartChessClock\STMSmartChessClock.ioc
- Podłączyć płytkę do komputera przez port mikto-usb
- Skompilować kod i wgrać go na płytkę poprzez IDE System Workbench z pliku STMSmartChessClock\.cproject
- Uruchomić projekt w Android Studio
- Podłączyć urządzenie mobilne do komputera
- Zbudować i uruchomić projekt według standarowych kroków

# Ulepszenia w przyszłości
- Komunikacja Bluetooth nie działa do końca poprawnie po stronie aplikacji android.
- Edytor presetów może być zaimplementowany również bezpośrednio na zegarze.

# Przypisania
Wszystkie grafiki widoczne w interfejsie graficznym aplikacji android pochodzą lub są wygenerowane na podstawie darmowych zasobów
zawartych w programie Android Studio.

# Licencja
MIT

### Twórcy:
- Marcin Borysiewicz
- Krystian Duchnowski

The project was conducted during the Microprocessor Lab course held by the Institute of Control and Information Engineering, Poznan University of Technology.
Supervisor: Tomasz Mańkowski
