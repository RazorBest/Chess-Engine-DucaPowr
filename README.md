
# Botul de sah DucaPowr

0. Trebuie sa dea la comanda citate memorabile.
1. Trebuie sa primeasca inputul de la xboard.
2. Trebuie sa scrie/raspunda catre xboard.
3. Trebuie sa stocheze pozitiile pieselor pe tabla de sah (vom defini asta ca
    fiind starea tablei).
4. Trebuie sa decida mutari pentru ambii jucatori, in functie de ce i se cere.
5. Trebuie sa decida mutarile intr-un anumit timp limita.

6. Vom spune ca botul "gandeste" atunci cand computeaza pozitii.
7. Trebuie sa raspunda la comenzi care au nevoie de raspuns instant. De
    exemplu, daca in timp ce botul gandeste, xboard trimite comanda
    "go", acesta trebuie sa se opreasca din gandit si sa raspunda instant
    cu o mutare.
8. Trebuie sa sa gandeasca in timp ce oponentul gandeste. Adica sa se
    foloseasca la maxim de timpul limita.

9. Botul este reprezentat de clasa Chess.

**Clasa Chess:**

	- trebuie sa stocheze starea tablei.
	- trebuie sa gandeasca.
	- trebuie sa aiba o metoda care actualizeaza starea tablei. (
        care e apelata atunci cand e facuta o mutare).
	- trebuie sa aiba o metoda de gandire. Aceasta trebuie sa
    aplice un algoritm pentru a calcula mutarea optima. Problema este ca
    aceasta metoda trebuie sa aiba un timp limita. Botul nu poate gandi la 
    nesfarsit. Totusi, metoda de gandire trebuie sa poata fi apelata de 
    repetate ori la rand. Cu cat e apelata de mai multe ori, cu atat botul 
    gandeste "mai mult". (Un exemplu de implementare a acestei metode este
    ca acesta sa nu depaseasca o secunda).
	- Clasa Chess trebuie sa aiba o metoda care returneaza o mutare in urma
    gandirii. (P.S.: daca mutarea e valida, atunci trebuie actualizata starea 
    tablei.)

