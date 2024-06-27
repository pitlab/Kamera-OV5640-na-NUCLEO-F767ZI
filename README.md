# Kamera OV5640 na Nucleo-F767ZI
Prezentacja wykorzystania kamery OV5640 o rozdzieczości 5Mpix w kontrolerze STM32F767.
Poniewż płytka Nucleo nie oszałamia nadmiarem peryfieriów, więc do podglądu wykorzystałem wyświetlacz 320x240 ze złączem typu arduino. W projekcie jest sterownik tego wyświetlacza dla płytki NUCLEO-F767ZI. 


## Dodatki
### Fraktale
Aby móc ocenić wydajność kontrolera obciążam go algrytmem i obserwują jak się zachowuje w porównaniu z innymi. Świetne do tego są fraktale. Poniżej przykłady generowania fraktala Julii i Mandelbrota. 
Julia łatwo się liczy, ale dla przyjetej w projekcie animacji, czas obliczeń każdej klatki animacji jest inny, przez co ciężko jest ją porówywać. Czas obliczeń Mandelbrota dla stałego powiekszenia jest stały.
Dla przykładu na STM32F767 fraktal Mandelbrota o rozdzielczości 320x240 liczony jest przez 313ms.
![Julia](obr/Julia_NUCLEO-F767ZI.jpg)
![Mandelbrot](obr/Mandelbrot_NUCLEO-F767ZI.jpg)

