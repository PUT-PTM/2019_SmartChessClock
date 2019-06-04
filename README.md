# 2019_SmartChessClock


# Inteligenty zegar do szachów szybkich

Zegar został stworzony z wykorzystaniem płytki STM32L100CDISCOVERY, kilku przycisków, dwóch wyświetlaczy siedmiosegmentowych oraz modułu bluetooth.
Zasilany jest domyślnie przez wbudowany w płytkę port micro-usb, a do sterowania nim wykorzystywane są 3 przyciski -> Dwa przyciski graczy oraz przycisk pauzy.
Wykorzystywana płytka z mikrokontrolerem, będąca sercem zegara umożliwia wysoką precyzję pomiaru czasu (1ms) oraz responsywność przycisków (czas reakcji 5ms).
Zegar można sparować ze smartfonem poprzez dedykowaną aplikację korzystając z modułu bluetooth aby dodatkowo rozszerzyć jego funkcjonalność.


# Charakterystyka zegara
-	Zegar posiada pulę wbudowanych presetów czasowych. Zawiera ona wszystkie najpopularniejsze formaty czasowe wykorzystywane w szachach szybkich. Kontrole czasowe takie jak Hyper Bullet czy Ultra Bullet nie zostały zaimplementowane z uwagi na bycie zbyt szybkimi i nie praktycznymi do grania na fizycznej szachownicy
-	Istnieje możliwość dokładnej konfiguracji presetu czasowego, a następnie wgranie go na zegar poprzez moduł bluetooth z poziomu aplikacji android. Taki preset należy wgrać zawsze po uruchomieniu zegara i wybraniu odpowiedniego trybu.
-	Zegar posiada 2 liczniki czasu dla każdego z graczy, które zliczają upływający czas z dokładnością do jednej milisekundy.
-	Wyświetlacze są odświeżane z częstotliwością 100Hz oraz posiadają migające dwukropki odzielające liczbę minut od liczby sekund danego gracza.
-	Jak w każdym zegarze szachowym instieje możliwość zmiany gracza, któremu obecnie ma upływać czas podczas pauzy.
-	W odróżnieniu od wiodących zegarów elektronicznych po upływie czasu jednego z graczy zegar "blokuje" się na nim i nie pozwala na włączenie upływu czasu drugiego gracza. Jest to duże udogodnienie, ponieważ nie istnieje możliwość że obu graczom "skończy" się czas.
-	Przyciski graczy w porównaniu do innych zegarów elektronicznych korzystających z klasycznych przycisków do przełączania czasu są duże, mają wyczuwalny ale nie zbyt głęboki skok, oraz posiadają wbudowane diody led, które informują któremu z graczy obecnie upływa czas.

# Presety czasowe i ich wybór
Po uruchomieniu zegara należy wybrać preset czasowy poruszając się przyciskami graczy. Na lewym wyświetlaczu widnieje numer presetu, na prawym odpowiednio przed dwukropkiem liczba minut, a za dwukropkiem, liczba dodawanych sekund co ruch.
Wybór presetu potwierdzamy przyciskiem pauzy. Wybór ostatniego presetu powoduje, że zegar oczekuje na przesłanie mu skonfigurowanego presetu z aplikacji android.
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

# Aspekty czysto techniczne - częśc programowa zegara
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


### Twórcy:
- Marcin Borysiewicz
- Krystian Duchnowski